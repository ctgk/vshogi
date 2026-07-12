#include <cmath>

#include "vshogi/engine/mcgs/edge.hpp"
#include "vshogi/engine/mcgs/node.hpp"

namespace vshogi::engine::mcgs
{

Edge* Node::select()
{
    constexpr float c_puct_init = 2.5f;
    constexpr float c_puct_base = 19652.f;
    constexpr float log_c_puct_base = 9.88593439328f; // std::log(c_puct_base);
    constexpr float c_puct_bias = c_puct_init - log_c_puct_base;

    Edge* out = nullptr;
    if (m_child == nullptr)
        return out;
    float max_score = -std::numeric_limits<float>::infinity();

    const float n_s = static_cast<float>(m_visits); // sum_{a}N(s,a) + 1
    const float sqrtn = std::sqrt(n_s);

    // eq(6) in https://arxiv.org/pdf/2012.11045
    const float c_puct = std::log(n_s + c_puct_base) + c_puct_bias;
    const float q_fpu = first_play_urgency();
    for (Edge* e = m_child; e && (e->m_parent == this); ++e) {
        if (e->is_mate_to_win()) {
            out = e;
            break;
        }
        const float n_sa = static_cast<float>(e->get_visits());
        // eq (4) in https://arxiv.org/pdf/2012.11045
        const float u = e->get_prior_proba() * sqrtn / (1.f + n_sa);
        const float q = e->has_child() ? e->get_value() : q_fpu;
        const float score = q + c_puct * u;
        if (score > max_score) {
            out = e;
            max_score = score;
        }
    }
    assert(!is_mate_to_win() || out == nullptr || out->is_mate_to_win());
    return out;
}

Edge* Node::select_unexplored()
{
    if (is_leaf())
        return nullptr;
    for (Edge* e = m_child; e->get_parent() == this; ++e) {
        if (e->get_visits() == 0u)
            return e;
    }
    return nullptr;
}

void Node::simulate_mate_and_expand(
    ContiguousBuffer<Edge>& buffer, const move_t action)
{
    m_value = 1.f;
    if (has_child()) {
        Edge* c = m_child;
        for (; c->m_parent == this; ++c) {
            if (c->get_action() == action) {
                c->m_mate = 1;
                return;
            }
        }
        throw std::invalid_argument("Given action not found.");
    } else if (not buffer.is_full()) {
        m_child = buffer.emplace_next(this, 1.f, action);
        m_child->m_mate = 1;
    }
}

void Node::backprop()
{
    m_value = m_prior_value;
    m_visits = 1u;
    if (is_leaf())
        return;

    bool is_mate_to_win = false;
    bool is_mate_to_lose = true;
    for (const Edge* e = m_child; e->m_parent == this; ++e) {
        is_mate_to_win |= e->is_mate_to_win();
        is_mate_to_lose &= e->is_mate_to_lose();
        m_value += e->get_value() * static_cast<float>(e->get_visits());
        m_visits += e->get_visits();
    }

    if (is_mate_to_win or m_prior_value == 1.f) {
        m_value = 1.f;
    } else if (is_mate_to_lose or m_prior_value == -1.f) {
        m_value = -1.f;
    } else
        m_value /= static_cast<float>(m_visits);
}

const Node* Node::get_child_node_of(const move_t action) const
{
    for (auto e = get_child(); e; e = e->get_sibling()) {
        if (e->get_action() == action)
            return e->get_child();
    }
    return nullptr;
}

float Node::first_play_urgency() const
{
    if (m_child == nullptr)
        return -1.f;
    // https://lczero.org/dev/lc0/search/alphazero/#first-play-urgency-fpu
    float q = m_value;
    for (const Edge* e = m_child; e->m_parent == this; ++e) {
        if (e->m_visits)
            q -= e->m_prior_proba;
    }
    return q;
}

} // namespace vshogi::engine::mcgs
