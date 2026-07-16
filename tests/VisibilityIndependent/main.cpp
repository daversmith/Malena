// Locks the Flag::VISIBILITY_INDEPENDENT opt-out: a child marked independent is
// NOT forced visible/hidden by its parent container's setVisible/setActive
// cascade (Panel), so pooled/parked children can manage their own visibility
// without the off-screen-park workaround. Window-free.

#include <Malena/Core/Component.h>
#include <Malena/Graphics/Layouts/Panel.h>
#include <iostream>

namespace {

struct Leaf : public ml::Component<>
{
    void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        ++failures; \
    } \
} while (0)

void test_flag_round_trips()
{
    Leaf leaf;
    CHECK(!leaf.isVisibilityIndependent());
    leaf.setVisibilityIndependent(true);
    CHECK(leaf.isVisibilityIndependent());
    leaf.setVisibilityIndependent(false);
    CHECK(!leaf.isVisibilityIndependent());
}

void test_independent_child_ignores_setVisible_cascade()
{
    ml::Panel panel;
    Leaf normal, indep;
    panel.addComponent(normal);
    panel.addComponent(indep);
    indep.setVisibilityIndependent(true);

    // Parent hides itself: the normal child follows; the independent one doesn't.
    panel.setVisible(false);
    CHECK(!normal.isVisible());
    CHECK(indep.isVisible());

    // The independent child hides itself; the parent showing must NOT revive it.
    indep.setVisible(false);
    panel.setVisible(true);
    CHECK(normal.isVisible());     // normal follows the parent back on
    CHECK(!indep.isVisible());     // stayed hidden despite the cascade
}

void test_independent_child_ignores_setActive_cascade()
{
    ml::Panel panel;
    Leaf normal, indep;
    panel.addComponent(normal);
    panel.addComponent(indep);
    indep.setVisibilityIndependent(true);

    indep.setVisible(false);
    panel.setActive(true);         // cascades setActive(=setVisible+setEnabled)
    CHECK(normal.isVisible());
    CHECK(!indep.isVisible());     // independent child not revived by setActive either
}

} // namespace

int main()
{
    test_flag_round_trips();
    test_independent_child_ignores_setVisible_cascade();
    test_independent_child_ignores_setActive_cascade();

    if (failures == 0) { std::cout << "VisibilityIndependent: all checks passed\n"; return 0; }
    std::cerr << "VisibilityIndependent: " << failures << " check(s) failed\n";
    return 1;
}
