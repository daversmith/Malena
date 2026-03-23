#pragma once
// ============================================================
//  include/Malena/Core/Export.h
// ============================================================
//
//  Defines ML_EXPORT for Fireable types only.
//  Plugin.h includes this and redefines ML_EXPORT to also handle plugins.
//
//  Include graph (no circular dependencies):
//    Fireable.h → Export.h   (Fireable-only ML_EXPORT)
//    Plugin.h   → Export.h   (then #undef + redefines ML_EXPORT)
//    common.hpp → Plugin.h   (full ML_EXPORT always available)
//
// ============================================================

namespace ml
{
    class Fireable;  // forward declare only
}

/**
 * @file Export.h
 * @brief Provides @c ML_EXPORT for registering Malena types at startup.
 * @ingroup Core
 */

/**
 * @def ML_EXPORT(ClassName)
 * @brief Register a Malena type with the framework.
 *
 * Fireable-only version. Plugin.h redefines this to also handle plugins.
 * Uses @c FireableHelper to avoid type-checking @c new @c ClassName() for
 * non-Fireable types — @c if @c constexpr alone is not sufficient in
 * non-template contexts.
 *
 * @param ClassName The class to register. Must be fully defined.
 */
#define ML_EXPORT(ClassName)                                                    \
    namespace ml::exports {                                                     \
        /** @cond INTERNAL */                                                   \
        inline struct _Register_##ClassName                                     \
        {                                                                       \
            _Register_##ClassName()                                             \
            {                                                                   \
                /* FireableHelper selects the correct specialization at        */ \
                /* compile time. new ClassName() only appears in the Fireable  */ \
                /* specialization — never type-checked for non-Fireable types. */ \
                ml::exports::FireableHelper<ClassName>::doRegister();           \
            }                                                                   \
        } _instance_##ClassName;                                                \
        /** @endcond */                                                         \
    }
