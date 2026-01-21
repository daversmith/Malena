//
// PluginManager.h
//

#ifndef PLUGINHOST_H
#define PLUGINHOST_H

#include <Malena/Application/Application.h>
#include <Malena/Plugin/Plugin.h>
#include <Malena/Managers/DeferredOperationsManager.h>

#ifdef _WIN32
	#include <windows.h>
	#define LIB_HANDLE HMODULE
	#define LOAD_LIB(path) LoadLibraryA(path)
	#define GET_FUNC(handle, name) GetProcAddress(handle, name)
	#define CLOSE_LIB(handle) FreeLibrary(static_cast<HMODULE>(handle))
#else
	#include <dlfcn.h>
	#define LIB_HANDLE void*
	#define LOAD_LIB(path) dlopen(path, RTLD_NOW | RTLD_GLOBAL)
	#define GET_FUNC(handle, name) dlsym(handle, name)
	#define CLOSE_LIB(handle) dlclose(handle)
#endif

namespace fs = std::filesystem;

namespace ml
{
	class PluginManager : public DeferredOperationsManager<PluginManager>
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
		void loadPluginsFromDirectory(const std::string& dir);
		std::vector<Plugin*> getPlugins();

		void unloadPlugin(Plugin* plugin);

	private:
		void doUnloadPlugin(Plugin* plugin);
	};
}

#endif //PLUGINHOST_H