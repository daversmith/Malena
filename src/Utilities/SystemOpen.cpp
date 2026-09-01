// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Utilities/SystemOpen.h>

#if defined(_WIN32)
  #include <windows.h>
  #include <shellapi.h>
#else
  #include <cerrno>
  #include <fcntl.h>
  #include <sys/wait.h>
  #include <unistd.h>
#endif

namespace ml {

namespace {
#if !defined(_WIN32)
/**
 * fork + exec rather than system().
 *
 * system() runs the string through a shell, so any character the caller did not
 * anticipate is a command. The URIs here are constants today, but a helper that
 * is safe only while its arguments stay constant is a trap for whoever passes a
 * variable to it next.
 *
 * The return value reports whether EXEC succeeded, not merely whether fork did.
 * That distinction is the whole point: openWifiSettings() picks between two
 * macOS URIs based on this answer, and a version that always said "yes" would
 * never try the second one - so a wrong first guess would silently do nothing.
 *
 * The mechanism is a close-on-exec pipe. A successful exec closes the write end
 * and the parent reads EOF; a failed exec writes errno before _exit, and the
 * parent reads it. Either way the parent learns the outcome without waiting for
 * the launched application, which on a cold start takes seconds and would freeze
 * the UI thread.
 */
bool spawn(const char* prog, const std::string& arg)
{
    int fds[2];
    if (::pipe(fds) != 0) return false;
    ::fcntl(fds[1], F_SETFD, ::fcntl(fds[1], F_GETFD) | FD_CLOEXEC);

    const pid_t pid = ::fork();
    if (pid < 0) { ::close(fds[0]); ::close(fds[1]); return false; }

    if (pid == 0) {
        ::close(fds[0]);
        ::execlp(prog, prog, arg.c_str(), (char*)nullptr);
        const int err = errno;
        (void)!::write(fds[1], &err, sizeof err);
        ::_exit(127);                    // exec failed; never return into the child
    }

    ::close(fds[1]);
    int childErrno = 0;
    const ssize_t n = ::read(fds[0], &childErrno, sizeof childErrno);
    ::close(fds[0]);

    // The child is reaped rather than left for init: it has already exec'd or
    // died, so this returns immediately and does not leave a zombie behind for
    // every link an app opens over a long session.
    int status = 0;
    ::waitpid(pid, &status, n > 0 ? 0 : WNOHANG);

    return n == 0;                       // EOF means exec replaced the child
}
#endif
} // namespace

bool systemOpen(const std::string& uri)
{
    if (uri.empty()) return false;

#if defined(_WIN32)
    // Above 32 means success, per ShellExecute's (unusual) contract.
    const auto r = ::ShellExecuteA(nullptr, "open", uri.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    return reinterpret_cast<INT_PTR>(r) > 32;
#elif defined(__APPLE__)
    return spawn("open", uri);
#else
    return spawn("xdg-open", uri);
#endif
}

bool openWifiSettings()
{
#if defined(_WIN32)
    return systemOpen("ms-settings:network-wifi");
#elif defined(__APPLE__)
    // Ventura and later, where System Preferences became System Settings. There
    // is deliberately no fallback to the old com.apple.preference.network pane:
    // `open` accepts an unknown scheme, fails on its own, and exits non-zero, so
    // the exec-success this returns cannot distinguish the two - a fallback
    // chained on it would never fire and would only look like it worked.
    // Anyone needing Monterey or earlier wants that identifier and a real check
    // on `open`'s exit status, which means blocking the click handler.
    return systemOpen("x-apple.systempreferences:com.apple.wifi-settings-extension");
#else
    // No settings URI is common to every desktop, so open the one GNOME uses and
    // let the caller's on-screen instruction carry the rest.
    return systemOpen("gnome-control-center wifi");
#endif
}

} // namespace ml
