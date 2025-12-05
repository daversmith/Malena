//
// ShaderLibrary.h
//

#ifndef SHADERLIBRARY_H
#define SHADERLIBRARY_H

#include <string_view>
#include <SFML/Graphics.hpp>
#include <functional>

namespace ml
{
	class ShaderLibrary
	{
	public:
	    struct Params {
	        // Position/Center parameters
	        float centerX = 0.5f;
	        float centerY = 0.5f;

	        // Scale parameters
	        float minScale = 0.6f;
	        float maxScale = 1.5f;
	        float scaleWidth = 0.3f;

	        // Blur parameters
	        float blurAmount = 2.0f;
	        float focusWidth = 0.2f;

	        // Color parameters
	        sf::Glsl::Vec3 tintColor = {1.0f, 0.8f, 0.6f};
	        sf::Glsl::Vec3 glowColor = {1.0f, 1.0f, 0.8f};
	        float tintStrength = 0.3f;
	        float glowStrength = 0.3f;

	        // Fade/Vignette parameters
	        float fadeWidth = 50.0f;
	        float strength = 0.5f;
	        float radius = 0.8f;

	        // 3D/Perspective parameters
	        float perspective = 0.8f;
	        float rotationAngle = 0.5f;
	        float skewAmount = 0.3f;

	        // Animation parameters
	        float time = 0.0f;
	        float amplitude = 0.02f;
	        float frequency = 10.0f;
	        float pulseSpeed = 2.0f;
	        float pulseAmount = 0.05f;

	        // Other parameters
	        float parallaxStrength = 0.1f;
	        float reflectionHeight = 0.5f;
	        float reflectionAlpha = 0.4f;
	        float colorWidth = 0.25f;
	        float intensity = 0.8f;
	        float glowWidth = 0.3f;

	        // Texture size (updated automatically)
	        sf::Vector2f textureSize = {720.0f, 480.0f};
	        float totalWidth = 720.0f;
	    };
	    enum class Style {
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

	    // Shader code getters
	    static std::string_view fadeEdges();
	    static std::string_view scaleCenter();
	    static std::string_view blurUnfocused();
	    static std::string_view coverflow3D();
	    static std::string_view colorTint();
	    static std::string_view waveDistortion();
	    static std::string_view depthOfField();
	    static std::string_view vignette();
	    static std::string_view grayscaleEdges();
	    static std::string_view reflection();
	    static std::string_view rotation3D();
	    static std::string_view parallax();
	    static std::string_view glowCenter();
	    static std::string_view slideAngle();
	    static std::string_view sepiaTone();
	    static std::string_view zoomPulse();
		static void applyUniforms(sf::Shader& shader, Style style, const Params& params);
	};
}
#endif // SHADERLIBRARY_H