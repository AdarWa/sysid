import numpy as np
from matplotlib import pyplot as plt

from core.feedback.solvers.cost_feedback_solver import CostFeedbackSolver, simulate_closed_loop
from core.sample.logged_sample import LoggedSample


def test_cost_feedback():
    dt = 0.02
    T_total = 12.0
    N_samples = int(T_total / dt)
    t_data = np.linspace(0, T_total, N_samples)

    dummy_data = [LoggedSample(0.0, 0.0, 0.0, 0.0) for t in t_data]

    # 2. Define FOPDT parameters
    K = 3.4936
    tau = 1.1752
    theta = 0.4132

    # 3. Initialize solver and fit gains
    solver = CostFeedbackSolver(dummy_data, dt=dt, wc=1.0, K=K, tau=tau, theta=theta)
    optimal_gains = solver.fit()

    print(f"Optimization Successful!")
    print(f"Optimal Kp: {optimal_gains[0]:.4f}")
    print(f"Optimal Ki: {optimal_gains[1]:.4f}")
    print(f"Optimal Kd: {optimal_gains[2]:.4f}")

    # 4. Simulate closed-loop response using optimal gains
    y_opt, u_opt, e_opt = simulate_closed_loop(optimal_gains, K, tau, theta, dt, N_samples)

    # 5. Generate Graphs
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

    # Top Plot: Process Variable vs Setpoint
    ax1.plot(t_data, np.ones(N_samples), 'r--', label='Setpoint (SP = 1.0)', alpha=0.7)
    ax1.plot(t_data, y_opt, 'b-', label='Process Variable (PV)', linewidth=2)
    ax1.set_ylabel('Process Output')
    ax1.set_title(f'FOPDT Closed-Loop Step Response (K={K}, tau={tau}, theta={theta})')
    ax1.grid(True, linestyle=':', alpha=0.6)
    ax1.legend(loc='lower right')

    # Bottom Plot: Control Signal u(t)
    ax2.plot(t_data, u_opt, 'g-', label='Control Action u(t)', linewidth=2)
    ax2.set_xlabel('Time (seconds)')
    ax2.set_ylabel('Controller Output')
    ax2.set_title('Control Effort and Aggression')
    ax2.grid(True, linestyle=':', alpha=0.6)
    ax2.legend(loc='upper right')

    plt.tight_layout()
    plt.show()