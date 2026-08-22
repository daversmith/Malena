//
// UTF-8 -> sf::String decoding.
//

#ifndef MALENA_UTF8_H
#define MALENA_UTF8_H

#include <SFML/System/String.hpp>
#include <string>

namespace ml
{
    /**
     * @brief Decode a UTF-8 byte string for display.
     *
     * WHY THIS EXISTS: @c sf::String's implicit constructor from @c std::string
     * and @c const char* decodes using the current locale — effectively Latin-1 —
     * so it emits one codepoint PER BYTE. Every multi-byte character therefore
     * renders as a run of tofu boxes: an em-dash ("—", bytes E2 80 94) became
     * three boxes, a curly quote two. Because the conversion is implicit, this
     * happens silently at any site that hands a @c std::string to @c sf::Text.
     *
     * Everything the app displays — JSON from the server, file contents, typed
     * input — is UTF-8, so route it through here.
     *
     * The @c sf::String overload is a pass-through, which means wrapping a call
     * site is ALWAYS safe: it is correct whether the argument is raw bytes or an
     * already-decoded string, and applying it twice does nothing.
     */
    inline sf::String utf8(const std::string& s)
    {
        return sf::String::fromUtf8(s.begin(), s.end());
    }

    inline sf::String utf8(const char* s)
    {
        if (!s) return {};
        const std::string t(s);
        return sf::String::fromUtf8(t.begin(), t.end());
    }

    inline const sf::String& utf8(const sf::String& s) { return s; }

    /**
     * @brief Encode an sf::String back to UTF-8 bytes.
     *
     * The mirror hazard: sf::String converts IMPLICITLY to std::string via
     * @c toAnsiString(), which replaces every codepoint above 255 with '?'. That
     * silently destroyed text on any round trip — word-wrapping a paragraph
     * turned "—" into "?". Use this whenever an sf::String has to become a
     * std::string.
     */
    inline std::string toUtf8(const sf::String& s)
    {
        const auto bytes = s.toUtf8();
        return std::string(bytes.begin(), bytes.end());
    }
}

#endif //MALENA_UTF8_H
