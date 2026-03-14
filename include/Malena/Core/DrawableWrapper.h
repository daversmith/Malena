//
// Created by Dave Smith on 3/14/26.
//

#ifndef DRAWABLEWRAPPER_H
#define DRAWABLEWRAPPER_H
namespace ml
{
    template<typename T, typename = void>
    struct HasSetPosition : std::false_type {};
    template<typename T>
    struct HasSetPosition<T, std::void_t<
        decltype(std::declval<T>().setPosition(std::declval<sf::Vector2f>()))
    >> : std::true_type {};

    template<typename T, typename = void>
    struct HasGetPosition : std::false_type {};
    template<typename T>
    struct HasGetPosition<T, std::void_t<
        decltype(std::declval<T>().getPosition())
    >> : std::true_type {};

    template<typename T, typename = void>
    struct HasGetGlobalBounds : std::false_type {};
    template<typename T>
    struct HasGetGlobalBounds<T, std::void_t<
        decltype(std::declval<T>().getGlobalBounds())
    >> : std::true_type {};

    // ── DrawableWrapper ───────────────────────────────────────────────────────
    // Makes any sf::Drawable into an ml::Core.
    // Used internally by GamePlugin — students never see this.

    template<typename ENTITY>
    class DrawableWrapper : public ENTITY,  public ml::Core
    {

    public:
        using ENTITY::ENTITY;

        void setPosition(const sf::Vector2f& pos) override
        {
            if constexpr (HasSetPosition<ENTITY>::value)
                ENTITY::setPosition(pos);
        }

        sf::Vector2f getPosition() const override
        {
            if constexpr (HasGetPosition<ENTITY>::value)
                return ENTITY::getPosition();
            return {};
        }

        sf::FloatRect getGlobalBounds() const override
        {
            if constexpr (HasGetGlobalBounds<ENTITY>::value)
                return ENTITY::getGlobalBounds();
            auto size = ml::WindowManager::getWindow().getSize();
            return sf::FloatRect({0.f, 0.f}, sf::Vector2f(size));
        }
    };
}//namespace ml
#endif //DRAWABLEWRAPPER_H
