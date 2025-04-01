import math

def calculate_stats(numbers):
    if not numbers: return None, None
    
    n = len(numbers)
    avg = sum(numbers) / n
    variance = sum((x - avg) ** 2 for x in numbers) / n
    std_dev = math.sqrt(variance)

    return avg, std_dev

numbers = [3.431, 0.436, 1.396, 1.852, 3.677, 0.747, 1.777, 2.319, 1.783, 0.981]
avg, std_dev = calculate_stats(numbers)
print(f"Durchschnitt: {avg:.3f}, Standardabweichung: {std_dev:.3f}")
