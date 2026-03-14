//
// PluginInfo.h
//

#pragma once
#include <string>
#include <optional>
#include <SFML/Graphics/Texture.hpp>

namespace ml
{
    /**
     * @brief Metadata about a plugin gathered during scanPlugins().
     * Thumbnail is copied from GPU to CPU before the plugin unloads,
     * so it remains valid after the plugin dylib is closed.
     */
    struct PluginInfo
    {
        std::string                path;       // absolute path to the dylib
        std::string                name;       // from Plugin::getName()
        std::string                version;    // from Plugin::getVersion()
        std::optional<sf::Texture> thumbnail;  // copied pixel data, empty if no THUMBNAIL
    };

} // namespace ml