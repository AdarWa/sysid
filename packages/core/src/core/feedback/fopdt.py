import math

import numpy as np
from sleipnir.autodiff import log, exp
from sleipnir.optimization import ExitStatus, Problem

from core.sample.logged_sample import LoggedSample


def fopdt_dynamics(t, K, tau, theta):
    # Softplus provides a continuously differentiable approximation for max(0.0, t - theta)
    t_diff = t - theta
    print(type(t_diff))
    t_eff = log(1.0 + exp(20.0 * t_diff)) / 20.0

    smooth_step = 1.0 / (1.0 + exp(-50.0 * t_diff))
    return K * (1.0 - exp(-t_eff / tau)) * smooth_step


class FOPDTSolver:
    def __init__(self, data: list[LoggedSample], dt: float):
        self.dt = dt
        self.N = len(data)
        self.u = np.array([sample.voltage for sample in data])
        self.y_meas = np.array([sample.velocity for sample in data])
        self.t_vec = np.arange(self.N) * self.dt

    def fit(self):
        problem = Problem()

        K = problem.decision_variable()
        tau = problem.decision_variable()
        theta = problem.decision_variable()

        K.set_value(self.y_meas[-1])
        tau.set_value(1.0)
        theta.set_value(0.1)

        max_time = float(self.N * self.dt)
        problem.subject_to(K >= 0.0)
        problem.subject_to(K <= 100.0)
        problem.subject_to(tau >= 1e-4)
        problem.subject_to(tau <= max_time)
        problem.subject_to(theta >= 0.0)
        problem.subject_to(theta <= max_time)

        cost = 0.0
        for t, y_m in zip(self.t_vec, self.y_meas):
            y_pred = fopdt_dynamics(t, K, tau, theta)
            cost += (y_m - y_pred) ** 2

        problem.minimize(cost)

        status = problem.solve()

        if status == ExitStatus.SUCCESS:
            K_fit = K.value()
            tau_fit = tau.value()
            theta_fit = theta.value()

            print("Optimization converged successfully.")
            print(f"Fitted Gain (K):          {K_fit:.4f}")
            print(f"Fitted Time Constant (τ): {tau_fit:.4f} s")
            print(f"Fitted Dead Time (θ):     {theta_fit:.4f} s")
            print(f"Final Mean Squared Error: {cost.value() / self.N:.6f}")
            return result_vals(K_fit, tau_fit, theta_fit, status)
        else:
            print(f"Solver failed to converge: {status.exit_status}")
            return None


def result_vals(K, tau, theta, status):
    class Result:
        pass
    res = Result()
    res.x = np.array([K, tau, theta])
    res.success = True
    res.status = status
    return res