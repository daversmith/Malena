//
// PluginManager.cpp
//

#include <Malena/Managers/PluginManager.h>
#include <Malena/Managers/EventsManager.h>
#include <Malena/Managers/MessageManager.h>
#include <iostream>

namespace ml
{
    PluginManager::PluginManager(const std::string& pluginPath)
    {
    }

    Plugin* PluginManager::loadPlugin(const std::string& path_) {
        auto files = getPluginFiles(path_);
        if (files.empty()) {
            std::cerr << "No plugin files found in: " << path_ << std::endl;
            return nullptr;
        }

        auto path = files[0];
        LIB_HANDLE handle = LOAD_LIB(path.c_str());
        if (!handle) {
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

        if (!createPlugin) {
            std::cerr << "No createPlugin function found" << std::endl;
            CLOSE_LIB(handle);
            return nullptr;
        }

        Plugin* plugin = createPlugin();
        _plugins.push_back({plugin, handle});
        plugin->onLoad();

        return plugin;
    }

    void PluginManager::loadPluginsFromDirectory(const std::string& dir) {
        auto files = getPluginFiles(dir);
        for (const auto& file : files) {
            loadPlugin(file);
        }
    }

    std::vector<std::string> PluginManager::getPluginFiles(const std::string& directory) {
        std::vector<std::string> files;

        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            return files;
        }

        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".dll" || ext == ".so" || ext == ".dylib") {
                    files.push_back(entry.path().string());
                }
            }
        }

        return files;
    }

    std::vector<std::string> PluginManager::getFilesInDirectory(const std::string& directory,
                                                      const std::string& extension) {
        std::vector<std::string> files;

        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            std::cerr << "Directory does not exist: " << directory << std::endl;
            return files;
        }

        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string path = entry.path().string();

                if (!extension.empty()) {
                    if (entry.path().extension() == extension) {
                        files.push_back(path);
                    }
                } else {
                    files.push_back(path);
                }
            }
        }

        return files;
    }

    std::vector<Plugin*> PluginManager::getPlugins() {
        std::vector<Plugin*> result;
        result.reserve(_plugins.size());
        for (const auto& pd : _plugins) {
            result.push_back(pd.plugin);
        }
        return result;
    }

    void PluginManager::unloadPlugin(Plugin* plugin) {
        deferOrExecute([this, plugin]() {  //  Using base class method
            doUnloadPlugin(plugin);
        });
    }

    void PluginManager::doUnloadPlugin(Plugin* plugin) {
        //  Don't unload if ANY manager is busy - FORCE queue it
        if (EventsManager::isBusy() || MessageManager::isBusy()) {
            // Force add to pending queue, don't execute
            pendingOperations.push_back([this, plugin]() {
                doUnloadPlugin(plugin);
            });
            return;
        }

        Plugin* toDelete = nullptr;
        void* handle = nullptr;

        for (auto it = _plugins.begin(); it != _plugins.end(); ++it) {
            if (it->plugin == plugin) {
                toDelete = it->plugin;
                handle = it->handle;
                _plugins.erase(it);
                break;
            }
        }

        if (toDelete) {
            // Force immediate unsubscribe
            if (auto* uiComp = dynamic_cast<UIComponent*>(toDelete)) {
                EventsManager::forceUnsubscribeAll(uiComp);
            }

            if (auto* messenger = dynamic_cast<Messenger*>(toDelete)) {
                MessageManager::forceUnsubscribeAll(messenger);
            }

            toDelete->onUnload();
            delete toDelete;
            CLOSE_LIB(handle);
        }
    }

} // namespace ml