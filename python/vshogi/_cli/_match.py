import itertools

import click as cl
import numpy as np
from tqdm import tqdm

import vshogi as vs


def _get_results_of_single_pair(
    shogi_variant: str,
    player1: str,
    player2: str,
    num_games_each: int,
    show_pbar: bool,
    az_init_args: dict,
    search_args: dict,
    select_args: dict,
) -> vs.Record:
    shogi = getattr(vs, shogi_variant)
    player1 = vs.engine.AlphaZero(
        vs.dlshogi.PolicyValueFunction(player1),
        **az_init_args,
    )
    player2 = vs.engine.AlphaZero(
        vs.dlshogi.PolicyValueFunction(player2),
        **az_init_args,
    )
    record_of_p1 = vs.Record(0, 0, 0, 0, 0, 0)
    iterator = range(num_games_each * 2)
    show_az_search: bool = isinstance(search_args['n_or_t'], float)
    p1_search_total = 0
    p2_search_total = 0
    if show_pbar:
        iterator = tqdm(iterator, ncols=80)
        iterator.set_description(str({'p1': 0, 'draw': 0, 'p2': 0}))
    for i in iterator:
        if i % 2 == 0:
            out = vs.play_game(
                shogi.Game(), player1, player2,
                search_args=search_args,
                select_args=select_args,
                _return_num_searched=isinstance(
                    search_args['n_or_t'], float),
            )
            if isinstance(search_args['n_or_t'], float):
                result = out[0].result
                p1_search_total += np.nanmean(np.asarray(out[1][::2], float))
                p2_search_total += np.nanmean(np.asarray(out[1][1::2], float))
            else:
                result = out.result
            record_of_p1 += vs.Record.from_black_result(result)
        else:
            out = vs.play_game(
                shogi.Game(), player2, player1,
                search_args=search_args,
                select_args=select_args,
                _return_num_searched=isinstance(
                    search_args['n_or_t'], float),
            )
            if isinstance(search_args['n_or_t'], float):
                result = out[0].result
                p2_search_total += np.nanmean(np.asarray(out[1][::2], float))
                p1_search_total += np.nanmean(np.asarray(out[1][1::2], float))
            else:
                result = out.result
            record_of_p1 += vs.Record.from_white_result(result)
        if show_pbar:
            iterator.set_description(str({
                'p1': record_of_p1.wins_total,
                'draw': record_of_p1.draws_total,
                'p2': record_of_p1.losses_total,
            }))
    if show_pbar and show_az_search:
        print(
            'Initial search counts:',
            f'p1={p1_search_total / (num_games_each * 2):.2f},',
            f'p2={p2_search_total / (num_games_each * 2):.2f}',
        )
    return record_of_p1


def _print_results(record: vs.Record):
    template = '''\
+---------+-------+-------+-------+
| Player1 | total | black | white |
+---------+-------+-------+-------+
|   #Win  | {:5d} | {:5d} | {:5d} |
+---------+-------+-------+-------+
|  #Draw  | {:5d} | {:5d} | {:5d} |
+---------+-------+-------+-------+
|  #Loss  | {:5d} | {:5d} | {:5d} |
+---------+-------+-------+-------+
'''
    print(template.format(
        record.wins_total,
        record.wins_black,
        record.wins_white,
        record.draws_total,
        record.draws_black,
        record.draws_white,
        record.losses_total,
        record.losses_black,
        record.losses_white,
    ))


# https://stackoverflow.com/questions/48391777/nargs-equivalent-for-options-in-click
class _OptionEatAll(cl.Option):

    def __init__(self, *args, **kwargs):
        self.save_other_options = kwargs.pop('save_other_options', True)
        nargs = kwargs.pop('nargs', -1)
        assert nargs == -1, 'nargs, if set, must be -1 not {}'.format(nargs)
        super(_OptionEatAll, self).__init__(*args, **kwargs)
        self._previous_parser_process = None
        self._eat_all_parser = None

    def add_to_parser(self, parser, ctx):

        def parser_process(value, state):
            # method to hook to the parser.process
            done = False
            value = [value]
            if self.save_other_options:
                # grab everything up to the next option
                while state.rargs and not done:
                    for prefix in self._eat_all_parser.prefixes:
                        if state.rargs[0].startswith(prefix):
                            done = True
                    if not done:
                        value.append(state.rargs.pop(0))
            else:
                # grab everything remaining
                value += state.rargs
                state.rargs[:] = []
            value = tuple(value)

            # call the actual process
            self._previous_parser_process(value, state)

        retval = super(_OptionEatAll, self).add_to_parser(parser, ctx)
        for name in self.opts:
            our_parser = (
                parser._long_opt.get(name) or parser._short_opt.get(name)
            )
            if our_parser:
                self._eat_all_parser = our_parser
                self._previous_parser_process = our_parser.process
                our_parser.process = parser_process
                break
        return retval


@cl.command()
@cl.argument(
    'shogi-variant',
    type=cl.Choice(['shogi', 'judkins_shogi', 'minishogi']),
)
@cl.option(
    '-p1', '--player1',
    cls=_OptionEatAll,
    type=tuple,
    required=True,
)
@cl.option(
    '-p2', '--player2',
    cls=_OptionEatAll,
    type=tuple,
    required=True,
)
@cl.option(
    '--num-games-each',
    type=int,
    default=10,
    show_default=True,
)
@cl.option(
    '--az-kldgain-threshold',
    type=float,
    default=None,
)
@cl.option(
    '--az-search-count',
    type=int,
    default=None,
)
@cl.option(
    '--az-search-second',
    type=float,
    default=None,
)
@cl.option(
    '--az-coeff-puct',
    type=float,
    default=4.0,
    show_default=True,
)
@cl.option(
    '--az-temperature',
    type=float,
    default=0.0,
    show_default=True,
)
@cl.option(
    '--dfpn-search-root',
    type=int,
    default=10000,
    show_default=True,
)
@cl.option(
    '--dfpn-search-leaf',
    type=int,
    default=100,
    show_default=True,
)
@cl.option(
    '--show-pbar',
    is_flag=True,
)
def _match(
    shogi_variant,
    player1,
    player2,
    num_games_each,
    az_kldgain_threshold,
    az_search_count,
    az_search_second,
    az_coeff_puct,
    az_temperature,
    dfpn_search_root,
    dfpn_search_leaf,
    show_pbar,
):
    if (az_search_count is None) and (az_search_second is None):
        raise ValueError(
            "Either `az_search_count` or `az_search_second` must be given")

    record_of_p1_group = vs.Record(0, 0, 0, 0, 0, 0)
    for p1, p2 in itertools.product(player1, player2):
        if show_pbar:
            print(f'player1: {p1}')
            print(f'player2: {p2}')
        record_of_p1 = _get_results_of_single_pair(
            shogi_variant,
            p1, p2, num_games_each, show_pbar,
            az_init_args={
                'coeff_puct': az_coeff_puct,
                'dfpn_search_root': dfpn_search_root,
                'dfpn_search_leaf': dfpn_search_leaf,
                'kldgain_threshold': az_kldgain_threshold,
            },
            search_args={
                'n_or_t': az_search_count or az_search_second,
            },
            select_args={
                'temperature': az_temperature,
            },
        )
        record_of_p1_group += record_of_p1

        if show_pbar:
            _print_results(record_of_p1)

    if (not show_pbar) or (len(player1) > 1) or (len(player2) > 1):
        print(f'player1: {player1}')
        print(f'player2: {player2}')
        _print_results(record_of_p1_group)


if __name__ == '__main__':
    _match()
