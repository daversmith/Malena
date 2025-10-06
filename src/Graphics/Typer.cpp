#ifndef TYPER_CPP
#define TYPER_CPP
#include <Malena/Graphics/Typer.h>

namespace ml
{
	template<typename MANIFEST>
	TyperWith<MANIFEST>::TyperWith(const sf::Font &font) : Shape<sf::Text, MANIFEST>(font)
	{
		registerEvents();
	}

} // namespace ml
template<typename Manifest>
	void ml::TyperWith<Manifest>::registerEvents()
{
	this->onTextEntered([this](const std::optional<sf::Event>&  event) {
		if (this->checkFlag(ml::Flag::ENABLED))
		{
			std::string text = this->getString();
			const char c = event->getIf<sf::Event::TextEntered>()->unicode;

			if (c == '\b' && !text.empty())
				text.pop_back();

			if (c >= ' ' || c == '\n' || c == '\t')
				text.push_back(c);

			this->setString(text);
		}
	});
}

#endif