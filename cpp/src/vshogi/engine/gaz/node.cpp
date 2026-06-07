#include "vshogi/engine/gaz/node.hpp"

namespace vshogi::engine::gaz
{

Node::Node()
    : tree::Node<Node>(), m_action{}, m_is_mate(false), m_logit(0.f),
      m_visit_count(0u), m_q_value(0.f)
{
}

void Node::init()
{
    tree::Node<Node>::init();
    m_logit = 0.f;
    m_visit_count = 0u;
    m_q_value = 0.f;
    m_is_mate = false;
}
void Node::init(Node* const parent, const move_t& action, const float logit)
{
    init();
    m_parent = parent;
    m_action = action;
    m_logit = logit;
}

void Node::init_as_begin()
{
    tree::Node<Node>::init_as_begin();
    m_action = static_cast<move_t>(0);
}

float Node::get_q_value(const uint greedy_depth, const uint min_visits) const
{
    if (m_is_mate || !m_child_1st)
        return m_q_value;
    if (!greedy_depth || (m_child_1st->get_visit_count() < min_visits))
        return compute_v_pi();
    return -m_child_1st->get_q_value(greedy_depth - 1u, min_visits);
}

float Node::compute_v_pi() const
{
    float n = 0.f;
    float d = 1e-8f; // to prevent 0 division
    float pi[max_legal_moves] = {};
    uint ii = 0u;
    for (const Node* c = get_child(); c; c = c->get_sibling()) {
        if (c->get_visit_count())
            pi[ii++] = c->get_logit();
    }
    softmax(pi, ii);
    ii = 0u;
    for (const Node* c = get_child(); c; c = c->get_sibling()) {
        if (c->get_visit_count()) {
            const float p = pi[ii++];
            n += p * -c->get_q_value();
            d += p;
        }
    }
    return n / d;
}

const Node* Node::get_child_of(const move_t& action) const
{
    for (const Node* c = get_child(); c; c = c->get_sibling()) {
        if (c->m_action == action)
            return c;
    }
    return nullptr;
}

void Node::improved_policy(float* const out, const float temperature) const
{
    const uint max_visits = m_child_1st ? m_child_1st->get_visit_count() : 0u;
    uint ii = 0u;
    for (const Node* c = get_child(); c; c = c->get_sibling()) {
        const float s = sigma(completed_q_value_of(c), max_visits);
        out[ii++] = (c->get_logit() + s) / temperature;
    }
    softmax(out, ii);
}

float Node::completed_q_value_of(const Node* const child) const
{
    // eq. 10 in https://openreview.net/pdf?id=bERaNdoegnO
    if (child->get_visit_count())
        return -child->m_q_value;
    return m_q_value;
}

void Node::simulate_mate_and_expand(
    ContiguousBuffer<Node>& buffer, const move_t& action)
{
    m_q_value = 1.f;
    m_is_mate = true;
    if (has_child()) {
        for (Node* c = m_child; c->m_parent == this; ++c) {
            if (c->get_action() == action) {
                m_child_1st = c;
                m_child_1st->m_q_value = -1.f;
                m_child_1st->m_is_mate = true;
                return;
            }
        }
        throw std::invalid_argument("Given action not found.");
    } else if (not buffer.is_full()) {
        m_child = buffer.emplace_next(this, action, 1.f);
        m_child_1st = m_child;
        m_child_1st->m_q_value = -1.f;
        m_child_1st->m_is_mate = true;
    }
}

void Node::update_most_visited_child(Node* const candidate)
{
    // |   candidate(c)  |                    m_child_1st(m)                  |
    // |                 | nullptr | is_mate_to_win | is_mate_to_lose | other |
    // |         nullptr |    #    |       (m)      |       (m)       |  (m)  |
    // |  is_mate_to_win |   (c)   |        #       |        x        |  (m)  |
    // | is_mate_to_lose |   (c)   |       (c)      |        #        |  (c)  |
    // |           other |   (c)   |       (c)      |        x        |   ?   |
    // #: don't care
    // x: impossible to happen.

    if (m_child_1st == nullptr) {
        m_child_1st = candidate;
        return;
    }
    if (candidate == nullptr) {
        return;
    }
    assert(
        candidate->is_mate_to_win() ? !m_child_1st->is_mate_to_lose() : true);
    if (candidate->is_mate_to_lose() || m_child_1st->is_mate_to_win()) {
        m_child_1st = candidate;
        return;
    }
    if (candidate->get_visit_count() > m_child_1st->get_visit_count())
        m_child_1st = candidate;
    else if (
        (candidate->get_visit_count() == m_child_1st->get_visit_count())
        && (candidate->m_q_value < m_child_1st->m_q_value))
        m_child_1st = candidate;
}

bool Node::all_childs_are_mate_to_win() const
{
    for (const Node* c = get_child(); c; c = c->get_sibling()) {
        if (!c->is_mate_to_win())
            return false;
    }
    return true;
}

Node* Node::backprop(const float v, Node* const child)
{
    const auto count_before = static_cast<float>(m_visit_count++);
    const auto count_after = static_cast<float>(m_visit_count);
    update_most_visited_child(child);
    if (m_is_mate) {
        // preserve `m_q_value` if it is already found to be mate.
    } else if (
        has_child()
        && ((m_child_1st && m_child_1st->is_mate_to_lose())
            || all_childs_are_mate_to_win())) {
        m_is_mate = true;
        assert((0.99f < std::abs(v)) && (std::abs(v) < 1.01f));
        m_q_value = v;
    } else if (!m_is_mate) {
        m_q_value = (v + count_before * m_q_value) / count_after;
    }
    return m_parent;
}

} // namespace vshogi::engine::gaz
