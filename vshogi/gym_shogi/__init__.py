from gym.envs.registration import register

register(
    id='Shogi-v0',
    entry_point='vshogi.gym_shogi.envs:ShogiEnv',
)
