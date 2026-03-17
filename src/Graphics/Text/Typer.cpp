
#include <Malena/Graphics/Text/Typer.h>

namespace ml
{
	Typer::Typer(const sf::Font &font) : Graphic<sf::Text>(font)
	{
		registerEvents();
	}

} // namespace ml

	void ml::Typer::registerEvents()
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

