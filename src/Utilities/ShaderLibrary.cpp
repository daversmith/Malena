//
// Created by Dave Smith on 12/3/25.
//

#include <Malena/Utilities/ShaderLibrary.h>
namespace ml
{
    std::string_view ShaderLibrary::fadeEdges()
	 {
        return R"(
            uniform sampler2D texture;
            uniform float fadeWidth;
            uniform float totalWidth;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;
                vec4 color = texture2D(texture, pos);

                float x = pos.x * totalWidth;
                float alpha = 1.0;

                if (x < fadeWidth) {
                    alpha = smoothstep(0.0, fadeWidth, x);
                }
                if (x > totalWidth - fadeWidth) {
                    alpha = smoothstep(totalWidth, totalWidth - fadeWidth, x);
                }

                gl_FragColor = color * vec4(1.0, 1.0, 1.0, alpha);
            }
        )";
    }

    std::string_view ShaderLibrary::scaleCenter() {
        return R"(
            uniform sampler2D texture;
            uniform float centerX;
            uniform float minScale;
            uniform float scaleWidth;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;
                float dist = abs(pos.x - centerX);
                float scale = 1.0;
                if (dist < scaleWidth) {
                    scale = minScale + (1.0 - minScale) * (1.0 - dist / scaleWidth);
                } else {
                    scale = minScale;
                }
                vec2 center = vec2(centerX, 0.5);
                vec2 toCenter = pos - center;
                vec2 scaled = center + toCenter / scale;

                if (scaled.x < 0.0 || scaled.x > 1.0 || scaled.y < 0.0 || scaled.y > 1.0) {
                    gl_FragColor = vec4(0.0);
                } else {
                    gl_FragColor = texture2D(texture, scaled);
                }
            }
        )";
    }

    std::string_view ShaderLibrary::blurUnfocused() {
        return R"(
            uniform sampler2D texture;
            uniform float blurAmount;
            uniform vec2 textureSize;
            uniform float centerX;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;
                vec4 color = vec4(0.0);

                float dist = abs(pos.x - centerX);
                float blur = blurAmount * dist * 2.0;

                int samples = int(blur * 5.0) + 1;
                float total = 0.0;

                for (int x = -samples; x <= samples; x++) {
                    for (int y = -samples; y <= samples; y++) {
                        vec2 offset = vec2(float(x), float(y)) * blur / textureSize;
                        float weight = exp(-float(x*x + y*y) / (2.0 * blur * blur));
                        color += texture2D(texture, pos + offset) * weight;
                        total += weight;
                    }
                }

                gl_FragColor = color / total;
            }
        )";
    }

    std::string_view ShaderLibrary::coverflow3D() {
        return R"(
            uniform sampler2D texture;
            uniform float centerX;
            uniform float perspective;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;

                float dist = (pos.x - centerX) * 2.0;
                float angle = dist * perspective;
                float scale = cos(angle);

                float newX = centerX + (pos.x - centerX) * scale;
                float lighting = 0.7 + 0.3 * cos(angle);

                if (newX < 0.0 || newX > 1.0) {
                    gl_FragColor = vec4(0.0);
                } else {
                    vec4 color = texture2D(texture, vec2(newX, pos.y));
                    gl_FragColor = color * vec4(lighting, lighting, lighting, 1.0);
                }
            }
        )";
    }

    std::string_view ShaderLibrary::colorTint() {
        return R"(
            uniform sampler2D texture;
            uniform vec3 tintColor;
            uniform float tintStrength;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;
                vec4 color = texture2D(texture, pos);

                vec3 tinted = mix(color.rgb, color.rgb * tintColor, tintStrength);

                gl_FragColor = vec4(tinted, color.a);
            }
        )";
    }

    std::string_view ShaderLibrary::waveDistortion() {
        return R"(
            uniform sampler2D texture;
            uniform float time;
            uniform float amplitude;
            uniform float frequency;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;

                float wave = sin(pos.x * frequency + time) * amplitude;
                vec2 distorted = vec2(pos.x, pos.y + wave);

                if (distorted.y < 0.0 || distorted.y > 1.0) {
                    gl_FragColor = vec4(0.0);
                } else {
                    gl_FragColor = texture2D(texture, distorted);
                }
            }
        )";
    }

    std::string_view ShaderLibrary::depthOfField() {
        return R"(
            uniform sampler2D texture;
            uniform vec2 textureSize;
            uniform float centerX;
            uniform float focusWidth;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;
                vec4 color = vec4(0.0);

                float dist = abs(pos.x - centerX);
                float blur = 0.0;

                if (dist > focusWidth) {
                    blur = (dist - focusWidth) * 3.0;
                }

                if (blur < 0.01) {
                    gl_FragColor = texture2D(texture, pos);
                } else {
                    int samples = 8;
                    float total = 0.0;

                    for (int x = -samples; x <= samples; x++) {
                        for (int y = -samples; y <= samples; y++) {
                            vec2 offset = vec2(float(x), float(y)) * blur / textureSize;
                            float weight = 1.0;
                            color += texture2D(texture, pos + offset) * weight;
                            total += weight;
                        }
                    }

                    gl_FragColor = color / total;
                }
            }
        )";
    }

    std::string_view ShaderLibrary::vignette() {
        return R"(
            uniform sampler2D texture;
            uniform float strength;
            uniform float radius;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;
                vec4 color = texture2D(texture, pos);

                vec2 center = vec2(0.5, 0.5);
                float dist = length(pos - center);

                float vignette = smoothstep(radius, radius - 0.3, dist);
                vignette = mix(1.0 - strength, 1.0, vignette);

                gl_FragColor = color * vec4(vignette, vignette, vignette, 1.0);
            }
        )";
    }

    std::string_view ShaderLibrary::grayscaleEdges() {
        return R"(
            uniform sampler2D texture;
            uniform float centerX;
            uniform float colorWidth;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;
                vec4 color = texture2D(texture, pos);

                float dist = abs(pos.x - centerX);

                float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
                vec3 grayColor = vec3(gray);

                float mixFactor = smoothstep(colorWidth, 0.0, dist);
                vec3 finalColor = mix(grayColor, color.rgb, mixFactor);

                gl_FragColor = vec4(finalColor, color.a);
            }
        )";
    }

    std::string_view ShaderLibrary::reflection() {
        return R"(
            uniform sampler2D texture;
            uniform float reflectionHeight;
            uniform float reflectionAlpha;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;

                if (pos.y > 0.5) {
                    vec2 reflectPos = vec2(pos.x, 1.0 - pos.y);
                    vec4 color = texture2D(texture, reflectPos);

                    float fade = (pos.y - 0.5) * 2.0;
                    fade = 1.0 - fade * fade;

                    gl_FragColor = color * vec4(1.0, 1.0, 1.0, fade * reflectionAlpha);
                } else {
                    gl_FragColor = texture2D(texture, pos);
                }
            }
        )";
    }

    std::string_view ShaderLibrary::rotation3D() {
        return R"(
            uniform sampler2D texture;
            uniform float centerX;
            uniform float rotationAngle;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;

                float dist = (pos.x - centerX) * 2.0;
                float angle = dist * rotationAngle;

                float cosAngle = cos(angle);
                vec2 center = vec2(centerX, 0.5);
                vec2 toCenter = pos - center;

                float newX = center.x + toCenter.x * cosAngle;
                float perspective = 0.7 + 0.3 * abs(cosAngle);

                if (newX < 0.0 || newX > 1.0) {
                    gl_FragColor = vec4(0.0);
                } else {
                    vec4 color = texture2D(texture, vec2(newX, pos.y));
                    gl_FragColor = color * vec4(perspective, perspective, perspective, 1.0);
                }
            }
        )";
    }

    std::string_view ShaderLibrary::parallax() {
        return R"(
            uniform sampler2D texture;
            uniform float centerX;
            uniform float parallaxStrength;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;

                float dist = pos.x - centerX;
                float offset = dist * parallaxStrength;

                vec2 parallaxPos = vec2(pos.x + offset, pos.y);

                if (parallaxPos.x < 0.0 || parallaxPos.x > 1.0) {
                    gl_FragColor = vec4(0.0);
                } else {
                    gl_FragColor = texture2D(texture, parallaxPos);
                }
            }
        )";
    }

    std::string_view ShaderLibrary::glowCenter() {
        return R"(
            uniform sampler2D texture;
            uniform float centerX;
            uniform float glowWidth;
            uniform vec3 glowColor;
            uniform float glowStrength;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;
                vec4 color = texture2D(texture, pos);

                float dist = abs(pos.x - centerX);
                float glow = 1.0 - smoothstep(0.0, glowWidth, dist);
                glow = pow(glow, 2.0) * glowStrength;

                vec3 glowEffect = color.rgb + glowColor * glow;

                gl_FragColor = vec4(glowEffect, color.a);
            }
        )";
    }

    std::string_view ShaderLibrary::slideAngle() {
        return R"(
            uniform sampler2D texture;
            uniform float centerX;
            uniform float skewAmount;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;

                float dist = (pos.x - centerX);
                float skew = pos.y * dist * skewAmount;

                vec2 skewedPos = vec2(pos.x + skew, pos.y);

                if (skewedPos.x < 0.0 || skewedPos.x > 1.0) {
                    gl_FragColor = vec4(0.0);
                } else {
                    gl_FragColor = texture2D(texture, skewedPos);
                }
            }
        )";
    }

    std::string_view ShaderLibrary::sepiaTone() {
        return R"(
            uniform sampler2D texture;
            uniform float intensity;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;
                vec4 color = texture2D(texture, pos);

                vec3 sepia;
                sepia.r = dot(color.rgb, vec3(0.393, 0.769, 0.189));
                sepia.g = dot(color.rgb, vec3(0.349, 0.686, 0.168));
                sepia.b = dot(color.rgb, vec3(0.272, 0.534, 0.131));

                vec3 final = mix(color.rgb, sepia, intensity);

                gl_FragColor = vec4(final, color.a);
            }
        )";
    }

    std::string_view ShaderLibrary::zoomPulse() {
        return R"(
            uniform sampler2D texture;
            uniform float time;
            uniform float pulseSpeed;
            uniform float pulseAmount;

            void main() {
                vec2 pos = gl_TexCoord[0].xy;

                float pulse = sin(time * pulseSpeed) * pulseAmount;
                float scale = 1.0 + pulse;

                vec2 center = vec2(0.5, 0.5);
                vec2 toCenter = pos - center;
                vec2 scaled = center + toCenter / scale;

                if (scaled.x < 0.0 || scaled.x > 1.0 || scaled.y < 0.0 || scaled.y > 1.0) {
                    gl_FragColor = vec4(0.0);
                } else {
                    gl_FragColor = texture2D(texture, scaled);
                }
            }
        )";
    }
	void ShaderLibrary::applyUniforms(sf::Shader& shader, Style style, const Params& params) {
        switch (style) {
            case Style::FADE_EDGES:
                shader.setUniform("fadeWidth", params.fadeWidth);
                shader.setUniform("totalWidth", params.totalWidth);
                break;

            case Style::SCALE_CENTER:
                shader.setUniform("centerX", params.centerX);
                shader.setUniform("minScale", params.minScale);
                shader.setUniform("scaleWidth", params.scaleWidth);
                break;

            case Style::BLUR_UNFOCUSED:
                shader.setUniform("blurAmount", params.blurAmount);
                shader.setUniform("textureSize", params.textureSize);
                shader.setUniform("centerX", params.centerX);
                break;

            case Style::COVERFLOW_3D:
                shader.setUniform("centerX", params.centerX);
                shader.setUniform("perspective", params.perspective);
                break;

            case Style::COLOR_TINT:
                shader.setUniform("tintColor", params.tintColor);
                shader.setUniform("tintStrength", params.tintStrength);
                break;

            case Style::WAVE_DISTORTION:
                shader.setUniform("time", params.time);
                shader.setUniform("amplitude", params.amplitude);
                shader.setUniform("frequency", params.frequency);
                break;

            case Style::DEPTH_OF_FIELD:
                shader.setUniform("textureSize", params.textureSize);
                shader.setUniform("centerX", params.centerX);
                shader.setUniform("focusWidth", params.focusWidth);
                break;

            case Style::VIGNETTE:
                shader.setUniform("strength", params.strength);
                shader.setUniform("radius", params.radius);
                break;

            case Style::GRAYSCALE_EDGES:
                shader.setUniform("centerX", params.centerX);
                shader.setUniform("colorWidth", params.colorWidth);
                break;

            case Style::REFLECTION:
                shader.setUniform("reflectionHeight", params.reflectionHeight);
                shader.setUniform("reflectionAlpha", params.reflectionAlpha);
                break;

            case Style::ROTATION_3D:
                shader.setUniform("centerX", params.centerX);
                shader.setUniform("rotationAngle", params.rotationAngle);
                break;

            case Style::PARALLAX:
                shader.setUniform("centerX", params.centerX);
                shader.setUniform("parallaxStrength", params.parallaxStrength);
                break;

            case Style::GLOW_CENTER:
                shader.setUniform("centerX", params.centerX);
                shader.setUniform("glowWidth", params.glowWidth);
                shader.setUniform("glowColor", params.glowColor);
                shader.setUniform("glowStrength", params.glowStrength);
                break;

            case Style::SLIDE_ANGLE:
                shader.setUniform("centerX", params.centerX);
                shader.setUniform("skewAmount", params.skewAmount);
                break;

            case Style::SEPIA_TONE:
                shader.setUniform("intensity", params.intensity);
                break;

            case Style::ZOOM_PULSE:
                shader.setUniform("time", params.time);
                shader.setUniform("pulseSpeed", params.pulseSpeed);
                shader.setUniform("pulseAmount", params.pulseAmount);
                break;

            default:
                break;
        }
    }
}