#ifndef PYTHON_VSHOGI_HPP
#define PYTHON_VSHOGI_HPP

#include <algorithm>
#include <cmath>

#include "vshogi/engine/mcts.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyvshogi
{

inline void softmax(std::vector<float>& v)
{
    const float maximum_value = *std::max_element(v.cbegin(), v.cend());
    float sum = 0.f;
    for (auto&& e : v) {
        e -= maximum_value;
        e = std::exp(e);
        sum += e;
    }
    for (auto&& e : v) {
        e /= sum;
    }
}

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

template <class Stand, class Piece, const Piece array[], int Size>
inline void export_piece_stand(pybind11::module& m)
{
    pybind11::class_<Stand>(m, "Stand")
        .def("count", &Stand::count)
        .def("any", &Stand::any)
        .def("to_dict", [](const Stand& self) -> pybind11::dict {
            pybind11::dict out;
            for (int i = 0; i < Size; ++i) {
                const auto p = array[i];
                out[pybind11::cast(p)] = self.count(p);
            }
            return out;
        });
}

template <class Move, class Square, class Piece>
inline void export_move(pybind11::module& m)
{
    pybind11::class_<Move>(m, "Move")
        .def(
            pybind11::init<const Square, const Square, const bool>(),
            pybind11::arg("dst"),
            pybind11::arg("src"),
            pybind11::arg("promote") = false)
        .def(
            pybind11::init<const Square, const Piece>(),
            pybind11::arg("dst"),
            pybind11::arg("src"))
        .def_property_readonly("destination", &Move::destination)
        .def_property_readonly("promote", &Move::promote)
        .def_property_readonly(
            "source",
            [](const Move& self) -> pybind11::object {
                if (self.is_drop())
                    return pybind11::cast(static_cast<Piece>(
                        static_cast<int>(self.source())
                        - static_cast<int>(Square::NUM_SQ)));
                return pybind11::cast(static_cast<Square>(self.source()));
            })
        .def("is_drop", &Move::is_drop)
        .def("rotate", &Move::rotate)
        .def("_to_dlshogi_policy_index", &Move::to_dlshogi_policy_index)
        .def_static("_num_policy_per_square", &Move::num_policy_per_square)
        .def("__hash__", &Move::hash)
        .def("__eq__", &Move::operator==)
        .def("__ne__", &Move::operator!=);
}

template <class Game, class Move>
inline void export_game(pybind11::module& m)
{
    pybind11::class_<Game>(m, "_Game")
        .def(pybind11::init<>())
        .def(pybind11::init<const std::string&>())
        .def("get_turn", &Game::get_turn)
        .def("get_board", &Game::get_board)
        .def("get_stand", &Game::get_stand)
        .def("get_result", &Game::get_result)
        .def("record_length", &Game::record_length)
        .def("get_legal_moves", &Game::get_legal_moves)
        .def("to_sfen", &Game::to_sfen)
        .def("is_legal", &Game::is_legal)
        .def("apply", &Game::apply)
        .def("get_move_at", &Game::get_move_at)
        .def("get_sfen_at", &Game::get_sfen_at)
        .def_static("ranks", &Game::ranks)
        .def_static("files", &Game::files)
        .def_static("feature_channels", &Game::feature_channels)
        .def_static("num_dlshogi_policy", &Game::num_dlshogi_policy)
        .def(
            "__array__",
            [](const Game& self) -> pybind11::array_t<float> {
                const auto shape = std::vector<pybind11::ssize_t>(
                    {1,
                     Game::ranks(),
                     Game::files(),
                     Game::feature_channels()});
                auto out = pybind11::array_t<float>(shape);
                self.to_feature_map(out.mutable_data());
                return out;
            })
        .def(
            "_policy_logits_to_policy_dict_probas",
            [](const Game& self, const pybind11::array_t<float>& policy_logits)
                -> pybind11::dict {
                const auto moves = self.get_legal_moves();
                if (moves.empty()) {
                    return pybind11::dict();
                }

                const float* const ptr = policy_logits.data();
                auto probas = std::vector<float>(moves.size());
                const auto turn = self.get_turn();
                for (std::size_t ii = moves.size(); ii--;) {
                    const auto index
                        = (turn == vshogi::BLACK)
                              ? moves[ii].to_dlshogi_policy_index()
                              : moves[ii].rotate().to_dlshogi_policy_index();
                    probas[ii] = ptr[index];
                }
                softmax(probas);

                pybind11::dict out;
                for (std::size_t ii = moves.size(); ii--;) {
                    out[pybind11::cast(moves[ii])] = probas[ii];
                }
                return out;
            })
        .def(
            "to_dlshogi_policy",
            [](const Game& self,
               const Move action,
               float max_value) -> pybind11::array_t<float> {
                const auto turn = self.get_turn();
                const auto legal_moves = self.get_legal_moves();
                const auto num_legal_moves
                    = static_cast<float>(legal_moves.size());
                const auto action_is_legal = has(legal_moves, action);
                const auto eps = (1.f - max_value)
                                 / (action_is_legal ? (num_legal_moves - 1.f)
                                                    : num_legal_moves);

                const auto size = Game::num_dlshogi_policy();
                auto out = pybind11::array_t<float>(
                    std::vector<pybind11::ssize_t>({size}));
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
        .def("copy", [](const Game& self) { return Game(self); })
        .def(
            "__deepcopy__",
            [](const Game& self, pybind11::dict) { return Game(self); },
            pybind11::arg("memo"));
}

template <class Game, class Move>
inline void export_node(pybind11::module& m)
{
    using Node = vshogi::engine::Node<Game, Move>;
    pybind11::class_<Node>(m, "Node")
        .def(
            pybind11::init<
                const float,
                const std::vector<Move>&,
                const std::vector<float>&>(),
            pybind11::arg("value"),
            pybind11::arg("actions"),
            pybind11::arg("probas"))
        .def("get_visit_count", &Node::get_visit_count)
        .def("get_value", &Node::get_value)
        .def("get_q_value", &Node::get_q_value)
        .def("get_actions", &Node::get_actions)
        .def("get_proba", &Node::get_proba)
        .def(
            "get_child",
            [](Node& node, const Move& action) {
                return pybind11::cast(
                    node.get_child(action),
                    pybind11::return_value_policy::reference);
            })
        .def("set_value_action_proba", &Node::set_value_action_proba)
        .def(
            "explore",
            [](Node& node,
               Game& game,
               const float coeff_puct,
               const float random_proba,
               const int random_depth) -> pybind11::object {
                const auto out = node.explore(
                    game, coeff_puct, random_proba, random_depth);
                if (out == nullptr)
                    return pybind11::none();
                return pybind11::cast(
                    *out, pybind11::return_value_policy::reference);
            })
        .def("pop_child", &Node::pop_child);
}

} // namespace pyvshogi

#endif // PYTHON_VSHOGI_HPP
