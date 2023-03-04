#include <string>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "cshogi.h"
#include "dfpn.h"
#include "init.hpp"
#include "parser.h"
#include "position.hpp"


namespace py = pybind11;

PYBIND11_MODULE(_cshogi, m)
{
    m.def("initTable", &initTable);
    py::class_<Position>(m, "Position")
        .def_static("initZobrist", Position::initZobrist);
    m.def("HuffmanCodedPos_init", &HuffmanCodedPos_init);
    m.def("PackedSfen_init", &PackedSfen_init);
    m.def("Book_init", &Book_init);
    m.def("to_usi", &__to_usi);
    m.def("to_csa", &__to_csa);
    py::class_<__Board>(m, "_Board")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def(py::init<const __Board&>())
        .def("set", &__Board::set)
        .def("set_position", &__Board::set_position)
        // .def("set_pieces", &__Board::set_pieces)
        .def("set_hcp", &__Board::set_hcp)
        .def("set_psfen", &__Board::set_psfen)
        .def("reset", &__Board::reset)
        .def("dump", &__Board::dump)
        .def("push", &__Board::push)
        .def("pop", &__Board::pop)
        .def("peek", &__Board::peek)
        .def("get_history", &__Board::get_history)
        .def("is_game_over", &__Board::is_game_over)
        .def("isDraw", &__Board::isDraw)
        .def("move", &__Board::move)
        .def("drop_move", &__Board::drop_move)
        .def("move_from_usi", &__Board::move_from_usi)
        .def("move_from_csa", &__Board::move_from_csa)
        .def("move_from_move16", &__Board::move_from_move16)
        .def("move_from_psv", &__Board::move_from_psv)
        .def("turn", &__Board::turn)
        .def("setTurn", &__Board::setTurn)
        .def("ply", &__Board::ply)
        .def("setPly", &__Board::setPly)
        .def("toSFEN", &__Board::toSFEN)
        .def("toCSAPos", &__Board::toCSAPos)
        .def(
            "toHuffmanCodedPos",
            [](const __Board& self, const py::array_t<uint8_t>& hcp){
                return self.toHuffmanCodedPos(reinterpret_cast<char*>(
                    hcp.request().ptr));
            }
        )
        .def("toPackedSfen", &__Board::toPackedSfen)
        .def("piece", &__Board::piece)
        .def("inCheck", &__Board::inCheck)
        .def("mateMoveIn1Ply", &__Board::mateMoveIn1Ply)
        .def("mateMove", &__Board::mateMove)
        .def("is_mate", &__Board::is_mate)
        .def("getKey", &__Board::getKey)
        .def("moveIsPseudoLegal", &__Board::moveIsPseudoLegal)
        .def("moveIsLegal", &__Board::moveIsLegal)
        .def("pieces_in_hand", &__Board::pieces_in_hand)
        .def("pieces", &__Board::pieces)
        .def("is_nyugyoku", &__Board::is_nyugyoku)
        .def("piece_planes", &__Board::piece_planes)
        .def("piece_planes_rotate", &__Board::piece_planes_rotate)
        .def("_dlshogi_make_input_features", &__Board::_dlshogi_make_input_features)
        .def("push_pass", &__Board::push_pass)
        .def("pop_pass", &__Board::pop_pass)
        .def("isOK", &__Board::isOK)
        .def("bookKey", &__Board::bookKey);
    m.def("piece_to_piece_type", &__piece_to_piece_type);
    m.def("hand_piece_to_piece_type", &__hand_piece_to_piece_type);
    py::class_<__LegalMoveList>(m, "_LegalMoveList")
        .def(py::init<>())
        .def(py::init<const __Board&>())
        .def("end", &__LegalMoveList::end)
        .def("move", &__LegalMoveList::move)
        .def("next", &__LegalMoveList::next)
        .def("size", &__LegalMoveList::size);
    py::class_<__PseudoLegalMoveList>(m, "_PseudoLegalMoveList")
        .def(py::init<>())
        .def(py::init<const __Board&>())
        .def("end", &__PseudoLegalMoveList::end)
        .def("move", &__PseudoLegalMoveList::move)
        .def("next", &__PseudoLegalMoveList::next)
        .def("size", &__PseudoLegalMoveList::size);
    m.def("move_to", &__move_to);
    m.def("move_from", &__move_from);
    m.def("move_cap", &__move_cap);
    m.def("move_is_promotion", &__move_is_promotion);
    m.def("move_is_drop", &__move_is_drop);
    m.def("move_from_piece_type", &__move_from_piece_type);
    m.def("move_drop_hand_piece", &__move_drop_hand_piece);
    m.def("move16", &__move16);
    m.def("move16_from_psv", &__move16_from_psv);
    m.def("move16_to_psv", &__move16_to_psv);
    m.def("move_rotate", &__move_rotate);
    m.def("move_to_usi", &__move_to_usi);
    m.def("move_to_csa", &__move_to_csa);
    m.def("_dlshogi_make_move_label", &__dlshogi_make_move_label);
    py::class_<parser::__Parser>(m, "_Parser")
        .def(py::init<>())
        .def_readonly("version", &parser::__Parser::version)
        .def_readonly("informations", &parser::__Parser::informations)
        .def_readonly("sfen", &parser::__Parser::sfen)
        .def_readonly("endgame", &parser::__Parser::endgame)
        .def_readonly("names", &parser::__Parser::names)
        .def_readonly("ratings", &parser::__Parser::ratings)
        .def_readonly("moves", &parser::__Parser::moves)
        .def_readonly("times", &parser::__Parser::times)
        .def_readonly("scores", &parser::__Parser::scores)
        .def_readonly("comments", &parser::__Parser::comments)
        .def_readonly("comment", &parser::__Parser::comment)
        .def_readonly("win", &parser::__Parser::win)
        .def("parse_csa_file", &parser::__Parser::parse_csa_file)
        .def("parse_csa_str", &parser::__Parser::parse_csa_str);
    py::class_<__DfPn>(m, "_DfPn")
        .def(py::init<const int, const unsigned int, const int>())
        .def("search", &__DfPn::search)
        .def("search_andnode", &__DfPn::search_andnode)
        .def("stop", &__DfPn::stop)
        .def("get_move", &__DfPn::get_move)
        .def("get_pv", &__DfPn::get_pv)
        .def_property_readonly(
            "pv", [](const __DfPn& self){
                return std::vector<unsigned int>(self.pv.begin(), self.pv.end());
            })
        .def("set_draw_ply", &__DfPn::set_draw_ply)
        .def("set_maxdepth", &__DfPn::set_maxdepth)
        .def("set_max_search_node", &__DfPn::set_max_search_node)
        .def("get_searched_node", &__DfPn::get_searched_node);
}
