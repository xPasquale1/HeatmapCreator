import math

def calculate_stats(numbers):
    if not numbers: return None, None
    
    n = len(numbers)
    avg = sum(numbers) / n
    variance = sum((x - avg) ** 2 for x in numbers) / n
    std_dev = math.sqrt(variance)

    return avg, std_dev

numbers = [0.945, 1.634, 1.084, 3.145, 1.479, 2.642, 1.151, 4.027]
avg, std_dev = calculate_stats(numbers)
print(f"Durchschnitt: {avg:.3f}, Standardabweichung: {std_dev:.3f}")
