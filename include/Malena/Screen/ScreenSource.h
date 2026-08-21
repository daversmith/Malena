// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#pragma once
#ifndef MALENA_SCREEN_SOURCE_H
#define MALENA_SCREEN_SOURCE_H

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics/Image.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ml {

/**
 * @brief Grabs the desktop as pixels, using the OS's own capture API.
 *
 * This is the capture half of screen sharing with no third-party media runtime:
 * no GStreamer, nothing for the user to install. Each platform uses its native
 * call (CGDisplayCreateImage on macOS, BitBlt on Windows, XGetImage on X11), and
 * the result is handed back as a plain sf::Image so the rest of the pipeline is
 * platform-agnostic.
 *
 * Deliberately a *pull* API: the caller decides the frame rate by how often it
 * calls grab(). A shared screen only needs a handful of frames per second, and
 * pulling keeps the cost proportional to what is actually sent.
 *
 * @code
 * ml::ScreenSource src;
 * if (auto frame = src.grab())
 *     if (auto jpeg = ml::ScreenSource::encodeJpeg(*frame))
 *         send(*jpeg);
 * @endcode
 */
class MALENA_API ScreenSource
{
public:
    ScreenSource();
    ~ScreenSource();

    ScreenSource(const ScreenSource&)            = delete;
    ScreenSource& operator=(const ScreenSource&) = delete;

    /** Which display to capture (0 = main). Out-of-range falls back to main. */
    void setDisplayIndex(int index);
    int  displayIndex() const;

    /**
     * Cap the long edge of captured frames, preserving aspect (0 = no scaling).
     * A Retina desktop is enormous; downscaling before encoding is the single
     * biggest win for both bandwidth and encode time.
     */
    void setMaxDimension(unsigned int pixels);

    /** Grab one frame, or nullopt if capture failed / is unavailable here. */
    std::optional<sf::Image> grab();

    /** True when this platform has a capture backend compiled in. */
    static bool isSupported();

    /** Human-readable reason capture is unavailable, or "" when it works. */
    const std::string& lastError() const;

    /**
     * Encode to JPEG in memory. Uses SFML's own encoder, so it needs no extra
     * dependency; quality is fixed by SFML. Returns nullopt on failure.
     */
    static std::optional<std::vector<std::uint8_t>> encodeJpeg(const sf::Image& frame);

    /** Decode a frame produced by encodeJpeg (or any format SFML reads). */
    static std::optional<sf::Image> decode(const std::uint8_t* data, std::size_t size);

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace ml

#endif // MALENA_SCREEN_SOURCE_H
