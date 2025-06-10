# Symulator oddziaływania grawitacyjnego między kilkoma ciałami
Zbigniew Sztanga, 2025

## Kompilacja
Wymagane biblioteki to glew i opengl.
Kopilacja na Linuxach:
```
mkdir build
cd build
cmake ..
make
```
na windowsie: może być ciężko.

## Uruchomienie
```
./n-body_sim <konfiguracja>.json
```

## Użyte biblioteki
GUI: Dear ImGui i ImPlot3D

Json: nlohmann/json