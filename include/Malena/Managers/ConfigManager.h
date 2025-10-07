#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <type_traits>
#include <string>
namespace ml {
    // Helper to check if Manifest has a specific type
    template<typename T, typename = void>
    struct has_Strings : std::false_type {};
    
    template<typename T>
    struct has_Strings<T, std::void_t<typename T::Text>> : std::true_type {};
    
    template<typename T, typename = void>
    struct has_Ints : std::false_type {};
    
    template<typename T>
    struct has_Ints<T, std::void_t<typename T::Ints>> : std::true_type {};

    template<typename T, typename = void>
    struct has_Floats : std::false_type {};

    template<typename T>
    struct has_Floats<T, std::void_t<typename T::Floats>> : std::true_type {};

    template<typename T, typename = void>
    struct has_Booleans : std::false_type {};

    template<typename T>
    struct has_Booleans<T, std::void_t<typename T::Floats>> : std::true_type {};

    template<typename Manifest>
    class ConfigManager {
    public:
        static void ensureInitialized();
        
        // Only available if Manifest has Strings enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Strings<M>::value, const std::string&>
        get(typename M::Text config);
        
        // Only available if Manifest has Ints enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Ints<M>::value, int>
        get(typename M::Integers config);

        // Only available if Manifest has Floats enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Floats<M>::value, float>
        get(typename M::Floats config);

        // Only available if Manifest has Floats enum
        template<typename M = Manifest>
        static std::enable_if_t<has_Booleans<M>::value, bool>
        get(typename M::Booleans config);
    };
}

#include "../../../src/Managers/ConfigManager.tpp"

#endif //CONFIGMANAGER_H