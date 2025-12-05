//
// Created by Dave Smith on 12/5/25.
//

#include <Malena/Plugin/PluginHost.h>


namespace ml
{

	PluginHost::PluginHost(const std::string& pluginPath)
	{
		// loadPluginsFromDirectory(pluginPath);
	}

	void PluginHost::loadPluginsFromDirectory(const std::string& dir) {
	    for (const auto& file : getPluginFiles(dir)) {
	        loadPlugin(file);
	    }
	}

	Plugin* PluginHost::loadPlugin(const std::string& path) {
	    LIB_HANDLE handle = LOAD_LIB(path.c_str());
	    if (!handle) {
	#ifdef _WIN32
	        std::cerr << "Failed to load: " << path << " Error: " << GetLastError() << std::endl;
	#else
	        std::cerr << "Failed to load: " << path << std::endl;
	        std::cerr << "Error: " << dlerror() << std::endl;  // <-- Add this for detailed error
	#endif
	        return nullptr;
	    }
	    if (!handle) {
	        std::cerr << "Failed to load: " << path << std::endl;
	        return nullptr;
	    }

	    // Every plugin exports this function
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

	void PluginHost::unloadPlugin(Plugin* plugin) {
		for (auto it = _plugins.begin(); it != _plugins.end(); ++it) {
			if (it->plugin == plugin) {
				it->plugin->onUnload();
				delete it->plugin;
				CLOSE_LIB(it->handle);
				_plugins.erase(it);
				return;
			}
		}
	}

	std::vector<std::string> PluginHost::getFilesInDirectory(const std::string& directory,
	                                                  const std::string& extension) {
	    std::vector<std::string> files;

	    if (!fs::exists(directory) || !fs::is_directory(directory)) {
	        std::cerr << "Directory does not exist: " << directory << std::endl;
	        return files;
	    }

	    for (const auto& entry : fs::directory_iterator(directory)) {
	        if (entry.is_regular_file()) {
	            std::string path = entry.path().string();

	            // If extension specified, filter by it
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

	// Overload that checks for both .dll and .so
	std::vector<std::string> PluginHost::getPluginFiles(const std::string& directory) {
	    std::vector<std::string> files;

	    if (!fs::exists(directory) || !fs::is_directory(directory)) {
	        return files;
	    }

	    for (const auto& entry : fs::directory_iterator(directory)) {
	        if (entry.is_regular_file()) {
	            std::string ext = entry.path().extension().string();
	            if (ext == ".dll" || ext == ".so" || ext == ".dylib") {  // .dylib for macOS
	                files.push_back(entry.path().string());
	            }
	        }
	    }

	    return files;
	}
	std::vector<Plugin*> PluginHost::getPlugins() const {
	    std::vector<Plugin*> result;
	    result.reserve(_plugins.size());
	    for (const auto& pd : _plugins) {
	        result.push_back(pd.plugin);
	    }
	    return result;
	}
}