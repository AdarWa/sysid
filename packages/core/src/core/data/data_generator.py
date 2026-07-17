from math import exp

from core.feedforward.eq import signum
from core.sample.logged_sample import LoggedSample


def calculate_quas_voltage(torque: float, omega: float, kt: float, kv: float, int_res: float) -> float:
    """
    Calculate the voltage that is needed for a DC Motor to be at for the following parameters to apply
    :param torque: Torque (N*m)
    :param omega: Angular Acceleration (rad/s)
    :param kt: Torque constant (N*m/A)
    :param kv: Motor velocity constant ((rad/s)/V)
    :param int_res: Internal resistance (Ohms)
    :return: Voltage (Volts)
    """
    return torque/kt*int_res + omega/kv

def calculate_dynamic_voltage(torque_friction: float, inertia: float, omega: float, alpha: float,
                              kt: float, kv: float, int_res: float) -> float:
    total_torque = torque_friction + (inertia * alpha)
    return (total_torque / kt) * int_res + (omega / kv)

def make_const_accel_data(x0: float, v0: float, a: float, time: float, dt: float = 0.005) -> list[LoggedSample]:
    """
    Generate a list of `LoggedSample` with synthesized dynamic data with a constant acceleration
    :param x0: Starting position (Meters)
    :param v0: Starting velocity (m/s)
    :param a: Acceleration (m/s^2)
    :param time: Time span to generate this data for (sec)
    :param dt: dt to space each sample (sec)
    :return: list of `LoggedSample`
    """
    data = []
    kt = 0.1
    kv_motor = 0.1
    int_res = 0.02

    friction_torque = 0.5
    mass = 0.5 # kg
    g = 9.81
    dam_radius = 0.05 # m
    gravity_torque = mass*g*dam_radius

    for i in range(0, int(time/dt)):
        t = i*dt
        x = x0+v0*t+0.5*a*t**2
        v = v0+a*t

        V = calculate_quas_voltage(friction_torque*signum(v) + gravity_torque, v, kt, kv_motor, int_res)
        data.append(LoggedSample(x,v,a,V))
    return data


def make_dynamic_step_data(time: float, dt: float = 0.005) -> list[LoggedSample]:
    data = []
    kt, kv_motor, int_res = 0.1, 0.1, 0.02
    friction_torque = 0.5
    inertia = 0.25  # kg*m^2

    # Velocity exponential curve
    # v(t) = v_max * (1 - e^(-t / tau))
    v_max = 5.0
    tau = 0.4  # Time constant (sec)

    x = 0.0
    for i in range(int(time / dt)):
        t = i * dt
        v = v_max * (1.0 - exp(-t / tau))
        a = (v_max / tau) * exp(-t / tau)
        x += v * dt

        V = calculate_dynamic_voltage(friction_torque, inertia, v, a, kt, kv_motor, int_res)
        data.append(LoggedSample(x, v, a, V))

    return data