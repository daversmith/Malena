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

#include <Malena/common.hpp>
#include <Malena/Manifests/Resources.h>
#include <memory>
#include <vector>
#include "SignalBoardManifest.h"
#include "SignalTower.h"
#include "EventLog.h"
#include "ScoreDisplay.h"

class SignalBoard : public ml::Application,
                    public ml::Resources<SignalBoardManifest>
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
