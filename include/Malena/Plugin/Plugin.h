// Plugin.h
#pragma once
// #ifndef MALENA_PLUGIN_H
// #define MALENA_PLUGIN_H
#ifdef _WIN32
	#define PLUGIN_EXPORT __declspec(dllexport)
#else
	#define PLUGIN_EXPORT
#endif

#include <Malena/Traits/Messenger.h>
namespace ml {

	class Plugin : public Messenger
	{
	public:
		virtual ~Plugin() = default;
		virtual const char* getName() const { return "Unnamed Plugin"; }
		virtual const char* getVersion() const { return "1.0"; }
		virtual void onLoad() {}
		virtual void onUnload() {}
		// Plugin.h
		template<typename T>
		bool is() const {
			return dynamic_cast<const T*>(this) != nullptr;
		}

		template<typename T>
		T* getIf() {  // Returns nullptr if not this type
			return dynamic_cast<T*>(this);
		}

		template<typename T>
		const T* getIf() const {
			return dynamic_cast<const T*>(this);
		}
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

// #endif