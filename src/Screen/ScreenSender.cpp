// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Screen/ScreenSender.h>

#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

#if defined(_WIN32)
  #include <windows.h>
#else
  #include <spawn.h>
  #include <signal.h>
  #include <sys/wait.h>
  #include <unistd.h>
  extern char** environ;
#endif

// Screen capture runs OUT OF PROCESS.
//
// The capture+encode pipeline (avfvideosrc/d3d11/ximagesrc → HW H.264 encoder →
// rtspclientsink) inherently uses the GPU (VideoToolbox / Media Foundation /
// V4L2). Running it in-process activates a GL/Metal context that invalidates the
// host app's SFML textures → the next UI draw aborts in sf::Texture::bind. To stay
// completely clear of the app's GL context we spawn the pipeline as a child
// `gst-launch-1.0` process and just manage its lifecycle (start/stop/respawn).

namespace ml {

namespace {

// Locate gst-launch-1.0: explicit override, then common install paths, then PATH.
std::string resolveGstLaunch()
{
    if (const char* e = std::getenv("LOCKIN_GST_LAUNCH"); e && *e) return e;
#if defined(_WIN32)
    // A BUNDLED copy first, beside the executable. A shipped app cannot assume
    // GStreamer is installed — the machine it runs on belongs to an instructor,
    // not a developer — so distribution ships gst-launch and the GStreamer DLLs
    // in the app's own directory and this finds them before any system install.
    //
    // Same directory as the .exe, not a gstreamer\bin subdirectory: the app also
    // loads GStreamer in-process (the receiver), and Windows resolves a DLL
    // against the directory the PROCESS was loaded from. One copy at the root
    // serves both. The old subdirectory layout is still accepted so a bundle
    // built by an older script keeps working.
    {
        char self[MAX_PATH] = {0};
        if (GetModuleFileNameA(nullptr, self, MAX_PATH)) {
            std::string dir(self);
            const auto slash = dir.find_last_of("\\/");
            if (slash != std::string::npos) {
                const std::string base = dir.substr(0, slash);
                for (const std::string& bundled : {
                         base + "\\gst-launch-1.0.exe",
                         base + "\\gstreamer\\bin\\gst-launch-1.0.exe" })
                    if (GetFileAttributesA(bundled.c_str()) != INVALID_FILE_ATTRIBUTES)
                        return bundled;
            }
        }
    }

    // The GStreamer Windows installer sets GSTREAMER_1_0_ROOT_MSVC_X86_64 (or _MINGW).
    for (const char* root : { "GSTREAMER_1_0_ROOT_MSVC_X86_64",
                              "GSTREAMER_1_0_ROOT_MINGW_X86_64",
                              "GSTREAMER_1_0_ROOT_X86_64" }) {
        if (const char* r = std::getenv(root); r && *r) {
            std::string p = std::string(r) + "\\bin\\gst-launch-1.0.exe";
            if (GetFileAttributesA(p.c_str()) != INVALID_FILE_ATTRIBUTES) return p;
        }
    }
    const char* winCandidates[] = {
        "C:\\gstreamer\\1.0\\msvc_x86_64\\bin\\gst-launch-1.0.exe",
        "C:\\gstreamer\\1.0\\mingw_x86_64\\bin\\gst-launch-1.0.exe",
    };
    for (const char* c : winCandidates)
        if (GetFileAttributesA(c) != INVALID_FILE_ATTRIBUTES) return c;
    return "gst-launch-1.0.exe";   // fall back to PATH lookup
#else
    const char* candidates[] = {
  #if defined(__APPLE__)
        "/opt/homebrew/bin/gst-launch-1.0",
        "/usr/local/bin/gst-launch-1.0",
        "/Library/Frameworks/GStreamer.framework/Versions/Current/bin/gst-launch-1.0",
  #else
        "/usr/bin/gst-launch-1.0",
        "/usr/local/bin/gst-launch-1.0",
  #endif
    };
    for (const char* c : candidates)
        if (::access(c, X_OK) == 0) return c;
    return "gst-launch-1.0";   // fall back to PATH lookup
#endif
}

// Split a pipeline description into whitespace-separated argv tokens. None of our
// pipeline values contain spaces (URLs, caps, element props), so this is exact —
// and because gst-launch is given the pipeline via argv, no shell quoting is used.
std::vector<std::string> tokenize(const std::string& s)
{
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

} // namespace

// =============================================================================
// Impl
// =============================================================================

struct ScreenSenderBase::Impl
{
    enum class Src { Display, Window };

    std::string  name;
    std::string  url;
    int          captureIndex = 0;   // which display (macOS avfvideosrc device-index)
    Src          src          = Src::Display;
    unsigned int windowId     = 0;   // when src == Window (macOS CGWindowID)
    std::string  windowHelper;       // per-window capture helper executable

    std::atomic<bool> publishing { false };
    std::atomic<bool> running    { false };
    std::thread       watcher;

#if defined(_WIN32)
    std::atomic<void*> childHandle { nullptr };   // HANDLE to the gst-launch child
#else
    std::atomic<pid_t> childPid { -1 };
#endif

#if !defined(_WIN32) && !defined(__APPLE__)
    // Which H.264 encoders this machine actually has, asked once.
    //
    // Probed by running gst-inspect rather than linking GStreamer: the pipeline
    // runs in a CHILD process (gst-launch), so the host app has no registry of
    // its own to query and no reason to gain one.
    //
    // Order is hardware first, then portable software. x264enc is absent on
    // purpose — GPL.
    static std::string pickLinuxEncoder()
    {
        static const std::string chosen = []() -> std::string {
            const std::pair<const char*, const char*> candidates[] = {
                // Raspberry Pi's V4L2 stateful encoder.
                { "v4l2h264enc", "v4l2h264enc extra-controls=controls,video_bitrate=6000000" },
                // Intel/AMD via VA-API, when the driver stack is installed.
                { "vaapih264enc", "vaapih264enc bitrate=6000 keyframe-period=30" },
                // Cisco openh264: BSD licensed, software, works anywhere.
                { "openh264enc", "openh264enc bitrate=6000000 gop-size=30" },
            };
            for (const auto& [element, pipeline] : candidates) {
                const std::string probe = std::string("gst-inspect-1.0 ") + element
                                        + " >/dev/null 2>&1";
                if (std::system(probe.c_str()) == 0) {
                    std::fprintf(stderr, "[ScreenSender] H.264 encoder: %s\n", element);
                    return pipeline;
                }
            }
            std::fprintf(stderr,
                "[ScreenSender] no usable H.264 encoder found. Install one of:\n"
                "    gstreamer1.0-plugins-bad   (openh264enc, portable)\n"
                "    gstreamer1.0-vaapi         (vaapih264enc, Intel/AMD hardware)\n");
            return {};
        }();
        return chosen;
    }
#endif

    // Per-platform: desktop capture → convert → non-GPL H.264 encode → RTSP push.
    // Built as an argv-style (unquoted) pipeline for gst-launch-1.0.
    std::string buildPipeline() const
    {
        // rtspclientsink publishes (RTSP RECORD) to the given location.
        const std::string sink =
            " h264parse ! rtspclientsink location=" + url + " latency=0";

#if defined(__APPLE__)
        // VideoToolbox HW encoder (vtenc_h264) — Apple-provided, not GPL.
        // Downscale to 720p / cap 15fps before encode so the receiver's decoder
        // keeps up (a full Retina desktop at native fps grows latency to seconds).
        return
            "avfvideosrc capture-screen=true capture-screen-cursor=true device-index="
            + std::to_string(captureIndex) + " ! "
            "video/x-raw ! videorate ! videoscale add-borders=true ! "
            "video/x-raw,width=1280,height=720,framerate=15/1 ! "
            "videoconvert ! "
            "vtenc_h264 realtime=true allow-frame-reordering=false "
            "max-keyframe-interval=15 bitrate=4000 !" + sink;

#elif defined(_WIN32)
        // Media Foundation HW encoder (mfh264enc) — shipped with Windows, not GPL.
        // monitor-index selects which display to capture (DXGI output order, which
        // matches the picker's EnumDisplayMonitors order in the common case).
        return
            "d3d11screencapturesrc monitor-index=" + std::to_string(captureIndex) + " ! "
            "videoconvert ! "
            "mfh264enc low-latency=true bitrate=6000 !" + sink;

#else
        // Linux: the encoder is chosen at RUNTIME, because "Linux" here spans a
        // Raspberry Pi and an instructor's laptop, and they share no encoder.
        // This used to hard-code v4l2h264enc — the Pi's V4L2 hardware encoder —
        // which simply does not exist on a normal x86 machine, so screen sharing
        // could never have worked there however it was packaged.
        //
        // x264enc is deliberately NOT in the list: it is GPL, and this ships in a
        // commercial product. openh264 (Cisco, BSD) is the portable fallback.
        //
        // ximagesrc is X11. Under a bare Wayland session there is no X server to
        // capture and this will fail; the fix there is pipewiresrc plus a portal
        // permission, which is a separate piece of work. Most desktops still run
        // XWayland, where this works.
        const std::string enc = pickLinuxEncoder();
        if (enc.empty()) return {};   // caller logs; nothing sane to launch

        return "ximagesrc use-damage=false ! videoconvert ! "
               "video/x-raw,format=I420 ! " + enc + " !" + sink;
#endif
    }

    // The child-process argv: the per-window helper for Src::Window (when one is
    // configured), otherwise gst-launch with the per-platform display pipeline.
    std::vector<std::string> launchArgs() const
    {
        if (src == Src::Window && !windowHelper.empty()) {
            return { windowHelper, "--window-id", std::to_string(windowId),
                     "--url", url, "--fps", "15", "--bitrate", "4000" };
        }

#if defined(__APPLE__)
        // Whole-display capture ALSO goes through the helper when one is
        // configured, not just single windows.
        //
        // The alternative — gst-launch-1.0 with avfvideosrc — is a separate,
        // Homebrew-installed, unsigned binary, so macOS assigns it its own TCC
        // identity. Screen Recording then has to be granted to gst-launch-1.0
        // rather than to the host app: the toggle in System Settings looks right,
        // capture still fails, and the prompt keeps returning. Worse, a Homebrew
        // upgrade replaces the binary and silently voids the grant.
        //
        // The helper ships inside the app bundle and is signed with the same
        // Developer ID, so macOS attributes it to the app and one grant holds.
        if (src == Src::Display && !windowHelper.empty()) {
            return { windowHelper, "--display-index", std::to_string(captureIndex),
                     "--url", url, "--fps", "15", "--bitrate", "4000" };
        }
#endif
        std::vector<std::string> a;
        a.push_back(resolveGstLaunch());
        a.push_back("-e");                                   // EOS on shutdown → clean finalize
        for (const std::string& t : tokenize(buildPipeline())) a.push_back(t);
        return a;
    }

#if !defined(_WIN32)
    void startPipeline()
    {
        stopPipeline();
        if (url.empty()) return;

        const std::vector<std::string> args = launchArgs();
        if (args.empty()) return;

        std::vector<char*> argv;
        argv.reserve(args.size() + 1);
        for (const std::string& s : args) argv.push_back(const_cast<char*>(s.c_str()));
        argv.push_back(nullptr);

        pid_t pid = -1;
        const int rc = posix_spawnp(&pid, args[0].c_str(), nullptr, nullptr, argv.data(), environ);
        if (rc != 0) {
            std::fprintf(stderr, "[ScreenSender:%s] spawn failed (%s): rc=%d\n",
                         name.c_str(), args[0].c_str(), rc);
            childPid.store(-1);
            return;
        }
        childPid.store(pid);
        publishing.store(true);
        std::fprintf(stderr, "[ScreenSender:%s] capturing via %s → publishing %s (out-of-process pid %d)\n",
                     name.c_str(), args[0].c_str(), url.c_str(), static_cast<int>(pid));
    }

    void stopPipeline()
    {
        publishing.store(false);
        pid_t pid = childPid.exchange(-1);
        if (pid <= 0) return;

        ::kill(pid, SIGTERM);
        for (int i = 0; i < 20; ++i) {            // up to ~1s for a clean EOS finalize
            const pid_t r = ::waitpid(pid, nullptr, WNOHANG);
            if (r == pid || r == -1) { pid = -1; break; }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if (pid > 0) { ::kill(pid, SIGKILL); ::waitpid(pid, nullptr, 0); }
    }

    // Background watcher: respawn the capture process if it dies (server not up
    // yet, transient error), with a short backoff. Reaps the child either way.
    void watch()
    {
        using namespace std::chrono;
        while (running.load()) {
            const pid_t pid = childPid.load();
            if (pid <= 0) { std::this_thread::sleep_for(milliseconds(200)); continue; }

            const pid_t r = ::waitpid(pid, nullptr, WNOHANG);
            if (r == 0) { std::this_thread::sleep_for(milliseconds(200)); continue; }  // alive

            // Child exited (reaped) → back off and respawn.
            childPid.store(-1);
            publishing.store(false);
            std::fprintf(stderr, "[ScreenSender:%s] capture process exited — retrying\n", name.c_str());
            if (running.load()) {
                std::this_thread::sleep_for(seconds(1));
                if (running.load()) startPipeline();
            }
        }
    }
#else
    // Windows: spawn gst-launch via CreateProcess (the d3d11/Media-Foundation
    // capture pipeline runs in the child, never touching the host app's context).
    void startPipeline()
    {
        stopPipeline();
        if (url.empty()) return;

        const std::string launch = resolveGstLaunch();
        // CreateProcess takes one command line. Quote the exe (it may live under a
        // path with spaces); the pipeline tokens themselves contain no spaces.
        std::string cmd = "\"" + launch + "\" -e " + buildPipeline();
        std::vector<char> cmdline(cmd.begin(), cmd.end());
        cmdline.push_back('\0');

        STARTUPINFOA si{};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};
        const BOOL ok = CreateProcessA(
            nullptr, cmdline.data(), nullptr, nullptr, FALSE,
            CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);   // no console window for the GUI app
        if (!ok) {
            std::fprintf(stderr, "[ScreenSender:%s] CreateProcess failed (%s): %lu\n",
                         name.c_str(), launch.c_str(), static_cast<unsigned long>(GetLastError()));
            childHandle.store(nullptr);
            return;
        }
        CloseHandle(pi.hThread);
        childHandle.store(pi.hProcess);
        publishing.store(true);
        std::fprintf(stderr, "[ScreenSender:%s] capturing → publishing %s (out-of-process pid %lu)\n",
                     name.c_str(), url.c_str(), static_cast<unsigned long>(pi.dwProcessId));
    }

    void stopPipeline()
    {
        publishing.store(false);
        HANDLE h = static_cast<HANDLE>(childHandle.exchange(nullptr));
        if (!h) return;
        TerminateProcess(h, 0);            // no clean EOS on Windows — abrupt is fine for a live publisher
        WaitForSingleObject(h, 2000);
        CloseHandle(h);
    }

    // Background watcher: respawn the capture process if it dies, with a backoff.
    void watch()
    {
        using namespace std::chrono;
        while (running.load()) {
            HANDLE h = static_cast<HANDLE>(childHandle.load());
            if (!h) { std::this_thread::sleep_for(milliseconds(200)); continue; }

            if (WaitForSingleObject(h, 0) == WAIT_TIMEOUT) {     // still alive
                std::this_thread::sleep_for(milliseconds(200));
                continue;
            }
            // Exited → close the handle and back off before respawning.
            HANDLE old = static_cast<HANDLE>(childHandle.exchange(nullptr));
            if (old) CloseHandle(old);
            publishing.store(false);
            std::fprintf(stderr, "[ScreenSender:%s] capture process exited — retrying\n", name.c_str());
            if (running.load()) {
                std::this_thread::sleep_for(seconds(1));
                if (running.load()) startPipeline();
            }
        }
    }
#endif
};

// =============================================================================
// ScreenSenderBase
// =============================================================================

ScreenSenderBase::ScreenSenderBase(const char* name, const char* rtspUrl)
    : _impl(std::make_unique<Impl>())
{
    _impl->name = name ? name : "Share";
    _impl->url  = rtspUrl ? rtspUrl : "";
}

ScreenSenderBase::~ScreenSenderBase()
{
    stop();
}

void ScreenSenderBase::start()
{
    if (_impl->running.load()) return;
    _impl->running.store(true);
    _impl->startPipeline();
    _impl->watcher = std::thread([d = _impl.get()] { d->watch(); });
}

void ScreenSenderBase::stop()
{
    _impl->running.store(false);
    if (_impl->watcher.joinable()) _impl->watcher.join();
    _impl->stopPipeline();
}

void ScreenSenderBase::setUrl(const std::string& rtspUrl)
{
    if (rtspUrl == _impl->url) return;
    const bool wasRunning = _impl->running.load();
    if (wasRunning) stop();
    _impl->url = rtspUrl;
    if (wasRunning) start();
}

void ScreenSenderBase::setCaptureIndex(int index)
{
    if (index == _impl->captureIndex && _impl->src == Impl::Src::Display) return;
    const bool wasRunning = _impl->running.load();
    if (wasRunning) stop();
    _impl->captureIndex = index;
    _impl->src          = Impl::Src::Display;
    if (wasRunning) start();
}

void ScreenSenderBase::setCaptureWindow(unsigned int windowId)
{
    if (_impl->src == Impl::Src::Window && _impl->windowId == windowId) return;
    const bool wasRunning = _impl->running.load();
    if (wasRunning) stop();
    _impl->src      = Impl::Src::Window;
    _impl->windowId = windowId;
    if (wasRunning) start();
}

void ScreenSenderBase::setWindowHelperPath(const std::string& path)
{
    _impl->windowHelper = path;
}

bool ScreenSenderBase::isPublishing() const { return _impl->publishing.load(); }
const std::string& ScreenSenderBase::senderName() const { return _impl->name; }
const std::string& ScreenSenderBase::url() const { return _impl->url; }

} // namespace ml
