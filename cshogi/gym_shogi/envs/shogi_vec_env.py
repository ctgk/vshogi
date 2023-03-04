from cshogi.gym_shogi.envs import ShogiEnv

class ShogiVecEnv:
    metadata = {'render.modes': ['sfen']}

    def __init__(self, num_envs: int):
        self.num_envs = num_envs
        self.envs = [ShogiEnv() for _ in range(num_envs)]

    def reset(self):
        for env in self.envs:
            env.reset()

    def render(self, mode='sfen'):
        return [env.render(mode=mode) for env in self.envs]

    def step(self, moves: list):
        rewards = []
        dones = []
        is_draws = []

        for i, move in enumerate(moves):
            reward, done, is_draw = self.envs[i].step(move)
            rewards.append(reward)
            dones.append(done)
            is_draws.append(is_draw)
            if done:
                self.envs[i].reset()

        return rewards, dones, is_draws
