// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Screen/ScreenSource.h>

#include <algorithm>
#include <cmath>

#if defined(__APPLE__)
#  include <CoreGraphics/CoreGraphics.h>
#elif defined(_WIN32)
#  include <windows.h>
#else
#  if __has_include(<X11/Xlib.h>)
#    include <X11/Xlib.h>
#    include <X11/Xutil.h>
#    define MALENA_HAVE_X11 1
#  endif
#endif

namespace ml {

struct ScreenSource::Impl
{
    int          display = 0;
    unsigned int maxDim  = 0;
    std::string  error;

    // Scale down to maxDim on the long edge, preserving aspect. Done on the CPU
    // with nearest-neighbour: the frame is about to be JPEG'd and viewed at a
    // fraction of desktop size, so filtering quality is not the bottleneck.
    static sf::Image downscale(const sf::Image& src, unsigned int maxDim)
    {
        const sf::Vector2u s = src.getSize();
        const unsigned int longEdge = std::max(s.x, s.y);
        if (maxDim == 0 || longEdge <= maxDim || s.x == 0 || s.y == 0) return src;

        const float k = static_cast<float>(maxDim) / static_cast<float>(longEdge);
        const sf::Vector2u d{ std::max(1u, static_cast<unsigned int>(s.x * k)),
                              std::max(1u, static_cast<unsigned int>(s.y * k)) };

        std::vector<std::uint8_t> px(static_cast<std::size_t>(d.x) * d.y * 4);
        for (unsigned int y = 0; y < d.y; ++y)
        {
            const unsigned int sy = std::min(s.y - 1, static_cast<unsigned int>(y / k));
            for (unsigned int x = 0; x < d.x; ++x)
            {
                const unsigned int sx = std::min(s.x - 1, static_cast<unsigned int>(x / k));
                const sf::Color c = src.getPixel({sx, sy});
                const std::size_t o = (static_cast<std::size_t>(y) * d.x + x) * 4;
                px[o + 0] = c.r; px[o + 1] = c.g; px[o + 2] = c.b; px[o + 3] = c.a;
            }
        }
        return sf::Image(d, px.data());
    }
};

ScreenSource::ScreenSource() : _impl(std::make_unique<Impl>()) {}
ScreenSource::~ScreenSource() = default;

void ScreenSource::setDisplayIndex(int index) { _impl->display = index < 0 ? 0 : index; }
int  ScreenSource::displayIndex() const       { return _impl->display; }
void ScreenSource::setMaxDimension(unsigned int px) { _impl->maxDim = px; }
const std::string& ScreenSource::lastError() const  { return _impl->error; }

bool ScreenSource::isSupported()
{
#if defined(__APPLE__) || defined(_WIN32) || defined(MALENA_HAVE_X11)
    return true;
#else
    return false;
#endif
}

std::optional<sf::Image> ScreenSource::grab()
{
    _impl->error.clear();

#if defined(__APPLE__)
    // Pick the display by index from the active list; fall back to main.
    CGDirectDisplayID ids[16] = {};
    uint32_t count = 0;
    CGGetActiveDisplayList(16, ids, &count);
    const CGDirectDisplayID did =
        (count > 0 && _impl->display < static_cast<int>(count)) ? ids[_impl->display]
                                                                : CGMainDisplayID();

    CGImageRef img = CGDisplayCreateImage(did);
    if (!img) { _impl->error = "CGDisplayCreateImage failed (Screen Recording permission?)"; return std::nullopt; }

    const std::size_t w = CGImageGetWidth(img);
    const std::size_t h = CGImageGetHeight(img);
    std::vector<std::uint8_t> rgba(w * h * 4);

    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    // kCGImageAlphaPremultipliedLast + ByteOrderDefault gives us RGBA directly.
    CGContextRef ctx = CGBitmapContextCreate(rgba.data(), w, h, 8, w * 4, cs,
                                             kCGImageAlphaPremultipliedLast);
    if (ctx)
    {
        CGContextDrawImage(ctx, CGRectMake(0, 0, static_cast<CGFloat>(w), static_cast<CGFloat>(h)), img);
        CGContextRelease(ctx);
    }
    else _impl->error = "CGBitmapContextCreate failed";
    CGColorSpaceRelease(cs);
    CGImageRelease(img);
    if (!_impl->error.empty()) return std::nullopt;

    sf::Image frame(sf::Vector2u{static_cast<unsigned>(w), static_cast<unsigned>(h)}, rgba.data());
    return Impl::downscale(frame, _impl->maxDim);

#elif defined(_WIN32)
    HDC screen = GetDC(nullptr);
    if (!screen) { _impl->error = "GetDC failed"; return std::nullopt; }
    const int w = GetDeviceCaps(screen, HORZRES);
    const int h = GetDeviceCaps(screen, VERTRES);

    HDC     mem = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateCompatibleBitmap(screen, w, h);
    HGDIOBJ old = SelectObject(mem, bmp);
    BitBlt(mem, 0, 0, w, h, screen, 0, 0, SRCCOPY);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;          // negative = top-down
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    std::vector<std::uint8_t> px(static_cast<std::size_t>(w) * h * 4);
    const int got = GetDIBits(mem, bmp, 0, h, px.data(), &bi, DIB_RGB_COLORS);

    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    if (got == 0) { _impl->error = "GetDIBits failed"; return std::nullopt; }

    // GDI hands back BGRA; SFML wants RGBA. Alpha from BitBlt is meaningless.
    for (std::size_t i = 0; i + 3 < px.size(); i += 4)
    {
        std::swap(px[i], px[i + 2]);
        px[i + 3] = 255;
    }
    sf::Image frame(sf::Vector2u{static_cast<unsigned>(w), static_cast<unsigned>(h)}, px.data());
    return Impl::downscale(frame, _impl->maxDim);

#elif defined(MALENA_HAVE_X11)
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) { _impl->error = "XOpenDisplay failed (no DISPLAY?)"; return std::nullopt; }
    Window root = DefaultRootWindow(dpy);
    XWindowAttributes attr{};
    XGetWindowAttributes(dpy, root, &attr);

    XImage* xi = XGetImage(dpy, root, 0, 0, attr.width, attr.height, AllPlanes, ZPixmap);
    if (!xi) { XCloseDisplay(dpy); _impl->error = "XGetImage failed"; return std::nullopt; }

    std::vector<std::uint8_t> px(static_cast<std::size_t>(attr.width) * attr.height * 4);
    for (int y = 0; y < attr.height; ++y)
        for (int x = 0; x < attr.width; ++x)
        {
            const unsigned long p = XGetPixel(xi, x, y);
            const std::size_t o = (static_cast<std::size_t>(y) * attr.width + x) * 4;
            px[o + 0] = static_cast<std::uint8_t>((p & xi->red_mask)   >> 16);
            px[o + 1] = static_cast<std::uint8_t>((p & xi->green_mask) >> 8);
            px[o + 2] = static_cast<std::uint8_t>((p & xi->blue_mask));
            px[o + 3] = 255;
        }
    XDestroyImage(xi);
    XCloseDisplay(dpy);

    sf::Image frame(sf::Vector2u{static_cast<unsigned>(attr.width), static_cast<unsigned>(attr.height)}, px.data());
    return Impl::downscale(frame, _impl->maxDim);

#else
    _impl->error = "no screen-capture backend on this platform";
    return std::nullopt;
#endif
}

std::optional<std::vector<std::uint8_t>> ScreenSource::encodeJpeg(const sf::Image& frame)
{
    return frame.saveToMemory("jpg");
}

std::optional<sf::Image> ScreenSource::decode(const std::uint8_t* data, std::size_t size)
{
    sf::Image img;
    if (!data || size == 0) return std::nullopt;
    if (!img.loadFromMemory(data, size)) return std::nullopt;
    return img;
}

} // namespace ml
