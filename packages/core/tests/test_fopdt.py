import matplotlib.pyplot as plt
import numpy as np

from core.feedback.fopdt import fopdt_dynamics, FOPDTSolver
from core.sample.logged_sample import LoggedSample


def test_fopdt():
    dt = 0.05
    time_horizon = np.arange(0, 10, dt)
    true_velocity = fopdt_dynamics(time_horizon, K=3.5, tau=1.2, theta=0.4) # TODO: iterate
    noisy_velocity = true_velocity + np.random.normal(0.0, 0.05, len(time_horizon))

    sample_log = [
        LoggedSample(voltage=1.0, velocity=vel, position=0, acceleration=0)
        for vel in noisy_velocity
    ]

    solver = FOPDTSolver(sample_log, dt)
    fit_result = solver.fit()

    # Accommodate solvers returning either a SciPy OptimizeResult or a raw tuple
    if hasattr(fit_result, "x"):
        K_fit, tau_fit, theta_fit = fit_result.x
    else:
        K_fit, tau_fit, theta_fit = fit_result

    fitted_velocity = fopdt_dynamics(time_horizon, K=K_fit, tau=tau_fit, theta=theta_fit)

    fig, ax = plt.subplots(figsize=(10, 6))

    # Scatter plot with reduced opacity prevents dense sample points from hiding the trend lines
    ax.scatter(
        time_horizon,
        noisy_velocity,
        color="gray",
        alpha=0.5,
        label="Noisy Measurements",
        s=15
    )

    ax.plot(
        time_horizon,
        true_velocity,
        color="black",
        linestyle="--",
        label="True Model (K=3.50, τ=1.20, θ=0.40)",
        linewidth=1.5
    )

    ax.plot(
        time_horizon,
        fitted_velocity,
        color="#1f77b4",
        label=f"Fitted Model (K={K_fit:.2f}, τ={tau_fit:.2f}, θ={theta_fit:.2f})",
        linewidth=2.0
    )

    ax.set_title("FOPDT Parameter Estimation Results")
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Velocity")
    ax.grid(True, linestyle=":", alpha=0.6)
    ax.legend(loc="lower right")

    plt.tight_layout()
    plt.show()
