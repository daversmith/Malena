// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// ShaderLibrary.h
//

#ifndef SHADERLIBRARY_H
#define SHADERLIBRARY_H

#include <Malena/Core/malena_export.h>
#include <string_view>
#include <SFML/Graphics.hpp>
#include <functional>

namespace ml
{
    /**
     * @brief Built-in GLSL shaders and uniform parameters for @c Carousel effects.
      * @ingroup Utilities
     *
     * @c ShaderLibrary provides a catalogue of pre-written fragment shaders
     * that can be applied to a @c Carousel's render texture, along with a
     * @c Params struct that centralises every uniform value the shaders accept.
     *
     * ### Selecting a style
     * Pass a @c Style value to @c Carousel::setStyle():
     * @code
     * carousel.setStyle(ml::ShaderLibrary::Style::COVERFLOW_3D);
     * @endcode
     *
     * ### Tuning parameters
     * Retrieve and modify the carousel's @c Params, then the shader will
     * pick up the new values on the next frame:
     * @code
     * auto& p = carousel.getShaderParams();
     * p.blurAmount  = 4.f;
     * p.focusWidth  = 0.15f;
     * @endcode
     *
     * ### Custom shader
     * Use @c Style::CUSTOM and supply your own shader via
     * @c Carousel::setShader():
     * @code
     * carousel.setStyle(ml::ShaderLibrary::Style::CUSTOM);
     * carousel.setShader(&myShader, [](sf::Shader& s){
     *     s.setUniform("uTime", clock.getElapsedTime().asSeconds());
     * });
     * @endcode
     *
     * @see Carousel, CarouselManifest
     */
    class MALENA_API ShaderLibrary
    {
    public:
        /**
         * @brief All uniform parameters shared across the built-in shaders.
         *
         * Not every shader uses every field. Parameters that are irrelevant
         * to the active style are simply ignored. @c textureSize and
         * @c totalWidth are updated automatically by @c Carousel before
         * the shader is applied.
         */
        struct Params
        {
            // ── Position / center ────────────────────────────────────────────
            float centerX = 0.5f;     ///< Normalized horizontal center (0–1).
            float centerY = 0.5f;     ///< Normalized vertical center (0–1).

            // ── Scale ────────────────────────────────────────────────────────
            float minScale   = 0.6f;  ///< Minimum scale applied to off-center items.
            float maxScale   = 1.5f;  ///< Maximum scale applied to the center item.
            float scaleWidth = 0.3f;  ///< Normalized width of the scale transition zone.

            // ── Blur ─────────────────────────────────────────────────────────
            float blurAmount = 2.0f;  ///< Blur radius in pixels for unfocused items.
            float focusWidth = 0.2f;  ///< Normalized width of the in-focus zone.

            // ── Color ─────────────────────────────────────────────────────────
            sf::Glsl::Vec3 tintColor    = {1.0f, 0.8f, 0.6f}; ///< RGB tint color.
            sf::Glsl::Vec3 glowColor    = {1.0f, 1.0f, 0.8f}; ///< RGB glow color.
            float          tintStrength = 0.3f; ///< Tint blend factor (0 = no tint, 1 = full).
            float          glowStrength = 0.3f; ///< Glow intensity factor.

            // ── Fade / vignette ───────────────────────────────────────────────
            float fadeWidth = 50.0f;  ///< Edge fade width in pixels.
            float strength  = 0.5f;   ///< General effect strength multiplier.
            float radius    = 0.8f;   ///< Vignette radius (normalized, 0–1).

            // ── 3D / perspective ──────────────────────────────────────────────
            float perspective    = 0.8f; ///< Perspective distortion amount.
            float rotationAngle  = 0.5f; ///< 3D rotation angle in radians.
            float skewAmount     = 0.3f; ///< Horizontal skew factor.

            // ── Animation ─────────────────────────────────────────────────────
            float time        = 0.0f;  ///< Elapsed time in seconds (updated per frame).
            float amplitude   = 0.02f; ///< Wave distortion amplitude (normalized).
            float frequency   = 10.0f; ///< Wave distortion frequency.
            float pulseSpeed  = 2.0f;  ///< Pulse animation speed in cycles per second.
            float pulseAmount = 0.05f; ///< Pulse scale magnitude.

            // ── Miscellaneous ─────────────────────────────────────────────────
            float parallaxStrength  = 0.1f;  ///< Parallax scroll factor.
            float reflectionHeight  = 0.5f;  ///< Reflection zone height (normalized).
            float reflectionAlpha   = 0.4f;  ///< Reflection opacity.
            float colorWidth        = 0.25f; ///< Colored band width (normalized).
            float intensity         = 0.8f;  ///< General intensity multiplier.
            float glowWidth         = 0.3f;  ///< Glow falloff width (normalized).

            // ── Auto-updated by Carousel ──────────────────────────────────────
            sf::Vector2f textureSize = {720.0f, 480.0f}; ///< Render texture dimensions in pixels.
            float        totalWidth  = 720.0f;            ///< Total carousel width in pixels.
        };

        /**
         * @brief Available built-in visual styles.
         *
         * | Value | Effect |
         * |-------|--------|
         * | @c DEFAULT          | No shader — plain rendering |
         * | @c FADE_EDGES       | Alpha fade toward left and right edges |
         * | @c BLUR_UNFOCUSED   | Blur items away from the center |
         * | @c COVERFLOW_3D     | CoverFlow-style 3D perspective tilt |
         * | @c COLOR_TINT       | Apply a color tint to off-center items |
         * | @c WAVE_DISTORTION  | Sinusoidal distortion animation |
         * | @c DEPTH_OF_FIELD   | Depth-of-field blur with focus zone |
         * | @c SCALE_CENTER     | Scale up the center item |
         * | @c VIGNETTE         | Dark vignette around the edges |
         * | @c GRAYSCALE_EDGES  | Desaturate items away from center |
         * | @c REFLECTION       | Mirror reflection below items |
         * | @c ROTATION_3D      | Full 3D rotation effect |
         * | @c PARALLAX         | Parallax scroll layer offset |
         * | @c GLOW_CENTER      | Glow halo on the center item |
         * | @c SLIDE_ANGLE      | Angled slide transition |
         * | @c SEPIA_TONE       | Sepia color grading |
         * | @c ZOOM_PULSE       | Pulsing zoom animation |
         * | @c CUSTOM           | User-supplied shader via @c Carousel::setShader() |
         */
        enum class Style
        {
            DEFAULT,
            FADE_EDGES,
            BLUR_UNFOCUSED,
            COVERFLOW_3D,
            COLOR_TINT,
            WAVE_DISTORTION,
            DEPTH_OF_FIELD,
            SCALE_CENTER,
            VIGNETTE,
            GRAYSCALE_EDGES,
            REFLECTION,
            ROTATION_3D,
            PARALLAX,
            GLOW_CENTER,
            SLIDE_ANGLE,
            SEPIA_TONE,
            ZOOM_PULSE,
            CUSTOM
        };

        // ── GLSL source getters ───────────────────────────────────────────────

        /// @brief Return the GLSL source for the @c FADE_EDGES style.
        static std::string_view fadeEdges();
        /// @brief Return the GLSL source for the @c SCALE_CENTER style.
        static std::string_view scaleCenter();
        /// @brief Return the GLSL source for the @c BLUR_UNFOCUSED style.
        static std::string_view blurUnfocused();
        /// @brief Return the GLSL source for the @c COVERFLOW_3D style.
        static std::string_view coverflow3D();
        /// @brief Return the GLSL source for the @c COLOR_TINT style.
        static std::string_view colorTint();
        /// @brief Return the GLSL source for the @c WAVE_DISTORTION style.
        static std::string_view waveDistortion();
        /// @brief Return the GLSL source for the @c DEPTH_OF_FIELD style.
        static std::string_view depthOfField();
        /// @brief Return the GLSL source for the @c VIGNETTE style.
        static std::string_view vignette();
        /// @brief Return the GLSL source for the @c GRAYSCALE_EDGES style.
        static std::string_view grayscaleEdges();
        /// @brief Return the GLSL source for the @c REFLECTION style.
        static std::string_view reflection();
        /// @brief Return the GLSL source for the @c ROTATION_3D style.
        static std::string_view rotation3D();
        /// @brief Return the GLSL source for the @c PARALLAX style.
        static std::string_view parallax();
        /// @brief Return the GLSL source for the @c GLOW_CENTER style.
        static std::string_view glowCenter();
        /// @brief Return the GLSL source for the @c SLIDE_ANGLE style.
        static std::string_view slideAngle();
        /// @brief Return the GLSL source for the @c SEPIA_TONE style.
        static std::string_view sepiaTone();
        /// @brief Return the GLSL source for the @c ZOOM_PULSE style.
        static std::string_view zoomPulse();

        /**
         * @brief Push the appropriate @c Params fields as uniforms to @p shader.
         *
         * Called by @c Carousel once per frame before rendering. Only the
         * uniforms relevant to @p style are set — unused fields are ignored.
         *
         * @param shader The active @c sf::Shader to update.
         * @param style  The currently selected @c Style.
         * @param params The parameter values to upload.
         */
        static void applyUniforms(sf::Shader& shader, Style style, const Params& params);
    };

} // namespace ml

#endif // SHADERLIBRARY_H
