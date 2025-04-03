import math

def calculate_stats(numbers):
    if not numbers: return None, None
    
    n = len(numbers)
    avg = sum(numbers) / n
    variance = sum((x - avg) ** 2 for x in numbers) / n
    std_dev = math.sqrt(variance)

    return avg, std_dev

numbers = [0.251, 1.534, 1.247, 0.470, 3.165, 0.603, 1.867, 0.318, 0.722, 0.959, 0.879]
avg, std_dev = calculate_stats(numbers)
print(f"Durchschnitt: {avg:.3f}, Standardabweichung: {std_dev:.3f}")
