from typing import NamedTuple


class FeedforwardGains(NamedTuple):
    ks: float
    kv: float
    ka: float
    kg: float