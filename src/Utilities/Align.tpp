//
// Created by Dave Smith on 3/8/26.
//

#ifndef ALIGN_TPP
#define ALIGN_TPP
#include <Malena/Utilities/Align.h>

namespace ml
{
    // =========================================================================
    // Outline compensation helpers
    // =========================================================================

    // For shapes with an outline, getGlobalBounds().position is offset inward
    // by the outline thickness (position - thickness). When we want to place an
    // object so its *visual* edge lands at a target coordinate, we need to add
    // back the offset between the setPosition point and the visual top-left.
    //
    // For sf::Text, local bounds have a built-in ascender offset — we compensate
    // without touching the origin so as not to create positioning side effects.

    namespace detail
    {
        // Returns the offset from obj.setPosition() to its visual top-left.
        // For a shape with outline thickness T: offset = {T, T}.
        // For text or objects without outline: offset = {0, 0}.
        template<class S>
        sf::Vector2f posToVisualOffset(const S& obj)
        {
            sf::FloatRect bounds = obj.getGlobalBounds();
            sf::Vector2f  pos    = obj.getPosition();
            return { pos.x - bounds.position.x, pos.y - bounds.position.y };
        }
    }

    // =========================================================================
    // centerText — no origin modification
    // =========================================================================

    template<class T>
    void Align::centerText(const T& obj, sf::Text& text)
    {
        sf::FloatRect lb     = text.getLocalBounds();
        sf::FloatRect bounds = obj.getGlobalBounds();

        // Position the text so its visual centre aligns with bounds centre.
        // Assumes default origin {0,0} on the text — does not modify origin.
        text.setPosition({
            bounds.position.x + bounds.size.x / 2.f - lb.position.x - lb.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f - lb.position.y - lb.size.y / 2.f
        });
    }

    // =========================================================================
    // FloatRect overloads — canonical implementations
    // =========================================================================

    template<class S>
    void Align::setRightOf(const sf::FloatRect& bounds, S& obj, float spacing)
    {
        const sf::Vector2f off = detail::posToVisualOffset(obj);
        obj.setPosition({
            bounds.position.x + bounds.size.x + spacing + off.x,
            bounds.position.y + off.y
        });
    }

    template<class S>
    void Align::setLeftOf(const sf::FloatRect& bounds, S& obj, float spacing)
    {
        const sf::FloatRect objBounds = obj.getGlobalBounds();
        const sf::Vector2f  off       = detail::posToVisualOffset(obj);
        obj.setPosition({
            bounds.position.x - objBounds.size.x - spacing + off.x,
            bounds.position.y + off.y
        });
    }

    template<class S>
    void Align::setBelow(const sf::FloatRect& bounds, S& obj, float spacing)
    {
        const sf::Vector2f off = detail::posToVisualOffset(obj);
        obj.setPosition({
            bounds.position.x + off.x,
            bounds.position.y + bounds.size.y + spacing + off.y
        });
    }

    template<class S>
    void Align::setAbove(const sf::FloatRect& bounds, S& obj, float spacing)
    {
        const sf::FloatRect objBounds = obj.getGlobalBounds();
        const sf::Vector2f  off       = detail::posToVisualOffset(obj);
        obj.setPosition({
            bounds.position.x + off.x,
            bounds.position.y - objBounds.size.y - spacing + off.y
        });
    }

    template<class S>
    void Align::centerOn(const sf::FloatRect& bounds, S& obj)
    {
        if constexpr (std::is_base_of_v<sf::Text, S>)
        {
            sf::FloatRect lb = obj.getLocalBounds();
            obj.setPosition({
                bounds.position.x + bounds.size.x / 2.f - lb.position.x - lb.size.x / 2.f,
                bounds.position.y + bounds.size.y / 2.f - lb.position.y - lb.size.y / 2.f
            });
        }
        else
        {
            const sf::FloatRect objBounds = obj.getGlobalBounds();
            const sf::Vector2f  off       = detail::posToVisualOffset(obj);
            obj.setPosition({
                bounds.position.x + bounds.size.x / 2.f - objBounds.size.x / 2.f + off.x,
                bounds.position.y + bounds.size.y / 2.f - objBounds.size.y / 2.f + off.y
            });
        }
    }

    template<class S>
    void Align::centerVertically(const sf::FloatRect& bounds, S& obj)
    {
        if constexpr (std::is_base_of_v<sf::Text, S>)
        {
            sf::FloatRect lb = obj.getLocalBounds();
            obj.setPosition({
                obj.getPosition().x,
                bounds.position.y + bounds.size.y / 2.f - lb.position.y - lb.size.y / 2.f
            });
        }
        else
        {
            const sf::FloatRect objBounds = obj.getGlobalBounds();
            const sf::Vector2f  off       = detail::posToVisualOffset(obj);
            obj.setPosition({
                obj.getPosition().x,
                bounds.position.y + bounds.size.y / 2.f - objBounds.size.y / 2.f + off.y
            });
        }
    }

    template<class S>
    void Align::centerHorizontally(const sf::FloatRect& bounds, S& obj)
    {
        if constexpr (std::is_base_of_v<sf::Text, S>)
        {
            sf::FloatRect lb = obj.getLocalBounds();
            obj.setPosition({
                bounds.position.x + bounds.size.x / 2.f - lb.position.x - lb.size.x / 2.f,
                obj.getPosition().y
            });
        }
        else
        {
            const sf::FloatRect objBounds = obj.getGlobalBounds();
            const sf::Vector2f  off       = detail::posToVisualOffset(obj);
            obj.setPosition({
                bounds.position.x + bounds.size.x / 2.f - objBounds.size.x / 2.f + off.x,
                obj.getPosition().y
            });
        }
    }

    // =========================================================================
    // Object overloads — delegate to FloatRect versions
    // =========================================================================

    template<class T, class S>
    void Align::setRightOf(const T& constObj, S& obj, float spacing)
    {
        setRightOf(constObj.getGlobalBounds(), obj, spacing);
    }

    template<class T, class S>
    void Align::setLeftOf(const T& constObj, S& obj, float spacing)
    {
        setLeftOf(constObj.getGlobalBounds(), obj, spacing);
    }

    template<class T, class S>
    void Align::setBelow(const T& constObj, S& obj, float spacing)
    {
        setBelow(constObj.getGlobalBounds(), obj, spacing);
    }

    template<class T, class S>
    void Align::setAbove(const T& constObj, S& obj, float spacing)
    {
        setAbove(constObj.getGlobalBounds(), obj, spacing);  // was calling top() — bug fixed
    }

    template<class T, class S>
    void Align::centerOn(const T& constObj, S& obj)
    {
        centerOn(constObj.getGlobalBounds(), obj);
    }

    template<class T, class S>
    void Align::centerVertically(const T& constObj, S& obj)
    {
        centerVertically(constObj.getGlobalBounds(), obj);
    }

    template<class T, class S>
    void Align::centerHorizontally(const T& constObj, S& obj)
    {
        centerHorizontally(constObj.getGlobalBounds(), obj);
    }

    // =========================================================================
    // Vector2 overloads — treat as a rect anchored at the origin
    // =========================================================================

    template<class S, class U>
    void Align::setRightOf(const sf::Vector2<U>& size, S& obj, float spacing)
    {
        setRightOf(sf::FloatRect{{0.f, 0.f}, static_cast<sf::Vector2f>(size)}, obj, spacing);
    }

    template<class S, class U>
    void Align::setLeftOf(const sf::Vector2<U>& size, S& obj, float spacing)
    {
        setLeftOf(sf::FloatRect{{0.f, 0.f}, static_cast<sf::Vector2f>(size)}, obj, spacing);
    }

    template<class S, class U>
    void Align::setBelow(const sf::Vector2<U>& size, S& obj, float spacing)
    {
        setBelow(sf::FloatRect{{0.f, 0.f}, static_cast<sf::Vector2f>(size)}, obj, spacing);
        // was calling bottom() — bug fixed
    }

    template<class S, class U>
    void Align::setAbove(const sf::Vector2<U>& size, S& obj, float spacing)
    {
        setAbove(sf::FloatRect{{0.f, 0.f}, static_cast<sf::Vector2f>(size)}, obj, spacing);
        // was calling top() — bug fixed
    }

    template<class S, class U>
    void Align::centerOn(const sf::Vector2<U>& size, S& obj)
    {
        centerOn(sf::FloatRect{{0.f, 0.f}, static_cast<sf::Vector2f>(size)}, obj);
    }

    template<class S, class U>
    void Align::centerVertically(const sf::Vector2<U>& size, S& obj)
    {
        centerVertically(sf::FloatRect{{0.f, 0.f}, static_cast<sf::Vector2f>(size)}, obj);
    }

    template<class S, class U>
    void Align::centerHorizontally(const sf::Vector2<U>& size, S& obj)
    {
        centerHorizontally(sf::FloatRect{{0.f, 0.f}, static_cast<sf::Vector2f>(size)}, obj);
    }

} // namespace ml
#endif // ALIGN_TPP
