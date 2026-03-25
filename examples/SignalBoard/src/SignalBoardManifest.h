#pragma once
// ============================================================
//  SignalBoardManifest.h
// ============================================================
//
//  The Manifest is the static resource registry for this program.
//  It declares WHAT resources are needed (enum values) and
//  WHERE to find them (file paths).
//
//  The inline static initializer runs at program startup before
//  any component constructor, so the path is registered before
//  the first AssetsManager::get() call.
//
//  AssetsManager::get() loads the file on first call, then returns
//  the same cached sf::Font& on every subsequent call — including
//  every frame in draw(). No repeated file I/O.
//
// ============================================================

#include <Malena/Manifests/Manifest.h>

class SignalBoardManifest : public ml::Manifest
{
public:
    enum class Fonts { Main };

private:
    inline static const auto _ = []()
    {
        set(Fonts::Main, "/Users/dsmizzle/CLionProjects/MalenaDeferred/examples/SignalBoard/assets/fonts/Outfit/static/Outfit-Regular.ttf");
        return 0;
    }();
};
