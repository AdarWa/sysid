import numpy as np

from core.sample.logged_sample import LoggedSample


def make_arrays(data: list[LoggedSample], dt: float):
    """
    :param data:
    :param dt:
    :return: N, u, y_meas, t_vec
    """
    N = len(data)
    u = np.array([sample.voltage for sample in data])
    y_meas = np.array([sample.velocity for sample in data])
    t_vec = np.arange(N) * dt
    return N, u, y_meas, t_vec