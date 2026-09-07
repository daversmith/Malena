#!/usr/bin/env python3
"""
add_ingroup.py

Automatically inserts @ingroup tags into Malena Doxygen comments.

Usage:
    python3 add_ingroup.py /path/to/include/Malena

The script finds the primary Doxygen comment for each target class/struct/enum
and inserts @ingroup right after the @brief line. It is idempotent — running it
twice will not add duplicate tags.
"""

import re
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Map: relative path from the Malena include root → (group, identifier)
# identifier is what we look for to find the right comment block
# ---------------------------------------------------------------------------
TARGETS = {
    # Core
    "Core/Component.h":                  ("Core",             "class Component"),
    "Core/Core.h":                        ("Core",             "class Core"),
    "Core/CoreAdapter.h":                 ("Core",             "class CoreAdapter"),
    "Core/CoreManager.h":                 ("Core",             "class CoreManager"),
    "Core/DeferredOperationsManager.h":   ("Core",             "class DeferredOperationsManager"),
    "Core/DrawableWrapper.h":             ("Core",             "class DrawableWrapper"),
    "Core/Manager.h":                     ("Core",             "class Manager"),

    # ECS/Core
    "ECS/Core/ECSManager.h":             ("ECSCore",           "class ECSManager"),
    "ECS/Core/EntitiesAndComponents.h":  ("ECSCore",           "struct PositionComponent"),
    "ECS/Core/EventManagerECS.h":        ("ECSCore",           "class EventManagerECS"),

    # ECS/System
    "ECS/System/HealthSystem.h":         ("ECSSystem",         "class HealthSystem"),
    "ECS/System/MovementSystem.h":       ("ECSSystem",         "class MovementSystem"),
    "ECS/System/RenderingSystem.h":      ("ECSSystem",         "class RenderingSystem"),
    "ECS/System/SizeSystem.h":           ("ECSSystem",         "class SizeSystem"),

    # Engine/App
    "Engine/App/Application.h":          ("EngineApp",         "class Application"),
    "Engine/App/AppManager.h":           ("EngineApp",         "class AppManager"),
    "Engine/App/Controller.h":           ("EngineApp",         "class Controller"),
    "Engine/App/UIController.h":         ("EngineApp",         "class UIController"),

    # Engine/Events
    "Engine/Events/EventsManager.h":     ("EngineEvents",      "class EventsManager"),

    # Engine/Messaging
    "Engine/Messaging/MessageManager.h": ("EngineMessaging",   "class MessageManager"),

    # Engine/Plugins
    "Engine/Plugins/Plugin.h":           ("EnginePlugins",     "class Plugin"),
    "Engine/Plugins/PluginInfo.h":       ("EnginePlugins",     "struct PluginInfo"),
    "Engine/Plugins/PluginManager.h":    ("EnginePlugins",     "class PluginManager"),

    # Engine/Window — namespace, not class; tag goes on getWindow()
    "Engine/Window/WindowManager.h":     ("EngineWindow",      "inline sf::RenderWindow"),

    # Graphics/Base
    "Graphics/Base/Graphic.h":           ("GraphicsBase",      "class Graphic"),
    "Graphics/Base/Button.h":            ("GraphicsBase",      "class Button"),

    # Graphics/Controls
    "Graphics/Controls/Carousel.h":      ("GraphicsControls",  "class Carousel"),
    "Graphics/Controls/CircleButton.h":  ("GraphicsControls",  "class CircleButton"),
    "Graphics/Controls/ConvexButton.h":  ("GraphicsControls",  "class ConvexButton"),
    "Graphics/Controls/RectangleButton.h": ("GraphicsControls","class RectangleButton"),

    # Graphics/Layouts
    "Graphics/Layouts/Grid.h":           ("GraphicsLayouts",   "class Grid"),

    # Graphics/Primitives
    "Graphics/Primitives/Rectangle.h":   ("GraphicsPrimitives","class Rectangle"),
    "Graphics/Primitives/Circle.h":      ("GraphicsPrimitives","class Circle"),
    "Graphics/Primitives/Convex.h":      ("GraphicsPrimitives","class Convex"),
    "Graphics/Primitives/RoundedRectangle.h": ("GraphicsPrimitives","class RoundedRectangle"),
    "Graphics/Primitives/VertexArray.h": ("GraphicsPrimitives","class VertexArray"),

    # Graphics/Sprites
    "Graphics/Sprites/Sprite.h":         ("GraphicsSprites",   "class Sprite"),

    # Graphics/Text
    "Graphics/Text/Text.h":              ("GraphicsText",       "class Text"),
    "Graphics/Text/TextBox.h":           ("GraphicsText",       "class TextBox"),
    "Graphics/Text/TextInput.h":         ("GraphicsText",       "class TextInput"),
    "Graphics/Text/Typer.h":             ("GraphicsText",       "class Typer"),
    "Graphics/Text/Cursor.h":            ("GraphicsText",       "class Cursor"),

    # Manifests
    "Manifests/Manifest.h":              ("Manifests",          "class Manifest"),
    "Manifests/DefaultManifest.h":       ("Manifests",          "class DefaultManifest"),
    "Manifests/ManifestAliases.h":       ("Manifests",          "struct ManifestAliases"),
    "Manifests/Context.h":               ("Manifests",          "struct Context"),
    "Manifests/Resources.h":             ("Manifests",          "struct Resources"),

    # Resources
    "Resources/ResourceManager.h":       ("Resources",          "class ResourceManager"),
    "Resources/AssetsManager.h":         ("Resources",          "class AssetsManager"),
    "Resources/TextureManager.h":        ("Resources",          "class TextureManager"),
    "Resources/FontManager.h":           ("Resources",          "class FontManager"),
    "Resources/SoundManager.h":          ("Resources",          "class SoundManager"),
    "Resources/ConfigManager.h":         ("Resources",          "class ConfigManager"),
    "Resources/FlagManager.h":           ("Resources",          "class FlagManager"),
    "Resources/StateManager.h":          ("Resources",          "class StateManager"),

    # Traits
    "Traits/Subscribable.h":             ("Traits",             "class Subscribable"),
    "Traits/Flaggable.h":                ("Traits",             "class Flaggable"),
    "Traits/Positionable.h":             ("Traits",             "class Positionable"),
    "Traits/Draggable.h":                ("Traits",             "class Draggable"),
    "Traits/Messenger.h":                ("Traits",             "class Messenger"),
    "Traits/Customizable.h":             ("Traits",             "class Customizable"),
    "Traits/CustomFlaggable.h":          ("Traits",             "class CustomFlaggable"),
    "Traits/Base/Trait.h":               ("TraitsBase",         "class Trait"),
    "Traits/Base/MultiCustomFlaggable.h":("TraitsBase",         "class MultiCustomFlaggable"),
    "Traits/Base/MultiCustomStateManager.h":("TraitsBase",      "class MultiCustomStateManager"),

    # Utilities
    "Utilities/Align.h":                 ("Utilities",          "class Align"),
    "Utilities/Flag.h":                  ("Utilities",          "enum class Flag"),
    "Utilities/MouseEvents.h":           ("Utilities",          "class MouseEvents"),
    "Utilities/Tween.h":                 ("Utilities",          "enum Tween"),
    "Utilities/ShaderLibrary.h":         ("Utilities",          "class ShaderLibrary"),
    "Utilities/TextManipulators.h":      ("Utilities",          "class TextManipulators"),
    "Utilities/TextureSlicer.h":         ("Utilities",          "class TextureSlicer"),
    "Utilities/ImageRects.h":            ("Utilities",          "class ImageRects"),
    "Utilities/EnumClassHash.h":         ("Utilities",          None),  # may have no comment
    "Utilities/TypeExtraction.h":        ("Utilities",          None),
    "Utilities/LoaderFunctions.h":       ("Utilities",          None),
    "Utilities/Helper.h":                ("Utilities",          None),
}


def find_comment_end_before(lines, identifier):
    """
    Find the index of the last line of the /** ... */ block that immediately
    precedes the line containing `identifier`. Returns None if not found.
    """
    # Find the line with the identifier
    target_line = None
    for i, line in enumerate(lines):
        if identifier in line:
            target_line = i
            break
    if target_line is None:
        return None

    # Walk backwards from target_line to find the closing */ of a doc comment
    for i in range(target_line - 1, -1, -1):
        stripped = lines[i].strip()
        if stripped == "*/":
            return i
        # Skip blank lines between */ and the class declaration
        if stripped == "":
            continue
        # Hit something that isn't blank or */ — no doc comment directly above
        break
    return None


def find_brief_line(lines, comment_end):
    """
    Given the index of the */ line, walk back into the comment to find
    the line containing @brief. Returns its index or None.
    """
    for i in range(comment_end, -1, -1):
        if "@brief" in lines[i]:
            return i
        if "/**" in lines[i]:
            break
    return None


def already_has_ingroup(lines, comment_end):
    """Return True if the comment block already contains @ingroup."""
    for i in range(comment_end, -1, -1):
        if "@ingroup" in lines[i]:
            return True
        if "/**" in lines[i]:
            break
    return False


def get_indent(line):
    """Return the leading whitespace of a line."""
    return line[: len(line) - len(line.lstrip())]


def insert_ingroup(lines, brief_line_idx, group, indent="     * "):
    """Insert @ingroup after the @brief line."""
    tag_line = indent + "@ingroup " + group + "\n"
    lines.insert(brief_line_idx + 1, tag_line)


def add_file_level_comment(lines, group, filename):
    """
    For files with no existing doc comment on their primary symbol,
    prepend a minimal file-level @file comment with @ingroup.
    """
    tag = f"/**\n * @file {filename}\n * @ingroup {group}\n */\n"
    # Insert after the last #pragma once / include guard line near the top
    insert_pos = 0
    for i, line in enumerate(lines[:10]):
        if line.strip().startswith("#pragma") or line.strip().startswith("#ifndef") or line.strip().startswith("#define"):
            insert_pos = i + 1
    lines.insert(insert_pos, tag)


def process_file(filepath, group, identifier):
    with open(filepath, "r", encoding="utf-8") as f:
        lines = f.readlines()

    original = lines[:]

    if identifier is None:
        # No primary symbol — add a @file comment if @ingroup not already present
        if not any("@ingroup" in l for l in lines):
            add_file_level_comment(lines, group, filepath.name)
    else:
        comment_end = find_comment_end_before(lines, identifier)

        if comment_end is None:
            # No doc comment above the identifier — create a minimal one
            # Find the identifier line
            for i, line in enumerate(lines):
                if identifier in line:
                    indent = get_indent(line)
                    new_comment = (
                        indent + "/**\n"
                        + indent + " * @brief " + identifier.split()[-1] + ".\n"
                        + indent + " * @ingroup " + group + "\n"
                        + indent + " */\n"
                    )
                    lines.insert(i, new_comment)
                    break
        else:
            if already_has_ingroup(lines, comment_end):
                return False  # already done, no change

            brief_idx = find_brief_line(lines, comment_end)
            if brief_idx is None:
                # No @brief — insert @ingroup just before */
                indent = get_indent(lines[comment_end])
                lines.insert(comment_end, indent + " * @ingroup " + group + "\n")
            else:
                indent = get_indent(lines[brief_idx])
                insert_ingroup(lines, brief_idx, group, indent + " * ")

    if lines != original:
        with open(filepath, "w", encoding="utf-8") as f:
            f.writelines(lines)
        return True
    return False


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 add_ingroup.py /path/to/include/Malena")
        sys.exit(1)

    root = Path(sys.argv[1])
    if not root.is_dir():
        print(f"Error: {root} is not a directory")
        sys.exit(1)

    modified = []
    skipped = []
    missing = []

    for rel_path, (group, identifier) in TARGETS.items():
        filepath = root / rel_path
        if not filepath.exists():
            missing.append(str(rel_path))
            continue
        changed = process_file(filepath, group, identifier)
        if changed:
            modified.append(str(rel_path))
        else:
            skipped.append(str(rel_path))

    print(f"\n✅  Modified ({len(modified)}):")
    for f in modified:
        print(f"    {f}")

    print(f"\n⏭️   Already done / no change ({len(skipped)}):")
    for f in skipped:
        print(f"    {f}")

    if missing:
        print(f"\n⚠️   Not found ({len(missing)}):")
        for f in missing:
            print(f"    {f}")

    print("\nDone.")


if __name__ == "__main__":
    main()
