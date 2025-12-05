// Plugin.h
#pragma once

#ifdef _WIN32
	#define PLUGIN_EXPORT __declspec(dllexport)
#else
	#define PLUGIN_EXPORT
#endif

namespace ml {

	class Plugin {
	public:
		virtual ~Plugin() = default;
		virtual const char* getName() const { return "Unnamed Plugin"; }
		virtual const char* getVersion() const { return "1.0"; }
		virtual void onLoad() {}
		virtual void onUnload() {}
	};

} // namespace ml

#define REGISTER_PLUGIN(GameClass) \
extern "C" { \
PLUGIN_EXPORT ml::Plugin* createPlugin() { \
return new GameClass(); \
} \
PLUGIN_EXPORT void destroyPlugin(ml::Plugin* plugin) { \
delete plugin; \
} \
}