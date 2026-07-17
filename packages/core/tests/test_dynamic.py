import numpy as np
from matplotlib import pyplot as plt

from core.data.data_generator import make_dynamic_step_data
from core.feedforward.dynamic_solver import DynamicSolver
from core.sample.logged_sample import LoggedSample


def plot_dynamic_identification(data: list[LoggedSample], ks: float, kv: float, ka: float, kg: float):
    times = np.array([i * 0.005 for i in range(len(data))])
    velocities = np.array([s.velocity for s in data])
    accelerations = np.array([s.acceleration for s in data])
    voltages = np.array([s.voltage for s in data])

    v_signs = np.sign(velocities)
    v_signs[velocities == 0] = 1.0

    # Isolate the voltage responsible purely for acceleration
    quasistatic_voltage = (ks * v_signs) + (kv * velocities) + kg
    accel_voltages = voltages - quasistatic_voltage
    predicted_accel_voltages = ka * accelerations

    # Total model predictions and residuals
    total_predicted_voltages = quasistatic_voltage + predicted_accel_voltages
    residuals = voltages - total_predicted_voltages

    fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(16, 5))
    fig.suptitle('Dynamic Acceleration System Identification Diagnostics', fontsize=14, fontweight='bold')

    # 1. Acceleration Voltage vs. Acceleration (Linear regression for Ka)
    ax1.scatter(accelerations, accel_voltages, color='green', alpha=0.3, label='Isolated Telemetry (V - Ks - Kvv)',
                s=10)
    ax1.plot(accelerations, predicted_accel_voltages, color='red', linewidth=2, label=f'Model: V_accel = {ka:.4f}a')
    ax1.set_title('Acceleration Voltage vs. Acceleration')
    ax1.set_xlabel('Acceleration (unit/s²)')
    ax1.set_ylabel('Acceleration Voltage (V)')
    ax1.grid(True, linestyle='--', alpha=0.6)
    ax1.legend()

    # 2. Total Voltage over Time (Actual vs. Predicted)
    ax2.plot(times, voltages, color='green', label='Actual Battery Voltage', linewidth=2)
    ax2.plot(times, total_predicted_voltages, color='red', linestyle='--', label='Predicted Total Voltage', linewidth=2)
    ax2.set_title('Voltage Profile Over Time')
    ax2.set_xlabel('Time (s)')
    ax2.set_ylabel('Voltage (V)')
    ax2.grid(True, linestyle='--', alpha=0.6)
    ax2.legend()

    # 3. Residuals over Time
    ax3.plot(times, residuals, color='purple', linewidth=1.5)
    ax3.axhline(0, color='black', linestyle='--', linewidth=1)
    ax3.set_title('Model Residuals (Actual - Predicted)')
    ax3.set_xlabel('Time (s)')
    ax3.set_ylabel('Voltage Error (V)')
    ax3.grid(True, linestyle='--', alpha=0.6)

    plt.tight_layout()
    plt.show()

def test_dynamic():
    data = make_dynamic_step_data(5)
    solver = DynamicSolver(0.1, 10, 0.05)
    solver.init_data(data)
    gains = solver.solve()
    if gains:
        plot_dynamic_identification(data, gains.ks, gains.kv, gains.ka, gains.kg)