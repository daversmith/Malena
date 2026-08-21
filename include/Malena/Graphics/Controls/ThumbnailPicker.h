// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_THUMBNAILPICKER_H
#define MALENA_THUMBNAILPICKER_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ml
{
    class MALENA_API ThumbnailPickerManifest : public ml::Manifest
    {
    public:
        enum class Flag { ACTIVE };
    };

    /**
     * @brief A grid of captioned image thumbnails from which exactly one is
     *        selectable — a Zoom-style "what do you want to share?" picker.
     * @ingroup GraphicsControls
     *
     * Self-contained: it owns each thumbnail's @c sf::Texture, lays the tiles out
     * in a grid, draws them with a caption and a highlight on the selected tile,
     * and resolves clicks itself by hit-testing the tile rectangles against the
     * mouse (the same approach @c Modal uses), so it needs no child components.
     *
     * Designed to be handed to a @c Modal as its content (it is window-centred and
     * drawn by the modal). Call @c setActive(true) while the modal is up so clicks
     * are interpreted, and read @c getSelectedIndex() on confirm.
     *
     * ### Usage
     * @code
     * ml::ThumbnailPicker picker;
     * picker.addThumbnail(displayImage, "Display 1  (1920x1080, main)");
     * picker.addThumbnail(windowImage,  "Keynote — Lecture 3");
     * picker.onSelectionChanged([](int i){  });
     * modal.setContent(picker);
     * @endcode
     */
    class MALENA_API ThumbnailPicker : public ComponentWith<ThumbnailPickerManifest>
    {
    public:
        using Flag = ThumbnailPickerManifest::Flag;

    private:
        std::vector<std::unique_ptr<sf::Texture>> _textures;
        std::vector<std::string>                  _captions;
        std::vector<sf::FloatRect>                _cells;   // world-space tile rects

        const sf::Font* _font;
        int             _selected = 0;
        int             _columns  = 3;
        float           _tileW    = 184.f;
        float           _tileH    = 128.f;
        float           _gap      = 16.f;
        float           _captionH = 24.f;
        sf::Vector2f    _position = {0.f, 0.f};
        bool            _active   = false;
        bool            _prevDown = false;

        std::function<void(int)> _onSelectionChanged;

        void layout();

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        explicit ThumbnailPicker(const sf::Font& font = FontManager<>::getDefault());

        ThumbnailPicker(const ThumbnailPicker&)            = delete;
        ThumbnailPicker& operator=(const ThumbnailPicker&) = delete;

        // ── Content ───────────────────────────────────────────────────────────
        void clear();
        void addThumbnail(const sf::Image& image, const std::string& caption);
        [[nodiscard]] int count() const;

        // ── Layout ────────────────────────────────────────────────────────────
        void setColumns(int columns);
        void setTileSize(float w, float h);

        // ── Selection ─────────────────────────────────────────────────────────
        void setSelectedIndex(int index);
        [[nodiscard]] int getSelectedIndex() const;
        void onSelectionChanged(std::function<void(int)> cb);

        /** While active the picker hit-tests clicks; set false when not shown. */
        void setActive(bool active);

        // ── Positionable ──────────────────────────────────────────────────────
        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

} // namespace ml

#endif // MALENA_THUMBNAILPICKER_H
