//
// Created by Dave Smith on 10/5/25.
//

#ifndef STATEFUL_H
#define STATEFUL_H

#include <Malena/Resources/FlagManager.h>
#include <Malena/Utilities/Flag.h>
#include <Malena/Traits/Base/Trait.h>

namespace ml
{
	/**
	 * @brief Trait that provides system-level boolean flag management.
	 * @ingroup Traits
	 *
	 * @c Flaggable layers @c FlagManager<ml::Flag> onto the @c Trait base,
	 * giving any inheriting class the ability to read and write the
	 * framework's built-in @c ml::Flag values.
	 *
	 * It is one of the three core traits automatically included in every
	 * @c ml::Core object (@c Subscribable, @c Flaggable, @c Positionable).
	 * Framework dispatchers check @c ml::Flag values to gate built-in behavior:
	 *
	 * | Flag | Effect |
	 * |------|--------|
	 * | @c ml::Flag::DRAGGABLE | Enables drag handling in @c DraggableDispatcher |
	 * | @c ml::Flag::HOVERED   | Set by @c HoverableDispatcher while the mouse is over the component |
	 * | @c ml::Flag::FOCUSED   | Set by @c ClickableDispatcher when the component has keyboard focus |
	 *
	 * ### Usage
	 * @code
	 * // Enable dragging on a component
	 * myComponent.setFlag(ml::Flag::DRAGGABLE);
	 *
	 * // Check hover state
	 * if (myComponent.checkFlag(ml::Flag::HOVERED)) { ... }
	 *
	 * // Toggle focus
	 * myComponent.toggleFlag(ml::Flag::FOCUSED);
	 * @endcode
	 *
	 * For custom (manifest-declared) flags, see @c CustomFlaggable and
	 * @c MultiCustomFlaggable, which are composed into components separately.
	 *
	 * @see FlagManager, ml::Flag, Core, CustomFlaggable
	 */
	class Flaggable : public Trait, public FlagManager<Flag> {};

} // namespace ml

#endif // STATEFUL_H
