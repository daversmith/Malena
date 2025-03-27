#include <Malena/Managers/FontManager.h>
#include <Malena/Managers/Fonts/arial.h>

#include <cstring>
#include <fstream>
#include <algorithm>

namespace ml
{
	// static methods for the public api
	const sf::Font &FontManager::getDefault()
	{
		return instance().getDefaultImpl();
	}

	const sf::Font &FontManager::getFont(const std::string_view id)
	{
		return instance().getFontImpl(id);
	}

	bool FontManager::loadFont(const std::string_view id, const void *data, size_t size)
	{
		return instance().loadFontImpl(id, data, size);
	}

	bool FontManager::loadFontFromFile(const std::string_view id, const std::string_view filePath)
	{
		return instance().loadFontFromFileImpl(id, filePath);
	}

	bool FontManager::hasFont(const std::string_view id)
	{
		return instance().hasFontImpl(id);
	}

	bool FontManager::unloadFont(const std::string_view id)
	{
		return instance().unloadFontImpl(id);
	}

	bool FontManager::isBuiltInFont(const std::string_view id)
	{
		return instance().isBuiltInFontImpl(id);
	}

	FontManager::FontFormat FontManager::validateFont(const void *data, const size_t size)
	{
		return instance().validateFontImpl(data, size);
	}

	FontManager &FontManager::instance()
	{
		static FontManager instance;
		return instance;
	}

	FontManager::FontManager()
	{
		fallbackFont = sf::Font();
		loadBuiltinFonts();
	}

	// impl methods
	const sf::Font &FontManager::getDefaultImpl()
	{
		return getFontImpl(ARIAL);
	}

	const sf::Font &FontManager::getFontImpl(const std::string_view id)
	{

		auto it = fonts.find(id.data());
		if (it != fonts.end())
		{
			return it->second;
		}
		if (id != ARIAL)
		{
			std::cerr << "Warning: Font '" << id << "' not found. Using default font instead." << std::endl;
			it = fonts.find(ARIAL);
			if (it != fonts.end())
			{
				return it->second;
			}
		}

		std::cerr << "Error: Default font not available! Using fallback." << std::endl;
		return fallbackFont;
	}

	bool FontManager::loadFontImpl(const std::string_view id, const void *data, size_t size)
	{
		if (id.empty() || !data || size == 0)
		{
			std::cerr << "Error: Invalid parameters for font loading." << std::endl;
			return false;
		}

		FontFormat format = validateFontImpl(data, size);
		if (format == FontFormat::INVALID)
		{
			std::cerr << "Error: Invalid font data for '" << id << "'." << std::endl;
			return false;
		}

		sf::Font font;
		if (!font.openFromMemory(data, size))
		{
			std::cerr << "Error: Failed to load font '" << id << "' from memory!" << std::endl;
			return false;
		}

		fonts[id.data()] = std::move(font);
		return true;
	}

	// note: we dont use openFromFile directly since we need to validate the font first
	bool FontManager::loadFontFromFileImpl(const std::string_view id, const std::string_view filePath)
	{
		if (id.empty() || filePath.empty())
		{
			std::cerr << "Error: Invalid parameters for font loading from file." << std::endl;
			return false;
		}

		std::ifstream file(id.data(), std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			std::cerr << "Error: Failed to open font file '" << filePath << "'." << std::endl;
			return false;
		}

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char> buffer(size);
		if (!file.read(buffer.data(), size))
		{
			std::cerr << "Error: Failed to read font file '" << filePath << "'." << std::endl;
			return false;
		}

		file.close();

		return loadFontImpl(id, buffer.data(), buffer.size());
	}

	bool FontManager::hasFontImpl(const std::string_view id)
	{
		return fonts.find(id.data()) != fonts.end();
	}

	bool FontManager::unloadFontImpl(const std::string_view id)
	{
		if (!hasFontImpl(id))
		{
			std::cerr << "Warning: Cannot unload font '" << id << "' as it doesn't exist." << std::endl;
			return false;
		}

		if (isBuiltInFontImpl(id))
		{
			std::cerr << "Warning: Cannot unload built-in font '" << id << "'." << std::endl;
			return false;
		}

		fonts.erase(id.data());
		return true;
	}

	bool FontManager::isBuiltInFontImpl(const std::string_view id)
	{
		return std::find(builtInFontIds.begin(), builtInFontIds.end(), id) != builtInFontIds.end();
	}

	FontManager::FontFormat FontManager::validateFontImpl(const void *data, size_t size)
	{
		if (!data || size < 4)
		{
			return FontFormat::INVALID;
		}

		const auto *bytes = static_cast<const uint8_t *>(data);

		// TrueType/OpenType signatures
		// TrueType signature (0x00010000)
		if (bytes[0] == 0x00 && bytes[1] == 0x01 && bytes[2] == 0x00 && bytes[3] == 0x00)
		{
			return FontFormat::TRUETYPE;
		}

		// opentype with CFF data signature ('OTTO')
		if (bytes[0] == 'O' && bytes[1] == 'T' && bytes[2] == 'T' && bytes[3] == 'O')
		{
			return FontFormat::OPENTYPE_CFF;
		}

		// WOFF signature ('wOFF')
		if (bytes[0] == 'w' && bytes[1] == 'O' && bytes[2] == 'F' && bytes[3] == 'F')
		{
			// need to check WOFF flavor to determine if its OpenType CFF or TrueType
			if (size >= 12)
			{
				// the original flavor is at offset 4
				if (bytes[8] == 'O' && bytes[9] == 'T' && bytes[10] == 'T' && bytes[11] == 'O')
				{
					return FontFormat::OPENTYPE_CFF;
				}
				else if (bytes[8] == 0x00 && bytes[9] == 0x01 && bytes[10] == 0x00 && bytes[11] == 0x00)
				{
					return FontFormat::TRUETYPE;
				}
			}
			// If we can't determine flavor, assume it's a valid font
			return FontFormat::SFNT;
		}

		// WOFF2 signature ('wOF2')
		if (bytes[0] == 'w' && bytes[1] == 'O' && bytes[2] == 'F' && bytes[3] == '2')
		{
			return FontFormat::SFNT; // General SFNT format
		}

		// Type 1 fonts
		if (size > 2 && bytes[0] == 0x80 && bytes[1] == 0x01)
		{
			return FontFormat::TYPE1; // PFB binary format
		}

		if (size > 14 && bytes[0] == '%' && bytes[1] == '!' &&
			(std::memcmp(bytes + 2, "PS-AdobeFont", 12) == 0 || std::memcmp(bytes + 2, "FontType", 8) == 0))
		{
			return FontFormat::TYPE1; // PFA ASCII format
		}

		// PCF fonts
		if (size > 4 && bytes[0] == 1 && bytes[1] == 'f' && bytes[2] == 'c' && bytes[3] == 'p')
		{
			return FontFormat::X11_PCF;
		}

		// BDF fonts (ASCII format starting with "STARTFONT")
		if (size > 9 && std::memcmp(bytes, "STARTFONT", 9) == 0)
		{
			return FontFormat::BDF;
		}

		// Windows FNT/FON
		if (size > 2 && ((bytes[0] == 0x00 && bytes[1] == 0x00) || // New Windows FNT
						 (bytes[0] == 0x4D && bytes[1] == 0x5A)))
		{ // MZ header (FON)
			return FontFormat::WINDOWS_FNT;
		}

		// PFR (Portable Font Resource)
		if (size > 4 && bytes[0] == 'P' && bytes[1] == 'F' && bytes[2] == 'R' && bytes[3] == '0')
		{
			return FontFormat::PFR;
		}

		// Type 42 is hard to detect as its a PostScript wrapper around TrueType
		// we wopuld need to parse the PS code to find the embedded TrueType font,
		// this is a simplified check for the beginning of a Type 42 font
		if (size > 20 && bytes[0] == '%' && bytes[1] == '!' && bytes[2] == 'P' && bytes[3] == 'S' &&
			std::memcmp(bytes + 4, "-Adobe-", 7) == 0)
		{
			// Look for /sfnts in the first few hundred bytes
			for (size_t i = 0; i < std::min(size_t(500), size - 6); i++)
			{
				if (bytes[i] == '/' && bytes[i + 1] == 's' && bytes[i + 2] == 'f' && bytes[i + 3] == 'n' &&
					bytes[i + 4] == 't' && bytes[i + 5] == 's')
				{
					return FontFormat::TYPE42;
				}
			}
		}

		return FontFormat::INVALID;
	}

	void FontManager::loadBuiltinFonts()
	{
		if (loadFontImpl(ARIAL, fonts::arial_data.data(), fonts::arial_data_size))
		{
			builtInFontIds.emplace_back(ARIAL);
		}
		else
		{
			std::cerr << "Critical Error: Failed to load built-in Arial font!" << std::endl;
		}
	}


} // namespace ml
