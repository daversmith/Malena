//
// Created by Dave Smith on 4/8/25.
//

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H
#include <algorithm>
#include <unordered_map>
#include <SFML/Graphics/Texture.hpp>
namespace ml
{
    struct EnumClassHash {
        template <typename T>
        std::size_t operator()(T t) const {
            return static_cast<std::size_t>(t);
        }
    };

    template <typename Manifest>
    class TextureManager {
    public:
        using Image = typename Manifest::Image;

        static const sf::Texture& getTexture(Image image) {
            //Verify there is a Image enum in the class
            static_assert(
                std::is_enum<Image>::value,
                "Manifest must have a nested enum class called 'Image'"
            );
            // Verify getFilepath exists
            static_assert(
                std::is_same_v<
                    decltype(Manifest::getFilepath(image)),
                    std::string
                >,
                "Manifest must have a static getFilepath(Image) method returning std::string"
            );

            static std::unordered_map<Image, sf::Texture, EnumClassHash> textures;

            auto it = textures.find(image);
            if (it != textures.end()) {
                return it->second;
            }

            sf::Texture texture;
            if (!texture.loadFromFile(Manifest::getFilepath(image))) {
                throw std::runtime_error("Failed to load texture");
            }

            textures[image] = std::move(texture);
            return textures[image];
        }

        static const sf::Texture& unloadTexture(Image image)
        {
            //Verify there is a Image enum in the class
            static_assert(
                std::is_enum<Image>::value,
                "Manifest must have a nested enum class called 'Image'"
            );
            // Verify getFilepath exists
            static_assert(
                std::is_same_v<
                    decltype(Manifest::getFilepath(image)),
                    std::string
                >,
                "Manifest must have a static getFilepath(Image) method returning std::string"
            );

            static std::unordered_map<Image, sf::Texture, EnumClassHash> textures;
            textures.erase(image);
        }
    };

}


#endif //TEXTUREMANAGER_H
