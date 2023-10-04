#include <algorithm>

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "vshogi/color.hpp"
#include "vshogi/engine/mcts.hpp"
#include "vshogi/result.hpp"

#include "vshogi/animal_shogi/game.hpp"
#include "vshogi/judkins_shogi/game.hpp"
#include "vshogi/minishogi/game.hpp"
#include "vshogi/shogi/game.hpp"

namespace nb = nanobind;

template <class Board>
inline void export_board(nb::module_& m)
{
    using SquareEnum = typename Board::SquareEnum;
    nb::class_<Board>(m, "Board")
        .def(
            "__getitem__",
            nb::overload_cast<SquareEnum>(&Board::operator[], nb::const_));
}

template <class Stand, class Pieces>
inline void export_piece_stand(nb::module_& m)
{
    nb::class_<Stand>(m, "Stand")
        .def("count", &Stand::count)
        .def("any", &Stand::any)
        .def("to_dict", [](const Stand& self) -> nb::dict {
            nb::dict out;
            for (auto p : Pieces::stand_piece_array)
                out[nb::cast(p)] = self.count(p);
            return out;
        });
}

template <class Move>
inline void export_move(nb::module_& m)
{
    using SquareEnum = typename Move::SquareEnum;
    using PieceTypeEnum = typename Move::PieceTypeEnum;
    nb::class_<Move>(m, "Move")
        .def(
            nb::init<const SquareEnum, const SquareEnum, const bool>(),
            nb::arg("dst"),
            nb::arg("src"),
            nb::arg("promote") = false)
        .def(
            nb::init<const SquareEnum, const PieceTypeEnum>(),
            nb::arg("dst"),
            nb::arg("src"))
        .def_prop_ro("destination", &Move::destination)
        .def_prop_ro("promote", &Move::promote)
        .def_prop_ro(
            "source",
            [](const Move& self) -> nb::object {
                if (self.is_drop())
                    return nb::cast(self.source_piece());
                return nb::cast(self.source_square());
            })
        .def("is_drop", &Move::is_drop)
        .def("rotate", &Move::rotate)
        .def("_to_dlshogi_policy_index", &Move::to_dlshogi_policy_index)
        .def_static("_num_policy_per_square", &Move::num_policy_per_square)
        .def("__hash__", &Move::hash)
        .def("__eq__", &Move::operator==)
        .def("__ne__", &Move::operator!=);
}

template <class T>
inline bool has(const std::vector<T>& vec, const T& target)
{
    return std::find(vec.cbegin(), vec.cend(), target) != vec.cend();
}

template <class Game>
inline void export_game(nb::module_& m)
{
    using Move = typename Game::Move;
    nb::class_<Game>(m, "_Game")
        .def(nb::init<>())
        .def(nb::init<const std::string&>())
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
            "to_dlshogi_features",
            [](const Game& self) {
                constexpr int size
                    = Game::ranks() * Game::files() * Game::feature_channels();
                float* data = new float[size];
                self.to_feature_map(data);
                nb::capsule owner(
                    data, [](void* p) noexcept { delete[](float*) p; });
                return nb::ndarray<
                    nb::numpy,
                    float,
                    nb::shape<
                        1,
                        Game::ranks(),
                        Game::files(),
                        Game::feature_channels()>>(
                    data,
                    {1, Game::ranks(), Game::files(), Game::feature_channels()},
                    owner);
            })
        .def(
            "to_dlshogi_features",
            [](const Game& self, nb::ndarray<float> out) {
                self.to_feature_map(out.data());
            })
        .def(
            "to_dlshogi_policy",
            [](const Game& self, const Move action, float max_value) {
                const auto turn = self.get_turn();
                const auto legal_moves = self.get_legal_moves();
                const auto num_legal_moves
                    = static_cast<float>(legal_moves.size());
                const auto action_is_legal = has(legal_moves, action);
                const auto eps = (1.f - max_value)
                                 / (action_is_legal ? (num_legal_moves - 1.f)
                                                    : num_legal_moves);

                constexpr int size = Game::num_dlshogi_policy();
                float* data = new float[size]{0.f};
                for (auto&& move : legal_moves) {
                    const auto index
                        = (turn == vshogi::BLACK)
                              ? move.to_dlshogi_policy_index()
                              : move.rotate().to_dlshogi_policy_index();
                    data[index] = (action == move) ? max_value : eps;
                }
                nb::capsule owner(
                    data, [](void* p) noexcept { delete[](float*) p; });
                return nb::ndarray<nb::numpy, const float, nb::shape<size>>(
                    data, {size}, owner);
            })
        .def("copy", [](const Game& self) { return Game(self); });
}

template <class Game, class Move>
inline void export_node(nb::module_& m)
{
    using Node = vshogi::engine::Node<Game, Move>;
    using Policy = nb::ndarray<
        const float,
        nb::shape<Game::num_dlshogi_policy()>,
        nb::device::cpu>;
    nb::class_<Node>(m, "Node")
        .def(
            "__init__",
            [](Node* n, const Game& g, const float v, Policy logits) {
                new (n) Node(g, v, logits.data());
            })
        .def("get_visit_count", &Node::get_visit_count)
        .def("get_value", &Node::get_value)
        .def("get_q_value", &Node::get_q_value)
        .def("get_actions", &Node::get_actions)
        .def("get_proba", &Node::get_proba)
        .def("get_child", &Node::get_child, nb::rv_policy::reference)
        .def(
            "set_value_policy_logits",
            [](Node& self,
               const Game& game,
               const float value,
               Policy policy_logits) {
                const auto data = policy_logits.data();
                self.set_value_policy_logits(game, value, data);
            })
        .def(
            "explore",
            [](Node& node,
               Game& game,
               const float coeff_puct,
               const float random_proba,
               const int random_depth) -> nb::object {
                const auto out = node.explore(
                    game, coeff_puct, random_proba, random_depth);
                if (out == nullptr)
                    return nb::none();
                return nb::cast(*out, nb::rv_policy::reference);
            })
        .def("pop_child", &Node::pop_child);
}

void export_animal_shogi(nb::module_& m)
{
    namespace sg = vshogi::animal_shogi;
    nb::enum_<sg::Pieces::PieceTypeEnum>(m, "Piece")
        .value("CH", sg::Pieces::CH)
        .value("EL", sg::Pieces::EL)
        .value("GI", sg::Pieces::GI)
        .value("LI", sg::Pieces::LI)
        .value("HE", sg::Pieces::HE)
        .value("NA", sg::Pieces::NA);
    nb::enum_<sg::Pieces::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_CH", sg::Pieces::B_CH)
        .value("B_EL", sg::Pieces::B_EL)
        .value("B_GI", sg::Pieces::B_GI)
        .value("B_LI", sg::Pieces::B_LI)
        .value("B_HE", sg::Pieces::B_HE)
        .value("W_CH", sg::Pieces::W_CH)
        .value("W_EL", sg::Pieces::W_EL)
        .value("W_GI", sg::Pieces::W_GI)
        .value("W_LI", sg::Pieces::W_LI)
        .value("W_HE", sg::Pieces::W_HE)
        .value("VOID", sg::Pieces::VOID);
    nb::enum_<sg::Squares::SquareEnum>(m, "Square")
        .value("A1", sg::Squares::SQ_A1)
        .value("B1", sg::Squares::SQ_B1)
        .value("C1", sg::Squares::SQ_C1)
        .value("A2", sg::Squares::SQ_A2)
        .value("B2", sg::Squares::SQ_B2)
        .value("C2", sg::Squares::SQ_C2)
        .value("A3", sg::Squares::SQ_A3)
        .value("B3", sg::Squares::SQ_B3)
        .value("C3", sg::Squares::SQ_C3)
        .value("A4", sg::Squares::SQ_A4)
        .value("B4", sg::Squares::SQ_B4)
        .value("C4", sg::Squares::SQ_C4);
    export_board<sg::Board>(m);
    export_piece_stand<sg::Stand, sg::Pieces>(m);
    export_move<sg::Move>(m);
    export_game<sg::Game>(m);
    export_node<sg::Game, sg::Move>(m);
}

void export_minishogi(nb::module_& m)
{
    namespace sg = vshogi::minishogi;
    nb::enum_<sg::Pieces::PieceTypeEnum>(m, "Piece")
        .value("FU", sg::Pieces::FU)
        .value("GI", sg::Pieces::GI)
        .value("KA", sg::Pieces::KA)
        .value("HI", sg::Pieces::HI)
        .value("KI", sg::Pieces::KI)
        .value("OU", sg::Pieces::OU)
        .value("TO", sg::Pieces::TO)
        .value("NG", sg::Pieces::NG)
        .value("UM", sg::Pieces::UM)
        .value("RY", sg::Pieces::RY)
        .value("NA", sg::Pieces::NA);
    nb::enum_<sg::Pieces::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_FU", sg::Pieces::B_FU)
        .value("B_GI", sg::Pieces::B_GI)
        .value("B_KA", sg::Pieces::B_KA)
        .value("B_HI", sg::Pieces::B_HI)
        .value("B_KI", sg::Pieces::B_KI)
        .value("B_OU", sg::Pieces::B_OU)
        .value("B_TO", sg::Pieces::B_TO)
        .value("B_NG", sg::Pieces::B_NG)
        .value("B_UM", sg::Pieces::B_UM)
        .value("B_RY", sg::Pieces::B_RY)
        .value("W_FU", sg::Pieces::W_FU)
        .value("W_GI", sg::Pieces::W_GI)
        .value("W_KA", sg::Pieces::W_KA)
        .value("W_HI", sg::Pieces::W_HI)
        .value("W_KI", sg::Pieces::W_KI)
        .value("W_OU", sg::Pieces::W_OU)
        .value("W_TO", sg::Pieces::W_TO)
        .value("W_NG", sg::Pieces::W_NG)
        .value("W_UM", sg::Pieces::W_UM)
        .value("W_RY", sg::Pieces::W_RY)
        .value("VOID", sg::Pieces::VOID);
    nb::enum_<sg::Squares::SquareEnum>(m, "Square")
        .value("A5", sg::Squares::SQ_5A)
        .value("A4", sg::Squares::SQ_4A)
        .value("A3", sg::Squares::SQ_3A)
        .value("A2", sg::Squares::SQ_2A)
        .value("A1", sg::Squares::SQ_1A)
        .value("B5", sg::Squares::SQ_5B)
        .value("B4", sg::Squares::SQ_4B)
        .value("B3", sg::Squares::SQ_3B)
        .value("B2", sg::Squares::SQ_2B)
        .value("B1", sg::Squares::SQ_1B)
        .value("C5", sg::Squares::SQ_5C)
        .value("C4", sg::Squares::SQ_4C)
        .value("C3", sg::Squares::SQ_3C)
        .value("C2", sg::Squares::SQ_2C)
        .value("C1", sg::Squares::SQ_1C)
        .value("D5", sg::Squares::SQ_5D)
        .value("D4", sg::Squares::SQ_4D)
        .value("D3", sg::Squares::SQ_3D)
        .value("D2", sg::Squares::SQ_2D)
        .value("D1", sg::Squares::SQ_1D)
        .value("E5", sg::Squares::SQ_5E)
        .value("E4", sg::Squares::SQ_4E)
        .value("E3", sg::Squares::SQ_3E)
        .value("E2", sg::Squares::SQ_2E)
        .value("E1", sg::Squares::SQ_1E)
        .value("SQ_5A", sg::Squares::SQ_5A)
        .value("SQ_4A", sg::Squares::SQ_4A)
        .value("SQ_3A", sg::Squares::SQ_3A)
        .value("SQ_2A", sg::Squares::SQ_2A)
        .value("SQ_1A", sg::Squares::SQ_1A)
        .value("SQ_5B", sg::Squares::SQ_5B)
        .value("SQ_4B", sg::Squares::SQ_4B)
        .value("SQ_3B", sg::Squares::SQ_3B)
        .value("SQ_2B", sg::Squares::SQ_2B)
        .value("SQ_1B", sg::Squares::SQ_1B)
        .value("SQ_5C", sg::Squares::SQ_5C)
        .value("SQ_4C", sg::Squares::SQ_4C)
        .value("SQ_3C", sg::Squares::SQ_3C)
        .value("SQ_2C", sg::Squares::SQ_2C)
        .value("SQ_1C", sg::Squares::SQ_1C)
        .value("SQ_5D", sg::Squares::SQ_5D)
        .value("SQ_4D", sg::Squares::SQ_4D)
        .value("SQ_3D", sg::Squares::SQ_3D)
        .value("SQ_2D", sg::Squares::SQ_2D)
        .value("SQ_1D", sg::Squares::SQ_1D)
        .value("SQ_5E", sg::Squares::SQ_5E)
        .value("SQ_4E", sg::Squares::SQ_4E)
        .value("SQ_3E", sg::Squares::SQ_3E)
        .value("SQ_2E", sg::Squares::SQ_2E)
        .value("SQ_1E", sg::Squares::SQ_1E);
    export_board<sg::Board>(m);
    export_piece_stand<sg::Stand, sg::Pieces>(m);
    export_move<sg::Move>(m);
    export_game<sg::Game>(m);
    export_node<sg::Game, sg::Move>(m);
}

void export_shogi(nb::module_& m)
{
    namespace sg = vshogi::shogi;
    nb::enum_<sg::Pieces::PieceTypeEnum>(m, "Piece")
        .value("FU", sg::Pieces::FU)
        .value("KY", sg::Pieces::KY)
        .value("KE", sg::Pieces::KE)
        .value("GI", sg::Pieces::GI)
        .value("KA", sg::Pieces::KA)
        .value("HI", sg::Pieces::HI)
        .value("KI", sg::Pieces::KI)
        .value("OU", sg::Pieces::OU)
        .value("TO", sg::Pieces::TO)
        .value("NY", sg::Pieces::NY)
        .value("NK", sg::Pieces::NK)
        .value("NG", sg::Pieces::NG)
        .value("UM", sg::Pieces::UM)
        .value("RY", sg::Pieces::RY)
        .value("NA", sg::Pieces::NA);
    nb::enum_<sg::Pieces::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_FU", sg::Pieces::B_FU)
        .value("B_KY", sg::Pieces::B_KY)
        .value("B_KE", sg::Pieces::B_KE)
        .value("B_GI", sg::Pieces::B_GI)
        .value("B_KA", sg::Pieces::B_KA)
        .value("B_HI", sg::Pieces::B_HI)
        .value("B_KI", sg::Pieces::B_KI)
        .value("B_OU", sg::Pieces::B_OU)
        .value("B_TO", sg::Pieces::B_TO)
        .value("B_NY", sg::Pieces::B_NY)
        .value("B_NK", sg::Pieces::B_NK)
        .value("B_NG", sg::Pieces::B_NG)
        .value("B_UM", sg::Pieces::B_UM)
        .value("B_RY", sg::Pieces::B_RY)
        .value("W_FU", sg::Pieces::W_FU)
        .value("W_KY", sg::Pieces::W_KY)
        .value("W_KE", sg::Pieces::W_KE)
        .value("W_GI", sg::Pieces::W_GI)
        .value("W_KA", sg::Pieces::W_KA)
        .value("W_HI", sg::Pieces::W_HI)
        .value("W_KI", sg::Pieces::W_KI)
        .value("W_OU", sg::Pieces::W_OU)
        .value("W_TO", sg::Pieces::W_TO)
        .value("W_NY", sg::Pieces::W_NY)
        .value("W_NK", sg::Pieces::W_NK)
        .value("W_NG", sg::Pieces::W_NG)
        .value("W_UM", sg::Pieces::W_UM)
        .value("W_RY", sg::Pieces::W_RY)
        .value("VOID", sg::Pieces::VOID);
    nb::enum_<sg::Squares::SquareEnum>(m, "Square")
        .value("SQ_9A", sg::Squares::SQ_9A)
        .value("SQ_8A", sg::Squares::SQ_8A)
        .value("SQ_7A", sg::Squares::SQ_7A)
        .value("SQ_6A", sg::Squares::SQ_6A)
        .value("SQ_5A", sg::Squares::SQ_5A)
        .value("SQ_4A", sg::Squares::SQ_4A)
        .value("SQ_3A", sg::Squares::SQ_3A)
        .value("SQ_2A", sg::Squares::SQ_2A)
        .value("SQ_1A", sg::Squares::SQ_1A)
        .value("SQ_9B", sg::Squares::SQ_9B)
        .value("SQ_8B", sg::Squares::SQ_8B)
        .value("SQ_7B", sg::Squares::SQ_7B)
        .value("SQ_6B", sg::Squares::SQ_6B)
        .value("SQ_5B", sg::Squares::SQ_5B)
        .value("SQ_4B", sg::Squares::SQ_4B)
        .value("SQ_3B", sg::Squares::SQ_3B)
        .value("SQ_2B", sg::Squares::SQ_2B)
        .value("SQ_1B", sg::Squares::SQ_1B)
        .value("SQ_9C", sg::Squares::SQ_9C)
        .value("SQ_8C", sg::Squares::SQ_8C)
        .value("SQ_7C", sg::Squares::SQ_7C)
        .value("SQ_6C", sg::Squares::SQ_6C)
        .value("SQ_5C", sg::Squares::SQ_5C)
        .value("SQ_4C", sg::Squares::SQ_4C)
        .value("SQ_3C", sg::Squares::SQ_3C)
        .value("SQ_2C", sg::Squares::SQ_2C)
        .value("SQ_1C", sg::Squares::SQ_1C)
        .value("SQ_9D", sg::Squares::SQ_9D)
        .value("SQ_8D", sg::Squares::SQ_8D)
        .value("SQ_7D", sg::Squares::SQ_7D)
        .value("SQ_6D", sg::Squares::SQ_6D)
        .value("SQ_5D", sg::Squares::SQ_5D)
        .value("SQ_4D", sg::Squares::SQ_4D)
        .value("SQ_3D", sg::Squares::SQ_3D)
        .value("SQ_2D", sg::Squares::SQ_2D)
        .value("SQ_1D", sg::Squares::SQ_1D)
        .value("SQ_9E", sg::Squares::SQ_9E)
        .value("SQ_8E", sg::Squares::SQ_8E)
        .value("SQ_7E", sg::Squares::SQ_7E)
        .value("SQ_6E", sg::Squares::SQ_6E)
        .value("SQ_5E", sg::Squares::SQ_5E)
        .value("SQ_4E", sg::Squares::SQ_4E)
        .value("SQ_3E", sg::Squares::SQ_3E)
        .value("SQ_2E", sg::Squares::SQ_2E)
        .value("SQ_1E", sg::Squares::SQ_1E)
        .value("SQ_9F", sg::Squares::SQ_9F)
        .value("SQ_8F", sg::Squares::SQ_8F)
        .value("SQ_7F", sg::Squares::SQ_7F)
        .value("SQ_6F", sg::Squares::SQ_6F)
        .value("SQ_5F", sg::Squares::SQ_5F)
        .value("SQ_4F", sg::Squares::SQ_4F)
        .value("SQ_3F", sg::Squares::SQ_3F)
        .value("SQ_2F", sg::Squares::SQ_2F)
        .value("SQ_1F", sg::Squares::SQ_1F)
        .value("SQ_9G", sg::Squares::SQ_9G)
        .value("SQ_8G", sg::Squares::SQ_8G)
        .value("SQ_7G", sg::Squares::SQ_7G)
        .value("SQ_6G", sg::Squares::SQ_6G)
        .value("SQ_5G", sg::Squares::SQ_5G)
        .value("SQ_4G", sg::Squares::SQ_4G)
        .value("SQ_3G", sg::Squares::SQ_3G)
        .value("SQ_2G", sg::Squares::SQ_2G)
        .value("SQ_1G", sg::Squares::SQ_1G)
        .value("SQ_9H", sg::Squares::SQ_9H)
        .value("SQ_8H", sg::Squares::SQ_8H)
        .value("SQ_7H", sg::Squares::SQ_7H)
        .value("SQ_6H", sg::Squares::SQ_6H)
        .value("SQ_5H", sg::Squares::SQ_5H)
        .value("SQ_4H", sg::Squares::SQ_4H)
        .value("SQ_3H", sg::Squares::SQ_3H)
        .value("SQ_2H", sg::Squares::SQ_2H)
        .value("SQ_1H", sg::Squares::SQ_1H)
        .value("SQ_9I", sg::Squares::SQ_9I)
        .value("SQ_8I", sg::Squares::SQ_8I)
        .value("SQ_7I", sg::Squares::SQ_7I)
        .value("SQ_6I", sg::Squares::SQ_6I)
        .value("SQ_5I", sg::Squares::SQ_5I)
        .value("SQ_4I", sg::Squares::SQ_4I)
        .value("SQ_3I", sg::Squares::SQ_3I)
        .value("SQ_2I", sg::Squares::SQ_2I)
        .value("SQ_1I", sg::Squares::SQ_1I)
        .value("A9", sg::Squares::SQ_9A)
        .value("A8", sg::Squares::SQ_8A)
        .value("A7", sg::Squares::SQ_7A)
        .value("A6", sg::Squares::SQ_6A)
        .value("A5", sg::Squares::SQ_5A)
        .value("A4", sg::Squares::SQ_4A)
        .value("A3", sg::Squares::SQ_3A)
        .value("A2", sg::Squares::SQ_2A)
        .value("A1", sg::Squares::SQ_1A)
        .value("B9", sg::Squares::SQ_9B)
        .value("B8", sg::Squares::SQ_8B)
        .value("B7", sg::Squares::SQ_7B)
        .value("B6", sg::Squares::SQ_6B)
        .value("B5", sg::Squares::SQ_5B)
        .value("B4", sg::Squares::SQ_4B)
        .value("B3", sg::Squares::SQ_3B)
        .value("B2", sg::Squares::SQ_2B)
        .value("B1", sg::Squares::SQ_1B)
        .value("C9", sg::Squares::SQ_9C)
        .value("C8", sg::Squares::SQ_8C)
        .value("C7", sg::Squares::SQ_7C)
        .value("C6", sg::Squares::SQ_6C)
        .value("C5", sg::Squares::SQ_5C)
        .value("C4", sg::Squares::SQ_4C)
        .value("C3", sg::Squares::SQ_3C)
        .value("C2", sg::Squares::SQ_2C)
        .value("C1", sg::Squares::SQ_1C)
        .value("D9", sg::Squares::SQ_9D)
        .value("D8", sg::Squares::SQ_8D)
        .value("D7", sg::Squares::SQ_7D)
        .value("D6", sg::Squares::SQ_6D)
        .value("D5", sg::Squares::SQ_5D)
        .value("D4", sg::Squares::SQ_4D)
        .value("D3", sg::Squares::SQ_3D)
        .value("D2", sg::Squares::SQ_2D)
        .value("D1", sg::Squares::SQ_1D)
        .value("E9", sg::Squares::SQ_9E)
        .value("E8", sg::Squares::SQ_8E)
        .value("E7", sg::Squares::SQ_7E)
        .value("E6", sg::Squares::SQ_6E)
        .value("E5", sg::Squares::SQ_5E)
        .value("E4", sg::Squares::SQ_4E)
        .value("E3", sg::Squares::SQ_3E)
        .value("E2", sg::Squares::SQ_2E)
        .value("E1", sg::Squares::SQ_1E)
        .value("F9", sg::Squares::SQ_9F)
        .value("F8", sg::Squares::SQ_8F)
        .value("F7", sg::Squares::SQ_7F)
        .value("F6", sg::Squares::SQ_6F)
        .value("F5", sg::Squares::SQ_5F)
        .value("F4", sg::Squares::SQ_4F)
        .value("F3", sg::Squares::SQ_3F)
        .value("F2", sg::Squares::SQ_2F)
        .value("F1", sg::Squares::SQ_1F)
        .value("G9", sg::Squares::SQ_9G)
        .value("G8", sg::Squares::SQ_8G)
        .value("G7", sg::Squares::SQ_7G)
        .value("G6", sg::Squares::SQ_6G)
        .value("G5", sg::Squares::SQ_5G)
        .value("G4", sg::Squares::SQ_4G)
        .value("G3", sg::Squares::SQ_3G)
        .value("G2", sg::Squares::SQ_2G)
        .value("G1", sg::Squares::SQ_1G)
        .value("H9", sg::Squares::SQ_9H)
        .value("H8", sg::Squares::SQ_8H)
        .value("H7", sg::Squares::SQ_7H)
        .value("H6", sg::Squares::SQ_6H)
        .value("H5", sg::Squares::SQ_5H)
        .value("H4", sg::Squares::SQ_4H)
        .value("H3", sg::Squares::SQ_3H)
        .value("H2", sg::Squares::SQ_2H)
        .value("H1", sg::Squares::SQ_1H)
        .value("I9", sg::Squares::SQ_9I)
        .value("I8", sg::Squares::SQ_8I)
        .value("I7", sg::Squares::SQ_7I)
        .value("I6", sg::Squares::SQ_6I)
        .value("I5", sg::Squares::SQ_5I)
        .value("I4", sg::Squares::SQ_4I)
        .value("I3", sg::Squares::SQ_3I)
        .value("I2", sg::Squares::SQ_2I)
        .value("I1", sg::Squares::SQ_1I);
    export_board<sg::Board>(m);
    export_piece_stand<sg::Stand, sg::Pieces>(m);
    export_move<sg::Move>(m);
    export_game<sg::Game>(m);
    export_node<sg::Game, sg::Move>(m);
}

void export_judkins_shogi(nb::module_& m)
{
    namespace sg = vshogi::judkins_shogi;
    nb::enum_<sg::Pieces::PieceTypeEnum>(m, "Piece")
        .value("FU", sg::Pieces::FU)
        .value("KE", sg::Pieces::KE)
        .value("GI", sg::Pieces::GI)
        .value("KA", sg::Pieces::KA)
        .value("HI", sg::Pieces::HI)
        .value("KI", sg::Pieces::KI)
        .value("OU", sg::Pieces::OU)
        .value("TO", sg::Pieces::TO)
        .value("NK", sg::Pieces::NK)
        .value("NG", sg::Pieces::NG)
        .value("UM", sg::Pieces::UM)
        .value("RY", sg::Pieces::RY)
        .value("NA", sg::Pieces::NA);
    nb::enum_<sg::Pieces::BoardPieceTypeEnum>(m, "BoardPiece")
        .value("B_FU", sg::Pieces::B_FU)
        .value("B_KE", sg::Pieces::B_KE)
        .value("B_GI", sg::Pieces::B_GI)
        .value("B_KA", sg::Pieces::B_KA)
        .value("B_HI", sg::Pieces::B_HI)
        .value("B_KI", sg::Pieces::B_KI)
        .value("B_OU", sg::Pieces::B_OU)
        .value("B_TO", sg::Pieces::B_TO)
        .value("B_NK", sg::Pieces::B_NK)
        .value("B_NG", sg::Pieces::B_NG)
        .value("B_UM", sg::Pieces::B_UM)
        .value("B_RY", sg::Pieces::B_RY)
        .value("W_FU", sg::Pieces::W_FU)
        .value("W_KE", sg::Pieces::W_KE)
        .value("W_GI", sg::Pieces::W_GI)
        .value("W_KA", sg::Pieces::W_KA)
        .value("W_HI", sg::Pieces::W_HI)
        .value("W_KI", sg::Pieces::W_KI)
        .value("W_OU", sg::Pieces::W_OU)
        .value("W_TO", sg::Pieces::W_TO)
        .value("W_NK", sg::Pieces::W_NK)
        .value("W_NG", sg::Pieces::W_NG)
        .value("W_UM", sg::Pieces::W_UM)
        .value("W_RY", sg::Pieces::W_RY)
        .value("VOID", sg::Pieces::VOID);
    nb::enum_<sg::Squares::SquareEnum>(m, "Square")
        .value("SQ_6A", sg::Squares::SQ_6A)
        .value("SQ_5A", sg::Squares::SQ_5A)
        .value("SQ_4A", sg::Squares::SQ_4A)
        .value("SQ_3A", sg::Squares::SQ_3A)
        .value("SQ_2A", sg::Squares::SQ_2A)
        .value("SQ_1A", sg::Squares::SQ_1A)
        .value("SQ_6B", sg::Squares::SQ_6B)
        .value("SQ_5B", sg::Squares::SQ_5B)
        .value("SQ_4B", sg::Squares::SQ_4B)
        .value("SQ_3B", sg::Squares::SQ_3B)
        .value("SQ_2B", sg::Squares::SQ_2B)
        .value("SQ_1B", sg::Squares::SQ_1B)
        .value("SQ_6C", sg::Squares::SQ_6C)
        .value("SQ_5C", sg::Squares::SQ_5C)
        .value("SQ_4C", sg::Squares::SQ_4C)
        .value("SQ_3C", sg::Squares::SQ_3C)
        .value("SQ_2C", sg::Squares::SQ_2C)
        .value("SQ_1C", sg::Squares::SQ_1C)
        .value("SQ_6D", sg::Squares::SQ_6D)
        .value("SQ_5D", sg::Squares::SQ_5D)
        .value("SQ_4D", sg::Squares::SQ_4D)
        .value("SQ_3D", sg::Squares::SQ_3D)
        .value("SQ_2D", sg::Squares::SQ_2D)
        .value("SQ_1D", sg::Squares::SQ_1D)
        .value("SQ_6E", sg::Squares::SQ_6E)
        .value("SQ_5E", sg::Squares::SQ_5E)
        .value("SQ_4E", sg::Squares::SQ_4E)
        .value("SQ_3E", sg::Squares::SQ_3E)
        .value("SQ_2E", sg::Squares::SQ_2E)
        .value("SQ_1E", sg::Squares::SQ_1E)
        .value("SQ_6F", sg::Squares::SQ_6F)
        .value("SQ_5F", sg::Squares::SQ_5F)
        .value("SQ_4F", sg::Squares::SQ_4F)
        .value("SQ_3F", sg::Squares::SQ_3F)
        .value("SQ_2F", sg::Squares::SQ_2F)
        .value("SQ_1F", sg::Squares::SQ_1F)
        .value("A6", sg::Squares::SQ_6A)
        .value("A5", sg::Squares::SQ_5A)
        .value("A4", sg::Squares::SQ_4A)
        .value("A3", sg::Squares::SQ_3A)
        .value("A2", sg::Squares::SQ_2A)
        .value("A1", sg::Squares::SQ_1A)
        .value("B6", sg::Squares::SQ_6B)
        .value("B5", sg::Squares::SQ_5B)
        .value("B4", sg::Squares::SQ_4B)
        .value("B3", sg::Squares::SQ_3B)
        .value("B2", sg::Squares::SQ_2B)
        .value("B1", sg::Squares::SQ_1B)
        .value("C6", sg::Squares::SQ_6C)
        .value("C5", sg::Squares::SQ_5C)
        .value("C4", sg::Squares::SQ_4C)
        .value("C3", sg::Squares::SQ_3C)
        .value("C2", sg::Squares::SQ_2C)
        .value("C1", sg::Squares::SQ_1C)
        .value("D6", sg::Squares::SQ_6D)
        .value("D5", sg::Squares::SQ_5D)
        .value("D4", sg::Squares::SQ_4D)
        .value("D3", sg::Squares::SQ_3D)
        .value("D2", sg::Squares::SQ_2D)
        .value("D1", sg::Squares::SQ_1D)
        .value("E6", sg::Squares::SQ_6E)
        .value("E5", sg::Squares::SQ_5E)
        .value("E4", sg::Squares::SQ_4E)
        .value("E3", sg::Squares::SQ_3E)
        .value("E2", sg::Squares::SQ_2E)
        .value("E1", sg::Squares::SQ_1E)
        .value("F6", sg::Squares::SQ_6F)
        .value("F5", sg::Squares::SQ_5F)
        .value("F4", sg::Squares::SQ_4F)
        .value("F3", sg::Squares::SQ_3F)
        .value("F2", sg::Squares::SQ_2F)
        .value("F1", sg::Squares::SQ_1F);
    export_board<sg::Board>(m);
    export_piece_stand<sg::Stand, sg::Pieces>(m);
    export_move<sg::Move>(m);
    export_game<sg::Game>(m);
    export_node<sg::Game, sg::Move>(m);
}

NB_MODULE(_vshogi_extension, m)
{
    nb::enum_<vshogi::ColorEnum>(m, "Color")
        .value("BLACK", vshogi::BLACK)
        .value("WHITE", vshogi::WHITE);
    nb::enum_<vshogi::ResultEnum>(m, "Result")
        .value("ONGOING", vshogi::ONGOING)
        .value("DRAW", vshogi::DRAW)
        .value("BLACK_WIN", vshogi::BLACK_WIN)
        .value("WHITE_WIN", vshogi::WHITE_WIN);

    auto animal_shogi_module = m.def_submodule("animal_shogi");
    export_animal_shogi(animal_shogi_module);

    auto minishogi_module = m.def_submodule("minishogi");
    export_minishogi(minishogi_module);

    auto judkins_shogi_module = m.def_submodule("judkins_shogi");
    export_judkins_shogi(judkins_shogi_module);

    auto shogi_module = m.def_submodule("shogi");
    export_shogi(shogi_module);
}
