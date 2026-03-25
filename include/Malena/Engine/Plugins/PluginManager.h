// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// PluginManager.h
//

#ifndef PLUGINHOST_H
#define PLUGINHOST_H

#include <Malena/Engine/App/Application.h>
#include <Malena/Engine/Plugins/Plugin.h>
#include <Malena/Core/DeferredOperationsManager.h>
#include <Malena/Engine/Plugins/PluginInfo.h>

#ifdef _WIN32
    #include <windows.h>
    #define LIB_HANDLE HMODULE
    #define LOAD_LIB(path)        LoadLibraryA(path)
    #define GET_FUNC(handle,name) GetProcAddress(handle, name)
    #define CLOSE_LIB(handle)     FreeLibrary(static_cast<HMODULE>(handle))
#else
    #include <dlfcn.h>
    #define LIB_HANDLE void*
    #define LOAD_LIB(path)        dlopen(path, RTLD_NOW | RTLD_GLOBAL)
    #define GET_FUNC(handle,name) dlsym(handle, name)
    #define CLOSE_LIB(handle)     dlclose(handle)
#endif

namespace fs = std::filesystem;

namespace ml
{
    /**
     * @brief Loads, manages, and safely unloads dynamically linked plugins.
      * @ingroup EnginePlugins
     *
     * @c PluginManager handles the full plugin lifecycle:
     *
     * 1. **Load** — @c loadPlugin opens a shared library, locates the
     *    @c createPlugin factory (exported by @c REGISTER_PLUGIN), constructs
     *    the plugin, and calls @c Plugin::onLoad().
     * 2. **Query** — @c getPlugins returns all active plugins; type-safe
     *    interface queries should use @c Plugin::getIf<T>().
     * 3. **Scan** — @c scanPlugins probes a directory for plugin libraries
     *    without keeping them loaded, returning @c PluginInfo records
     *    (including CPU-side thumbnail copies) for display in a carousel.
     * 4. **Unload** — @c unloadPlugin calls @c Plugin::onUnload(),
     *    force-unsubscribes all event and message subscriptions, deletes the
     *    plugin, and closes the library handle.
     *
     * ### Deferred unloading
     * @c PluginManager inherits @c DeferredOperationsManager<PluginManager>.
     * Calling @c unloadPlugin from within a message or event callback is safe —
     * the actual unload is deferred until the current iteration completes,
     * preventing use-after-free crashes.
     *
     * ### Cross-platform support
     * The @c LOAD_LIB, @c GET_FUNC, and @c CLOSE_LIB macros abstract the
     * platform differences between @c dlopen (macOS/Linux) and
     * @c LoadLibraryA (Windows).
     *
     * ### Typical use
     * @code
     * // Load all plugins from a directory
     * pluginManager.loadPluginsFromDirectory("path/to/plugins");
     *
     * // Query each plugin for an optional interface
     * for (auto* plugin : pluginManager.getPlugins()) {
     *     if (auto* ui = plugin->getIf<ml::UIComponent>()) {
     *         addComponent(*ui);
     *     }
     * }
     *
     * // Unload a specific plugin (safe even from a callback)
     * pluginManager.unloadPlugin(myPlugin);
     * @endcode
     *
     * @see Plugin, PluginWith, PluginInfo, DeferredOperationsManager, REGISTER_PLUGIN
     */
    class PluginManager : public DeferredOperationsManager<PluginManager>
    {
        struct PluginData
        {
            Plugin*    plugin;
            LIB_HANDLE handle;
        };

        std::vector<PluginData>  _plugins;

        std::vector<std::string> getPluginFiles(const std::string& directory);
        std::vector<std::string> getFilesInDirectory(const std::string& directory,
                                                      const std::string& extension = "");

    public:
        /**
         * @brief Construct a @c PluginManager with a default plugin search path.
         *
         * @param pluginPath Default directory used by @c loadPluginsFromDirectory
         *                   when no path is provided. Can be overridden per-call.
         */
        PluginManager(const std::string& pluginPath = "");

        /**
         * @brief Load a single plugin from a shared library path.
         *
         * Opens the library at @p path, calls the @c createPlugin factory
         * function (exported by @c REGISTER_PLUGIN), and calls
         * @c Plugin::onLoad() on the result. Returns @c nullptr if the
         * library cannot be opened or the factory symbol is not found.
         *
         * @param path Absolute or relative path to the plugin shared library
         *             (@c .dylib, @c .so, or @c .dll).
         * @return Pointer to the loaded plugin, or @c nullptr on failure.
         */
        Plugin* loadPlugin(const std::string& path);

        /**
         * @brief Load all plugin libraries found in a directory.
         *
         * Scans @p dir for files with the platform plugin extension and calls
         * @c loadPlugin for each one. Silently skips files that fail to load.
         *
         * @param dir Path to the directory to scan.
         */
        void loadPluginsFromDirectory(const std::string& dir);

        /**
         * @brief Return all currently loaded plugins.
         *
         * Processes any pending deferred unloads before returning, so the
         * vector is always consistent with the current manager state.
         *
         * @return Vector of pointers to all active @c Plugin instances.
         *         Do not store these pointers across calls to @c unloadPlugin.
         */
        std::vector<Plugin*> getPlugins();

        /**
         * @brief Unload a plugin and close its shared library.
         *
         * Queues the following sequence for deferred execution:
         * -# @c EventsManager::forceUnsubscribeAll and
         *    @c MessageManager::forceUnsubscribeAll — removes all subscriptions
         *    immediately so no callbacks fire after the plugin is gone.
         * -# @c Plugin::onUnload() — gives the plugin a chance to clean up.
         * -# @c delete plugin — destroys the plugin object.
         * -# @c CLOSE_LIB(handle) — unloads the shared library.
         *
         * If called from within an event or message callback, all of the above
         * is deferred until the active iteration completes.
         *
         * @param plugin Pointer to the plugin to unload. Must have been returned
         *               by @c loadPlugin or @c getPlugins. Passing @c nullptr is safe.
         */
        void unloadPlugin(Plugin* plugin);

        /**
         * @brief Probe a directory for plugins without keeping them loaded.
         *
         * Temporarily loads each plugin library, reads its name, version, and
         * thumbnail, copies the thumbnail texture to CPU memory, then immediately
         * unloads the library. The resulting @c PluginInfo records contain
         * everything needed to populate a game-selection carousel.
         *
         * @param dir Path to the directory to scan.
         * @return Vector of @c PluginInfo records, one per successfully probed plugin.
         */
        std::vector<PluginInfo> scanPlugins(const std::string& dir);

    private:
        /**
         * @brief Execute the actual unload sequence for one plugin.
         *
         * Called directly when the manager is idle, or scheduled as a
         * pending operation by @c DeferredOperationsManager when busy.
         *
         * @param plugin The plugin to destroy and unload.
         */
        void doUnloadPlugin(Plugin* plugin);
    };

} // namespace ml

#endif // PLUGINHOST_H
