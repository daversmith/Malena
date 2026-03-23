#pragma once
// ============================================================
//  include/Malena/Core/MLExport.h
// ============================================================
//
//  MLExport<T> is an alternative to ML_EXPORT for users who prefer
//  explicit C++ syntax over macros. Place it in a .cpp file after
//  the class definition:
//
//    template class ml::MLExport<ScoreFireable>;
//
//  This forces instantiation of MLExport<T>::Registrar, whose
//  constructor calls Fireable::_register exactly once.
//
//  NOTE: Unlike ML_EXPORT, this cannot be placed in a header —
//  explicit template instantiation violates ODR if the header is
//  included in multiple translation units. Use ML_EXPORT for
//  header-compatible registration.
//
//  This header must be included AFTER Fireable.h so that Fireable
//  is a complete type. In practice this is guaranteed because
//  Fireable.h includes this file at its bottom.
//
// ============================================================

#include <Malena/Events/Fireable.h>
#include <type_traits>

namespace ml
{
    /**
     * @brief Template alternative to @c ML_EXPORT for registering
     *        Malena types in @c .cpp files.
     * @ingroup Core
     *
     * @c MLExport<T> provides the same registration behaviour as
     * @c ML_EXPORT but uses explicit template instantiation syntax
     * rather than a macro. Use it in a @c .cpp file:
     *
     * @code
     * // ScoreFireable.cpp
     * #include "ScoreFireable.h"
     * template class ml::MLExport<ScoreFireable>;
     * @endcode
     *
     * This is exactly equivalent to:
     * @code
     * ML_EXPORT(ScoreFireable)
     * @endcode
     *
     * ### When to use MLExport vs ML_EXPORT
     * | Scenario | Use |
     * |---|---|
     * | Registration in a header file | @c ML_EXPORT — uses @c inline @c struct, ODR-safe |
     * | Registration in a @c .cpp file | Either — @c MLExport or @c ML_EXPORT both work |
     * | Prefer no macros | @c MLExport + explicit instantiation |
     *
     * ### How registration works
     * Forcing instantiation of @c MLExport<T> causes @c Registrar's
     * constructor to run exactly once before @c main(). For Fireable
     * types it calls @c Fireable::_register with a new singleton instance.
     * For non-Fireable non-Plugin types a @c static_assert fires at
     * compile time. Plugin @c extern @c "C" symbols are handled by the
     * @c ML_EXPORT macro's plugin block — @c MLExport does not emit them,
     * so plugins must use @c ML_EXPORT.
     *
     * @tparam T The type to register. Must derive from @c ml::Fireable.
     *           For @c ml::Plugin types use @c ML_EXPORT instead.
     *
     * @see ML_EXPORT, ml::Fireable
     */
    template<typename T>
    struct MLExport
    {
    private:
        /**
         * @brief Internal struct whose constructor triggers registration.
         * @cond INTERNAL
         */
        struct Registrar
        {
            Registrar()
            {
                if constexpr (std::is_base_of_v<Fireable, T>)
                {
                    // Calls private Fireable::_register —
                    // accessible via: template<typename U> friend struct MLExport;
                    // declared in Fireable.h
                    Fireable::_register(new T());
                }
                else if constexpr (!std::is_base_of_v<Plugin, T>)
                {
                    // Neither Fireable nor Plugin — unsupported type
                    static_assert(!sizeof(T),
                        "[Malena] MLExport: type is not a recognised exportable "
                        "Malena type. Must derive from ml::Fireable. "
                        "For ml::Plugin types use ML_EXPORT instead.");
                }
                // Plugin case: extern "C" symbols are emitted by ML_EXPORT —
                // MLExport does not handle plugins. Use ML_EXPORT for plugins.
            }
        };
        /** @endcond */

        // Constructed once when MLExport<T> is explicitly instantiated.
        // inline static ensures a single instance across translation units.
        inline static Registrar _registrar{};

        // Fireable::_register is private — MLExport accesses it via:
        // template<typename U> friend struct MLExport;  (declared in Fireable.h)
    };

} // namespace ml
