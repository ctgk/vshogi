#ifndef PYTHON_VSHOGI_WRAPPER_HPP
#define PYTHON_VSHOGI_WRAPPER_HPP

#include <algorithm>
#include <cmath>

#include "vshogi/common/config.hpp"
#include "vshogi/common/notation.hpp"
#include "vshogi/engine/az/searcher.hpp"
#include "vshogi/engine/dfpn/searcher.hpp"
#include "vshogi/engine/gaz/searcher.hpp"
#include "vshogi/engine/mcgs/searcher.hpp"
#include "vshogi/engine/piece_value.hpp"

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/vector.h>

namespace pyvshogi
{

template <class T>
inline bool has(const std::vector<T>& vec, const T& target)
{
    return std::find(vec.cbegin(), vec.cend(), target) != vec.cend();
}

template <class Parameters>
inline void export_to_jpn(nanobind::module_& m)
{
    namespace nb = nanobind;
    using C = vshogi::Configuration<Parameters>;
    using NT = vshogi::Notation<Parameters>;
    using PT = vshogi::PieceTraits<Parameters>;
    m.def("to_jpn", [](const typename C::PieceType pt) {
        return NT::to_jpn(pt);
    });
    m.def("to_jpn", [](const typename C::Piece p) {
        return NT::to_jpn(PT::to_piece_type(p));
    });
    m.def("to_jpn", [](const typename C::Square sq) { return NT::to_jpn(sq); });
}

template <class Parameters>
inline void export_to_sfen(nanobind::module_& m)
{
    namespace nb = nanobind;
    using C = vshogi::Configuration<Parameters>;
    using NT = vshogi::Notation<Parameters>;
    m.def("to_sfen", [](const typename C::PieceType pt) {
        return NT::to_sfen(pt);
    });
    m.def("to_sfen", [](const typename C::Piece p) { return NT::to_sfen(p); });
}

template <class Parameters>
inline void export_board(nanobind::module_& m)
{
    namespace nb = nanobind;
    using C = vshogi::Configuration<Parameters>;
    using Board = vshogi::Board<Parameters>;
    using Square = typename C::Square;
    nb::class_<Board>(m, "Board")
        .def(
            "__getitem__",
            nb::overload_cast<const Square&>(&Board::operator[], nb::const_))
        .def(
            "__getitem__",
            [](const Board& self, const uint index) {
                return self[static_cast<Square>(index)];
            })
        .def_prop_ro_static(
            "num_files", [](nb::handle) { return C::num_files; })
        .def_prop_ro_static(
            "num_ranks", [](nb::handle) { return C::num_ranks; })
        .def_prop_ro_static(
            "num_squares", [](nb::handle) { return C::num_squares; });
}

template <class Parameters>
inline void export_piece_stand(nanobind::module_& m)
{
    namespace nb = nanobind;
    using C = vshogi::Configuration<Parameters>;
    using Stand = vshogi::Stand<Parameters>;
    nb::class_<Stand>(m, "Stand")
        .def("count", &Stand::count)
        .def("any", &Stand::any)
        .def("to_dict", [](const Stand& self) -> nb::dict {
            nb::dict out;
            for (auto pt : C::stand_piece_type_iterator())
                out[nb::cast(pt)] = self.count(pt);
            return out;
        });
}

template <class P>
class Move
{
public:
    vshogi::move_t m_value;
    Move(const vshogi::move_t& m) : m_value{m}
    {
    }
};

template <class Parameters>
inline void export_move(nanobind::module_& m)
{
    namespace nb = nanobind;
    using MT = vshogi::MoveTraits<Parameters>;
    using NT = vshogi::Notation<Parameters>;
    using Square = typename Parameters::Square;
    using PieceType = typename Parameters::PieceType;
    using move_t = vshogi::move_t;
    using Move = Move<Parameters>;
    nb::class_<Move>(m, "Move")
        .def(
            "__init__",
            [](Move* t,
               const Square src,
               const Square dst,
               const bool promote) {
                new (t) Move(MT::make_move(src, dst, promote));
            },
            nb::arg("src"),
            nb::arg("dst"),
            nb::arg("promote") = false)
        .def(
            "__init__",
            [](Move* t, const PieceType src, const Square dst) {
                new (t) Move(MT::make_move(src, dst));
            },
            nb::arg("src"),
            nb::arg("dst"))
        .def(
            "__init__",
            [](Move* t, const std::string& sfen) {
                new (t) Move(MT::make_move(sfen.c_str()));
            },
            nb::arg("sfen"))
        .def(
            "__init__",
            [](Move* t, const uint value) {
                new (t) Move(static_cast<move_t>(value));
            },
            nb::arg("value"))
        .def_prop_ro(
            "destination", [](const Move& m) { return MT::get_dst(m.m_value); })
        .def_prop_ro(
            "promote", [](const Move& m) { return MT::get_promote(m.m_value); })
        .def_prop_ro(
            "source",
            [](const Move& m) {
                if (MT::is_drop(m.m_value))
                    return nb::cast(MT::get_src_pt(m.m_value));
                return nb::cast(MT::get_src_sq(m.m_value));
            })
        .def("is_drop", [](const Move& m) { return MT::is_drop(m.m_value); })
        .def(
            "rotate", [](const Move& m) { return Move(MT::rotate(m.m_value)); })
        .def("hflip", [](const Move& m) { return Move(MT::hflip(m.m_value)); })
        .def(
            "_to_dlshogi_policy_index",
            [](const Move& m, const vshogi::ColorEnum& by_side) {
                return MT::to_policy_index(m.m_value, by_side);
            })
        .def_static("_num_policy_per_square", &MT::num_policy_per_square)
        .def(
            "__hash__",
            [](const Move& m) { return static_cast<int>(m.m_value); })
        .def("to_sfen", [](const Move& m) { return NT::to_sfen(m.m_value); })
        .def(
            "__eq__",
            [](const Move& a, const Move& b) { return a.m_value == b.m_value; })
        .def(
            "__ne__",
            [](const Move& a, const Move& b) { return a.m_value != b.m_value; })
        .def(
            "__getstate__",
            [](const Move& m) {
                return nb::make_tuple(static_cast<int>(m.m_value));
            })
        .def("__setstate__", [](Move& m, nb::tuple t) {
            new (&m) Move(nb::cast<move_t>(t[0]));
        });
}

template <class Parameters>
inline void export_state(nanobind::module_& m)
{
    namespace nb = nanobind;
    using State = vshogi::State<Parameters>;
    using MoveTraits = vshogi::MoveTraits<Parameters>;
    using NT = vshogi::Notation<Parameters>;
    using Move = Move<Parameters>;
    using C = vshogi::Configuration<Parameters>;

    nb::class_<State>(m, "State")
        .def(nb::init<const std::string&>())
        .def("hflip", &State::hflip)
        .def("to_sfen", [](const State& self) { return NT::to_sfen(self); })
        .def(
            "to_dlshogi_features",
            [](const State& self, const bool promotion_zone) {
                const auto shape = std::vector<size_t>(
                    {1,
                     State::feature_channels() + promotion_zone,
                     State::num_files,
                     State::num_ranks});
                float* data
                    = new float[shape[0] * shape[1] * shape[2] * shape[3]];
                auto out = nb::ndarray<nb::numpy, float>(
                    data,
                    shape.size(),
                    shape.data(),
                    nb::capsule(data, [](void* p) noexcept {
                        delete[] static_cast<float*>(p);
                    }));
                self.to_feature_map(data, promotion_zone);
                return out;
            },
            nb::arg("promotion_zone") = false)
        .def(
            "to_dlshogi_features",
            [](const State& self,
               nb::ndarray<nb::numpy, float, nb::c_contig> out) {
                if ((out.ndim() != 3u) and (out.ndim() != 4u))
                    throw nb::python_error();
                switch (out.shape(out.ndim() - 3u)) {
                case State::feature_channels():
                    self.to_feature_map(out.data());
                    break;
                case State::feature_channels() + 1:
                    self.to_feature_map(out.data(), true);
                    break;
                default:
                    break;
                }
            })
        .def(
            "to_dlshogi_policy",
            [](const State& self,
               const nb::dict& action_proba,
               const float default_value) {
                const auto turn = self.get_turn();
                constexpr auto size = C::dlshogi_policy_size;
                const auto shape = std::vector<size_t>({size});
                float* data = new float[size];
                auto out = nb::ndarray<nb::numpy, float>(
                    data,
                    shape.size(),
                    shape.data(),
                    nb::capsule(data, [](void* p) noexcept {
                        delete[] static_cast<float*>(p);
                    }));
                std::fill(data, data + size, default_value);
                for (auto [key, value] : action_proba) {
                    const auto m = nb::cast<Move>(key);
                    const auto index
                        = MoveTraits::to_policy_index(m.m_value, turn);
                    data[index] = nb::cast<float>(value);
                }
                return out;
            },
            nb::arg("action_proba"),
            nb::arg("default_value"))
        .def(
            "to_dlshogi_policy",
            [](const State& self,
               const nb::dict& action_proba,
               const float default_value,
               nb::ndarray<nb::numpy, float, nb::c_contig> out) {
                const auto turn = self.get_turn();
                constexpr auto size = C::dlshogi_policy_size;
                float* const data = out.data();
                std::fill(data, data + size, default_value);
                for (auto [key, value] : action_proba) {
                    const auto m = nb::cast<Move>(key);
                    const auto index
                        = MoveTraits::to_policy_index(m.m_value, turn);
                    data[index] = nb::cast<float>(value);
                }
            },
            nb::arg("action_proba"),
            nb::arg("default_value"),
            nb::arg("out"));
}

template <class Parameters>
inline void export_game(nanobind::module_& m)
{
    namespace nb = nanobind;
    using C = vshogi::Configuration<Parameters>;
    using NT = vshogi::Notation<Parameters>;
    using Game = vshogi::Game<Parameters>;
    using MoveTraits = vshogi::MoveTraits<Parameters>;
    using Move = Move<Parameters>;
    nb::class_<Game>(m, "_Game")
        .def(nb::init<>())
        .def(nb::init<const std::string&>())
        .def("get_turn", &Game::get_turn)
        .def("get_board", &Game::get_board)
        .def(
            "get_stand",
            nb::overload_cast<const vshogi::ColorEnum>(
                &Game::get_stand, nb::const_))
        .def("get_result", &Game::get_result)
        .def("get_zobrist_hash", &Game::get_zobrist_hash)
        .def("ply", &Game::ply)
        .def("count_repetitions", &Game::count_repetitions)
        .def(
            "get_legal_moves",
            [](const Game& g) {
                std::vector<Move> out{};
                for (auto&& m : g.get_legal_moves())
                    out.emplace_back(m);
                return out;
            })
        .def(
            "get_check_moves",
            [](const Game& g) {
                std::vector<Move> out{};
                for (auto&& m : g.get_check_moves())
                    out.emplace_back(m);
                return out;
            })
        .def(
            "to_sfen",
            [](const Game& self, const bool include_move_count) {
                return NT::to_sfen(self, include_move_count);
            })
        .def(
            "is_legal",
            [](const Game& self, const Move& m) {
                return self.is_legal(m.m_value);
            })
        .def(
            "is_aigoma",
            [](const Game& self, const Move& m) {
                return self.is_aigoma(m.m_value);
            })
        .def("in_check", &Game::in_check)
        .def("is_valid_piece_count", &Game::is_valid_piece_count)
        .def("hflip", &Game::hflip)
        .def("rotate", &Game::rotate)
        .def(
            "to_jpn",
            [](const Game& g, const Move& m) {
                return NT::to_jpn(m.m_value, g);
            })
        .def(
            "to_eng",
            [](const Game& g, const Move& m) {
                return NT::to_eng(m.m_value, g);
            })
        .def(
            "apply",
            [](Game& self, const Move& m) { return self.apply(m.m_value); })
        .def("resign", &Game::resign)
        .def("declare_draw", &Game::declare_draw)
        .def("undo", &Game::undo)
        .def(
            "apply_discard",
            [](Game& self, const Move& m) {
                return self.apply_discard(m.m_value);
            })
        .def("undo_discard", &Game::undo_discard)
        .def_static("ranks", []() { return &Game::num_ranks; })
        .def_static("files", []() { return &Game::num_files; })
        .def_static("feature_channels", &Game::feature_channels)
        .def_static(
            "num_dlshogi_policy", []() { return &C::dlshogi_policy_size; })
        .def_static(
            "get_attention",
            []() {
                const auto n = Game::num_squares;
                const auto shape = std::vector<size_t>({n, n});
                float* data = new float[n * n];
                auto out = nb::ndarray<nb::numpy, float>(
                    data,
                    shape.size(),
                    shape.data(),
                    nb::capsule(data, [](void* p) noexcept {
                        delete[] static_cast<float*>(p);
                    }));
                Game::attention_matrix(data);
                return out;
            })
        .def_static(
            "get_local_attentions",
            []() {
                constexpr uint num_dir = C::num_dir;
                const auto n = Game::num_squares;
                const auto shape = std::vector<size_t>({num_dir, n, n});
                float* data = new float[num_dir * n * n];
                auto out = nb::ndarray<nb::numpy, float>(
                    data,
                    shape.size(),
                    shape.data(),
                    nb::capsule(data, [](void* p) noexcept {
                        delete[] static_cast<float*>(p);
                    }));
                for (auto dir : C::direction_iterator()) {
                    Game::attention_matrix(
                        &data[static_cast<int>(dir) * n * n], {dir}, true);
                }
                return out;
            })
        .def_static(
            "get_adjacent_attention",
            []() {
                const auto n = Game::num_squares;
                const auto shape = std::vector<size_t>({n, n});
                float* data = new float[n * n];
                auto out = nb::ndarray<nb::numpy, float>(
                    data,
                    shape.size(),
                    shape.data(),
                    nb::capsule(data, [](void* p) noexcept {
                        delete[] static_cast<float*>(p);
                    }));
                Game::attention_matrix(
                    data,
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
                const auto shape = std::vector<size_t>({n, n});
                float* data = new float[n * n];
                auto out = nb::ndarray<nb::numpy, float>(
                    data,
                    shape.size(),
                    shape.data(),
                    nb::capsule(data, [](void* p) noexcept {
                        delete[] static_cast<float*>(p);
                    }));
                Game::attention_matrix(
                    data,
                    {vshogi::DIR_NW,
                     vshogi::DIR_NE,
                     vshogi::DIR_SW,
                     vshogi::DIR_SE});
                return out;
            })
        .def(
            "to_dlshogi_features",
            [](const Game& self, const bool promotion_zone) {
                const auto shape = std::vector<size_t>(
                    {1,
                     Game::feature_channels() + promotion_zone,
                     Game::num_files,
                     Game::num_ranks});
                float* data
                    = new float[shape[0] * shape[1] * shape[2] * shape[3]];
                auto out = nb::ndarray<nb::numpy, float>(
                    data,
                    shape.size(),
                    shape.data(),
                    nb::capsule(data, [](void* p) noexcept {
                        delete[] static_cast<float*>(p);
                    }));
                self.to_feature_map(data, promotion_zone);
                return out;
            })
        .def(
            "to_dlshogi_features",
            [](const Game& self,
               nb::ndarray<nb::numpy, float, nb::c_contig> out) {
                if ((out.ndim() != 3u) and (out.ndim() != 4u))
                    throw nb::python_error();
                switch (out.shape(out.ndim() - 3u)) {
                case Game::feature_channels():
                    self.to_feature_map(out.data());
                    break;
                case Game::feature_channels() + 1:
                    self.to_feature_map(out.data(), true);
                    break;
                default:
                    break;
                }
            })
        .def(
            "to_dlshogi_policy",
            [](const Game& self,
               const nb::dict& visit_proba,
               const float default_value) {
                const auto turn = self.get_turn();
                constexpr auto size = C::dlshogi_policy_size;
                const auto shape = std::vector<size_t>({size});
                float* data = new float[size];
                auto out = nb::ndarray<nb::numpy, float>(
                    data,
                    shape.size(),
                    shape.data(),
                    nb::capsule(data, [](void* p) noexcept {
                        delete[] static_cast<float*>(p);
                    }));
                std::fill(data, data + size, default_value);
                for (auto [key, value] : visit_proba) {
                    const auto m = nb::cast<Move>(key);
                    const auto index
                        = MoveTraits::to_policy_index(m.m_value, turn);
                    data[index] = nb::cast<float>(value);
                }
                return out;
            })
        .def(
            "masked_softmax",
            [](const Game& self,
               const nb::ndarray<nb::numpy, float, nb::c_contig>& logits)
                -> nb::dict {
                nb::dict out;
                const auto t = self.get_turn();
                const auto& actions = self.get_legal_moves();
                auto proba = std::vector<float>(actions.size());
                const auto data = logits.data();
                for (std::size_t ii = actions.size(); ii--;) {
                    const auto index
                        = MoveTraits::to_policy_index(actions[ii], t);
                    proba[ii] = data[index];
                }
                vshogi::softmax(proba);
                for (std::size_t ii = actions.size(); ii--;) {
                    out[nb::cast(Move(actions[ii]))] = proba[ii];
                }
                return out;
            })
        .def(
            "get_mate_moves_if_any",
            [](Game& self, const unsigned int num_dfpn_nodes) -> nb::object {
                vshogi::engine::dfpn::Searcher<Parameters> dfpn{
                    10u * num_dfpn_nodes};
                dfpn.search(self, num_dfpn_nodes);
                if (dfpn.proved_mate()) {
                    std::vector<Move> out{};
                    for (auto&& m : dfpn.get_mate_moves(self))
                        out.emplace_back(m);
                    if (out.size() > 0u)
                        return nb::cast(out);
                }
                return nb::none();
            },
            nb::arg("num_dfpn_nodes"))
        .def("copy", [](const Game& self) { return Game(self); });
}

template <class Parameters>
inline void export_az_searcher(nanobind::module_& m)
{
    namespace nb = nanobind;
    using Game = vshogi::Game<Parameters>;
    using Node = vshogi::engine::az::Node;
    using Move = pyvshogi::Move<Parameters>;
    using Searcher = vshogi::engine::az::Searcher<Parameters>;

    nb::class_<Searcher>(m, "AlphaZero")
        .def(nb::init<const uint, const uint, const uint>())
        .def("init", &Searcher::init)
        .def(
            "search",
            [](Searcher& self,
               Game& game,
               const float c_puct,
               const float p_random) -> nb::object {
                const auto out = self.search(game, c_puct, p_random);
                if (out == nullptr)
                    return nb::none();
                return nb::cast(*out, nb::rv_policy::reference);
            })
        .def(
            "simulate_expand_backprop",
            [](Searcher& self,
               Node* const leaf,
               Game& game,
               const float value,
               const nb::ndarray<nb::numpy, float, nb::c_contig>&
                   policy_logits) {
                self.simulate_expand_backprop(
                    leaf, game, value, policy_logits.data());
            })
        .def(
            "apply",
            [](Searcher& self, Game& g, const Move& m) {
                self.apply(g, m.m_value);
            })
        .def(
            "get_root",
            [](Searcher& self) -> nb::object {
                const Node& out = self.get_root();
                return nb::cast(out, nb::rv_policy::reference);
            })
        .def("proved_mate", &Searcher::proved_mate)
        .def("get_search_count", &Searcher::get_search_count)
        .def(
            "select_action",
            [](const Searcher& self) { return Move(self.select_action()); })
        .def(
            "select_action",
            [](const Searcher& self, const float temperature) {
                return Move(self.select_action(temperature));
            })
        .def("get_mate_moves", [](Searcher& self, Game& g) {
            std::vector<Move> out{};
            for (auto&& m : self.get_mate_moves(g))
                out.emplace_back(m);
            return out;
        });
}

template <class P>
inline void export_gaz_searcher(nanobind::module_& m)
{
    namespace nb = nanobind;
    using Game = vshogi::Game<P>;
    using Move = pyvshogi::Move<P>;
    using Node = vshogi::engine::gaz::Node;
    using Searcher = vshogi::engine::gaz::Searcher<P>;
    nb::class_<Searcher>(m, "GumbelAlphaZero")
        .def(nb::init<const uint, const uint, const uint>())
        .def("init", &Searcher::init)
        .def("get_search_count", &Searcher::get_search_count)
        .def("count_active_childs", &Searcher::count_active_childs)
        .def("proved_mate", &Searcher::proved_mate)
        .def(
            "get_root",
            [](const Searcher& self) {
                return nb::cast(self.get_root(), nb::rv_policy::reference);
            })
        .def("keep_top_n_actions", &Searcher::keep_top_n_actions)
        .def(
            "search",
            [](Searcher& self, Game& game) -> nb::object {
                const auto out = self.search(game);
                if (out == nullptr)
                    return nb::none();
                return nb::cast(*out, nb::rv_policy::reference);
            })
        .def(
            "simulate_expand_backprop",
            [](Searcher& self,
               Node* const leaf,
               Game& game,
               const float value,
               const nb::ndarray<nb::numpy, float, nb::c_contig>&
                   policy_logits) {
                self.simulate_expand_backprop(
                    leaf, game, value, policy_logits.data());
            })
        .def(
            "select_action",
            [](const Searcher& self) { return Move(self.select_action()); })
        .def(
            "select_action",
            [](const Searcher& self, const float temperature) {
                return Move(self.select_action(temperature));
            })
        .def(
            "get_mate_moves",
            [](Searcher& self, Game& g) {
                std::vector<Move> out{};
                for (auto&& m : self.get_mate_moves(g))
                    out.emplace_back(m);
                return out;
            })
        .def(
            "improved_policy",
            [](const Searcher& self) -> nb::dict {
                nb::dict out{};
                const Node& root = self.get_root();
                const uint n = root.count_childs();
                if (n == 0u)
                    return out;
                std::vector<float> probas(n);
                root.improved_policy(probas.data());
                const float* p = probas.data();
                for (const Node* c = root.get_child(); c; c = c->get_sibling())
                    out[nb::cast(Move(c->get_action()))] = *p++;
                return out;
            })
        .def("apply", [](Searcher& self, Game& game, const Move& action) {
            self.apply(game, action.m_value);
        });
}

template <class Parameters>
inline void export_mcgs_searcher(nanobind::module_& m)
{
    namespace nb = nanobind;
    using Game = vshogi::Game<Parameters>;
    using Node = vshogi::engine::mcgs::Node;
    using Move = pyvshogi::Move<Parameters>;
    using Searcher = vshogi::engine::mcgs::Searcher<Parameters>;

    nb::class_<Searcher>(m, "Mcgs")
        .def(nb::init<const uint, const uint, const uint>())
        .def("init", &Searcher::init)
        .def(
            "search",
            [](Searcher& self,
               Game& game,
               const float epsilon_greedy) -> nb::object {
                const auto out = self.search(game, epsilon_greedy);
                if (out == nullptr)
                    return nb::none();
                return nb::cast(*out, nb::rv_policy::reference);
            },
            nb::arg("game"),
            nb::arg("epsilon_greedy"))
        .def(
            "simulate_expand_backprop",
            [](Searcher& self,
               Node* const leaf,
               Game& game,
               const float value,
               const nb::ndarray<nb::numpy, float, nb::c_contig>&
                   policy_logits) {
                self.simulate_expand_backprop(
                    leaf, game, value, policy_logits.data());
            })
        .def(
            "apply",
            [](Searcher& self, Game& g, const Move& m) {
                self.apply(g, m.m_value);
            })
        .def(
            "get_root",
            [](Searcher& self) -> nb::object {
                const Node& out = self.get_root();
                return nb::cast(out, nb::rv_policy::reference);
            })
        .def("proved_mate", &Searcher::proved_mate)
        .def("get_search_count", &Searcher::get_search_count)
        .def(
            "select_action",
            [](const Searcher& self) { return Move(self.select_action()); })
        .def(
            "select_action",
            [](const Searcher& self, const float temperature) {
                return Move(self.select_action(temperature));
            })
        .def("get_mate_moves", [](Searcher& self, Game& g) {
            std::vector<Move> out{};
            for (auto&& m : self.get_mate_moves(g))
                out.emplace_back(m);
            return out;
        });
}

template <class Parameters>
inline void export_dfpn_searcher(nanobind::module_& m)
{
    namespace nb = nanobind;
    using Searcher = vshogi::engine::dfpn::Searcher<Parameters>;
    using Move = pyvshogi::Move<Parameters>;

    nb::class_<Searcher>(m, "DfpnSearcher")
        .def(nb::init<const uint>())
        .def("init", &Searcher::init)
        .def("search", &Searcher::search)
        .def("proved_mate", &Searcher::proved_mate)
        .def("proved_no_mate", &Searcher::proved_no_mate)
        .def("proved", &Searcher::proved)
        .def("get_search_count", &Searcher::get_search_count)
        .def(
            "select",
            [](const Searcher& self) { return Move(self.select_action()); })
        .def(
            "get_mate_moves",
            [](const Searcher& self, vshogi::Game<Parameters>& g) {
                std::vector<Move> out{};
                for (auto&& m : self.get_mate_moves(g))
                    out.emplace_back(m);
                return out;
            })
        .def("get_root", [](const Searcher& self) -> nb::object {
            return nb::cast(self.get_root(), nb::rv_policy::reference);
        });
}

template <class Parameters>
inline void export_value_functions(nanobind::module_& m)
{
    namespace nb = nanobind;
    m.def("piece_value_func", &vshogi::engine::piece_value_func<Parameters>);
}

template <class Parameters>
void export_classes(nanobind::module_& m)
{
    export_to_jpn<Parameters>(m);
    export_to_sfen<Parameters>(m);
    export_board<Parameters>(m);
    export_piece_stand<Parameters>(m);
    export_move<Parameters>(m);
    export_state<Parameters>(m);
    export_game<Parameters>(m);
    export_az_searcher<Parameters>(m);
    export_gaz_searcher<Parameters>(m);
    export_mcgs_searcher<Parameters>(m);
    export_value_functions<Parameters>(m);
    export_dfpn_searcher<Parameters>(m);
}

} // namespace pyvshogi

#endif // PYTHON_VSHOGI_WRAPPER_HPP
