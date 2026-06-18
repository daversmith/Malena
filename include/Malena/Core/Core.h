// Copyright (c) 2025 Dave R. Smith. All rights reserved.
// Malena Framework — Proprietary Software. See LICENSE for terms.

//
// Created by Dave Smith on 10/4/25.
//

#ifndef MALENA_UICOMPONENTBASE_H
#define MALENA_UICOMPONENTBASE_H

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics.hpp>
#include <functional>
#include <type_traits>
#include <vector>

#include <Malena/Traits/Communication/Subscribable.h>
#include <Malena/Traits/Interaction/Draggable.h>
#include <Malena/Traits/Spatial/Positionable.h>
#include <Malena/Traits/State/Flaggable.h>

#include <Malena/Traits/Interaction/Keyable.h>
#include <Malena/Traits/Interaction/Scrollable.h>
#include <Malena/Traits/Lifecycle/Updatable.h>
#include <Malena/Traits/Interaction/Clickable.h>
#include <Malena/Traits/Interaction/Focusable.h>
#include <Malena/Traits/Interaction/Hoverable.h>

#include <Malena/Traits/Communication/Unsubscribable.h>

namespace ml
{
    /**
     * @brief Framework-provided default layer enum.
     * @ingroup Core
     *
     * Use this when a component has no manifest of its own and just needs to
     * place children into broad strokes. For component-specific vocabulary,
     * declare a @c Layer enum on the component's manifest and alias it into
     * scope.
     *
     * @code
     * addComponent(_background, ml::Layer::Background);
     * addComponent(_content,    ml::Layer::Content);
     * addComponent(_overlay,    ml::Layer::Overlay);
     * @endcode
     *
     * Values are spaced 100 apart so callers can slip a custom layer in
     * between (e.g. @c 50 for a backdrop behind Content) without renumbering.
     */
    enum class Layer : int
    {
        Background = 0,
        Content    = 100,
        Overlay    = 200,
    };

    /**
     * @brief Virtual base class for all Malena framework objects.
      * @ingroup Core
     *
     * @c Core combines the fundamental traits that every framework object
     * is expected to support:
     * - @c Subscribable — can subscribe to and publish framework events
     * - @c Flaggable    — carries system-level boolean flags (@c ml::Flag)
     * - @c Positionable — has a position, global bounds, and spatial identity
     * - @c Clickable, @c Hoverable, @c Focusable — pointer interaction
     * - @c Keyable, @c Scrollable — keyboard and scroll-wheel input
     * - @c Updatable    — receives a per-frame update callback
     * - @c Unsubscribable — can remove all its event subscriptions at once
     *
     * All inheritance is @c virtual so that the diamond formed by
     * @c ComponentCore and its trait hierarchy collapses to a single
     * instance of each base.
     *
     * @c Core does not inherit @c sf::Drawable. Drawable behavior is added
     * by @c ComponentBase (for standard components) or by the shape type @c T
     * in @c Graphic (for graphics primitives).
     *
     * @note Direct instantiation is not intended. Inherit through
     *       @c ml::Component, @c ml::Graphic, or a plugin base.
     *
     * @see Component, CoreAdapter, ComponentCore
     */
    class MALENA_API Core : public virtual Subscribable,
                 public virtual Flaggable,
                 public virtual Positionable,
                 public virtual Clickable,
				 public virtual Hoverable,
				 public virtual Focusable,
                 public virtual Keyable,
                 public virtual Scrollable,
                 // public virtual Draggable,
                 public virtual Updatable,
				 public Unsubscribable

    {
    public:
        Core();
        virtual ~Core();

        virtual sf::RenderStates getRenderStates() const { return sf::RenderStates(); }

        // ── Enabled state ──────────────────────────────────────────────────
        // Two-flag model: effective enabled = _selfEnabled && _parentEnabled,
        // mirrored into Flag::ENABLED (what hit-testing filters on). This keeps
        // "a component I explicitly disabled stays disabled when its parent is
        // re-enabled" working.
        //
        // To REACT to an enable/disable change (sync visuals, a private DISABLED
        // flag, close a popup, …), override onEnabledChanged(bool effective) —
        // it fires on BOTH the direct (setEnabled) and cascade (setParentEnabled)
        // paths. Do NOT override setEnabled/setParentEnabled to do that work, or
        // the cascade path silently skips it.

        /// Set this component's own enabled intent (_selfEnabled). Public API and
        /// non-virtual: there is exactly one enable mechanism. To react to an
        /// enable/disable change, override onEnabledChanged — never this.
        void setEnabled(bool enabled);

        /// Cascade plumbing: set the inherited (_parentEnabled) component of the
        /// effective state. Called by the framework when an ancestor's effective
        /// state changes (and by Modal/Panel for explicit subtree gating). NOT
        /// an override point — react via onEnabledChanged instead.
        void setParentEnabled(bool enabled);

        virtual void setVisible(bool visible);
        virtual void setActive(bool active);

        /// Called by AppManager on every registered component when the window is
        /// resized. Override in responsive components to reflow to the new pixel
        /// size (the framework keeps a 1:1 view, so width/height are window pixels).
        virtual void onWindowResize(unsigned int /*width*/, unsigned int /*height*/) {}
        bool isEnabled() const;
        bool isVisible() const;

        /**
         * @brief True only if this component and every ancestor are visible.
         *
         * @c isVisible() reports a single component's own @c HIDDEN flag, but
         * visibility is not cascaded the way enabled-state is: hiding a
         * container does not flip the @c HIDDEN flag on its descendants. A
         * descendant of a hidden container is therefore not drawn (the parent
         * never reaches it) yet still looks visible to anything that inspects
         * its own flag — notably hit-testing. This walks the parent chain so
         * such a descendant is correctly treated as not visible.
         */
        bool isEffectivelyVisible() const;

        /// Default layer key when none is specified. Chosen at 100 so callers
        /// can place children both above and below the default without
        /// renumbering. Within a layer, children draw in registration order.
        static constexpr int DefaultLayer = 100;

        /**
         * @brief A registered child plus the layer key it draws/iterates under.
         *
         * Layer ascending = draw order. Within a layer, registration order
         * (insertion) is preserved by the stable sort in addComponent.
         */
        struct Child
        {
            Core* component;
            int   layer;
        };

        void addComponent(Core& child);
        void addComponent(Core& child, int layer);

        /**
         * @brief Register a child at a layer identified by any enum value.
         *
         * Sugar over @c addComponent(child, int) — the enum's underlying
         * integer value is used as the layer key. Lets manifests declare a
         * domain-meaningful @c Layer enum and pass it directly at the call
         * site without explicit casts:
         *
         * @code
         * class MyManifest : public ml::Manifest {
         * public:
         *     enum class Layer : int {
         *         Backdrop  = 0,
         *         Content   = 100,
         *         Controls  = 150,
         *     };
         * };
         *
         * class MyWidget : public ml::ComponentWith<MyManifest> {
         * public:
         *     using Layer = MyManifest::Layer;
         *     MyWidget() {
         *         addComponent(_bg,    Layer::Backdrop);
         *         addComponent(_input, Layer::Controls);
         *     }
         * };
         * @endcode
         *
         * Also accepts @c ml::Layer (the framework-provided default) when a
         * component has no manifest of its own.
         */
        template<typename E,
                 typename = std::enable_if_t<std::is_enum_v<E>>>
        void addComponent(Core& child, E layer)
        {
            addComponent(child, static_cast<int>(layer));
        }

        void removeComponent(Core& child);

        /**
         * @brief Find the front-most (last-painted) node in this subtree that
         *        satisfies @p accept, searching topmost-first.
         *
         * Walks this Core and its descendants in reverse paint order — children
         * before their parent, and within a parent the highest layer first — so
         * the first node for which @p accept returns @c true is the one visually
         * on top at that location. Invisible subtrees are skipped.
         *
         * This is the hit-testing seam the event system uses to route a click to
         * only the top-most interactive target instead of every overlapping one.
         * @p accept encapsulates the test (e.g. "is hovered AND a click target"),
         * keeping @c Core free of any geometry/bounds knowledge.
         *
         * @param accept Predicate evaluated per node, topmost-first.
         * @return The front-most accepted node, or @c nullptr if none match.
         */
        Core* topmostMatching(const std::function<bool(Core&)>& accept);

        /**
         * @brief Register multiple child components at the default layer.
         *
         * Equivalent to calling @c addComponent for each argument. Use this in
         * a composite component's constructor when several fixed member
         * components need to participate in the enable/disable cascade.
         *
         * @code
         * ChatWindow() { addComponents(_scrollPane, _input, _sendBtn); }
         * @endcode
         */
        template<typename... Children>
        void addComponents(Children&... children)
        {
            static_assert((std::is_base_of_v<Core, Children> && ...),
                "addComponents() requires Core-derived arguments");
            (addComponent(children), ...);
        }

        /**
         * @brief Register multiple child components at a shared layer.
         *
         * @code
         * ChatWindow() {
         *     addComponents(200, _input, _sendBtn);  // foreground row
         * }
         * @endcode
         */
        template<typename... Children>
        void addComponents(int layer, Children&... children)
        {
            static_assert((std::is_base_of_v<Core, Children> && ...),
                "addComponents() requires Core-derived arguments");
            (addComponent(children, layer), ...);
        }

        /**
         * @brief Variadic shared-layer registration with an enum layer key.
         *
         * @code
         * addComponents(Layer::Controls, _input, _sendBtn);
         * @endcode
         */
        template<typename E, typename... Children,
                 typename = std::enable_if_t<std::is_enum_v<E>>>
        void addComponents(E layer, Children&... children)
        {
            static_assert((std::is_base_of_v<Core, Children> && ...),
                "addComponents() requires Core-derived arguments");
            (addComponent(children, static_cast<int>(layer)), ...);
        }

        static void linkChild(Core* parent, Core* child);
        static void unlinkAll(Core* core);

    protected:
        /// THE hook for reacting to an enable/disable change. Fires whenever the
        /// effective enabled state flips — on both the direct (setEnabled) and
        /// cascade (setParentEnabled) paths — so visuals, a private DISABLED
        /// flag, popup teardown, etc. stay correct no matter how the change
        /// arrived. Override this instead of setEnabled/setParentEnabled.
        /// @param enabled the new effective enabled state.
        virtual void onEnabledChanged(bool enabled) {}

        /// Read-only view of registered children, sorted by layer ascending.
        /// Within a layer, entries appear in registration order. Each Child
        /// carries its component pointer and layer key — derived classes that
        /// only care about the components walk @c entry.component.
        const std::vector<Child>& getChildren() const { return _children; }

        /// Render all registered children in layer order, skipping invisible
        /// ones. The framework default draw uses this; derived classes that
        /// override @c draw to add procedural visuals (background, decoration)
        /// or to wrap the loop in a clip view call this at the appropriate
        /// point.
        ///
        /// @code
        /// void Panel::draw(target, states) const {
        ///     Rectangle::draw(target, states);    // background
        ///     drawChildren(target, states);        // children loop
        /// }
        /// @endcode
        void drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        bool _selfEnabled   = true;
        bool _parentEnabled = true;

        void applyEnabled(bool selfEnabled, bool parentEnabled);

        // ── Per-instance parent/child topology ──────────────────────────────
        // Replaces the static _childMap. Single-parent: a Core may appear in
        // at most one parent's _children vector at a time. linkChild silently
        // moves a child if its previous parent differs.
        Core*               _parent = nullptr;
        std::vector<Child>  _children;

        // Reentrancy guard for cascades that may indirectly trigger
        // removeComponent / linkChild. Mutations while iterating defer until
        // the depth returns to zero.
        int                                     _iterDepth = 0;
        std::vector<std::function<void()>>      _pendingOps;

        void doRemoveChild(Core* child);
        void runPendingIfDepthZero();

        // Insert a child keeping _children sorted by layer ascending, with
        // registration order preserved within a layer. O(n) positional insert,
        // not a full re-sort. Sets the child's _parent. Caller guarantees the
        // child is not already present.
        void insertChildSorted(Core& child, int layer);

        static bool isDescendantOf(Core* ancestor, Core* component);
        friend class AppManager;
    };

} // namespace ml

#endif // MALENA_UICOMPONENTBASE_H
