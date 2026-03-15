#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <type_traits>
#include <string>

namespace ml
{
    /// @cond INTERNAL

    /** @brief Detects whether @c T declares a @c Text enum (string config). */
    template<typename T, typename = void>
    struct has_Strings : std::false_type {};
    template<typename T>
    struct has_Strings<T, std::void_t<typename T::Text>> : std::true_type {};

    /** @brief Detects whether @c T declares an @c Ints enum (integer config). */
    template<typename T, typename = void>
    struct has_Ints : std::false_type {};
    template<typename T>
    struct has_Ints<T, std::void_t<typename T::Ints>> : std::true_type {};

    /** @brief Detects whether @c T declares a @c Floats enum (float config). */
    template<typename T, typename = void>
    struct has_Floats : std::false_type {};
    template<typename T>
    struct has_Floats<T, std::void_t<typename T::Floats>> : std::true_type {};

    /**
     * @brief Detects whether @c T declares a @c Booleans enum (bool config).
     *
     * @warning The specialization currently checks for @c T::Floats instead
     *          of @c T::Booleans, which means @c has_Booleans<T>::value is
     *          @c true whenever the manifest has a @c Floats enum — not a
     *          @c Booleans enum. This should be corrected to
     *          @c std::void_t<typename T::Booleans>.
     */
    template<typename T, typename = void>
    struct has_Booleans : std::false_type {};
    template<typename T>
    struct has_Booleans<T, std::void_t<typename T::Floats>> : std::true_type {}; // BUG: should be T::Booleans

    /// @endcond

    /**
     * @brief Manifest-driven accessor for typed configuration values.
     *
     * @c ConfigManager<Manifest> provides typed @c get() overloads for each
     * configuration enum declared in a manifest. Each overload is enabled only
     * when the corresponding enum exists, so missing config types are caught at
     * compile time.
     *
     * ### Manifest config enums
     *
     * | Enum name in manifest | Value type | Example |
     * |-----------------------|------------|---------|
     * | @c Text      | @c std::string | Window title, labels |
     * | @c Ints      | @c int         | Max players, tile size |
     * | @c Floats    | @c float       | Speed multiplier, volume |
     * | @c Booleans  | @c bool        | Debug mode, fullscreen |
     *
     * ### Usage
     * @code
     * class MyManifest : public ml::Manifest {
     * public:
     *     enum class Text    { Title };
     *     enum class Ints    { MaxPlayers };
     *     enum class Floats  { Speed };
     * private:
     *     inline static const auto _ = [](){
     *         set(Text::Title,       std::string("My Game"));
     *         set(Ints::MaxPlayers,  4);
     *         set(Floats::Speed,     1.5f);
     *         return 0;
     *     }();
     * };
     *
     * const std::string& title = ml::ConfigManager<MyManifest>::get(MyManifest::Text::Title);
     * int maxPlayers            = ml::ConfigManager<MyManifest>::get(MyManifest::Ints::MaxPlayers);
     * float speed               = ml::ConfigManager<MyManifest>::get(MyManifest::Floats::Speed);
     * @endcode
     *
     * Or more conveniently via @c Context:
     * @code
     * struct MyCtx : public ml::Context<MyManifest> {};
     * const std::string& title = MyCtx::ConfigMgr::get(MyManifest::Text::Title);
     * @endcode
     *
     * @tparam Manifest A @c Manifest subclass declaring config enums
     *                  (@c Text, @c Ints, @c Floats, @c Booleans).
     *
     * @see Manifest, Context, AssetsManager
     */
    template<typename Manifest>
    class ConfigManager
    {
    public:
        /**
         * @brief Ensure the manifest's config values have been registered.
         *
         * Called internally before the first @c get() access. May be called
         * explicitly if you need to guarantee initialization order.
         */
        static void ensureInitialized();

        /**
         * @brief Retrieve a string config value.
         *
         * Only available when @c Manifest declares a @c Text enum.
         *
         * @param config The @c Text enum value to look up.
         * @return Const reference to the registered @c std::string value.
         */
        template<typename M = Manifest>
        static std::enable_if_t<has_Strings<M>::value, const std::string&>
        get(typename M::Text config);

        /**
         * @brief Retrieve an integer config value.
         *
         * Only available when @c Manifest declares an @c Ints enum.
         *
         * @param config The @c Ints enum value to look up.
         * @return The registered @c int value.
         */
        template<typename M = Manifest>
        static std::enable_if_t<has_Ints<M>::value, int>
        get(typename M::Integers config);

        /**
         * @brief Retrieve a float config value.
         *
         * Only available when @c Manifest declares a @c Floats enum.
         *
         * @param config The @c Floats enum value to look up.
         * @return The registered @c float value.
         */
        template<typename M = Manifest>
        static std::enable_if_t<has_Floats<M>::value, float>
        get(typename M::Floats config);

        /**
         * @brief Retrieve a boolean config value.
         *
         * Only available when @c Manifest declares a @c Booleans enum.
         *
         * @note Due to a bug in @c has_Booleans, this overload is currently
         *       enabled whenever the manifest has a @c Floats enum rather than
         *       a @c Booleans enum. See @c has_Booleans for details.
         *
         * @param config The @c Booleans enum value to look up.
         * @return The registered @c bool value.
         */
        template<typename M = Manifest>
        static std::enable_if_t<has_Booleans<M>::value, bool>
        get(typename M::Booleans config);
    };

} // namespace ml

#include "../../../src/Resources/ConfigManager.tpp"
#endif // CONFIGMANAGER_H
