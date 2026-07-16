from sleipnir.optimization import Problem, ExitStatus

from . import eq
from core.sample.logged_sample import LoggedSample
from .gains import FeedforwardGains


class QuasistaticSolver:

    def __init__(self) -> None:
        """
        Solver class for the quasistatic part of the motion.
        Solves for K_s and K_v. The dynamic solver can then
        be used after in order to find K_a based on the found K_s, K_v.
        """
        self.problem = Problem()
        self.ks,self.kv = self.problem.decision_variable(2)
        self.problem.subject_to(self.ks >= 0)
        self.problem.subject_to(self.kv >= 0)

    def init_data(self, data: list[LoggedSample]) -> None:
        J = 0
        for sample in data:
            # The purpose of the quasistatic test is to find K_v and K_s, this can be easily done when the acceleration is zero.
            # The quasistatic part must have an acceleration be as close as it can to zero.
            u_pred = eq.calculate_feedforward(self.ks, self.kv, 0, sample.velocity, 0)
            u = sample.voltage
            J += pow(u_pred-u, 2)

        self.problem.minimize(J)

    def solve(self) -> FeedforwardGains | None:
        stat = self.problem.solve()
        if stat != ExitStatus.SUCCESS:
            # TODO: Log
            return None

        return FeedforwardGains(self.ks.value(), self.kv.value(), 0)