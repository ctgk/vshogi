#ifndef VSHOGI_ENGINE_MCGS_EDGE_HPP
#define VSHOGI_ENGINE_MCGS_EDGE_HPP

#include <cstring>

#include "vshogi/common/move.hpp"

namespace vshogi::engine::mcgs
{

class Node;

class Edge
{
public:
    Edge() = default;
    ~Edge() = default; // Rule 1/5 destructor
    Edge(const Edge& other) = delete; // Rule 2/5 copy constructor
    Edge& operator=(const Edge& other) = delete; // Rule 3/5 copy assignment
    Edge(Edge&& other); // Rule 4/5 move constructor
    Edge& operator=(Edge&& other); // Rule 5/5 move assignment
    void backprop();
    void destruct();
    Node& set_child(Node& c);
    Node* pop_child();

    // clang-format off
    void init() { std::memset(reinterpret_cast<void*>(this), 0, sizeof(Edge)); }
    void init(Node* const parent, const float prior_proba, const move_t action)
    { init(); m_parent = parent; m_prior_proba = prior_proba; m_action = action; }
    void init_as_end() { m_parent = reinterpret_cast<Node*>(this); }
    Node* child() { return m_child; }

    const Node* get_parent() const { return m_parent; }
    const Node* get_child() const { return m_child; }
    const Edge* get_sibling() const
    { if (m_parent == nullptr) return nullptr; const Edge* const e = this + 1; if (m_parent == e->m_parent) return e; return nullptr; }
    float get_value() const;
    float get_prior_proba() const { return m_prior_proba; }
    uint get_visits() const { return m_visits; }
    move_t get_action() const { return m_action; }
    bool is_mate() const { return static_cast<bool>(m_mate); }
    bool is_mate_to_win() const { return m_mate > 0; }
    bool is_mate_to_lose() const { return m_mate < 0; }
    bool has_child() const { return static_cast<bool>(m_child); }
    bool is_leaf() const { return m_child == nullptr; }
    static constexpr bool can_be_root() { return false; }
    // clang-format on

    friend class Node;

private:
    Node* m_parent;
    Node* m_child;
    // Edge* m_sibling;
    float m_prior_proba;
    uint m_visits; //!< Number of times the edge selected by PUCT algorithm.
    move_t m_action;
    std::int8_t m_mate; //!< 1: mate to win, -1: mate to loss
};

} // namespace vshogi::engine::mcgs

#endif // VSHOGI_ENGINE_MCGS_EDGE_HPP
