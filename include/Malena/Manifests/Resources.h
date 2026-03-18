//
// Created by Dave Smith on 3/7/26.
//

#ifndef RESOURCES_H
#define RESOURCES_H

#include <Malena/Manifests/Context.h>
#include <Malena/Manifests/ManifestAliases.h>

namespace ml
{
    /**
     * @brief Top-level manifest convenience base combining @c Context and
     *        @c ManifestAliases.
     *
     * @c Resources<Manifest> is a thin struct that inherits both
     * @c Context<Manifest> (which provides @c AssetMgr, @c ConfigMgr, and
     * @c StateMgr type aliases) and @c ManifestAliases<Manifest> (which
     * injects the manifest's @c Images, @c Fonts, @c Sounds, @c State, and
     * @c Flags aliases directly into scope).
     *
     * In practice @c Context already inherits @c ManifestAliases, so
     * @c Resources and @c Context expose the same surface. @c Resources
     * exists as a semantic alias that reads more naturally as a base class
     * name when the intent is "give this class everything it needs to access
     * its manifest":
     *
     * @code
     * class MyPlugin : public ml::PluginWith<MyManifest>,
     *                  public ml::Resources<MyManifest>
     * {
     *     void onLoad() override
     *     {
     *         // Manager aliases from Context
     *         auto& tex = AssetMgr::get(Images::Background);
     *
     *         // Enum aliases from ManifestAliases — no qualification needed
     *         auto& fnt = AssetMgr::get(Fonts::Main);
     *     }
     * };
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass.
     *
     * @see Context, ManifestAliases, AssetsManager, ConfigManager
     */
    template<typename Manifest>
    /**
     * @brief Resources.
     * @ingroup Manifests
     */
    struct Resources : public Context<Manifest>, public ManifestAliases<Manifest>
    {};

} // namespace ml

#endif // RESOURCES_H
