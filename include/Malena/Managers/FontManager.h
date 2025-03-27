#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "SFML/Graphics/Font.hpp"

namespace ml
{

	/**
	 * @brief Manages loading, storing, and retrieving sf::Font objects.
	 *
	 * This class provides a centralized way to handle fonts within an application.
	 * It supports loading fonts from memory or files, provides access to built-in fonts,
	 * and uses a singleton pattern for global access.
	 */
	class FontManager
	{
	public:
		/// Default identifier for the built-in Arial font.
		static constexpr const char *ARIAL = "arial";

		/**
		 * @brief Represents the identified format of font data after validation.
		 */
		enum class FontFormat
		{
			UNKNOWN, ///< Format could not be determined or is not yet supported for specific identification.
			TRUETYPE, ///< .ttf - TrueType font format. Also includes OpenType fonts with TrueType outlines if not specifically identified as OPENTYPE_TTF.
			OPENTYPE_CFF, ///< .otf - OpenType font with CFF (PostScript) outline data. Magic number 'OTTO'.
			OPENTYPE_TTF, ///< .otf/.ttf - OpenType font specifically identified as having TrueType outlines (often shares .ttf extension or TrueType magic number).
			TYPE1, ///< .pfb/.pfa - Type 1 PostScript font (binary or ASCII).
			CFF, ///< .cff - Compact Font Format (often embedded in OpenType CFF).
			SFNT, ///< General SFNT container format (base for TrueType, OpenType, WOFF, WOFF2). Used when specific type isn't determined (e.g., for WOFF/WOFF2).
			X11_PCF, ///< .pcf - Portable Compiled Format, common on X11 systems.
			WINDOWS_FNT, ///< .fnt/.fon - Windows bitmap font format.
			BDF, ///< .bdf - Bitmap Distribution Format, an ASCII-based bitmap format.
			PFR, ///< .pfr - Portable Font Resource.
			TYPE42, ///< PostScript Type 42 font (TrueType font wrapped in PostScript).
			INVALID ///< The provided data does not correspond to a known, valid font format.
		};

		/**
		 * @brief Gets the singleton instance of the FontManager.
		 * @details This manager uses the Singleton pattern.
		 * @return Reference to the single FontManager instance.
		 */
		static FontManager &getInstance();

		/**
		 * @brief Gets the default font.
		 * @details Returns a reference to the font designated as the default (currently ARIAL).
		 *          If the default font failed to load during initialization, this might
		 *          return a default-constructed (empty) sf::Font.
		 * @return Constant reference to the default sf::Font object.
		 * @see ARIAL
		 */
		static const sf::Font &getDefault();

		/**
		 * @brief Retrieves a font by its unique string identifier.
		 * @param id The unique identifier of the font to retrieve.
		 * @return Constant reference to the requested sf::Font. If the font with the
		 *         given ID is not found, it returns the default font and logs a warning.
		 *         If the default font is also unavailable, returns an empty fallback font
		 *         and logs an error.
		 * @see getDefault()
		 */
		static const sf::Font &getFont(std::string_view id);

		/**
		 * @brief Loads a custom font into the manager from a memory buffer.
		 * @details The provided data is validated before attempting to load. The font
		 *          can later be retrieved using the provided ID. The data buffer pointed
		 *          to by `data` must remain valid for the lifetime of the sf::Font object,
		 *          as sf::Font::openFromMemory does not copy the buffer internally by default.
		 *          However, SFML's implementation *does* copy the data, so the original
		 *          buffer can typically be discarded after this call returns successfully.
		 * @param id The unique identifier to assign to this font. Must not be empty.
		 * @param data Pointer to the beginning of the font data in memory. Must not be null.
		 * @param size The size of the font data in bytes. Must be greater than 0.
		 * @return true if the font was successfully validated and loaded, false otherwise.
		 *         Logs an error message on failure (e.g., invalid parameters, invalid font data, SFML loading error).
		 * @see validateFont()
		 * @see getFont()
		 */
		static bool loadFont(std::string_view id, const void *data, size_t size);

		/**
		 * @brief Loads a custom font into the manager from a file path.
		 * @details Reads the entire file content into memory, validates it, and then loads it
		 *          using `loadFont`. The font can later be retrieved using the provided ID.
		 * @param id The unique identifier to assign to this font. Must not be empty.
		 * @param filePath The path to the font file on the filesystem. Must not be empty.
		 * @return true if the file was read and the font was successfully validated and loaded,
		 *         false otherwise. Logs an error message on failure (e.g., invalid parameters, file read error, invalid font data, SFML loading error).
		 * @see validateFont()
		 * @see loadFont()
		 * @see getFont()
		 */
		static bool loadFontFromFile(std::string_view id, std::string_view filePath);

		/**
		 * @brief Checks if a font with the specified identifier is currently loaded in the manager.
		 * @param id The unique identifier of the font to check.
		 * @return true if a font with the given ID exists (either built-in or custom loaded), false otherwise.
		 */
		static bool hasFont(std::string_view id);

		/**
		 * @brief Unloads a custom font from the manager.
		 * @details Removes the font associated with the given ID, freeing its resources.
		 *          Built-in fonts cannot be unloaded. Attempting to unload a non-existent
		 *          font or a built-in font will result in a warning log message and return false.
		 * @param id The unique identifier of the font to unload.
		 * @return true if the font was successfully unloaded, false if the font was not found
		 *         or if it is a built-in font.
		 * @see isBuiltInFont()
		 */
		static bool unloadFont(std::string_view id);

		/**
		 * @brief Checks if the given font identifier corresponds to a built-in font.
		 * @details Built-in fonts are loaded automatically during manager initialization
		 *          and cannot be unloaded.
		 * @param id The unique identifier of the font to check.
		 * @return true if the font ID represents a built-in font, false otherwise.
		 * @see unloadFont()
		 */
		static bool isBuiltInFont(std::string_view id);

		/**
		 * @brief Validates raw font data and attempts to identify its format.
		 * @details Checks magic numbers and signatures within the data buffer to determine
		 *          the potential font format. This is used internally before loading but
		 *          can be used externally for pre-validation or informational purposes.
		 * @param data Pointer to the beginning of the potential font data in memory. Must not be null.
		 * @param size The size of the data buffer in bytes. A minimum size (e.g., 4 bytes) is required for basic validation.
		 * @return FontFormat enum value indicating the identified format, or FontFormat::INVALID
		 *         if the data does not match known font signatures, is too small, or the pointer is null.
		 */
		static FontFormat validateFont(const void *data, size_t size);

	private:
		FontManager();

		FontManager(const FontManager &) = delete;
		FontManager &operator=(const FontManager &) = delete;

		static FontManager &instance();

		std::unordered_map<std::string, sf::Font> fonts;
		std::vector<std::string> builtInFontIds{}; // ids of builtin fonts
		sf::Font fallbackFont; // empty font used if default fails

		// Private implementation methods called by static public methods
		void loadBuiltinFonts();
		const sf::Font &getDefaultImpl();
		const sf::Font &getFontImpl(std::string_view id);
		bool loadFontImpl(std::string_view id, const void *data, size_t size);
		bool loadFontFromFileImpl(std::string_view id, std::string_view filePath);
		bool hasFontImpl(std::string_view id);
		bool unloadFontImpl(std::string_view id);
		bool isBuiltInFontImpl(std::string_view id);
		FontFormat validateFontImpl(const void *data, size_t size);
	};

} // namespace ml

#endif // FONTMANAGER_H