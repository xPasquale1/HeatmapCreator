import os
import json
import numpy as np
import matplotlib.pyplot as plt
from collections import Counter
import tkinter as tk
from tkinter import filedialog


def plot_from_file(filepath):
    """Lädt JSON-Daten und erstellt den Plot."""
    try:
        with open(filepath, "r") as f:
            data = json.load(f)
    except Exception as e:
        print(f"Fehler beim Laden der Datei: {e}")
        return

    colors = ["red", "blue", "green"]
    fig, ax = plt.subplots(figsize=(10, 6))
    filename = os.path.basename(filepath)
    fig.canvas.manager.set_window_title(filename)

    bar_width = 0.3
    offset = 0
    peak_x = None
    peak_y = -1

    for i, (key, values) in enumerate(data.items()):
        if not values:
            print(f"⚠️  {key} ist leer. Überspringe...")
            continue

        counts = Counter(values)
        x_values = sorted(counts.keys())
        y_values = [counts[x] for x in x_values]

        avg = np.mean(values)
        std_dev = np.std(values)

        plt.bar(
            [x + offset for x in x_values],
            y_values,
            width=bar_width,
            color=colors[i % len(colors)],
            label=f"{key} (AVG: {avg:.2f}, STD: {std_dev:.2f})",
            alpha=0.7,
        )

        plt.axvline(
            avg,
            color=colors[i % len(colors)],
            linestyle="--",
            linewidth=1,
            label=f"{key} AVG",
        )

        for x, y in zip(x_values, y_values):
            if y > peak_y:
                peak_x = x
                peak_y = y

        offset += bar_width

    plt.xlabel("Signalstärke in dB")
    plt.ylabel("Anzahl der Messungen")
    plt.title("Häufigkeitsverteilung der Heatmaps")
    plt.legend()
    plt.grid(axis="y", linestyle="--", alpha=0.6)

    plt.xlim(-10, -110)

    ax = plt.gca()
    xticks = list(ax.get_xticks())

    if peak_x is not None and peak_x not in xticks:
        xticks.append(peak_x)
        xticks.sort()
        ax.set_xticks(xticks)

    for tick in ax.get_xticklabels():
        if tick.get_text() == str(peak_x):
            tick.set_color("green")

    plt.show(block=False)


def open_file():
    """Öffnet eine Datei-Auswahlbox und plottet die ausgewählten Dateien."""
    file_paths = filedialog.askopenfilenames(filetypes=[("Heatmap-Daten", "*.data")])
    if file_paths:
        for file_path in file_paths:
            plot_from_file(file_path)


root = tk.Tk()
root.title("Datei auswählen")
root.geometry("300x150")

btn = tk.Button(root, text="JSON-Datei öffnen", command=open_file, font=("Arial", 12))
btn.pack(pady=30)

root.mainloop()
