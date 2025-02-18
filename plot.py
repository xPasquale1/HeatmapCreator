import sys
import json
import numpy as np
import matplotlib.pyplot as plt
from collections import Counter

if len(sys.argv) < 2:
    print("Kein Dateiname angegeben")
    quit()

with open(sys.argv[1], "r") as f:
    data = json.load(f)

colors = ["red", "blue", "green"]
plt.figure(figsize=(10, 6))

bar_width = 0.3
offset = 0

for i, (key, values) in enumerate(data.items()):
    if not values:
        print(f"⚠️ {key} ist leer. Überspringe...")
        continue

    counts = Counter(values)
    x_values = sorted(counts.keys())
    y_values = [counts[x] for x in x_values]

    avg = np.mean(values)
    std_dev = np.std(values)

    plt.bar([x + offset for x in x_values], y_values, width=bar_width, color=colors[i], label=f"{key} (AVG: {avg:.2f}, STD: {std_dev:.2f})", alpha=0.7)

    plt.axvline(avg, color=colors[i], linestyle="--", linewidth=1, label=f"{key} AVG")

    for x, y in zip(x_values, y_values):
        plt.text(x + offset, y + 0.5, str(y), ha="center", fontsize=9)

    offset += bar_width

plt.xlabel("Zahl im Array")
plt.ylabel("Anzahl (Häufigkeit)")
plt.title("Häufigkeitsverteilung der Heatmaps")
plt.legend()
plt.grid(axis="y", linestyle="--", alpha=0.6)
plt.show()
