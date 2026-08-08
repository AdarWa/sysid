from .helpers.Logger import Logger
from pykit.wpilog.wpilogwriter import WPILOGWriter
import random
import time

def run_flywheel_simulation(duration_seconds: float):
    dt = 0.02
    total_steps = int(duration_seconds / dt)

    # Motor and system constants
    # kv: Theoretical max velocity per volt (rad/s per V)
    # time_constant: Seconds required to reach ~63.2% of steady-state velocity
    kv = 40.0
    time_constant = 0.25

    current_velocity = 0.0

    for current_step in range(total_steps):
        time_elapsed = current_step * dt

        # Apply 12V power after 1 second of idle time
        input_voltage = 12.0 if time_elapsed >= 1.0 else 0.0

        # Calculate ideal physical states using a first-order model
        target_velocity = input_voltage * kv
        true_acceleration = (target_velocity - current_velocity) / time_constant
        current_velocity += true_acceleration * dt

        # Inject Gaussian noise to simulate electrical variance and sensor inaccuracy
        noisy_voltage = input_voltage + random.gauss(0, 0.15)
        noisy_velocity = current_velocity + random.gauss(0, 1.5)
        noisy_acceleration = true_acceleration + random.gauss(0, 4.0)

        Logger.recordOutput("Flywheel/Voltage", noisy_voltage)
        Logger.recordOutput("Flywheel/Velocity", noisy_velocity)
        Logger.recordOutput("Flywheel/Acceleration", noisy_acceleration)

        Logger.step()

        # Maintain a 20ms real-time loop execution
        time.sleep(dt)

if __name__ == "__main__":
    Logger.addDataReciever(WPILOGWriter())
    Logger.start()
    run_flywheel_simulation(5.0)
    Logger.end()