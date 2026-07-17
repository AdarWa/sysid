def signum(x):
    if x > 0:
        return 1
    elif x < 0:
        return -1
    return 0

def calculate_feedforward(ks, kv, ka, kg, v, a):
    return ks * signum(v) + kv * v + ka * a + kg