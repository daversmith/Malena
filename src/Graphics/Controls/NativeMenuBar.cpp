// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#include <Malena/Graphics/Controls/NativeMenuBar.h>
#include <Malena/Engine/Window/WindowManager.h>

// ── Platform includes ─────────────────────────────────────────────────────────

#ifdef __APPLE__
#import <AppKit/AppKit.h>

// Bridge object: holds a C++ std::function and forwards NSMenuItem actions to it.
// NSMenuItem does NOT retain its target, so NativeMenuBar keeps all targets alive
// in _targetList (an NSMutableArray) for the lifetime of the menu.
@interface MLMenuItemTarget : NSObject
{
@public
    std::function<void()> fn;
}
- (void)fire:(id)sender;
@end

@implementation MLMenuItemTarget
- (void)fire:(id)sender { if (fn) fn(); }
@end

#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unordered_map>

namespace
{
    std::unordered_map<UINT, std::function<void()>> s_callbacks;
    UINT    s_nextId      = 1000;
    WNDPROC s_prevWndProc = nullptr;

    // Intercept WM_COMMAND to dispatch menu-item callbacks.
    LRESULT CALLBACK menuWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        if (msg == WM_COMMAND)
        {
            auto it = s_callbacks.find(static_cast<UINT>(LOWORD(wp)));
            if (it != s_callbacks.end())
                it->second();
        }
        return CallWindowProc(s_prevWndProc, hwnd, msg, wp, lp);
    }

    std::wstring toWide(const std::string& s)
    {
        if (s.empty()) return {};
        const int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
        std::wstring ws(static_cast<std::size_t>(len - 1), L'\0');
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, ws.data(), len);
        return ws;
    }
}
#endif

namespace ml
{

// ── Menu ──────────────────────────────────────────────────────────────────────

NativeMenuBar::Menu::Menu(std::string label)
    : _label(std::move(label)) {}

NativeMenuBar::Menu& NativeMenuBar::Menu::addItem(const std::string& label,
                                                   std::function<void()> action)
{
    _items.push_back({Item::Type::Action, label, std::move(action)});
    return *this;
}

NativeMenuBar::Menu& NativeMenuBar::Menu::addSeparator()
{
    _items.push_back({Item::Type::Separator, "", nullptr});
    return *this;
}

// ── NativeMenuBar ─────────────────────────────────────────────────────────────

NativeMenuBar::~NativeMenuBar() { detach(); }

NativeMenuBar::Menu& NativeMenuBar::addMenu(const std::string& label)
{
    _menus.push_back(std::unique_ptr<Menu>(new Menu(label)));
    return *_menus.back();
}

void NativeMenuBar::attach()
{
    detach();

#ifdef __APPLE__

    NSMenu*        mainMenu = [[NSMenu alloc] initWithTitle:@"MainMenu"];
    NSMutableArray* targets = [[NSMutableArray alloc] init];

    for (const auto& menu : _menus)
    {
        NSString* title   = [NSString stringWithUTF8String:menu->_label.c_str()];
        NSMenu*   submenu = [[NSMenu alloc] initWithTitle:title];
        NSMenuItem* top   = [[NSMenuItem alloc] initWithTitle:title
                                                       action:nil
                                                keyEquivalent:@""];
        [top setSubmenu:submenu];
        [submenu release];
        [mainMenu addItem:top];
        [top release];

        for (const auto& item : menu->_items)
        {
            if (item.type == Item::Type::Separator)
            {
                [submenu addItem:[NSMenuItem separatorItem]];
                continue;
            }

            MLMenuItemTarget* target = [[MLMenuItemTarget alloc] init];
            target->fn = item.action;
            [targets addObject:target];
            [target release]; // targets array now owns it

            NSString* itemTitle = [NSString stringWithUTF8String:item.label.c_str()];
            NSMenuItem* mi = [[NSMenuItem alloc] initWithTitle:itemTitle
                                                        action:@selector(fire:)
                                                 keyEquivalent:@""];
            [mi setTarget:target];
            [submenu addItem:mi];
            [mi release];
        }
    }

    [NSApp setMainMenu:mainMenu]; // NSApp retains mainMenu
    _nativeHandle = mainMenu;    // weak ref — NSApp owns it
    _targetList   = targets;     // we own this; retains all target objects

#elif defined(_WIN32)

    HWND  hwnd = reinterpret_cast<HWND>(WindowManager::getWindow().getNativeHandle());
    HMENU bar  = CreateMenu();

    for (const auto& menu : _menus)
    {
        HMENU popup = CreatePopupMenu();
        for (const auto& item : menu->_items)
        {
            if (item.type == Item::Type::Separator)
            {
                AppendMenuW(popup, MF_SEPARATOR, 0, nullptr);
                continue;
            }
            const UINT id = s_nextId++;
            s_callbacks[id] = item.action;
            AppendMenuW(popup, MF_STRING, static_cast<UINT_PTR>(id),
                        toWide(item.label).c_str());
        }
        AppendMenuW(bar, MF_POPUP, reinterpret_cast<UINT_PTR>(popup),
                    toWide(menu->_label).c_str());
    }

    SetMenu(hwnd, bar);
    DrawMenuBar(hwnd);
    _nativeHandle = bar;

    // Subclass the window procedure to intercept WM_COMMAND
    s_prevWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(hwnd, GWLP_WNDPROC,
                         reinterpret_cast<LONG_PTR>(menuWndProc)));

#endif
}

void NativeMenuBar::detach()
{
#ifdef __APPLE__

    [NSApp setMainMenu:nil];
    _nativeHandle = nullptr;

    if (_targetList)
    {
        [(NSMutableArray*)_targetList release];
        _targetList = nullptr;
    }

#elif defined(_WIN32)

    if (_nativeHandle)
    {
        HWND hwnd = reinterpret_cast<HWND>(WindowManager::getWindow().getNativeHandle());

        if (s_prevWndProc)
        {
            SetWindowLongPtr(hwnd, GWLP_WNDPROC,
                             reinterpret_cast<LONG_PTR>(s_prevWndProc));
            s_prevWndProc = nullptr;
        }

        SetMenu(hwnd, nullptr);
        DrawMenuBar(hwnd);
        DestroyMenu(reinterpret_cast<HMENU>(_nativeHandle));
        _nativeHandle = nullptr;

        s_callbacks.clear();
        s_nextId = 1000;
    }

#endif
}

} // namespace ml
