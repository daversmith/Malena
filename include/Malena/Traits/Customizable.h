#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include <Malena/Managers/StateManager.h>
#include <Malena/Utilities/TypeExtraction.h>

namespace ml
{
    /**
     * @brief Tags a class with a manifest and adds StateManager.
     *
     * Does NOT own CustomFlaggable — flag storage is owned by
     * ComponentBase via MultiCustomFlaggable, which gathers all
     * manifests from the component and its traits automatically.
     *
     * Traits access their flags via dynamic_cast to SingleFlaggable:
     * @code
     * auto* f = dynamic_cast<SingleFlaggable<DraggableManifest::Flags>*>(this);
     * f->enableFlag(DraggableManifest::Flags::DRAGGING);
     * @endcode
     *
     * Usage:
     * @code
     * // Trait with manifest
     * class Draggable : public ml::Customizable<DraggableManifest> {};
     *
     * // Component with manifest + traits
     * class Carousel : public ml::ComponentWith<CarouselManifest, Draggable> {};
     * @endcode
     *
     * @tparam Manifest A class with optional Flags and State enums
     */
    template<typename Manifest>
    class Customizable : public StateManager<typename extract_State<Manifest>::type>
    {
    public:
        // Exposes manifest type for ComponentWith to gather
        using manifest_type = Manifest;

        Customizable() = default;
        virtual ~Customizable() = default;
    };

} // namespace ml

#endif // UICOMPONENT_H