//
// Created by Dave Smith on 1/23/26.
//

#include <Malena/Graphics/Controls/List.h>
#include <Malena/Graphics/Controls/ListItem.h>
#include <algorithm>


namespace ml
{
	List::List() : _position(0, 0)
{
}

// void List::addItem(ListComponent* item)
// {
//     addItem(std::unique_ptr<ListComponent>(item));
// }

void List::addItem(ListComponent* item)
{
    // If it's a nested List, set its indent
    if (auto* nestedList = dynamic_cast<List*>(item))
    {
        nestedList->setCurrentIndent(_currentIndent + _indentAmount);
    }

    _items.push_back(item);
    repositionItems();
    updateDividers();
}

void List::setPosition(const sf::Vector2f &position)
{
    _position = position;
    repositionItems();
    updateDividers();
}

sf::Vector2f List::getPosition() const
{
    return _position;
}

void List::setIndentAmount(float amount)
{
    _indentAmount = amount;

    // Update indent for all nested lists
    for (auto& item : _items)
    {
        if (auto* nestedList = dynamic_cast<List*>(item))
        {
            nestedList->setCurrentIndent(_currentIndent + _indentAmount);
        }
    }

    repositionItems();
}

void List::setShowDividers(bool show)
{
    _showDividers = show;

    // Propagate to nested lists
    for (auto& item : _items)
    {
        if (auto* nestedList = dynamic_cast<List*>(item))
        {
            nestedList->setShowDividers(show);
        }
    }
}

void List::setDividerColor(const sf::Color& color)
{
    _dividerColor = color;
    updateDividers();

    // Propagate to nested lists
    for (auto& item : _items)
    {
        if (auto* nestedList = dynamic_cast<List*>(item))
        {
            nestedList->setDividerColor(color);
        }
    }
}

void List::setDividerThickness(float thickness)
{
    _dividerThickness = thickness;
    updateDividers();

    // Propagate to nested lists
    for (auto& item : _items)
    {
        if (auto* nestedList = dynamic_cast<List*>(item))
        {
            nestedList->setDividerThickness(thickness);
        }
    }
}

void List::repositionItems()
{
    if (_items.empty())
        return;

    float currentY = _position.y;

    for (auto& item : _items)
    {
        if (auto* listItem = dynamic_cast<ListItem*>(item))
        {
            listItem->setPosition({_position.x + _currentIndent, currentY});
            currentY += listItem->getGlobalBounds().size.y;
        }
        else if (auto* nestedList = dynamic_cast<List*>(item))
        {
            nestedList->setPosition({_position.x, currentY});
            currentY += nestedList->getTotalHeight();
        }
    }
}

void List::updateDividers()
{
    _dividers.clear();

    if (!_showDividers || _items.empty())
        return;

    float currentY = _position.y;
    float maxWidth = getMaxWidth();

    // Create dividers between items
    for (size_t i = 0; i < _items.size(); ++i)
    {
        float itemHeight = 0.0f;

        if (auto* listItem = dynamic_cast<ListItem*>(_items[i]))
        {
            itemHeight = listItem->getGlobalBounds().size.y;
        }
        else if (auto* nestedList = dynamic_cast<List*>(_items[i]))
        {
            itemHeight = nestedList->getTotalHeight();
        }

        currentY += itemHeight;

        // Don't add divider after the last item
        if (i < _items.size() - 1)
        {
            sf::RectangleShape divider;
            divider.setSize({maxWidth, _dividerThickness});
            divider.setPosition({_position.x + _currentIndent, currentY - _dividerThickness / 2.0f});
            divider.setFillColor(_dividerColor);
            _dividers.push_back(divider);
        }
    }
}

float List::getTotalHeight() const
{
    float totalHeight = 0.0f;

    for (const auto& item : _items)
    {
        if (auto* listItem = dynamic_cast<ListItem*>(item))
        {
            totalHeight += listItem->getGlobalBounds().size.y;
        }
        else if (auto* nestedList = dynamic_cast<List*>(item))
        {
            totalHeight += nestedList->getTotalHeight();
        }
    }

    return totalHeight;
}

float List::getMaxWidth() const
{
    float maxWidth = 0.0f;

    for (const auto& item : _items)
    {
        float itemWidth = 0.0f;

        if (auto* listItem = dynamic_cast<ListItem*>(item))
        {
            itemWidth = listItem->getGlobalBounds().size.x;
        }
        else if (auto* nestedList = dynamic_cast<List*>(item))
        {
            itemWidth = nestedList->getMaxWidth() + _indentAmount;
        }

        if (itemWidth > maxWidth)
            maxWidth = itemWidth;
    }

    return maxWidth;
}

void List::autoSizeAllItems(unsigned int baseFontSize)
{
    if (_items.empty())
        return;

    // Step 1: Collect all ListItems (including from nested Lists)
    std::vector<ListItem*> allListItems;

    std::function<void(List*)> collectItems = [&](List* list) {
        for (auto& item : list->_items)
        {
            if (auto* listItem = dynamic_cast<ListItem*>(item))
            {
                allListItems.push_back(listItem);
            }
            else if (auto* nestedList = dynamic_cast<List*>(item))
            {
                collectItems(nestedList);
            }
        }
    };

    collectItems(this);

    if (allListItems.empty())
        return;

    // Step 2: Find maximum natural width
    float maxWidth = 0.0f;
    for (auto* item : allListItems)
    {
        float itemWidth = item->getNaturalWidth(baseFontSize);
        if (itemWidth > maxWidth)
            maxWidth = itemWidth;
    }

    // Step 3: Find the longest text
    ListItem* longestItem = nullptr;
    size_t maxTextLength = 0;

    for (auto* item : allListItems)
    {
        if (item->getText().length() > maxTextLength)
        {
            maxTextLength = item->getText().length();
            longestItem = item;
        }
    }

    // Step 4: Calculate optimal font size
    unsigned int finalFontSize = baseFontSize;

    if (longestItem)
    {
        float availableWidth = maxWidth - longestItem->getIconsWidth() - 20.0f;
        sf::Text tempText(ml::FontManager<>::getDefault());
        tempText.setString(longestItem->getText());

        unsigned int minSize = 10;
        unsigned int maxSize = baseFontSize;
        while (minSize < maxSize)
        {
            unsigned int midSize = (minSize + maxSize + 1) / 2;
            tempText.setCharacterSize(midSize);
            if (tempText.getGlobalBounds().size.x <= availableWidth)
                minSize = midSize;
            else
                maxSize = midSize - 1;
        }
        finalFontSize = minSize;
    }

    // Step 5: Apply sizing to all ListItems
    // for (auto* item : allListItems)
    {
        for (auto* item : allListItems)
        {
            item->setCharacterSize(finalFontSize);
            item->setSize({maxWidth, item->getGlobalBounds().size.y});
        }
        // Note: Lists don't have setCharacterSize or setSize
        // They're handled recursively in autoSizeAllItems
    }

    // Step 6: Reposition everything
    repositionItems();
    updateDividers();
}

sf::FloatRect List::getGlobalBounds() const
{
    if (_items.empty())
        return sf::FloatRect({_position.x, _position.y}, {0, 0});

    float maxWidth = getMaxWidth();
    return sf::FloatRect({_position.x + _currentIndent, _position.y}, {maxWidth, getTotalHeight()});
}

void List::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (size_t i = 0; i < _items.size(); ++i)
    {
        // Draw the item (works for both ListItem and List via polymorphism)
        if (auto* listItem = dynamic_cast<ListItem*>(_items[i]))
        {
            target.draw(*listItem, states);
        }
        else if (auto* nestedList = dynamic_cast<List*>(_items[i]))
        {
            target.draw(*nestedList, states);
        }

        // Draw divider if applicable
        if (_showDividers && i < _dividers.size())
        {
            target.draw(_dividers[i], states);
        }
    }
}

void List::clear()
{
    _items.clear();
    _dividers.clear();
}
}
