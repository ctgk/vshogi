#include "vshogi/engine/az/node.hpp"

namespace vshogi::engine::az
{

Node::Node()
    : tree::Node<Node>(), m_action{}, m_proba(0.f), m_visit_count(0u),
      m_visit_count_by_random(0u), m_sqrt_visit_count(0.f), m_q_value(0.f),
      m_is_mate(false)
{
}

Node::Node(const move_t& action, const float proba)
    : tree::Node<Node>(), m_action(action), m_proba(proba), m_visit_count(0u),
      m_visit_count_by_random(0u), m_sqrt_visit_count(0.f), m_q_value(0.f),
      m_is_mate(false)
{
}

void Node::init()
{
    tree::Node<Node>::init();
    m_action = static_cast<move_t>(0);
    m_proba = 0.f;
    m_visit_count = 0u;
    m_visit_count_by_random = 0u;
    m_sqrt_visit_count = 0.f;
    m_q_value = 0.f;
    m_is_mate = false;
}

float Node::get_q_value(const uint greedy_depth) const
{
    if (m_is_mate || !m_most_visited_child || !greedy_depth)
        return m_q_value;
    return -m_most_visited_child->get_q_value(greedy_depth - 1u);
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
            m_visit_count_by_random = c->m_visit_count_by_random;
            m_sqrt_visit_count = c->m_sqrt_visit_count;
            m_q_value = c->m_q_value;
            m_is_mate = c->m_is_mate;
            // m_parent = c->m_parent;
            // m_sibling = c->m_sibling;
            m_most_visited_child = c->m_most_visited_child;
            m_child = std::move(c->m_child);
            return *this;
        }
    }
    m_visit_count = 0u;
    m_visit_count_by_random = 0u;
    m_sqrt_visit_count = 0.f;
    m_q_value = 0.f;
    m_is_mate = false;
    m_most_visited_child = nullptr;
    m_child.reset();
    return *this;
}

Node* Node::select(const float& c_puct, const float& p_random)
{
    assert(has_child());
    Node* const c = select_best_or_random_child(c_puct, p_random);
    c->m_parent = this;
    return c;
}

Node* Node::select_best_or_random_child(
    const float c_puct, const float p_random)
{
    if (select_best_over_random(p_random))
        return select_best_child(c_puct);
    return select_random_child();
}

bool Node::select_best_over_random(const float p_random)
{
    if (m_is_mate)
        return true;
    constexpr float eps = 1e-3f;
    if (p_random < eps)
        return true;
    const float s = dist01(random_engine);
    return s > p_random;
}

Node* Node::select_best_child(const float c_puct)
{
    const float q_fpu = first_play_urgency();
    Node* out = nullptr;
    float max_puct_score = -100.f;

    for (Node* c = m_child.get(); c; c = c->m_sibling.get()) {
        const float score = puct_score_of(c, c_puct, q_fpu);
        if (score > max_puct_score) {
            max_puct_score = score;
            out = c;
        }
    }
    return out;
}

float Node::first_play_urgency() const
{
    // https://lczero.org/dev/lc0/search/alphazero/#first-play-urgency-fpu
    float q = m_q_value;
    for (const Node* c = m_child.get(); c; c = c->get_sibling()) {
        if (c->m_visit_count)
            q -= c->m_proba;
    }
    return q;
}

float Node::puct_score_of(
    const Node* const c, const float c_puct, const float q_fpu) const
{
    const float q = (m_visit_count) ? -c->m_q_value : q_fpu;
    if (c->is_mate_to_lose()) {
        const float p_plus_1 = c->m_proba + 1.f;
        return q + 2.f + p_plus_1 * m_sqrt_visit_count * c_puct;
    }
    const float u = c->m_proba * m_sqrt_visit_count
                    / static_cast<float>(1 + c->m_visit_count);
    return q + u * c_puct;
}

Node* Node::select_random_child()
{
    constexpr uint num_try_max = 3u;
    const uint num = count_childs();
    const float p = 1.f / static_cast<float>(num);
    Node* c = nullptr;
    for (uint ii = num_try_max; ii--;) {
        float s = dist01(random_engine);
        for (c = m_child.get(); c; c = c->m_sibling.get()) {
            if (s < p) {
                if (c->is_mate_to_win())
                    break; // Do not select child that leads to LOSS
                return c;
            } else
                s -= p;
        }
    }
    assert(c != nullptr);
    ++(c->m_visit_count_by_random);
    return c;
}

void Node::simulate_mate_and_expand(const move_t& action)
{
    m_q_value = 1.f;
    m_is_mate = true;
    if (has_child()) {
        Node* c = m_child.get();
        for (; c; c = c->m_sibling.get()) {
            if (c->get_action() == action) {
                m_most_visited_child = c;
                break;
            }
        }
        if (c == nullptr)
            throw std::invalid_argument("Given action not found.");
    } else {
        m_child = std::make_unique<Node>(action, 1.f);
        m_most_visited_child = m_child.get();
    }
    m_most_visited_child->m_q_value = -1.f;
    m_most_visited_child->m_is_mate = true;
}

void Node::update_most_visited_child(Node* const candidate)
{
    // |   candidate(c)  |                m_most_visited_child(m)             |
    // |                 | nullptr | is_mate_to_win | is_mate_to_lose | other |
    // |         nullptr |    #    |       (m)      |       (m)       |  (m)  |
    // |  is_mate_to_win |   (c)   |        #       |        x        |  (m)  |
    // | is_mate_to_lose |   (c)   |       (c)      |        #        |  (c)  |
    // |           other |   (c)   |       (c)      |        x        |   ?   |
    // #: don't care
    // x: impossible to happen.

    if (m_most_visited_child == nullptr) {
        m_most_visited_child = candidate;
        return;
    }
    if (candidate == nullptr) {
        return;
    }
    assert(
        candidate->is_mate_to_win() ? !m_most_visited_child->is_mate_to_lose()
                                    : true);
    if (candidate->is_mate_to_lose()
        || m_most_visited_child->is_mate_to_win()) {
        m_most_visited_child = candidate;
        return;
    }
    if (candidate->get_visit_count_excluding_random()
        > m_most_visited_child->get_visit_count_excluding_random())
        m_most_visited_child = candidate;
    else if (
        (candidate->get_visit_count_excluding_random()
         == m_most_visited_child->get_visit_count_excluding_random())
        && (candidate->m_q_value < m_most_visited_child->m_q_value))
        m_most_visited_child = candidate;
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
    m_sqrt_visit_count = std::sqrt(static_cast<float>(m_visit_count));
    update_most_visited_child(child);
    if (m_is_mate) {
        // preserve `m_q_value` if it is already found to be mate.
    } else if (
        (m_most_visited_child && m_most_visited_child->is_mate_to_lose())
        || all_childs_are_mate_to_win()) {
        m_is_mate = true;
        assert((0.99f < std::abs(v)) && (std::abs(v) < 1.01f));
        m_q_value = v;
    } else if (!m_is_mate) {
        m_q_value = (v + count_before * m_q_value) / count_after;
    }
    return m_parent;
}

} // namespace vshogi::engine::az
