#ifndef PYTHON_VSHOGI_WRAPPER_HPP
#define PYTHON_VSHOGI_WRAPPER_HPP

#include <algorithm>
#include <cmath>

#include "vshogi/common/config.hpp"
#include "vshogi/engine/dfpn/searcher.hpp"
#include "vshogi/engine/mcts.hpp"
#include "vshogi/engine/piece_value.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyvshogi
{

template <class T>
inline bool has(const std::vector<T>& vec, const T& target)
{
    return std::find(vec.cbegin(), vec.cend(), target) != vec.cend();
}

template <class Parameters>
inline void export_to_jpn(pybind11::module& m)
{
    namespace py = pybind11;
    using C = vshogi::Configuration<Parameters>;
    using PT = vshogi::PieceTraits<Parameters>;
    using ST = vshogi::Squares<Parameters>;
    m.def("to_jpn", [](const typename C::PieceType pt) {
        return PT::to_jpn(pt);
    });
    m.def("to_jpn", [](const typename C::Piece p) {
        return PT::to_jpn(PT::to_piece_type(p));
    });
    m.def("to_jpn", [](const typename C::Square sq) { return ST::to_jpn(sq); });
}

template <class Parameters>
inline void export_to_sfen(pybind11::module& m)
{
    namespace py = pybind11;
    using C = vshogi::Configuration<Parameters>;
    using PT = vshogi::PieceTraits<Parameters>;
    m.def("to_sfen", [](const typename C::PieceType pt) -> std::string {
        if ((pt == C::NA) || !PT::is_promoted(pt))
            return std::string(1, PT::to_char(pt));
        return std::string(1, '+') + PT::to_char(pt);
    });
    m.def("to_sfen", [](const typename C::Piece p) -> std::string {
        auto c = PT::to_char(PT::to_piece_type(p));
        if (PT::get_color(p) == vshogi::BLACK)
            c = std::toupper(c);
        if ((p == C::VOID) || !PT::is_promoted(p))
            return std::string(1, c);
        return std::string(1, '+') + c;
    });
}

template <class Parameters>
inline void export_board(pybind11::module& m)
{
    namespace py = pybind11;
    using C = vshogi::Configuration<Parameters>;
    using Board = vshogi::Board<Parameters>;
    using Square = typename C::Square;
    py::class_<Board>(m, "Board")
        .def(
            "__getitem__",
            py::overload_cast<const Square&>(&Board::operator[], py::const_))
        .def(
            "__getitem__",
            [](const Board& self, const uint index) {
                return self[static_cast<Square>(index)];
            })
        .def_property_readonly_static(
            "num_files", [](py::object) { return C::num_files; })
        .def_property_readonly_static(
            "num_ranks", [](py::object) { return C::num_ranks; })
        .def_property_readonly_static(
            "num_squares", [](py::object) { return C::num_squares; });
}

template <class Parameters>
inline void export_piece_stand(pybind11::module& m)
{
    using C = vshogi::Configuration<Parameters>;
    using Stand = vshogi::Stand<Parameters>;
    using PieceType = typename Parameters::PieceType;
    pybind11::class_<Stand>(m, "Stand")
        .def("count", &Stand::count)
        .def("any", &Stand::any)
        .def("to_dict", [](const Stand& self) -> pybind11::dict {
            pybind11::dict out;
            for (auto pt : C::stand_piece_type_iterator())
                out[pybind11::cast(pt)] = self.count(pt);
            return out;
        });
}

template <class Parameters>
inline void export_move(pybind11::module& m)
{
    namespace py = pybind11;
    using Move = vshogi::Move<Parameters>;
    using Square = typename Parameters::Square;
    using PieceType = typename Parameters::PieceType;
    py::class_<Move>(m, "Move")
        .def(
            py::init<const Square, const Square, const bool>(),
            py::arg("src"),
            py::arg("dst"),
            py::arg("promote") = false)
        .def(
            py::init<const PieceType, const Square>(),
            py::arg("src"),
            py::arg("dst"))
        .def(py::init<const std::uint16_t>(), py::arg("value"))
        .def(
            py::init(
                [](const std::string& sfen) { return Move(sfen.c_str()); }),
            py::arg("sfen"))
        .def_property_readonly("destination", &Move::destination)
        .def_property_readonly("promote", &Move::promote)
        .def_property_readonly(
            "source",
            [](const Move& self) -> py::object {
                if (self.is_drop())
                    return py::cast(self.source_piece());
                return py::cast(self.source_square());
            })
        .def("is_drop", &Move::is_drop)
        .def("rotate", &Move::rotate)
        .def("hflip", &Move::hflip)
        .def("_to_dlshogi_policy_index", &Move::to_dlshogi_policy_index)
        .def_static("_num_policy_per_square", &Move::num_policy_per_square)
        .def("__hash__", &Move::hash)
        .def(
            "to_sfen",
            [](const Move& self) {
                std::string out((self.promote()) ? 5 : 4, '\0');
                self.to_sfen(out.data());
                return out;
            })
        .def("__eq__", &Move::operator==)
        .def("__ne__", &Move::operator!=)
        .def(py::pickle(
            [](const Move& self) { return py::make_tuple(self.hash()); },
            [](py::tuple t) { return Move(t[0].cast<std::size_t>()); }));
}

template <class Parameters>
inline void export_state(pybind11::module& m)
{
    namespace py = pybind11;
    using State = vshogi::State<Parameters>;
    using Move = vshogi::Move<Parameters>;

    py::class_<State>(m, "State")
        .def(py::init<const std::string&>())
        .def("hflip", &State::hflip)
        .def("to_sfen", &State::to_sfen)
        .def(
            "to_dlshogi_features",
            [](const State& self) {
                const auto shape = std::vector<py::ssize_t>(
                    {1,
                     State::num_ranks,
                     State::num_files,
                     State::feature_channels()});
                auto out = py::array_t<float>(shape);
                self.to_feature_map(out.mutable_data());
                return out;
            })
        .def(
            "to_dlshogi_features",
            [](const State& self, py::array_t<float>& out) {
                self.to_feature_map(out.mutable_data());
            })
        .def(
            "to_dlshogi_policy",
            [](const State& self,
               const py::dict& action_proba,
               const float default_value) -> py::array_t<float> {
                const auto turn = self.get_turn();
                constexpr auto size = State::num_dlshogi_policy();
                auto out = py::array_t<float>(std::vector<py::ssize_t>({size}));
                float* const data = out.mutable_data();
                std::fill(data, data + size, default_value);
                for (auto it = action_proba.begin(); it != action_proba.end();
                     ++it) {
                    const auto move = it->first.cast<Move>();
                    const auto index
                        = (turn == vshogi::BLACK)
                              ? move.to_dlshogi_policy_index()
                              : move.rotate().to_dlshogi_policy_index();
                    data[index] = it->second.cast<float>();
                }
                return out;
            },
            py::arg("action_proba"),
            py::arg("default_value"))
        .def(
            "to_dlshogi_policy",
            [](const State& self,
               const py::dict& action_proba,
               const float default_value,
               py::array_t<float>& out) {
                const auto turn = self.get_turn();
                constexpr auto size = State::num_dlshogi_policy();
                float* const data = out.mutable_data();
                std::fill(data, data + size, default_value);
                for (auto it = action_proba.begin(); it != action_proba.end();
                     ++it) {
                    const auto move = it->first.cast<Move>();
                    const auto index
                        = (turn == vshogi::BLACK)
                              ? move.to_dlshogi_policy_index()
                              : move.rotate().to_dlshogi_policy_index();
                    data[index] = it->second.cast<float>();
                }
            },
            py::arg("action_proba"),
            py::arg("default_value"),
            py::arg("out"));
}

template <class Parameters>
inline void export_game(pybind11::module& m)
{
    namespace py = pybind11;
    using C = vshogi::Configuration<Parameters>;
    using Game = vshogi::Game<Parameters>;
    using Move = vshogi::Move<Parameters>;
    py::class_<Game>(m, "_Game")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("get_turn", &Game::get_turn)
        .def("get_board", &Game::get_board)
        .def(
            "get_stand",
            py::overload_cast<const vshogi::ColorEnum>(
                &Game::get_stand, py::const_))
        .def("get_result", &Game::get_result)
        .def("get_zobrist_hash", &Game::get_zobrist_hash)
        .def("ply", &Game::ply)
        .def("count_repetitions", &Game::count_repetitions)
        .def("get_legal_moves", &Game::get_legal_moves)
        .def("get_check_moves", &Game::get_check_moves)
        .def("to_sfen", &Game::to_sfen)
        .def("is_legal", &Game::is_legal)
        .def("in_check", &Game::in_check)
        .def("is_valid_piece_count", &Game::is_valid_piece_count)
        .def("hflip", &Game::hflip)
        .def("rotate", &Game::rotate)
        .def("to_jpn", &Game::to_jpn)
        .def("to_eng", &Game::to_eng)
        .def("apply", [](Game& self, const Move& m) { return self.apply(m); })
        .def("resign", &Game::resign)
        .def("declare_draw", &Game::declare_draw)
        .def("undo", &Game::undo)
        .def_static("ranks", []() { return &Game::num_ranks; })
        .def_static("files", []() { return &Game::num_files; })
        .def_static("feature_channels", &Game::feature_channels)
        .def_static("num_dlshogi_policy", &Game::num_dlshogi_policy)
        .def_static(
            "get_attention",
            []() {
                const auto n = Game::num_squares;
                const auto shape = std::vector<py::ssize_t>({n, n});
                auto out = py::array_t<float>(shape);
                Game::attention_matrix(out.mutable_data());
                return out;
            })
        .def_static(
            "get_local_attentions",
            []() {
                constexpr uint num_dir = Parameters::num_dir;
                const auto n = Game::num_squares;
                const auto shape = std::vector<py::ssize_t>({num_dir, n, n});
                auto out = py::array_t<float>(shape);
                for (auto dir : C::direction_iterator()) {
                    Game::attention_matrix(
                        &out.mutable_at(static_cast<int>(dir), 0, 0),
                        {dir},
                        true);
                }
                return out;
            })
        .def_static(
            "get_adjacent_attention",
            []() {
                const auto n = Game::num_squares;
                const auto shape = std::vector<py::ssize_t>({n, n});
                auto out = py::array_t<float>(shape);
                Game::attention_matrix(
                    out.mutable_data(),
                    {vshogi::DIR_N,
                     vshogi::DIR_W,
                     vshogi::DIR_E,
                     vshogi::DIR_S});
                return out;
            })
        .def_static(
            "get_diagonal_attention",
            []() {
                const auto n = Game::num_squares;
                const auto shape = std::vector<py::ssize_t>({n, n});
                auto out = py::array_t<float>(shape);
                Game::attention_matrix(
                    out.mutable_data(),
                    {vshogi::DIR_NW,
                     vshogi::DIR_NE,
                     vshogi::DIR_SW,
                     vshogi::DIR_SE});
                return out;
            })
        .def(
            "to_dlshogi_features",
            [](const Game& self) {
                const auto shape = std::vector<py::ssize_t>(
                    {1,
                     Game::num_ranks,
                     Game::num_files,
                     Game::feature_channels()});
                auto out = py::array_t<float>(shape);
                self.to_feature_map(out.mutable_data());
                return out;
            })
        .def(
            "to_dlshogi_features",
            [](const Game& self, py::array_t<float>& out) {
                self.to_feature_map(out.mutable_data());
            })
        .def(
            "to_dlshogi_policy",
            [](const Game& self,
               const py::dict& visit_proba,
               const float default_value) -> py::array_t<float> {
                const auto turn = self.get_turn();
                constexpr auto size = Game::num_dlshogi_policy();
                auto out = py::array_t<float>(std::vector<py::ssize_t>({size}));
                float* const data = out.mutable_data();
                std::fill(data, data + size, default_value);
                for (auto it = visit_proba.begin(); it != visit_proba.end();
                     ++it) {
                    const auto move = it->first.cast<Move>();
                    const auto index
                        = (turn == vshogi::BLACK)
                              ? move.to_dlshogi_policy_index()
                              : move.rotate().to_dlshogi_policy_index();
                    data[index] = it->second.cast<float>();
                }
                return out;
            })
        .def(
            "masked_softmax",
            [](const Game& self, const py::array_t<float>& logits) -> py::dict {
                py::dict out;
                const auto is_black_turn = (self.get_turn() == vshogi::BLACK);
                const auto& actions = self.get_legal_moves();
                auto proba = std::vector<float>(actions.size());
                const auto data = logits.data();
                for (std::size_t ii = actions.size(); ii--;) {
                    const auto index
                        = (is_black_turn)
                              ? actions[ii].to_dlshogi_policy_index()
                              : actions[ii].rotate().to_dlshogi_policy_index();
                    proba[ii] = data[index];
                }
                vshogi::softmax(proba);
                for (std::size_t ii = actions.size(); ii--;) {
                    out[py::cast(actions[ii])] = proba[ii];
                }
                return out;
            })
        .def(
            "get_mate_moves_if_any",
            [](Game& self, const int num_dfpn_nodes) -> py::object {
                vshogi::engine::dfpn::Searcher<Parameters> dfpn{};
                dfpn.search(self, num_dfpn_nodes);
                if (dfpn.proved_mate()) {
                    const auto moves = dfpn.get_mate_moves(self);
                    if (moves.size() > 0u)
                        return py::cast(moves);
                }
                return py::none();
            },
            py::arg("num_dfpn_nodes"))
        .def("copy", [](const Game& self) { return Game(self); });
}

template <class Parameters>
inline void export_mcts_node(pybind11::module& m)
{
    namespace py = pybind11;
    using Game = vshogi::Game<Parameters>;
    using Move = vshogi::Move<Parameters>;
    using Node = vshogi::engine::mcts::Node<Parameters>;

    py::class_<Node>(m, "MctsNode")
        .def("get_visit_count", &Node::get_visit_count)
        .def(
            "get_visit_count_excluding_random",
            &Node::get_visit_count_excluding_random)
        .def("get_value", &Node::get_value)
        .def("get_q_value", &Node::get_q_value)
        .def(
            "get_actions",
            [](const Node& self) {
                std::vector<Move> out;
                out.reserve(self.get_num_child());
                const Node* ch = self.get_child();
                for (; ch != nullptr; ch = ch->get_sibling()) {
                    out.emplace_back(ch->get_action());
                }
                return out;
            })
        .def("get_proba", &Node::get_proba)
        .def("get_child", [](Node& node, const Move& action) -> py::object {
            const auto out = node.get_child(action);
            if (out == nullptr)
                return py::none();
            return py::cast(*out, py::return_value_policy::reference);
        });
}

template <class Parameters>
inline void export_mcts_searcher(pybind11::module& m)
{
    namespace py = pybind11;
    using Game = vshogi::Game<Parameters>;
    using Node = vshogi::engine::mcts::Node<Parameters>;
    using Searcher = vshogi::engine::mcts::Searcher<Parameters>;

    py::class_<Searcher>(m, "Mcts")
        .def(py::init<const float, const float, const uint, const uint>())
        .def("init_root", &Searcher::init_root)
        .def(
            "search",
            [](Searcher& self, Game& game) -> py::object {
                const auto out = self.search(game);
                if (out == nullptr)
                    return py::none();
                return py::cast(*out, py::return_value_policy::reference);
            })
        .def(
            "simulate_expand_backprop",
            [](Searcher& self,
               Node* const leaf,
               Game& game,
               const float value,
               const py::array_t<float>& policy_logits) {
                self.simulate_expand_backprop(
                    leaf, game, value, policy_logits.data());
            })
        .def("apply", &Searcher::apply)
        .def(
            "get_root",
            [](Searcher& self) -> py::object {
                const auto out = self.get_root();
                if (out == nullptr)
                    return py::none();
                return py::cast(*out, py::return_value_policy::reference);
            })
        .def("proved_mate", &Searcher::proved_mate)
        .def("get_visit_count", &Searcher::get_visit_count)
        .def("get_action_by_visit_max", &Searcher::get_action_by_visit_max)
        .def(
            "get_action_by_visit_distribution",
            &Searcher::get_action_by_visit_distribution)
        .def(
            "get_action_by_q_distribution",
            &Searcher::get_action_by_q_distribution);
}

template <class Parameters>
inline void export_dfpn_node(pybind11::module& m)
{
    namespace py = pybind11;
    using Node = vshogi::engine::dfpn::Node<Parameters>;
    constexpr float unit = static_cast<float>(vshogi::engine::dfpn::unit);
    constexpr uint inf = vshogi::engine::dfpn::inf;
    py::class_<Node>(m, "DfpnNode")
        .def(
            "pn",
            [](const Node& self, const bool offence) {
                const auto n = self.pn(offence);
                if (n == inf)
                    return std::numeric_limits<float>::infinity();
                return static_cast<float>(n) / unit;
            })
        .def(
            "dn",
            [](const Node& self, const bool offence) {
                const auto n = self.dn(offence);
                if (n == inf)
                    return std::numeric_limits<float>::infinity();
                return static_cast<float>(n) / unit;
            })
        .def("get_action", &Node::get_action)
        .def("has_child", &Node::has_child)
        .def(
            "get_child_1st",
            [](const Node& self) -> py::object {
                const auto c = self.get_child_1st();
                if (c == nullptr)
                    return py::none();
                return py::cast(c, py::return_value_policy::reference);
            })
        .def(
            "get_child_2nd",
            [](const Node& self) -> py::object {
                const auto c = self.get_child_2nd();
                if (c == nullptr)
                    return py::none();
                return py::cast(c, py::return_value_policy::reference);
            })
        .def("get_children", [](const Node& self) -> py::object {
            std::vector<const Node*> out;
            if (self.has_child()) {
                for (auto ch = self.get_child(); ch; ch = ch->get_sibling())
                    out.emplace_back(ch);
            }
            return py::cast(out, py::return_value_policy::reference);
        });
}

template <class Parameters>
inline void export_dfpn_searcher(pybind11::module& m)
{
    namespace py = pybind11;
    using Searcher = vshogi::engine::dfpn::Searcher<Parameters>;

    py::class_<Searcher>(m, "DfpnSearcher")
        .def(py::init<const uint>())
        .def("init", &Searcher::init)
        .def("search", &Searcher::search)
        .def("proved_mate", &Searcher::proved_mate)
        .def("proved_no_mate", &Searcher::proved_no_mate)
        .def("proved", &Searcher::proved)
        .def("get_search_count", &Searcher::get_search_count)
        .def("get_mate_move", &Searcher::get_mate_move)
        .def("get_mate_moves", &Searcher::get_mate_moves)
        .def("get_root", [](const Searcher& self) -> py::object {
            const auto out = self.get_root();
            if (out == nullptr)
                return py::none();
            return py::cast(*out, py::return_value_policy::reference);
        });
}

template <class Parameters>
inline void export_value_functions(pybind11::module& m)
{
    namespace py = pybind11;
    m.def("piece_value_func", &vshogi::engine::piece_value_func<Parameters>);
}

template <class Parameters>
void export_classes(pybind11::module& m)
{
    export_to_jpn<Parameters>(m);
    export_to_sfen<Parameters>(m);
    export_board<Parameters>(m);
    export_piece_stand<Parameters>(m);
    export_move<Parameters>(m);
    export_state<Parameters>(m);
    export_game<Parameters>(m);
    export_mcts_searcher<Parameters>(m);
    export_mcts_node<Parameters>(m);
    export_value_functions<Parameters>(m);
    export_dfpn_searcher<Parameters>(m);
    export_dfpn_node<Parameters>(m);
}

} // namespace pyvshogi

#endif // PYTHON_VSHOGI_WRAPPER_HPP
