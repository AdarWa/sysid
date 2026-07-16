from typing import Any

import numpy as np
from matplotlib import pyplot as plt

from core.feedforward.quasistatic_solver import QuasistaticSolver
from core.sample.logged_sample import LoggedSample

def calculate_voltage(torque, omega, kt, kv, int_res):
    """
    :param torque: Torque (N*m)
    :param omega: Angular Acceleration (rad/s)
    :param kt: Torque constant (N*m/A)
    :param kv: Motor velocity constant ((rad/s)/V)
    :param int_res: Internal resistance (Ohms)
    :return:
    """
    return torque/kt*int_res + omega/kv

def make_const_accel_data(x0: float, v0: float, a: float, time: float, dt: float = 0.005) -> list[LoggedSample]:
    data = []
    kt = 0.1
    kv_motor = 0.1
    int_res = 0.02

    friction_torque = 0.5

    for i in range(0, int(time/dt)):
        t = i*dt
        x = x0+v0*t+0.5*a*t**2
        v = v0+a*t

        V = calculate_voltage(friction_torque, v, kt, kv_motor, int_res)
        data.append(LoggedSample(x,v,a,V))
    return data


def plot_quasistatic_identification(data: list[LoggedSample], ks: float, kv: float):
    # Unpack data into numpy arrays
    times = np.array([i * 0.005 for i in range(len(data))])
    velocities = np.array([s.velocity for s in data])
    voltages = np.array([s.voltage for s in data])

    # Calculate model predictions and residuals
    v_signs = np.sign(velocities)
    v_signs[velocities == 0] = 1.0  # Handle rest condition
    predicted_voltages = ks * v_signs + kv * velocities
    residuals = voltages - predicted_voltages

    # Create a 1x3 dashboard figure
    fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(16, 5))
    fig.suptitle('Quasistatic System Identification Diagnostics', fontsize=14, fontweight='bold')

    # 1. Voltage vs. Velocity (The linear regression model)
    ax1.scatter(velocities, voltages, color='blue', alpha=0.3, label='Logged Telemetry', s=10)
    ax1.plot(velocities, predicted_voltages, color='red', linewidth=2, label=f'Model: V = {ks:.2f} + {kv:.2f}v')
    ax1.set_title('Voltage vs. Velocity')
    ax1.set_xlabel('Velocity (unit/s)')
    ax1.set_ylabel('Voltage (V)')
    ax1.grid(True, linestyle='--', alpha=0.6)
    ax1.legend()

    # 2. Voltage over Time (Actual vs. Predicted)
    ax2.plot(times, voltages, color='blue', label='Actual Voltage', linewidth=2)
    ax2.plot(times, predicted_voltages, color='red', linestyle='--', label='Predicted Voltage', linewidth=2)
    ax2.set_title('Voltage Profile Over Time')
    ax2.set_xlabel('Time (s)')
    ax2.set_ylabel('Voltage (V)')
    ax2.grid(True, linestyle='--', alpha=0.6)
    ax2.legend()

    # 3. Residuals over Time (Error checking)
    ax3.plot(times, residuals, color='purple', linewidth=1.5)
    ax3.axhline(0, color='black', linestyle='--', linewidth=1)
    ax3.set_title('Model Residuals (Actual - Predicted)')
    ax3.set_xlabel('Time (s)')
    ax3.set_ylabel('Voltage Error (V)')
    ax3.grid(True, linestyle='--', alpha=0.6)

    plt.tight_layout()
    plt.show()

def test_quas():
    data = make_const_accel_data(0,0,0.001, 5)
    data += make_const_accel_data(data[-1].position, data[-1].velocity, -0.001, 5)
    solver = QuasistaticSolver()
    solver.init_data(data)
    gains = solver.solve()
    if gains:
        plot_quasistatic_identification(data, gains.ks, gains.kv)