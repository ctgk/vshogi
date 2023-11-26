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
            pybind11::overload_cast<Square>(
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
    using SquareEnum = typename Move::SquareEnum;
    using PieceTypeEnum = typename Move::PieceTypeEnum;
    pybind11::class_<Move>(m, "Move")
        .def(
            pybind11::init<const SquareEnum, const SquareEnum, const bool>(),
            pybind11::arg("dst"),
            pybind11::arg("src"),
            pybind11::arg("promote") = false)
        .def(
            pybind11::init<const SquareEnum, const PieceTypeEnum>(),
            pybind11::arg("dst"),
            pybind11::arg("src"))
        .def_property_readonly("destination", &Move::destination)
        .def_property_readonly("promote", &Move::promote)
        .def_property_readonly(
            "source",
            [](const Move& self) -> pybind11::object {
                if (self.is_drop())
                    return pybind11::cast(self.source_piece());
                return pybind11::cast(self.source_square());
            })
        .def("is_drop", &Move::is_drop)
        .def("rotate", &Move::rotate)
        .def("hflip", &Move::hflip)
        .def("_to_dlshogi_policy_index", &Move::to_dlshogi_policy_index)
        .def_static("_num_policy_per_square", &Move::num_policy_per_square)
        .def("__hash__", &Move::hash)
        .def("__eq__", &Move::operator==)
        .def("__ne__", &Move::operator!=);
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
        .def("record_length", &Game::record_length)
        .def("get_legal_moves", &Game::get_legal_moves)
        .def("to_sfen", &Game::to_sfen)
        .def("is_legal", &Game::is_legal)
        .def("hflip", &Game::hflip)
        .def("apply", &Game::apply)
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
               const Move action,
               float max_value) -> py::array_t<float> {
                const auto turn = self.get_turn();
                const auto legal_moves = self.get_legal_moves();
                const auto num_legal_moves
                    = static_cast<float>(legal_moves.size());
                const auto action_is_legal = has(legal_moves, action);
                const auto eps = (1.f - max_value)
                                 / (action_is_legal ? (num_legal_moves - 1.f)
                                                    : num_legal_moves);

                const auto size = Game::num_dlshogi_policy();
                auto out = py::array_t<float>(std::vector<py::ssize_t>({size}));
                float* const data = out.mutable_data();
                std::fill(data, data + size, 0.f);
                for (auto&& move : legal_moves) {
                    const auto index
                        = (turn == vshogi::BLACK)
                              ? move.to_dlshogi_policy_index()
                              : move.rotate().to_dlshogi_policy_index();
                    data[index] = (action == move) ? max_value : eps;
                }
                return out;
            })
        .def(
            "get_mate_moves_if_any",
            [](const Game& self, const int num_dfpn_nodes) -> py::object {
                auto dfpn = vshogi::engine::dfpn::Searcher<Game, Move>(
                    num_dfpn_nodes);
                dfpn.set_root(self);
                if (dfpn.explore()) {
                    return py::cast(dfpn.get_mate_moves());
                } else {
                    return py::none();
                }
            },
            py::arg("num_dfpn_nodes"))
        .def("copy", [](const Game& self) { return Game(self); });
}

template <class Game, class Move>
inline void export_mcts_node(pybind11::module& m)
{
    using Node = vshogi::engine::mcts::Node<Game, Move>;
    pybind11::class_<Node>(m, "MctsNode")
        .def(
            pybind11::init([](const Game& g,
                              const float v,
                              const pybind11::array_t<float>& logits) {
                return Node(g, v, logits.data());
            }),
            pybind11::arg("game"),
            pybind11::arg("value"),
            pybind11::arg("policy_logits"))
        .def("get_visit_count", &Node::get_visit_count)
        .def("get_value", &Node::get_value)
        .def("get_q_value", &Node::get_q_value)
        .def("get_actions", &Node::get_actions)
        .def("get_proba", &Node::get_proba)
        .def(
            "get_child",
            [](Node& node, const Move& action) -> pybind11::object {
                const auto out = node.get_child(action);
                if (out == nullptr)
                    return pybind11::none();
                return pybind11::cast(
                    *out, pybind11::return_value_policy::reference);
            })
        .def(
            "simulate_expand_and_backprop",
            [](Node& self,
               const Game& game,
               const float value,
               const pybind11::array_t<float>& policy_logits) {
                const auto data = policy_logits.data();
                self.simulate_expand_and_backprop(game, value, data);
            })
        .def(
            "select",
            [](Node& node,
               Game& game,
               const float coeff_puct,
               const int non_random_ratio,
               const int random_depth) -> pybind11::object {
                const auto out = node.select(
                    game, coeff_puct, non_random_ratio, random_depth);
                if (out == nullptr)
                    return pybind11::none();
                return pybind11::cast(
                    *out, pybind11::return_value_policy::reference);
            })
        .def("apply", &Node::apply);
}

template <class Game>
void export_classes(pybind11::module& m)
{
    using Board = typename Game::Board;
    using Move = typename Game::Move;
    using Pieces = typename Game::Pieces;
    using Squares = typename Game::Squares;
    using SquareEnum = typename Game::SquareEnum;
    using Stand = typename Game::Stand;

    export_board<Board, SquareEnum>(m);
    export_piece_stand<Stand, Pieces>(m);
    export_move<Move>(m);
    export_game<Game>(m);
    export_mcts_node<Game, Move>(m);
}

} // namespace pyvshogi

#endif // PYTHON_VSHOGI_WRAPPER_HPP
