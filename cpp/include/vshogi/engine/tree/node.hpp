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
    move_t m_action;
    std::unique_ptr<Derived> m_sibling;
    std::unique_ptr<Derived> m_child;
    Derived* m_child_1st;

public:
    Node() : m_parent{}, m_action{}, m_sibling{}, m_child{}, m_child_1st{}
    {
    }
    Node(const move_t& a)
        : m_parent{}, m_action(a), m_sibling{}, m_child{}, m_child_1st{}
    {
    }
    ~Node() = default; // Rule 1/5 destructor
    Node(const Node& other) = delete; // Rule 2/5 copy constructor
    Node& operator=(const Node& other) = delete; // Rule 3/5 copy assignment
    Node(Node&& other) = default; // Rule 4/5 move constructor
    Node& operator=(Node&& other) = default; // Rule 5/5 move assignment
    // clang-format off
    const Derived* get_parent() const { return m_parent; }
    move_t get_action() const { return m_action; }
    const Derived* get_sibling() const { return m_sibling.get(); }
    const Derived* get_child() const { return m_child.get(); }
    const Derived* get_child_1st() const { return m_child_1st; }
    bool has_child() const { return static_cast<bool>(m_child); }
    // clang-format on
    void init()
    {
        m_parent = nullptr;
        m_action = static_cast<move_t>(0);
        m_sibling.reset();
        m_child.reset();
        m_child_1st = nullptr;
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
