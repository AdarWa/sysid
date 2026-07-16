import dataclasses

@dataclasses.dataclass
class LoggedSample:
    position: float
    velocity: float
    acceleration: float
    voltage: float