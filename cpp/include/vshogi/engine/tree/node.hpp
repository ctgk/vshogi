#ifndef VSHOGI_ENGINE_TREE_NODE_HPP
#define VSHOGI_ENGINE_TREE_NODE_HPP

#include <memory>

namespace vshogi::engine::tree
{

template <class Derived>
class Node
{
protected:
    Derived* m_parent;
    Derived* m_child;
    Derived* m_child_1st;

public:
    Node() : m_parent{}, m_child{}, m_child_1st{}
    {
    }
    ~Node() = default; // Rule 1/5 destructor
    Node(const Node& other) = default; // Rule 2/5 copy constructor
    Node& operator=(const Node& other) = default; // Rule 3/5 copy assignment
    Node(Node&& other) = default; // Rule 4/5 move constructor
    Node& operator=(Node&& other) = default; // Rule 5/5 move assignment
    // clang-format off
    const Derived* get_parent() const { return m_parent; }
    const Derived* get_child() const { return m_child; }
    const Derived* get_child_1st() const { return m_child_1st; }
    bool has_child() const { return static_cast<bool>(m_child); }
    // clang-format on
    void init()
    {
        m_parent = nullptr;
        m_child = nullptr;
        m_child_1st = nullptr;
    }
    void init_as_begin()
    {
        m_parent = nullptr;
    }
    void init_as_end()
    {
        init();
        m_parent = reinterpret_cast<Derived*>(this);
    }
    void init_if_not_end()
    {
        if (m_parent == reinterpret_cast<Derived*>(this))
            return;
        init();
    }
    void destruct()
    {
        if (has_child()) {
            for (auto c = m_child;
                 c->m_parent == reinterpret_cast<Derived*>(this);
                 ++c) {
                c->destruct();
            }
        }
        init();
    }
    void update_child_of_parent(Derived* const new_child_of_parent)
    {
        assert(m_parent);
        m_parent->m_child = new_child_of_parent;
    }
    void update_child_1st_of_parent(Derived* const new_child_1st_of_parent)
    {
        assert(m_parent);
        m_parent->m_child_1st = new_child_1st_of_parent;
    }
    void update_parent_of_childs()
    {
        if (m_child == nullptr)
            return;
        const auto parent = m_child->m_parent;
        for (auto c = m_child; c->m_parent == parent; ++c)
            c->m_parent = reinterpret_cast<Derived*>(this);
    }
    const Derived* get_sibling() const
    {
        if (m_parent == nullptr) // root node does not have siblings.
            return nullptr;
        const auto sibling = reinterpret_cast<const Derived*>(this) + 1;
        return (m_parent == sibling->m_parent) ? sibling : nullptr;
    }
    uint count_childs() const
    {
        uint out = 0u;
        for (auto c = get_child(); c; c = c->get_sibling())
            ++out;
        return out;
    }
};

} // namespace vshogi::engine::tree

#endif // VSHOGI_ENGINE_TREE_NODE_HPP
