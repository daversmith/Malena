//
// Table.h
//

#ifndef MALENA_TABLE_H
#define MALENA_TABLE_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Manifests/Theme.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace ml
{
    class MALENA_API TableManifest : public ml::Manifest
    {
    public:
        enum class Flag  {};
        enum class State {};
    };

    /**
     * @brief A fixed-size, multi-column data table with a header row.
     * @ingroup GraphicsControls
     *
     * Columns are defined once via @c addColumn(); rows are appended with
     * @c addRow() and removed with @c clear() or @c removeRow(). When more
     * rows exist than can fit in the visible area the table shows the
     * *newest* rows (tail view) — no scrollbar, just a live window.
     *
     * ### Usage
     * @code
     * ml::Table table;
     * table.setSize({600.f, 400.f});
     * table.setPosition({20.f, 100.f});
     * table.addColumn("App",       300.f);
     * table.addColumn("Event",     300.f);
     * table.setMaxRows(200);
     *
     * table.addRow({"Chrome", ""});
     * table.addRow({"",       "copy"});
     *
     * addComponent(table);
     * @endcode
     *
     * Column widths of 0 share whatever space remains after fixed columns.
     *
     * @see TableManifest
     */
    class MALENA_API Table : public ComponentWith<TableManifest>, public Themeable
    {
    public:
        using Flag  = TableManifest::Flag;
        using State = TableManifest::State;

    private:
        struct Column
        {
            std::string header;
            float       width;  // 0 = flex
        };
        using Row = std::vector<std::string>;

        std::vector<Column> _columns;
        std::vector<Row>    _rows;
        std::size_t         _maxRows = 0;  // 0 = unlimited
        std::unordered_map<std::size_t, sf::Color> _rowColors;

        const sf::Font* _font;
        sf::Vector2f    _position    = {0.f, 0.f};
        sf::Vector2f    _size        = {400.f, 300.f};
        float           _rowHeight   = 28.f;
        float           _headerHeight= 32.f;
        float           _padding     = 8.f;
        unsigned int    _fontSize    = 13u;
        unsigned int    _headerFontSize = 13u;

        sf::Color _headerBgColor   = sf::Color(55,  55,  65);
        sf::Color _headerTxtColor  = sf::Color(200, 200, 210);
        sf::Color _rowBgColor      = sf::Color(30,  30,  30);
        sf::Color _altRowBgColor   = sf::Color(38,  38,  45);
        sf::Color _textColor       = sf::Color(220, 220, 220);
        sf::Color _dividerColor    = sf::Color(70,  70,  80);
        sf::Color _borderColor     = sf::Color(70,  70,  80);

        std::vector<float> resolveWidths() const;

        void drawHeader  (sf::RenderTarget&, const sf::RenderStates&,
                          const std::vector<float>& widths) const;
        void drawDataRow (sf::RenderTarget&, const sf::RenderStates&,
                          const Row&, float y, bool alt,
                          const std::vector<float>& widths,
                          const sf::Color* bgOverride = nullptr) const;

        static std::string clampText(const sf::Font&, unsigned charSize,
                                     const std::string& text, float maxWidth);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit Table(const sf::Font& font = FontManager<>::getDefault());

        Table(const Table&)            = delete;
        Table& operator=(const Table&) = delete;

        // ── Column / row management ───────────────────────────────────────────

        /**
         * @brief Define a column. Call before adding rows.
         * @param header  Column header label.
         * @param width   Width in pixels; 0 = distribute remaining space evenly.
         */
        void addColumn(const std::string& header, float width = 0.f);

        /**
         * @brief Append a data row.
         *
         * Extra cells are ignored; missing cells are left blank.
         * If @c setMaxRows() was called and the row count would exceed the
         * limit, the oldest row is discarded first.
         */
        void addRow(const std::vector<std::string>& cells);

        /** @brief Remove a row by index (0 = oldest). */
        bool removeRow(std::size_t index);

        /** @brief Remove all data rows (columns are kept). */
        void clear();

        /** @brief Remove all data rows AND all column definitions. */
        void reset();

        /**
         * @brief Override the background colour of a specific row.
         *
         * The override persists until @c clearRowColor() or @c clear() is
         * called. Indices shift automatically when rows are inserted or removed.
         *
         * @param rowIndex  0-based index of the row to highlight.
         * @param color     Background colour to use instead of the default.
         */
        void setRowColor(std::size_t rowIndex, sf::Color color);

        /** @brief Remove the colour override for a specific row. */
        void clearRowColor(std::size_t rowIndex);

        /**
         * @brief Cap the number of stored rows; oldest are dropped on overflow.
         * Pass 0 to disable the limit.
         */
        void setMaxRows(std::size_t max);

        [[nodiscard]] std::size_t rowCount()    const { return _rows.size(); }
        [[nodiscard]] std::size_t columnCount() const { return _columns.size(); }

        // ── Appearance ────────────────────────────────────────────────────────

        void setRowHeight   (float h)        { _rowHeight    = h; }
        void setHeaderHeight(float h)        { _headerHeight = h; }
        void setFontSize    (unsigned int s) { _fontSize     = s; }
        void setPadding     (float p)        { _padding      = p; }

        void setHeaderBgColor  (const sf::Color& c) { _headerBgColor  = c; }
        void setHeaderTextColor(const sf::Color& c) { _headerTxtColor = c; }
        void setRowBgColor     (const sf::Color& c) { _rowBgColor     = c; }
        void setAltRowBgColor  (const sf::Color& c) { _altRowBgColor  = c; }
        void setTextColor      (const sf::Color& c) { _textColor      = c; }
        void setDividerColor   (const sf::Color& c) { _dividerColor   = c; }
        void setBorderColor    (const sf::Color& c) { _borderColor    = c; }

        // ── Size / position ───────────────────────────────────────────────────

        void         setSize(const sf::Vector2f& size) { _size = size; }
        sf::Vector2f getSize() const                   { return _size; }

        void          setPosition(const sf::Vector2f& pos) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class TableWith : public Table, public Customizable<MANIFEST>
    { public: using Table::Table; };

} // namespace ml

#endif // MALENA_TABLE_H
