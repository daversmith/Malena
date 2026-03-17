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
     * @brief Lightweight metadata record for a discovered plugin.
     *
     * @c PluginInfo is populated by @c PluginManager::scanPlugins() when it
     * probes a directory for plugin shared libraries. Each record holds
     * everything the host application needs to display a plugin in a carousel
     * or menu — name, version, thumbnail image, and the path needed to load
     * it — without keeping the plugin's dylib open or the @c Plugin object
     * alive.
     *
     * ### Thumbnail ownership
     * The thumbnail texture is @b copied from the GPU to a CPU-side
     * @c sf::Texture before the probe plugin is unloaded. This means the
     * texture pixel data in @c PluginInfo remains valid after the dylib is
     * closed, and can be safely used to render a preview image without
     * re-loading the plugin.
     *
     * @c thumbnail is @c std::nullopt when the plugin's manifest does not
     * declare @c Images::THUMBNAIL, or when the texture fails to load.
     *
     * ### Typical use
     * @code
     * std::vector<ml::PluginInfo> infos =
     *     pluginManager.scanPlugins("path/to/plugins");
     *
     * for (const auto& info : infos) {
     *     std::cout << info.name << " v" << info.version << "\n";
     *     if (info.thumbnail)
     *         sprite.setTexture(*info.thumbnail);
     * }
     *
     * // Load a specific plugin when the user selects it
     * pluginManager.loadPlugin(infos[0].path);
     * @endcode
     *
     * @see PluginManager::scanPlugins, Plugin::getThumbnail
     */
    struct PluginInfo
    {
        std::string                path;      ///< Absolute path to the plugin shared library.
        std::string                name;      ///< Display name, from @c Plugin::getName().
        std::string                version;   ///< Version string, from @c Plugin::getVersion().
        std::optional<sf::Texture> thumbnail; ///< CPU-side copy of the plugin thumbnail,
                                              ///< or @c std::nullopt if unavailable.
    };

} // namespace ml
