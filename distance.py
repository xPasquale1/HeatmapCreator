import math

def calculate_stats(numbers):
    if not numbers: return None, None
    
    n = len(numbers)
    avg = sum(numbers) / n
    variance = sum((x - avg) ** 2 for x in numbers) / n
    std_dev = math.sqrt(variance)

    return avg, std_dev

numbers = [1.557, 3.746, 2.899, 4.389, 2.823, 1.880, 2.694, 0.444, 1.136, 2.677, 2.256]
avg, std_dev = calculate_stats(numbers)
print(f"Durchschnitt: {avg:.3f}, Standardabweichung: {std_dev:.3f}")
