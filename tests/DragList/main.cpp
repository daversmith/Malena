// Locks ml::DragList's interaction state machine: a click fires onTap, a real
// drag past the threshold fires onReorder(from,to) on release, displayOrder()
// reflects the in-progress drag, and a drop onto the same slot is a no-op.
// Pure logic, window-free.

#include <Malena/Graphics/Controls/DragList.h>
#include <iostream>
#include <vector>

namespace {
int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; ++failures; } \
} while (0)

ml::DragList makeList(int& tapped, int& from, int& to)
{
    ml::DragList dl;
    dl.setGeometry(0.f, 100.f, /*rowTop*/0.f, /*count*/4, /*rowH*/44.f);
    dl.onTap([&](int i){ tapped = i; });
    dl.onReorder([&](int f, int t){ from = f; to = t; });
    return dl;
}

void test_tap()
{
    int tapped = -1, from = -1, to = -1;
    auto dl = makeList(tapped, from, to);
    dl.update(50, 22, true,  true);   // press row 0
    dl.update(50, 24, true,  true);   // tiny move (< threshold)
    dl.update(50, 24, false, true);   // release
    CHECK(tapped == 0);
    CHECK(from == -1 && to == -1);    // no reorder
    CHECK(!dl.isDragging());
}

void test_drag_reorders()
{
    int tapped = -1, from = -1, to = -1;
    auto dl = makeList(tapped, from, to);
    dl.update(50, 22, true, true);    // grab row 0
    dl.update(50, 32, true, true);    // move 10px > threshold → dragging
    CHECK(dl.isDragging());
    dl.update(50, 150, true, true);   // drag down to the last row
    CHECK(dl.dragIndex() == 0);
    CHECK(dl.dropIndex() == 3);
    // Mid-drag display order: row 0 floats to slot 3.
    std::vector<int> order = dl.displayOrder(4);
    CHECK((order == std::vector<int>{1, 2, 3, 0}));
    dl.update(50, 150, false, true);  // release
    CHECK(from == 0 && to == 3);
    CHECK(tapped == -1);
    CHECK(!dl.isDragging());
}

void test_drop_same_slot_noop()
{
    int tapped = -1, from = -1, to = -1;
    auto dl = makeList(tapped, from, to);
    dl.update(50, 10, true, true);    // grab row 0 (top half)
    dl.update(50, 18, true, true);    // 8px move > threshold, still above row 0's midpoint (22) → slot 0
    CHECK(dl.isDragging() && dl.dropIndex() == 0);
    dl.update(50, 18, false, true);   // release on the same slot
    CHECK(from == -1 && to == -1);    // no reorder
    CHECK(tapped == -1);              // it WAS a drag, so no tap either
}

void test_press_outside_band_ignored()
{
    int tapped = -1, from = -1, to = -1;
    auto dl = makeList(tapped, from, to);
    dl.update(120, 22, true,  true);  // x outside [0,100]
    dl.update(120, 22, false, true);
    CHECK(tapped == -1 && from == -1);
}

} // namespace

int main()
{
    test_tap();
    test_drag_reorders();
    test_drop_same_slot_noop();
    test_press_outside_band_ignored();
    if (failures == 0) { std::cout << "DragList: all checks passed\n"; return 0; }
    std::cerr << "DragList: " << failures << " check(s) failed\n";
    return 1;
}
