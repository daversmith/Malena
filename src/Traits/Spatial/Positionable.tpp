#ifndef POSITIONABLE_TPP
#define POSITIONABLE_TPP
#include <Malena/Traits/Spatial/Positionable.h>

#include <Malena/Utilities/Align.h>
#include <Malena/Layout/Anchor.h>
#include <Malena/Layout/AnchorManager.h>
#include <cmath>
#include <type_traits>
namespace ml
{
	// Each relative-layout helper is RETAINED: it applies the placement now (instant
	// feedback, backward-compatible) and, when the reference is a Positionable whose
	// bounds can be re-read on resize, records a live constraint + a baseline for
	// external-move detection. Non-Positionable references can't be lifetime-tracked,
	// so those stay one-shot.
	namespace anchor_detail
	{
		template<class T>
		void retain(Positionable* self, AnchorOp op, T& obj, float spacing)
		{
			if constexpr (std::is_base_of_v<Positionable, T>)
			{
				AnchorManager::set(self, op, static_cast<const Positionable*>(&obj), spacing);
				AnchorManager::recordBaseline(self);
			}
		}
	}

	template<class T>
		void Positionable::setRightOf(T &obj, float spacing)
	{
		Align::setRightOf(obj, *this, spacing);
		anchor_detail::retain(this, AnchorOp::RightOf, obj, spacing);
	}

	template<class T>
	void Positionable::setLeftOf(T &obj, float spacing)
	{
		Align::setLeftOf(obj, *this, spacing);
		anchor_detail::retain(this, AnchorOp::LeftOf, obj, spacing);
	}

	template<class T>
	void Positionable::setBelow(T &obj, float spacing)
	{
		Align::setBelow(obj, *this, spacing);
		anchor_detail::retain(this, AnchorOp::Below, obj, spacing);
	}

	template<class T>
	void Positionable::setAbove(T &obj, float spacing)
	{
		Align::setAbove(obj, *this, spacing);
		anchor_detail::retain(this, AnchorOp::Above, obj, spacing);
	}

	template<class T>
	void Positionable::center(T &obj)
	{
		Align::centerOn(obj, *this);
		anchor_detail::retain(this, AnchorOp::CenterOn, obj, 0.f);
	}

	template<class T>
	void Positionable::centerHorizonally(T &obj)
	{
		Align::centerHorizontally(obj.getGlobalBounds(), *this);
		anchor_detail::retain(this, AnchorOp::CenterX, obj, 0.f);
	}

	template<class T>
	void Positionable::centerVertically(T &obj)
	{
		Align::centerVertically(obj.getGlobalBounds(), *this);
		anchor_detail::retain(this, AnchorOp::CenterY, obj, 0.f);
	}
}
#endif
