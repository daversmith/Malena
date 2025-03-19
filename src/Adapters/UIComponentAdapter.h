//
// Created by Dave Smith on 3/7/25.
//

#ifndef UICOMPONENTADAPTER_H
#define UICOMPONENTADAPTER_H

#include "../Interfaces/UIComponent.h"
#include <SFML/Graphics/RenderTarget.hpp>

namespace ml {

    class UIComponentAdapter : public UIComponent
    {
    protected:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    public:
        virtual void update();

        virtual void eventHandler();

        [[nodiscard]] sf::FloatRect getGlobalBounds() const override;

        void setPosition(const sf::Vector2f &position) override;

        sf::Vector2f getPosition() const override;
    };

} // namespace ml

#endif //UICOMPONENTADAPTER_H


