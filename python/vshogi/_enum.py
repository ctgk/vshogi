from enum import Enum


class _Enum(Enum):

    def __repr__(self) -> str:
        return f'{self.__class__.__name__}.{self.name}'
