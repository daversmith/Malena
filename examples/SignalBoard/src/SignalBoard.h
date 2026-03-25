#pragma once
// ============================================================
//  SignalBoard.h
// ============================================================
//
//  Inherits ml::Resources<SignalBoardManifest> which brings:
//    AssetMgr  — ml::AssetsManager<SignalBoardManifest>
//    Fonts     — SignalBoardManifest::Fonts (unqualified alias)
//
//  Layout is driven entirely by Positionable helpers
//  (setBelow, setRightOf, setLeftOf, centerHorizontally) in
//  initialization() — no manual pixel arithmetic.
//
// ============================================================

#include <Malena/Manifests/ManifestResources.h>
#include <Malena/common.hpp>
#include <memory>
#include <vector>
#include "EventLog.h"
#include "ScoreDisplay.h"
#include "SignalBoardManifest.h"
#include "SignalTower.h"

class SignalBoard : public ml::Application,
                    public ml::ManifestResources<SignalBoardManifest>
{
public:
    SignalBoard();
    void initialization() override;
    void registerEvents()  override;

private:
    // No sf::Font member — FontManager owns font lifetime.

    // Heap-allocated custom components (addComponent takes a ref to the live object)
    std::vector<std::unique_ptr<SignalTower>> _towers;
    std::unique_ptr<EventLog>     _log;
    std::unique_ptr<ScoreDisplay> _score;

    // Built-in Malena widgets — member variables, no heap allocation needed
    ml::RectangleButton _btnA, _btnB, _btnC, _btnD;
    ml::RectangleButton _btnFireAll, _btnReset;
};
