from sleipnir.optimization import Problem, ExitStatus

from . import eq
from core.sample.logged_sample import LoggedSample
from .gains import FeedforwardGains


class DynamicSolver:

    def __init__(self, ks: float, kv: float) -> None:
        self.ks = ks
        self.kv = kv
        self.problem = Problem()
        self.ka = self.problem.decision_variable()
        self.problem.subject_to(self.ka >= 0)

    def init_data(self, data: list[LoggedSample]) -> None:
        J = 0
        for sample in data:
            u_pred = eq.calculate_feedforward(self.ks, self.kv, self.ka, sample.velocity, sample.acceleration)
            u = sample.voltage
            J += pow(u_pred-u, 2)

        self.problem.minimize(J)

    def solve(self) -> FeedforwardGains | None:
        stat = self.problem.solve()
        if stat != ExitStatus.SUCCESS:
            # TODO: Log
            return None

        return FeedforwardGains(self.ks, self.kv, self.ka.value())