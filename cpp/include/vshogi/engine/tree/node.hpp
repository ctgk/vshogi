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
    std::unique_ptr<Derived> m_sibling;
    std::unique_ptr<Derived> m_child;
    Derived* m_most_visited_child;

public:
    Node() : m_parent{}, m_sibling{}, m_child{}, m_most_visited_child{}
    {
    }
    ~Node() = default; // Rule 1/5 destructor
    Node(const Node& other) = delete; // Rule 2/5 copy constructor
    Node& operator=(const Node& other) = delete; // Rule 3/5 copy assignment
    Node(Node&& other) = default; // Rule 4/5 move constructor
    Node& operator=(Node&& other) = default; // Rule 5/5 move assignment
    // clang-format off
    const Derived* get_parent() const { return m_parent; }
    const Derived* get_sibling() const { return m_sibling.get(); }
    const Derived* get_child() const { return m_child.get(); }
    const Derived* get_most_visited_child() const { return m_most_visited_child; }
    bool has_child() const { return static_cast<bool>(m_child); }
    // clang-format on
    void init()
    {
        m_parent = nullptr;
        m_sibling.reset();
        m_child.reset();
        m_most_visited_child = nullptr;
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
