import math

def calculate_stats(numbers):
    if not numbers: return None, None
    
    n = len(numbers)
    avg = sum(numbers) / n
    variance = sum((x - avg) ** 2 for x in numbers) / n
    std_dev = math.sqrt(variance)

    return avg, std_dev

numbers = [1.160, 0.696, 0.357, 0.299, 0.338, 2.224, 1.537, 0.676, 1.711, 1.286]
avg, std_dev = calculate_stats(numbers)
print(f"Durchschnitt: {avg:.3f}, Standardabweichung: {std_dev:.3f}")
