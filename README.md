# Open Mars

> A pixel-art isometric colony survival simulator set on the red planet. Inspirated by games like OpenTTD and Surviving Mars. Relase version soon!

---

## Overview

**Open Mars** is an isometric colony-building game where you establish and grow a self-sustaining human settlement on Mars. Manage energy production, water extraction, and resource logistics across procedurally generated Martian terrain — all rendered in a retro pixel-art isometric style.

The game is currently in **alpha** (`v1.0.0-alpha.1`).

---

## Screenshots

| | |
|---|---|
| ![Main Menu](https://github.com/user-attachments/assets/5544c09a-b3d6-4477-89fe-4509df7e390b) | ![Terrain Generation](https://github.com/user-attachments/assets/16a77218-b878-462a-8834-1cb106a41d3d) |
| *Main menu with animated Martian sky* |  *Custom world generation engine* |
| ![Colony Building](https://github.com/user-attachments/assets/d7fb5589-9a97-4a46-ac66-584d543f05c1) | ![Pipe Networks](https://github.com/user-attachments/assets/eb7be4e6-adb2-45c5-be0b-be70f17f1ea9) |
| *Colony with solar panels and pipe infrastructure* |  *Procedurally generated terrain with ice deposits* |


---
## Features

### World Generation
- Procedural terrain using **Perlin noise with domain warping**
- Configurable roughness, elevation, and warp strength
- Ice deposit generation in natural basins
- Three map sizes: Small (64×64), Medium (128×128), Large (192×192)
- Seed-based generation for reproducible worlds

### Buildings
| Structure | Function |
|---|---|
| **Solar Panel** | Produces energy; output scales with terrain elevation |
| **Pipe** | Routes materials between producers and consumers |
| **Ice Melter** | Consumes energy to melt ice into water |
| **Water Magazine** | Stores and buffers water supply |

### Pipe Networks
- Automatic network detection and merging on placement
- Material-type propagation (energy vs. water — no cross-contamination)
- Pipe routing across flat terrain and four valid slope orientations
- Real-time resource transfer between producers and consumers

### Simulation
- Tick-based resource production and consumption
- Inventory system with per-material capacity limits
- Adjustable game speed
- In-game calendar (day / month / year)

### Rendering
- Isometric projection with height-aware tile picking
- Painter's algorithm render ordering for correct depth
- Slope-aware tile and pipe texture selection via 4-bit mask atlas lookup
- Ice tile variant rendering
- Custom pixel-art cursor

### Save / Load
- Binary save format with full world state serialization
- Slot-based save system

---

## Controls

| Input | Action |
|---|---|
| `W / A / S / D` | Pan camera |
| `Mouse wheel` | Zoom (anchored to cursor) |
| `Right / Middle mouse button` | Drag-pan camera |
| `Left click` | Place / demolish structure |
| `ImGui toolbar` | Select tool and building type |

---

## Building

### Requirements

- C++17 compiler (MSVC / GCC / Clang)
- [raylib](https://www.raylib.com/)
- [Dear ImGui](https://github.com/ocornut/imgui) + `rlImGui` binding
- CMake 3.16+

### Build steps

```bash
git clone https://github.com/AdamCofala/Open-Mars-Colonization-Simulator.git
cd open-mars
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The binary expects a `resources/` directory alongside the executable containing the texture atlases:

```
resources/
├── tile_atlas.png
├── pipe_atlas.png
├── ice_atlas.png
├── cursor.png
├── solar_panels.png
├── ice_melter.png
├── pipe.png
└── water_magazine.png
```

---

## Architecture

```
src/
├── core/           # Game loop and state machine (main.cpp)
├── entities/       # Structure, Pipe, PipeNetwork, Inventory, Enums
├── structures/     # Concrete buildings (SolarPanel, IceMelter, WaterMagazine)
├── world/          # Map, Tile, World, WorldGenSettings
├── rendering/      # Renderer, GameCamera, TextureManager
├── player/         # InputManager, Gui, StartScreen
└── utils/          # PerlinNoise, Math, SaveSystem
```

Key design points:

- **`Map::rebuildNetworks()`** — full network graph reconstruction on every structural change; handles material-type propagation in a fixed-point loop before connection mask calculation.
- **`PipeNetwork::updateNetwork()`** — proportional resource distribution across all producers and consumers each tick.
- **Texture atlas** — slope and pipe variants indexed by a 4-bit binary mask derived from corner height deltas or neighbor connectivity.

---

## Roadmap

- [ ] Native file dialog for save/load
- [ ] Additional resource types and buildings
- [ ] Music and sound
- [ ] Colonist simulation
- [ ] Dust storms and environmental events
- [ ] Research tree
- [ ] Multiplayer (long-term)

---

## License

This project is open source and released under the [MIT License](LICENSE).

---

## Special Thanks
- Jakub Sałata @JLetuce
- Jędrzej Tymiński @Kayla137

---

*Built with [raylib](https://www.raylib.com/) and [Dear ImGui](https://github.com/ocornut/imgui).*
