from tqdm import tqdm

import vshogi


if __name__ == '__main__':
    from vshogi.shogi import Game, Move

    network = vshogi.dlshogi.build_policy_value_network(
        input_size=(Game.ranks, Game.files),
        input_channels=Game.feature_channels,
        num_policy_per_square=Move._num_policy_per_square(),
        hidden_channels=128,
        bottleneck_channels=32,
        num_backbone_blocks=10,
        attention_matrix=Game.get_attention(),
    )
    network.summary()
    player = vshogi.engine.DfpnMcts(
        vshogi.engine.DfpnSearcher(),
        vshogi.engine.Mcts(vshogi.dlshogi.PolicyValueFunction(network)),
    )

    game = Game()
    kifu = [
        "7g7f" , "3c3d" , "2g2f" , "4a3b" , "2f2e" , "2b3c" , "3i4h" , "3a2b" ,
        "6i7h" , "7a6b" , "6g6f" , "6c6d" , "7i6h" , "6b6c" , "6h6g" , "6c5d" ,
        "5g5f" , "9c9d" , "9g9f" , "8b6b" , "4i5h" , "6d6e" , "4h5g" , "5a4b" ,
        "8h7g" , "6a5b" , "6f6e" , "5d6e" , "P*6f" , "6e5d" , "3g3f" , "4b3a" ,
        "5i6h" , "4c4d" , "3f3e" , "5d4c" , "2h3h" , "3d3e" , "3h3e" , "P*3d" ,
        "3e3h" , "3c4b" , "5g4f" , "2b3c" , "1g1f" , "1c1d" , "6h7i" , "7c7d" ,
        "7g6h" , "P*6e" , "4f5g" , "5c5d" , "4g4f" , "7d7e" , "3h2h" , "3a2b" ,
        "6f6e" , "6b6e" , "5g6f" , "6e3e" , "P*3g" , "7e7f" , "6g7f" , "8a7c" ,
        "5h4g" , "P*7e" , "7f6g" , "9d9e" , "3g3f" , "3e6e" , "6f6e" , "7c6e" ,
        "P*6f" , "9e9f" , "P*9h" , "S*7f" , "6g5h" , "P*6g" , "6h5i" , "4d4e" ,
        "6f6e" , "7f6e" , "5h6g" , "P*6f" , "6g5h" , "4b6d" , "R*6a" , "5b5c" ,
        "5f5e" , "6d5e" , "6a6e+", "4e4f" , "4g3g" , "5c6d" , "6e5f" , "6f6g+",
        "5h6g" , "5e9i+", "S*8h" , "9i8h" , "7h8h" , "L*5e" , "N*4e" , "5e5f" ,
        "4e3c+", "2a3c" , "6g5f" , "R*4i" , "S*6h" , "5d5e" , "5f6g" , "4f4g+",
        "B*2g" , "4g3g" , "2g4i" , "3g2h" , "R*6a" , "G*3a" , "6a6d+", "R*3i" ,
        "4i5h" , "3i2i+", "P*4d" , "4c4d" , "6d4d" , "P*4c" , "4d6d" , "2h3h" ,
        "L*4f" , "N*4d" , "4f4d" , "4c4d" , "N*4f" , "N*4b" , "S*5c" , "S*4c" ,
        "5c4b+", "3a4b" , "N*5d" , "4c5d" , "4f5d" , "4b4c" , "S*4a" , "3h4h" ,
        "4a3b+", "2b3b" , "6d6b" , "L*4b" , "5d4b+", "4c4b" , "G*5c" , "S*3a" ,
        "5c4b" , "3a4b" , "G*5c" , "G*3a" , "G*4c" , "3b2a" , "5c4b" , "2i5i" ,
        "6h5i" , "4h5h" , "4b3a" , "2a3a" , "S*2b" , "3a4a" , "L*4b" ,
    ]
    for move in tqdm(kifu, ncols=100):
        if game.result != vshogi.Result.ONGOING:
            break
        player.set_game(game)
        player.search(
            mcts_searches=1000,
            mcts_endgame_searches=10000,
            kldgain_threshold=1e-4,
        )
        game.apply(move)
