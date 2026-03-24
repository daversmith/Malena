//
// PluginManager.cpp
//
#ifndef _WIN32
#include <dlfcn.h>
#endif
#include <Malena/Engine/Plugins/PluginManager.h>
#include <Malena/Engine/Plugins/PluginInfo.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Engine/Messaging/MessageManager.h>
#include <iostream>

namespace ml
{
    PluginManager::PluginManager(const std::string& pluginPath)
    {
    }

    Plugin* PluginManager::loadPlugin(const std::string& path_)
    {
        std::string path;

        // If path_ points directly to a file, use it as-is.
        // If it's a directory, find the first plugin file inside it.
        if (fs::is_regular_file(path_))
        {
            path = path_;
        }
        else if (fs::is_directory(path_))
        {
            auto files = getPluginFiles(path_);
            if (files.empty())
            {
                std::cerr << "No plugin files found in: " << path_ << std::endl;
                return nullptr;
            }
            path = files[0];
        }
        else
        {
            // Neither — try appending platform extension as a convenience
            // e.g. loadPlugin("plugins/core/main_screen") →
            //      "plugins/core/main_screen.dylib" / ".so" / ".dll"
#ifdef _WIN32
            const std::string ext = ".dll";
#elif __APPLE__
            const std::string ext = ".dylib";
#else
            const std::string ext = ".so";
#endif
            std::string candidate = path_ + ext;
            if (fs::is_regular_file(candidate))
            {
                path = candidate;
            }
            else
            {
                // const char* err = dlerror();
                // std::cerr << "Plugin failed to load: " << path_ << "\n";
                // std::cerr << "  reason: " << (err ? err : "(no error message)") << "\n";
                return nullptr;
            }
        }

        LIB_HANDLE handle = LOAD_LIB(path.c_str());
        if (!handle)
        {
#ifdef _WIN32
            std::cerr << "Failed to load: " << path << " Error: " << GetLastError() << std::endl;
#else
            std::cerr << "Failed to load: " << path << std::endl;
            std::cerr << "Error: " << dlerror() << std::endl;
#endif
            return nullptr;
        }

        typedef Plugin* (*CreatePluginFunc)();
        auto createPlugin = (CreatePluginFunc)GET_FUNC(handle, "createPlugin");

        if (!createPlugin)
        {
            std::cerr << "No createPlugin function found in: " << path << std::endl;
            CLOSE_LIB(handle);
            return nullptr;
        }

        Plugin* plugin = createPlugin();
        _plugins.push_back({plugin, handle});

        // If this plugin is also a Fireable, register it as an event dispatcher
        if (auto* fireable = dynamic_cast<Fireable*>(plugin))
            Fireable::_register(fireable);  // same instance, no new construction

        plugin->onLoad();

        return plugin;
    }

    void PluginManager::loadPluginsFromDirectory(const std::string& dir)
    {
        auto files = getPluginFiles(dir);
        for (const auto& file : files)
            loadPlugin(file);
    }

    std::vector<std::string> PluginManager::getPluginFiles(const std::string& directory)
    {
        std::vector<std::string> files;

        if (!fs::exists(directory) || !fs::is_directory(directory))
            return files;

        for (const auto& entry : fs::directory_iterator(directory))
        {
            if (entry.is_regular_file())
            {
                std::string ext = entry.path().extension().string();
                if (ext == ".dll" || ext == ".so" || ext == ".dylib")
                    files.push_back(entry.path().string());
            }
        }

        return files;
    }

    std::vector<std::string> PluginManager::getFilesInDirectory(const std::string& directory,
                                                                 const std::string& extension)
    {
        std::vector<std::string> files;

        if (!fs::exists(directory) || !fs::is_directory(directory))
        {
            std::cerr << "Directory does not exist: " << directory << std::endl;
            return files;
        }

        for (const auto& entry : fs::directory_iterator(directory))
        {
            if (entry.is_regular_file())
            {
                std::string path = entry.path().string();
                if (!extension.empty())
                {
                    if (entry.path().extension() == extension)
                        files.push_back(path);
                }
                else
                {
                    files.push_back(path);
                }
            }
        }

        return files;
    }

    std::vector<Plugin*> PluginManager::getPlugins()
    {
        std::vector<Plugin*> result;
        result.reserve(_plugins.size());
        for (const auto& pd : _plugins)
            result.push_back(pd.plugin);
        return result;
    }

    void PluginManager::unloadPlugin(Plugin* plugin)
    {
        if (auto* fireable = dynamic_cast<Fireable*>(plugin))
            Fireable::_unregister(fireable);  // need to add this method

        // Remove from draw loop immediately — safe, doesn't touch plugin RTTI
        if (auto* core = dynamic_cast<Core*>(plugin))
            CoreManager<Core>::removeComponent(core);

        // Defer everything else — plugin pointer may be mid-callback
        deferOrExecute([this, plugin]() {
            doUnloadPlugin(plugin);
        });
    }

    void PluginManager::doUnloadPlugin(Plugin* plugin)
    {
        // Re-queue if any manager is still busy
        if (EventManager::isBusy() || MessageManager::isBusy())
        {
            pendingOperations.push_back([this, plugin]() {
                doUnloadPlugin(plugin);
            });
            return;
        }

        Plugin* toDelete = nullptr;
        void*   handle   = nullptr;

        for (auto it = _plugins.begin(); it != _plugins.end(); ++it)
        {
            if (it->plugin == plugin)
            {
                toDelete = it->plugin;
                handle   = it->handle;
                _plugins.erase(it);
                break;
            }
        }

        if (toDelete)
        {
            if (auto* core = dynamic_cast<Core*>(toDelete))
                EventManager::forceUnsubscribeAll(core);

            if (auto* messenger = dynamic_cast<Messenger*>(toDelete))
                MessageManager::forceUnsubscribeAll(messenger);

            toDelete->onUnload();
            delete toDelete;
            CLOSE_LIB(handle);
        }
    }

    std::vector<PluginInfo> PluginManager::scanPlugins(const std::string& dir)
    {
        std::vector<PluginInfo> results;

        for (const auto& path : getPluginFiles(dir))
        {
            // Load fully through the proper pipeline — deferred cleanup,
            // forceUnsubscribeAll, etc. all handled by loadPlugin/unloadPlugin
            Plugin* plugin = loadPlugin(path);
            if (!plugin) continue;

            PluginInfo info;
            info.path    = path;
            info.name    = plugin->getName();
            info.version = plugin->getVersion();

            // Copy thumbnail pixel data to CPU before unloading the dylib.
            // sf::Texture lives on the GPU inside the plugin — once the dylib
            // closes that memory is gone. copyToImage() pulls it to CPU first,
            // then we create a fresh texture owned by PluginInfo.
            if (const sf::Texture* thumb = plugin->getThumbnail())
            {
                sf::Image pixels = thumb->copyToImage();
                sf::Texture copy;
                if (copy.loadFromImage(pixels))
                    info.thumbnail = std::move(copy);
            }

            results.push_back(std::move(info));
            unloadPlugin(plugin);
        }

        return results;
    }

} // namespace ml