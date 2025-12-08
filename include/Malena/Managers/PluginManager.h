//
// Created by Dave Smith on 12/5/25.
//

#ifndef PLUGINHOST_H
#define PLUGINHOST_H

#include <Malena/Application/Application.h>
#include <Malena/Plugin/Plugin.h>

#ifdef _WIN32
    #include <windows.h>
    #define LIB_HANDLE HMODULE
    #define LOAD_LIB(path) LoadLibraryA(path)
    #define GET_FUNC(handle, name) GetProcAddress(handle, name)
    #define CLOSE_LIB(handle) FreeLibrary(handle)
#else
    #include <dlfcn.h>
    #define LIB_HANDLE void*
    #define LOAD_LIB(path) dlopen(path, RTLD_NOW | RTLD_GLOBAL)  // <-- Changed this line
    #define GET_FUNC(handle, name) dlsym(handle, name)
    #define CLOSE_LIB(handle) dlclose(handle)
#endif
namespace fs = std::filesystem;
namespace ml
{
	class PluginManager
	{
	    struct PluginData {
	        Plugin* plugin;
	        LIB_HANDLE handle;
	    };
	    std::vector<PluginData> _plugins;
	    std::vector<std::string> getPluginFiles(const std::string& directory);
	    std::vector<std::string> getFilesInDirectory(const std::string& directory,
	                                                  const std::string& extension = "");
	public:
	    PluginManager(const std::string& pluginPath = "/Users/dsmizzle/CLionProjects/DaveStation2.0/cmake-build-debug/plugins");
	    Plugin* loadPlugin(const std::string& path);
	    void unloadPlugin(Plugin* plugin);
	    void loadPluginsFromDirectory(const std::string& dir);
	    std::vector<Plugin*> getPlugins() const;
	};
}
#endif //PLUGINHOST_H
