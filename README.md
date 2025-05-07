# HeatmapCreator
Dieses Programm nutzt WLAN-Daten, um einen Esp32 mit dem Program https://github.com/xPasquale1/DeviceLocatorEsp32 im Innenraum zu lokalisieren.
Das Programm wurde in C++ mit der Win-API und OpenGL geschrieben. Das Programm speichert Signalstärken in Raum, um eine Position des Esp32 zu bestimmen.


## Installation

Ein C++ Kompiler für Windows. Das Programm wurde nur mit der GNU Compiler Collection getestet.
Folgende Kompilerargumente werden zusätzlich benötigt:
```
-lgdi32
-lopengl32
-lWs2_32
-lComdlg32
```

Das Programm benötigt zusätzlich den Code des Programms https://github.com/xPasquale1/OpenGL-Library.
