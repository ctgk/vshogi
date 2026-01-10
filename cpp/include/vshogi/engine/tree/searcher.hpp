#ifndef VSHOGI_ENGINE_TREE_SEARCHER_HPP
#define VSHOGI_ENGINE_TREE_SEARCHER_HPP

#include <type_traits>
#include <vector>

#include "vshogi/engine/tree/node.hpp"

namespace vshogi::engine::tree
{

template <class N>
class Searcher
{
    static_assert(std::is_base_of_v<tree::Node<N>, N>);

public:
    Searcher(const uint tree_size = 1000000u);
    ~Searcher() = default; // 1/5 destructor
    Searcher(const Searcher& other) = delete; // 2/5 copy constructor
    Searcher& operator=(const Searcher& other) = delete; // 3/5 copy assignment
    Searcher(Searcher&& other) = delete; // 4/5 move constructor
    Searcher& operator=(Searcher&& other) = delete; // 5/5 move assignment
    void init();
    uint count_remaining_nodes() const;
    move_t select_action() const;
    void apply(const move_t& action);
    // clang-format off
    const N& get_root() const { return m_nodes.front(); }
    // clang-format on

protected:
    std::vector<N> m_nodes;
    N* m_next;

    template <class G>
    void backprop_to_root(G& game, N* const leaf);

private:
    void remove_unselected_nodes(const move_t& selected);
};

template <class N>
Searcher<N>::Searcher(const uint tree_size) : m_nodes(tree_size + 2u), m_next()
{
    init();
}

template <class N>
void Searcher<N>::init()
{
    m_nodes.front().init();
    m_next = std::next(m_nodes.data());
    m_next->init();
    m_nodes.back().init_as_end();
}

template <class N>
uint Searcher<N>::count_remaining_nodes() const
{
    return static_cast<uint>(m_nodes.size())
           - static_cast<uint>(m_next - m_nodes.data()) - 1u;
}

template <class N>
move_t Searcher<N>::select_action() const
{
    const auto c1 = get_root().get_child_1st();
    return c1 ? c1->get_action() : static_cast<move_t>(0);
}

template <class N>
template <class G>
void Searcher<N>::backprop_to_root(G& game, N* const leaf)
{
    float v = leaf->get_q_value();
    for (N *curr = leaf, *prev = nullptr;; v = -v) {
        N* const parent = curr->backprop(v, prev);
        prev = curr;
        curr = parent;
        if (parent == nullptr) // `curr` was root node.
            break;
        game.undo();
    }
}

template <class N>
void Searcher<N>::remove_unselected_nodes(const move_t& selected)
{
    for (N* c = m_nodes.data() + 1; c->get_parent() == m_nodes.data(); ++c) {
        if (c->get_action() != selected)
            c->destruct();
    }
    N* next = m_nodes.data();
    for (N* n = m_nodes.data() + 1; n < m_next; ++n) {
        if (n->get_parent() == nullptr)
            continue; // destructed node
        if (n->get_parent() != next) {
            if (n->get_parent()->get_child() == n)
                n->update_child_of_parent(next);
            if (n->get_parent()->get_child_1st() == n)
                n->update_child_1st_of_parent(next);
        }
        *next = *n;
        next->update_parent_of_childs();
        ++next;
    }
    if (next == m_nodes.data()) { // all childs have been eliminated
        next->init();
        ++next;
    }
    m_next = next;
    m_next->init();
}

template <class N>
void Searcher<N>::apply(const move_t& action)
{
    remove_unselected_nodes(action);
    assert(&m_nodes.front() < m_next);
    assert(m_next < &m_nodes.back());
    m_nodes.front().init_as_begin();
}

} // namespace vshogi::engine::tree

#endif // VSHOGI_ENGINE_TREE_SEARCHER_HPP
