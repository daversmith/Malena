// //
// // Created by Dave R. Smith on 3/5/25.
// //
//
// #ifndef CUSTOMIZABLE_TPP
// #define CUSTOMIZABLE_TPP
// #include <Malena/Traits/Customizable.h>
//
// namespace ml
// {
//     // ── Primary specialization (with Component) ───────────────────────────────
//
//     template<typename Manifest, typename Component>
//     template<typename Arg, typename... Args, typename>
//     Customizable<Manifest, Component>::Customizable(Arg&& arg, Args&&... args)
//         : Component(std::forward<Arg>(arg), std::forward<Args>(args)...)
//     {}
//
//     template<typename Manifest, typename Component>
//     template<typename F, typename>
//     void Customizable<Manifest, Component>::enableFlag(F flag)
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             this->CustomFlaggable<typename extract_Flags<Manifest>::type>::enableFlag(flag);
//     }
//
//     template<typename Manifest, typename Component>
//     template<typename F, typename>
//     void Customizable<Manifest, Component>::disableFlag(F flag)
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             this->CustomFlaggable<typename extract_Flags<Manifest>::type>::disableFlag(flag);
//     }
//
//     template<typename Manifest, typename Component>
//     template<typename F, typename>
//     bool Customizable<Manifest, Component>::checkFlag(F flag) const
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             return this->CustomFlaggable<typename extract_Flags<Manifest>::type>::checkFlag(flag);
//         return false;
//     }
//
//     template<typename Manifest, typename Component>
//     template<typename F, typename>
//     void Customizable<Manifest, Component>::setFlag(F flag, bool value)
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             this->CustomFlaggable<typename extract_Flags<Manifest>::type>::setFlag(flag, value);
//     }
//
//     template<typename Manifest, typename Component>
//     template<typename F, typename>
//     void Customizable<Manifest, Component>::toggleFlag(F flag)
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             this->CustomFlaggable<typename extract_Flags<Manifest>::type>::toggleFlag(flag);
//     }
//
//     // ── Standalone specialization (void Component) ────────────────────────────
//
//     template<typename Manifest>
//     template<typename F, typename>
//     void Customizable<Manifest, void>::enableFlag(F flag)
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             this->CustomFlaggable<typename extract_Flags<Manifest>::type>::enableFlag(flag);
//     }
//
//     template<typename Manifest>
//     template<typename F, typename>
//     void Customizable<Manifest, void>::disableFlag(F flag)
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             this->CustomFlaggable<typename extract_Flags<Manifest>::type>::disableFlag(flag);
//     }
//
//     template<typename Manifest>
//     template<typename F, typename>
//     bool Customizable<Manifest, void>::checkFlag(F flag) const
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             return this->CustomFlaggable<typename extract_Flags<Manifest>::type>::checkFlag(flag);
//         return false;
//     }
//
//     template<typename Manifest>
//     template<typename F, typename>
//     void Customizable<Manifest, void>::setFlag(F flag, bool value)
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             this->CustomFlaggable<typename extract_Flags<Manifest>::type>::setFlag(flag, value);
//     }
//
//     template<typename Manifest>
//     template<typename F, typename>
//     void Customizable<Manifest, void>::toggleFlag(F flag)
//     {
//         if constexpr (!std::is_void_v<typename extract_Flags<Manifest>::type>)
//             this->CustomFlaggable<typename extract_Flags<Manifest>::type>::toggleFlag(flag);
//     }
//
// } // namespace ml
// #endif