import numpy as np
from scipy.optimize import minimize
from sleipnir.optimization import Problem

from core.feedback.utils import make_arrays
from core.sample.logged_sample import LoggedSample

def simulate_closed_loop(gains, K, tau, theta, dt, N, sp=1.0, u_max=100):
    Kp, Ki, Kd = gains
    delay_steps = max(1, int(theta / dt))

    y = np.zeros(N)
    u = np.zeros(N)
    e = np.zeros(N)

    u_buffer = np.zeros(delay_steps)

    integral = 0.0
    prev_error = 0.0

    for k in range(1, N):
        y[k] = y[k - 1] + (dt / tau) * (-y[k - 1] + K * u_buffer[0]) # FOPDT differential form

        e[k] = sp - y[k]

        integral += e[k] * dt
        derivative = (e[k] - prev_error) / dt

        u_unbounded = Kp * e[k] + Ki * integral + Kd * derivative

        u[k] = np.clip(u_unbounded, 0.0, u_max)

        u_buffer = np.roll(u_buffer, -1)
        u_buffer[-1] = u[k]
        prev_error = e[k]

    return y, u, e

class CostFeedbackSolver:

    def __init__(self, data: list[LoggedSample], dt: float, wc: float, K: float, tau: float, theta: float):
        self.data = data
        self.dt = dt
        self.wc = wc
        self.K = K
        self.tau = tau
        self.theta = theta

        self.N, self.u, self.y_meas, self.t_vec = make_arrays(data, dt)

        self.problem = Problem()
        self.kp, self.ki, self.kd = self.problem.decision_variable(3)

    def calc_cost_function(self,gains, w_error=0.8, w_cost=0.3, w_aggression=0.2):
        y, u, e = simulate_closed_loop(gains, self.K, self.tau, self.theta, self.dt, self.N)

        # Integral of Squared Error penalizes tracking deviation from setpoint
        ise = np.sum(e ** 2) * self.dt

        # Penalizes total control effort to minimize resource consumption
        cost = np.sum(u ** 2) * self.dt

        # Penalizes rate of change of u(t) to prevent valve chatter and aggressive action
        du = np.diff(u) / self.dt
        aggression = np.sum(du ** 2) * self.dt

        return w_error * ise + w_cost * cost + w_aggression * aggression


    def fit(self):
        bounds = [(0.0, 50.0), (0.0, 50.0), (0.0, 10.0)]
        initial_gains = [1.0, 0.0, 0.2]

        result = minimize(
            self.calc_cost_function,
            initial_gains,
            method='SLSQP',
            bounds=bounds
        )
        return result.x
