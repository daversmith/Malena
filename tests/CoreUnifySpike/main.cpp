// Spike: validate the per-instance children + parent backpointer model that
// will replace Core::_childMap and CoreManager<Core>.
//
// Pure standard-library, no Malena dependency, so it compiles in seconds and
// we can iterate on the model freely. Once the real refactor lands, this
// directory can be deleted.
//
// Scenarios under test:
//   1. Enable cascade flows through a per-instance children vector.
//   2. Independent child disable survives a parent enable/disable round-trip.
//   3. Layer key determines draw-iteration order; insertion breaks ties.
//   4. removeChild() during iteration defers until iteration ends.
//   5. Parent destruction nulls out children's parent backpointer.
//   6. Child destruction removes itself from parent's children list.
//   7. Three-level cascade (root → mid → leaf).
//   8. Single-parent invariant is enforced (assert).

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>

namespace spike {

class Node
{
public:
    struct ChildEntry
    {
        Node*         child;
        int           layer;
        std::uint64_t insertOrder;
    };

    Node()                         = default;
    Node(const Node&)              = delete;
    Node& operator=(const Node&)   = delete;

    virtual ~Node()
    {
        if (_parent) _parent->doRemoveChild(this);
        for (auto& e : _children) e.child->_parent = nullptr;
    }

    // ── Registration ────────────────────────────────────────────────────────
    void addChild(Node& child, int layer = 100)
    {
        assert(child._parent == nullptr && "single-parent invariant");
        child._parent = this;
        _children.push_back({&child, layer, _nextInsert++});
        std::stable_sort(_children.begin(), _children.end(),
            [](const ChildEntry& a, const ChildEntry& b) { return a.layer < b.layer; });
        child.applyParentEnabled(isEnabled());
    }

    void removeChild(Node& child)
    {
        if (_iterDepth > 0)
            _pendingOps.emplace_back([this, c = &child]{ doRemoveChild(c); });
        else
            doRemoveChild(&child);
    }

    // ── Enable cascade ──────────────────────────────────────────────────────
    void setEnabled(bool e)
    {
        if (_selfEnabled == e) return;
        _selfEnabled = e;
        cascadeEnabled();
    }
    bool isEnabled() const { return _selfEnabled && _parentEnabled; }

    void applyParentEnabled(bool pe)
    {
        if (_parentEnabled == pe) return;
        _parentEnabled = pe;
        cascadeEnabled();
    }

    // ── Iteration ───────────────────────────────────────────────────────────
    // Safe under structural change: removeChild() during iteration is deferred
    // until iteration completes.
    void forEachChild(const std::function<void(Node&)>& f)
    {
        ++_iterDepth;
        for (auto& e : _children) f(*e.child);
        --_iterDepth;
        if (_iterDepth == 0)
        {
            auto ops = std::move(_pendingOps);
            for (auto& op : ops) op();
        }
    }

    Node*                          parent()   const { return _parent; }
    const std::vector<ChildEntry>& children() const { return _children; }

private:
    void cascadeEnabled()
    {
        const bool eff = _selfEnabled && _parentEnabled;
        for (auto& e : _children) e.child->applyParentEnabled(eff);
    }

    void doRemoveChild(Node* c)
    {
        auto it = std::find_if(_children.begin(), _children.end(),
            [c](const ChildEntry& e) { return e.child == c; });
        if (it == _children.end()) return;
        it->child->_parent = nullptr;
        _children.erase(it);
    }

    Node*                              _parent        = nullptr;
    std::vector<ChildEntry>            _children;
    std::uint64_t                      _nextInsert    = 0;
    int                                _iterDepth     = 0;
    std::vector<std::function<void()>> _pendingOps;
    bool                               _selfEnabled   = true;
    bool                               _parentEnabled = true;
};

} // namespace spike

// ─── Test harness ───────────────────────────────────────────────────────────
namespace {
int failures = 0;
#define CHECK(cond)                                                              \
    do {                                                                          \
        if (!(cond)) {                                                            \
            std::cerr << "FAIL " #cond " @ " << __FILE__ << ":" << __LINE__ << "\n";\
            ++failures;                                                           \
        }                                                                         \
    } while (0)
}

using spike::Node;

void test_basic_cascade()
{
    Node p, a, b;
    p.addChild(a);
    p.addChild(b);
    CHECK(a.parent() == &p);
    CHECK(a.isEnabled() && b.isEnabled());

    p.setEnabled(false);
    CHECK(!a.isEnabled() && !b.isEnabled());

    p.setEnabled(true);
    CHECK(a.isEnabled() && b.isEnabled());
}

void test_independent_child_disable_round_trips()
{
    Node p, a, b;
    p.addChild(a);
    p.addChild(b);

    a.setEnabled(false);
    CHECK(!a.isEnabled() && b.isEnabled());

    p.setEnabled(false);
    CHECK(!a.isEnabled() && !b.isEnabled());

    p.setEnabled(true);
    CHECK(!a.isEnabled() && b.isEnabled());   // a stays off (self was off)

    a.setEnabled(true);
    CHECK(a.isEnabled());
}

void test_layer_ordering()
{
    Node p, bg, content, overlay;
    p.addChild(overlay, 200);
    p.addChild(content, 100);
    p.addChild(bg,        0);

    const auto& k = p.children();
    CHECK(k.size() == 3);
    CHECK(k[0].child == &bg);
    CHECK(k[1].child == &content);
    CHECK(k[2].child == &overlay);
}

void test_insertion_order_within_layer()
{
    Node p, a, b, c;
    p.addChild(a, 100);
    p.addChild(b, 100);
    p.addChild(c, 100);

    const auto& k = p.children();
    CHECK(k[0].child == &a);
    CHECK(k[1].child == &b);
    CHECK(k[2].child == &c);
}

void test_remove_during_iteration_is_deferred()
{
    Node p, a, b, c;
    p.addChild(a); p.addChild(b); p.addChild(c);

    int visited = 0;
    p.forEachChild([&](Node& n) {
        ++visited;
        if (&n == &b) p.removeChild(b);   // must defer
    });
    CHECK(visited == 3);                   // b still visited this pass
    CHECK(p.children().size() == 2);       // b removed after iteration ends
}

void test_parent_destruction_releases_children()
{
    Node a;
    {
        Node p;
        p.addChild(a);
        CHECK(a.parent() == &p);
    }
    CHECK(a.parent() == nullptr);
}

void test_child_destruction_deregisters_from_parent()
{
    Node p;
    {
        Node a;
        p.addChild(a);
        CHECK(p.children().size() == 1);
    }
    CHECK(p.children().empty());
}

void test_three_level_cascade()
{
    Node root, mid, leaf;
    root.addChild(mid);
    mid.addChild(leaf);

    root.setEnabled(false);
    CHECK(!mid.isEnabled() && !leaf.isEnabled());

    root.setEnabled(true);
    CHECK(mid.isEnabled() && leaf.isEnabled());
}

void test_remove_outside_iteration_runs_immediately()
{
    Node p, a, b;
    p.addChild(a);
    p.addChild(b);
    p.removeChild(a);
    CHECK(p.children().size() == 1);
    CHECK(a.parent() == nullptr);
    CHECK(p.children()[0].child == &b);
}

int main()
{
    test_basic_cascade();
    test_independent_child_disable_round_trips();
    test_layer_ordering();
    test_insertion_order_within_layer();
    test_remove_during_iteration_is_deferred();
    test_parent_destruction_releases_children();
    test_child_destruction_deregisters_from_parent();
    test_three_level_cascade();
    test_remove_outside_iteration_runs_immediately();

    if (failures == 0) {
        std::cout << "CoreUnifySpike: all checks passed\n";
        return 0;
    }
    std::cerr << "CoreUnifySpike: " << failures << " check(s) failed\n";
    return 1;
}
