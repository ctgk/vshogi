#include "vshogi/engine/gaz/node.hpp"

namespace vshogi::engine::gaz
{

Node::Node()
    : tree::Node<Node>(), m_logit(0.f), m_visit_count(0u), m_q_value(0.f),
      m_is_mate(false)
{
}

Node::Node(const move_t& action, const float logit)
    : tree::Node<Node>(action), m_logit(logit), m_visit_count(0u),
      m_q_value(0.f), m_is_mate(false)
{
}

void Node::init()
{
    tree::Node<Node>::init();
    m_action = static_cast<move_t>(0);
    m_logit = 0.f;
    m_visit_count = 0u;
    m_q_value = 0.f;
    m_is_mate = false;
}

float Node::get_q_value(const uint greedy_depth) const
{
    if (m_is_mate || !m_child_1st || !greedy_depth)
        return m_q_value;
    return -m_child_1st->get_q_value(greedy_depth - 1u);
}

const Node* Node::get_child_of(const move_t& action) const
{
    for (const Node* c = get_child(); c; c = c->get_sibling()) {
        if (c->m_action == action)
            return c;
    }
    return nullptr;
}

Node& Node::apply(const move_t& action)
{
    assert(m_action == 0u);
    for (Node* c = m_child.get(); c; c = c->m_sibling.get()) {
        if (c->m_action == action) {
            // m_action = c->m_action;
            m_visit_count = c->m_visit_count;
            m_q_value = c->m_q_value;
            m_is_mate = c->m_is_mate;
            // m_parent = c->m_parent;
            // m_sibling = c->m_sibling;
            m_child_1st = c->m_child_1st;
            m_child = std::move(c->m_child);
            return *this;
        }
    }
    m_visit_count = 0u;
    m_q_value = 0.f;
    m_is_mate = false;
    m_child_1st = nullptr;
    m_child.reset();
    return *this;
}

void Node::improved_policy(float* const out) const
{
    const uint max_visits = m_child_1st ? m_child_1st->get_visit_count() : 0u;
    uint ii = 0u;
    for (const Node* c = get_child(); c; c = c->get_sibling()) {
        const float s = sigma(completed_q_value_of(c), max_visits);
        out[ii++] = c->get_logit() + s;
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

Node* Node::argmax_improved_policy()
{
    float pi_prime[max_legal_moves] = {};
    improved_policy(pi_prime);

    // assert(m_visit_count == (sum(c->get_visit_count()) + 1));
    uint ii = 0u;
    Node* out = m_child.get();
    float max_diff = pi_prime[0u]
                     - static_cast<float>(out->get_visit_count())
                           / static_cast<float>(m_visit_count);
    for (Node* c = out->m_sibling.get(); c; c = c->m_sibling.get()) {
        const float d = pi_prime[ii++]
                        - static_cast<float>(c->get_visit_count())
                              / static_cast<float>(m_visit_count);
        if (d > max_diff)
            out = c;
    }
    return out;
}

void Node::simulate_mate_and_expand(const move_t& action)
{
    m_q_value = 1.f;
    m_is_mate = true;
    if (has_child()) {
        Node* c = m_child.get();
        for (; c; c = c->m_sibling.get()) {
            if (c->get_action() == action) {
                m_child_1st = c;
                break;
            }
        }
        if (c == nullptr)
            throw std::invalid_argument("Given action not found.");
    } else {
        m_child = std::make_unique<Node>(action, 1.f);
        m_child_1st = m_child.get();
    }
    m_child_1st->m_q_value = -1.f;
    m_child_1st->m_is_mate = true;
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
        (m_child_1st && m_child_1st->is_mate_to_lose())
        || all_childs_are_mate_to_win()) {
        m_is_mate = true;
        assert((0.99f < std::abs(v)) && (std::abs(v) < 1.01f));
        m_q_value = v;
    } else if (!m_is_mate) {
        m_q_value = (v + count_before * m_q_value) / count_after;
    }
    return m_parent;
}

} // namespace vshogi::engine::gaz
