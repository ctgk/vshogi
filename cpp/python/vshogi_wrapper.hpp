#ifndef PYTHON_VSHOGI_WRAPPER_HPP
#define PYTHON_VSHOGI_WRAPPER_HPP

#include <algorithm>
#include <cmath>

#include "vshogi/engine/dfpn.hpp"
#include "vshogi/engine/mcts.hpp"

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

template <class Board, class Square>
inline void export_board(pybind11::module& m)
{
    pybind11::class_<Board>(m, "Board")
        .def(
            "__getitem__",
            pybind11::overload_cast<const Square&>(
                &Board::operator[], pybind11::const_));
}

template <class Stand, class Pieces>
inline void export_piece_stand(pybind11::module& m)
{
    pybind11::class_<Stand>(m, "Stand")
        .def("count", &Stand::count)
        .def("any", &Stand::any)
        .def("to_dict", [](const Stand& self) -> pybind11::dict {
            pybind11::dict out;
            for (auto p : Pieces::stand_piece_array)
                out[pybind11::cast(p)] = self.count(p);
            return out;
        });
}

template <class Move>
inline void export_move(pybind11::module& m)
{
    namespace py = pybind11;
    using SquareEnum = typename Move::SquareEnum;
    using PieceTypeEnum = typename Move::PieceTypeEnum;
    py::class_<Move>(m, "Move")
        .def(
            py::init<const SquareEnum, const SquareEnum, const bool>(),
            py::arg("dst"),
            py::arg("src"),
            py::arg("promote") = false)
        .def(
            py::init<const SquareEnum, const PieceTypeEnum>(),
            py::arg("dst"),
            py::arg("src"))
        .def(
            py::init([](const std::string& usi) { return Move(usi.c_str()); }),
            py::arg("usi"))
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
            "to_usi",
            [](const Move& self) {
                std::string out((self.promote()) ? 5 : 4, '\0');
                self.to_usi(out.data());
                return out;
            })
        .def("__eq__", &Move::operator==)
        .def("__ne__", &Move::operator!=)
        .def(py::pickle(
            [](const Move& self) { return py::make_tuple(self.hash()); },
            [](py::tuple t) { return Move(t[0].cast<std::size_t>()); }));
}

template <class State>
inline void export_state(pybind11::module& m)
{
    namespace py = pybind11;
    using Move = typename State::MoveType;

    py::class_<State>(m, "State")
        .def(py::init<const std::string&>())
        .def("hflip", &State::hflip)
        .def("to_sfen", &State::to_sfen)
        .def(
            "to_dlshogi_features",
            [](const State& self) {
                const auto shape = std::vector<py::ssize_t>(
                    {1,
                     State::ranks(),
                     State::files(),
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

template <class Game>
inline void export_game(pybind11::module& m)
{
    namespace py = pybind11;
    using Move = typename Game::Move;
    py::class_<Game>(m, "_Game")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("get_turn", &Game::get_turn)
        .def("get_board", &Game::get_board)
        .def("get_stand", &Game::get_stand)
        .def("get_result", &Game::get_result)
        .def("get_zobrist_hash", &Game::get_zobrist_hash)
        .def("record_length", &Game::record_length)
        .def("get_legal_moves", &Game::get_legal_moves)
        .def("to_sfen", &Game::to_sfen)
        .def("is_legal", &Game::is_legal)
        .def("hflip", &Game::hflip)
        .def("apply", [](Game& self, const Move& m) { return self.apply(m); })
        .def("resign", [](Game& self) { return self.resign(); })
        .def("get_move_at", &Game::get_move_at)
        .def("get_sfen_at", &Game::get_sfen_at)
        .def_static("ranks", &Game::ranks)
        .def_static("files", &Game::files)
        .def_static("feature_channels", &Game::feature_channels)
        .def_static("num_dlshogi_policy", &Game::num_dlshogi_policy)
        .def(
            "to_dlshogi_features",
            [](const Game& self) {
                const auto shape = std::vector<py::ssize_t>(
                    {1,
                     Game::ranks(),
                     Game::files(),
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
            "get_mate_moves_if_any",
            [](const Game& self, const int num_dfpn_nodes) -> py::object {
                if constexpr (std::is_same<Game, vshogi::animal_shogi::Game>::
                                  value) {
                    return py::none();
                } else {
                    auto dfpn = vshogi::engine::dfpn::Searcher<Game, Move>();
                    dfpn.set_game(self);
                    if (dfpn.explore(num_dfpn_nodes)) {
                        return py::cast(dfpn.get_mate_moves());
                    } else {
                        return py::none();
                    }
                }
            },
            py::arg("num_dfpn_nodes"))
        .def("copy", [](const Game& self) { return Game(self); });
}

template <class Game, class Move>
inline void export_mcts_node(pybind11::module& m)
{
    namespace py = pybind11;
    using Node = vshogi::engine::mcts::Node<Game, Move>;

    py::class_<Node>(m, "MctsNode")
        .def(
            py::init([](const Game& g,
                        const float v,
                        const py::array_t<float>& logits) {
                return Node(
                    g.get_legal_moves(), g.get_turn(), v, logits.data());
            }),
            py::arg("game"),
            py::arg("value"),
            py::arg("policy_logits"))
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
        .def(
            "get_child",
            [](Node& node, const Move& action) -> py::object {
                const auto out = node.get_child(action);
                if (out == nullptr)
                    return py::none();
                return py::cast(*out, py::return_value_policy::reference);
            })
        .def(
            "simulate_expand_and_backprop",
            [](Node& self,
               const Game& game,
               const float value,
               const py::array_t<float>& policy_logits) {
                const auto data = policy_logits.data();
                self.simulate_expand_and_backprop(
                    game.get_legal_moves(), game.get_turn(), value, data);
            })
        .def("simulate_mate_and_backprop", &Node::simulate_mate_and_backprop)
        .def(
            "_select_node_to_explore",
            [](Node& node,
               Game& game,
               const float coeff_puct,
               const int non_random_ratio,
               const int random_depth) -> py::object {
                const auto out = node.select(
                    game, coeff_puct, non_random_ratio, random_depth);
                if (out == nullptr)
                    return py::none();
                return py::cast(*out, py::return_value_policy::reference);
            })
        .def(
            "get_action_by_proba_max",
            [](const Node& self) {
                Move out{};
                float max_proba = -1.f;
                for (const Node* ch = self.get_child(); ch != nullptr;
                     ch = ch->get_sibling()) {
                    const auto p = ch->get_proba();
                    if (p > max_proba) {
                        max_proba = p;
                        out = ch->get_action();
                    }
                }
                return out;
            })
        .def("get_action_by_visit_max", &Node::get_action_by_visit_max)
        .def(
            "get_action_by_visit_distribution",
            &Node::get_action_by_visit_distribution)
        .def("apply", &Node::apply);
}

template <class Game, class Move>
inline void export_dfpn_searcher(pybind11::module& m)
{
    namespace py = pybind11;
    using Searcher = vshogi::engine::dfpn::Searcher<Game, Move>;

    py::class_<Searcher>(m, "DfpnSearcher")
        .def(py::init<>())
        .def("is_ready", &Searcher::is_ready)
        .def("set_game", &Searcher::set_game)
        .def("explore", &Searcher::explore)
        .def("found_mate", &Searcher::found_mate)
        .def("found_no_mate", &Searcher::found_no_mate)
        .def("found_conclusion", &Searcher::found_conclusion)
        .def("get_mate_moves", &Searcher::get_mate_moves);
}

template <class Game>
void export_classes(pybind11::module& m)
{
    using Board = typename Game::Board;
    using Move = typename Game::Move;
    using Pieces = typename Game::Pieces;
    using SquareEnum = typename Game::SquareEnum;
    using Stand = typename Game::Stand;
    using State = typename Game::StateType;

    export_board<Board, SquareEnum>(m);
    export_piece_stand<Stand, Pieces>(m);
    export_move<Move>(m);
    export_state<State>(m);
    export_game<Game>(m);
    export_mcts_node<Game, Move>(m);

    if constexpr (!std::is_same<Game, vshogi::animal_shogi::Game>::value)
        export_dfpn_searcher<Game, Move>(m);
}

} // namespace pyvshogi

#endif // PYTHON_VSHOGI_WRAPPER_HPP
