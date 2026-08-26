// UTF-8 invariants — the "em-dash renders as three boxes" defect class.
//
// Malena stores text as UTF-8 std::string but draws it through sf::String, and
// sf::String converts to/from std::string using the LOCALE (effectively
// Latin-1). Both directions are implicit, so every conversion was a silent
// corruption site:
//   * std::string -> sf::String  gave one codepoint per BYTE ("—" = 3 tofu)
//   * sf::String  -> std::string gave toAnsiString(), turning "—" into '?'
// On top of that RichTextBuffer is byte-indexed, so caret/delete/segmentation
// could cut a character in half — unrecoverable no matter how it's decoded.
//
// These checks lock all three layers. Window-free.

#include <Malena/Utilities/Utf8.h>
#include <Malena/Utilities/TextManipulators.h>
#include <Malena/Graphics/Text/RichTextBuffer.h>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <iostream>
#include <string>

namespace {

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        ++failures; \
        std::cerr << "FAIL " << __LINE__ << ": " << #cond << "\n"; \
    } \
} while (0)

// Characters that actually show up in teaching material.
const std::string EM_DASH  = "\xE2\x80\x94";        // U+2014  3 bytes
const std::string E_ACUTE  = "\xC3\xA9";            // U+00E9  2 bytes
const std::string ARROW    = "\xE2\x86\x92";        // U+2192  3 bytes

void test_decode_counts_characters_not_bytes()
{
    // The original bug: 3 bytes became 3 glyphs.
    CHECK(ml::utf8(EM_DASH).getSize() == 1);
    CHECK(ml::utf8(E_ACUTE).getSize() == 1);
    CHECK(ml::utf8("Caf" + E_ACUTE).getSize() == 4);
    CHECK(ml::utf8(EM_DASH)[0] == 0x2014);
    CHECK(ml::utf8(E_ACUTE)[0] == 0x00E9);
}

void test_round_trip_is_lossless()
{
    // The mirror bug: toAnsiString() replaced these with '?'.
    const std::string s = "Caf" + E_ACUTE + " " + EM_DASH + " 40" + "\xC2\xB0" + "C";
    CHECK(ml::toUtf8(ml::utf8(s)) == s);
    // Pass-through overload makes wrapping a call site always safe.
    CHECK(ml::utf8(ml::utf8(s)) == ml::utf8(s));
}

void test_wordwrap_preserves_non_ascii()
{
    // wordwrap round-trips text through std::string and an sf::Text probe; it
    // must not drop characters on the way. Needs a real font, so it is skipped
    // when none is available (window-free CI images may have no font file).
    sf::Font font;
    if (!font.openFromFile("resources/fonts/Outfit-Regular.ttf") &&
        !font.openFromFile("/System/Library/Fonts/Helvetica.ttc") &&
        !font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
    {
        std::cout << "Utf8Text: wordwrap check skipped (no font)\n";
        return;
    }
    const std::string in  = "alpha " + EM_DASH + " beta " + E_ACUTE;
    const std::string out = ml::TextManipulators::wordwrap(in, font, 20u, 10000.f);
    CHECK(out.find(EM_DASH) != std::string::npos);
    CHECK(out.find(E_ACUTE) != std::string::npos);

    // And via sf::Text, which is how ml::Text actually calls it.
    sf::Text probe(font, ml::utf8(in), 20u);
    const std::string viaText = ml::TextManipulators::wordwrap(probe, 10000.f);
    CHECK(viaText.find(EM_DASH) != std::string::npos);   // was '?' before the fix
    CHECK(viaText.find(E_ACUTE) != std::string::npos);
}

void test_caret_steps_whole_characters()
{
    ml::RichTextBuffer buf;
    buf.setText("a" + EM_DASH + "b");          // bytes: 0 | 1,2,3 | 4
    buf.setCursor(0);

    buf.moveCursor(+1, false);
    CHECK(buf.getCursor() == 1);
    buf.moveCursor(+1, false);
    CHECK(buf.getCursor() == 4);               // skipped all 3 bytes, not 1
    buf.moveCursor(+1, false);
    CHECK(buf.getCursor() == 5);

    buf.moveCursor(-1, false);
    CHECK(buf.getCursor() == 4);
    buf.moveCursor(-1, false);
    CHECK(buf.getCursor() == 1);               // back over the whole character
}

void test_delete_removes_whole_characters()
{
    // Backspace must not leave a dangling continuation byte.
    ml::RichTextBuffer back;
    back.setText("a" + EM_DASH);
    back.setCursor(4);
    back.backspace();
    CHECK(back.getText() == "a");

    ml::RichTextBuffer fwd;
    fwd.setText(EM_DASH + "b");
    fwd.setCursor(0);
    fwd.deleteForward();
    CHECK(fwd.getText() == "b");

    // And the text left behind must still decode to what it looks like.
    ml::RichTextBuffer mixed;
    mixed.setText("Caf" + E_ACUTE + ARROW);
    mixed.setCursor(static_cast<std::size_t>(mixed.getText().size()));
    mixed.backspace();
    CHECK(mixed.getText() == "Caf" + E_ACUTE);
    CHECK(ml::utf8(mixed.getText()).getSize() == 4);
}

} // namespace

int main()
{
    test_decode_counts_characters_not_bytes();
    test_round_trip_is_lossless();
    test_wordwrap_preserves_non_ascii();
    test_caret_steps_whole_characters();
    test_delete_removes_whole_characters();

    if (failures == 0) { std::cout << "Utf8Text: all checks passed\n"; return 0; }
    std::cerr << "Utf8Text: " << failures << " check(s) failed\n";
    return 1;
}
