import os
import json


def invert_numbers(data):
    """Rekursive Funktion, um alle Zahlen in JSON-Daten negativ zu machen."""
    if isinstance(data, list):
        return [invert_numbers(item) for item in data]
    elif isinstance(data, dict):
        return {key: invert_numbers(value) for key, value in data.items()}
    elif isinstance(data, (int, float)):
        return -abs(data)  # Zahlen immer negativ machen
    return data


def custom_json_dump(data, file):
    """Individuelle Funktion zum Speichern von JSON mit einer speziellen Formatierung für Arrays."""
    if isinstance(data, dict):
        file.write("{\n")
        for i, (key, value) in enumerate(data.items()):
            file.write(f'    "{key}": ')
            custom_json_dump(value, file)
            if i < len(data) - 1:
                file.write(",\n")
            else:
                file.write("\n")
        file.write("}")
    elif isinstance(data, list):
        file.write("[")
        for i, item in enumerate(data):
            custom_json_dump(item, file)
            if i < len(data) - 1:
                file.write(", ")
        file.write("]")
    else:
        json.dump(data, file, separators=(",", ": "), ensure_ascii=False)


def process_json_files(folder_path):
    """Alle JSON-Dateien im Ordner verarbeiten."""
    for filename in os.listdir(folder_path):
        if filename.endswith(".data"):
            file_path = os.path.join(folder_path, filename)

            # JSON-Datei einlesen
            with open(file_path, "r", encoding="utf-8") as file:
                try:
                    data = json.load(file)
                except json.JSONDecodeError:
                    print(f"Fehler beim Lesen von {filename}, wird übersprungen.")
                    continue

            # Zahlen invertieren
            modified_data = invert_numbers(data)

            # Datei überschreiben mit benutzerdefinierter JSON-Ausgabe
            with open(file_path, "w", encoding="utf-8") as file:
                custom_json_dump(modified_data, file)

            print(f"Bearbeitet: {filename}")


# Pfad zum Ordner mit den JSON-Dateien anpassen!
ordner_pfad = "output/Rotationstests/Ohne-Antenne"
process_json_files(ordner_pfad)
