// Rich-text wrapping invariants.
//
// buildSegments() splits at every character when wrapping is on, so the caret
// can sit anywhere and an unbreakable word can still be broken. The placement
// pass therefore has to put words back together: wrapping per SEGMENT chopped
// words in half ("submission" -> "submiss/ion").
//
// Rows are recovered through the public API — charIndexToPosition().y changes
// when a character lands on a new row.

#include <Malena/Graphics/Text/RichTextBuffer.h>
#include <Malena/Graphics/Text/RichTextRenderer.h>
#include <SFML/Graphics/Font.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace {

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        ++failures; \
        std::cerr << "FAIL " << __LINE__ << ": " << #cond << "\n"; \
    } \
} while (0)

bool loadFont(sf::Font& f)
{
    for (const char* p : { "/System/Library/Fonts/Helvetica.ttc",
                           "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                           "/usr/share/fonts/dejavu/DejaVuSans.ttf",
                           "C:/Windows/Fonts/arial.ttf" })
        if (f.openFromFile(p)) return true;
    return false;
}

// Buffer index of the first GLYPH on each rendered row, in order.
//
// Read from the rendered segments rather than charIndexToPosition(): that is a
// CARET mapping, and at a wrap boundary the index matches the previous
// segment's end (index <= bufEnd) so it reports the row above. Fine for a
// cursor, useless for asking where a character was drawn.
std::vector<std::size_t> rowStarts(const ml::RichTextRenderer& r, std::size_t)
{
    std::vector<std::size_t> starts;
    bool  first = true;
    float lastY = 0.f;
    for (const auto& line : r.getLines())
        for (const auto& seg : line.segments)
        {
            if (first || seg.position.y != lastY)
            { starts.push_back(seg.bufStart); lastY = seg.position.y; first = false; }
        }
    return starts;
}

void test_wraps_between_words(const sf::Font& font)
{
    const std::string text =
        "Two quick questions verifying delivery and submission of the answers";

    ml::RichTextBuffer buf;
    buf.setText(text);
    ml::RichTextRenderer r(buf, font, 20u, sf::Color::White);
    r.setOrigin({0.f, 0.f});
    r.setMaxWidth(220.f);
    r.rebuild();

    const auto starts = rowStarts(r, text.size());
    CHECK(starts.size() > 1);            // it must actually wrap at this width


    // Every row after the first must begin a word: the character before it is a
    // space. This is the check that failed before — rows began mid-word.
    for (std::size_t k = 1; k < starts.size(); ++k)
    {
        const std::size_t i = starts[k];
        const bool startsWord = (i > 0 && text[i - 1] == ' ');
        if (!startsWord)
            std::cerr << "  row " << k << " starts mid-word at " << i
                      << " (\"" << text.substr(i > 6 ? i - 6 : 0, 12) << "\")\n";
        CHECK(startsWord);
    }
}

void test_long_word_still_breaks(const sf::Font& font)
{
    // No break opportunity exists, so it must hard-break rather than run off
    // the edge — and must terminate.
    const std::string text = "Pneumonoultramicroscopicsilicovolcanoconiosis";

    ml::RichTextBuffer buf;
    buf.setText(text);
    ml::RichTextRenderer r(buf, font, 20u, sf::Color::White);
    r.setOrigin({0.f, 0.f});
    r.setMaxWidth(120.f);
    r.rebuild();

    const auto starts = rowStarts(r, text.size());
    CHECK(starts.size() > 1);            // broke somewhere
    for (const auto& line : r.getLines())
        for (const auto& seg : line.segments)
            CHECK(seg.position.x < 120.f + 40.f);           // nothing ran away
}

void test_short_text_does_not_wrap(const sf::Font& font)
{
    ml::RichTextBuffer buf;
    buf.setText("Short line");
    ml::RichTextRenderer r(buf, font, 20u, sf::Color::White);
    r.setOrigin({0.f, 0.f});
    r.setMaxWidth(600.f);
    r.rebuild();

    CHECK(rowStarts(r, buf.getText().size()).size() == 1);
}

void test_reflow_matches_layout(const sf::Font& font)
{
    // reflow() used to carry its own copy of the placement loop and drifted from
    // layoutLines(); since setOrigin() calls reflow() every frame, its copy was
    // the one on screen. Same origin must give the same rows.
    const std::string text =
        "Two quick questions verifying delivery and submission of the answers";

    ml::RichTextBuffer buf;
    buf.setText(text);
    ml::RichTextRenderer r(buf, font, 20u, sf::Color::White);
    r.setOrigin({0.f, 0.f});
    r.setMaxWidth(220.f);
    r.rebuild();
    const auto afterBuild = rowStarts(r, text.size());

    r.setOrigin({0.f, 0.f});             // triggers reflow()
    const auto afterReflow = rowStarts(r, text.size());

    CHECK(afterBuild == afterReflow);
}

} // namespace

int main()
{
    sf::Font font;
    if (!loadFont(font))
    {
        std::cout << "TextWrap: skipped (no system font available)\n";
        return 0;
    }

    test_wraps_between_words(font);
    test_long_word_still_breaks(font);
    test_short_text_does_not_wrap(font);
    test_reflow_matches_layout(font);

    if (failures == 0) { std::cout << "TextWrap: all checks passed\n"; return 0; }
    std::cerr << "TextWrap: " << failures << " check(s) failed\n";
    return 1;
}
