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
    az_init_args_p1: dict,
    az_init_args_p2: dict,
    search_args: dict,
    select_args: dict,
) -> vs.Record:
    shogi = getattr(vs, shogi_variant)
    player1 = vs.engine.Mcgs(
        vs.dlshogi.PolicyValueFunction(player1),
        **az_init_args_p1,
        name=player1,
    )
    player2 = vs.engine.Mcgs(
        vs.dlshogi.PolicyValueFunction(player2),
        **az_init_args_p2,
        name=player2,
    )
    record_of_p1 = vs.Record(0, 0, 0, 0, 0, 0)
    iterator = range(num_games_each * 2)
    show_az_search: bool = isinstance(search_args['budget'], float)
    p1_search_total = 0
    p2_search_total = 0
    if show_pbar:
        iterator = tqdm(iterator, ncols=80)
        iterator.set_description(str({'p1': 0, 'draw': 0, 'p2': 0}))
    for i in iterator:
        if i % 2 == 0:
            out = vs.play_game(
                shogi.Game(),
                player1,
                player2,
                search_args=search_args,
                select_args=select_args,
                draw_on_max_moves=True,
                _return_num_searched=isinstance(search_args['budget'], float),
            )
            if isinstance(search_args['budget'], float):
                result = out[0].result
                p1_search_total += np.nanmean(np.asarray(out[1][::2], float))
                p2_search_total += np.nanmean(np.asarray(out[1][1::2], float))
            else:
                result = out.result
            record_of_p1 += vs.Record.from_black_result(result)
        else:
            out = vs.play_game(
                shogi.Game(),
                player2,
                player1,
                search_args=search_args,
                select_args=select_args,
                draw_on_max_moves=True,
                _return_num_searched=isinstance(search_args['budget'], float),
            )
            if isinstance(search_args['budget'], float):
                result = out[0].result
                p2_search_total += np.nanmean(np.asarray(out[1][::2], float))
                p1_search_total += np.nanmean(np.asarray(out[1][1::2], float))
            else:
                result = out.result
            record_of_p1 += vs.Record.from_white_result(result)
        if show_pbar:
            iterator.set_description(
                str(
                    {
                        'p1': record_of_p1.wins_total,
                        'draw': record_of_p1.draws_total,
                        'p2': record_of_p1.losses_total,
                    }
                )
            )
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
    print(
        template.format(
            record.wins_total,
            record.wins_black,
            record.wins_white,
            record.draws_total,
            record.draws_black,
            record.draws_white,
            record.losses_total,
            record.losses_black,
            record.losses_white,
        )
    )


class _NargsOption(cl.Option):
    def __init__(self, *args, **kwargs):
        self.save_other_options = kwargs.pop('save_other_options', True)
        # pop nargs to avoid a check inside `click`
        self.nargs_custom = kwargs.pop('nargs', -1)

        super(_NargsOption, self).__init__(*args, **kwargs)
        self._previous_parser_process = None
        self._eat_all_parser = None

    def add_to_parser(self, parser, ctx):
        def parser_process(value, state):
            value = [value]
            done = False
            if self.save_other_options:
                while state.rargs and not done:
                    for prefix in self._eat_all_parser.prefixes:
                        if state.rargs[0].startswith(prefix):
                            done = True
                    if not done:
                        value.append(state.rargs.pop(0))
            else:
                value += state.rargs
                state.rargs[:] = []

            value = " ".join(value)
            self._previous_parser_process(value, state)

        retval = super(_NargsOption, self).add_to_parser(parser, ctx)
        for name in self.opts:
            our_parser = parser._long_opt.get(name) or parser._short_opt.get(
                name
            )
            if our_parser:
                self._eat_all_parser = our_parser
                self._previous_parser_process = our_parser.process
                our_parser.process = parser_process
                break
        return retval

    def type_cast_value(self, ctx, value):
        if isinstance(value, tuple):
            if self.nargs_custom == -1 or len(value) == self.nargs_custom:
                return value
            raise cl.BadParameter(
                f"expected {self.nargs_custom} values, got {len(value)}",
                ctx=ctx,
            )

        if isinstance(value, str):
            values = value.split()
        else:
            values = [value] if value is not None else []

        if self.nargs_custom > 1 and len(values) == 1:
            values = tuple(values[0] for _ in range(self.nargs_custom))

        if self.nargs_custom != -1 and len(values) != self.nargs_custom:
            raise cl.BadParameter(
                f"expected {self.nargs_custom} values, got {len(values)}",
                ctx=ctx,
            )

        if self.type:
            values = [self.type.convert(v, self, ctx) for v in values]
        return tuple(values)


@cl.command()
@cl.argument(
    'shogi-variant',
    type=cl.Choice(['shogi', 'judkins_shogi', 'minishogi']),
)
@cl.option(
    '-p1',
    '--player1',
    cls=_NargsOption,
    type=str,
    required=True,
)
@cl.option(
    '-p2',
    '--player2',
    cls=_NargsOption,
    type=str,
    required=True,
)
@cl.option(
    '--num-games-each',
    type=int,
    default=10,
    show_default=True,
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
    "--az-epsilon-greedy",
    cls=_NargsOption,
    type=cl.FLOAT,
    nargs=2,
    default=(0.1, 0.1),
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
    '--show-inner-pbar',
    is_flag=True,
)
@cl.option(
    "--show-outer-pbar",
    is_flag=True,
)
@cl.option(
    "--output",
    type=cl.Choice(['table', 'wdl']),
    default="wdl",
    show_default=True,
)
def _match(
    shogi_variant,
    player1,
    player2,
    num_games_each,
    az_search_count,
    az_search_second,
    az_epsilon_greedy,
    az_temperature,
    dfpn_search_root,
    dfpn_search_leaf,
    show_inner_pbar,
    show_outer_pbar,
    output,
):
    if (az_search_count is None) and (az_search_second is None):
        raise ValueError(
            "Either `az_search_count` or `az_search_second` must be given"
        )
    if len(az_epsilon_greedy) == 1:
        az_epsilon_greedy = (az_epsilon_greedy[0], az_epsilon_greedy[0])
    assert isinstance(az_epsilon_greedy[0], float), az_epsilon_greedy
    assert isinstance(az_epsilon_greedy[1], float)

    record_of_p1_group = vs.Record(0, 0, 0, 0, 0, 0)
    iterator = itertools.product(player1, player2)
    if show_outer_pbar:
        iterator = tqdm(iterator, total=len(player1) * len(player2))
    for p1, p2 in iterator:
        if show_inner_pbar:
            print(f'player1: {p1}')
            print(f'player2: {p2}')
        record_of_p1 = _get_results_of_single_pair(
            shogi_variant,
            p1,
            p2,
            num_games_each,
            show_inner_pbar,
            az_init_args_p1={
                "epsilon_greedy": az_epsilon_greedy[0],
                'dfpn_search_root': dfpn_search_root,
                'dfpn_search_leaf': dfpn_search_leaf,
            },
            az_init_args_p2={
                "epsilon_greedy": az_epsilon_greedy[1],
                'dfpn_search_root': dfpn_search_root,
                'dfpn_search_leaf': dfpn_search_leaf,
            },
            search_args={
                'budget': az_search_count or az_search_second,
            },
            select_args={
                'temperature': az_temperature,
            },
        )
        record_of_p1_group += record_of_p1

        if show_inner_pbar:
            _print_results(record_of_p1)

        if show_outer_pbar:
            iterator.set_description(f"p1 vs p2 = {record_of_p1_group.wdl()}")

    if (not show_inner_pbar) or (len(player1) > 1) or (len(player2) > 1):
        print(f'player1: {player1}')
        print(f'player2: {player2}')
        if output == "table":
            _print_results(record_of_p1_group)
        else:
            print(f"p1 vs p2 = {record_of_p1_group.wdl()}")


if __name__ == '__main__':
    _match()
