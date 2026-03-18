#ifndef SPRITE_H
#define SPRITE_H

#pragma once

#include <Malena/Graphics/Base/Graphic.h>
#include <Malena/Traits/Customizable.h>

namespace ml
{
    /**
     * @brief A framework-integrated sprite.
      * @ingroup GraphicsSprites
     *
     * @c Sprite wraps @c sf::Sprite inside @c Graphic<>, giving it the full
     * framework trait set: @c Subscribable (click, hover, update),
     * @c Flaggable, @c Positionable, and @c Draggable. Use it anywhere you
     * would use @c sf::Sprite but want event callbacks and manager integration.
     *
     * ### Usage
     * @code
     * ml::Sprite player;
     * player.setTexture(ml::TextureManager<MyManifest>::get(MyManifest::Images::Player));
     * player.setPosition({200.f, 300.f});
     * player.onClick([]{ std::cout << "player clicked\n"; });
     * addComponent(player);
     * @endcode
     *
     * ### With a manifest
     * @c SpriteWith<Manifest> layers a manifest's flags, states, and resource
     * access onto the sprite:
     * @code
     * class PlayerSprite : public ml::SpriteWith<PlayerManifest> {
     *     PlayerSprite() {
     *         setTexture(AssetMgr::get(PlayerManifest::Images::Idle));
     *     }
     * };
     * @endcode
     *
     * @see Graphic, TextureManager, AssetsManager
     */
    class Sprite : public Graphic<sf::Sprite>
    {
    public:
        using Graphic::Graphic;
    };

    /**
     * @brief @c Sprite with an attached manifest.
     *
     *
     * @tparam MANIFEST A @c Manifest subclass declaring @c Flag/@c State enums
     *                  and optionally @c Images for texture access.
     *
     * @see Sprite, Customizable
     */
    template<typename MANIFEST>
    class SpriteWith : public Sprite, public Customizable<MANIFEST> {}; // TODO: rename to SpriteWith

} // namespace ml

#endif // SPRITE_H
