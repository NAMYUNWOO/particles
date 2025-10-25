# Particle Storm

A physics-based particle simulation game built with Raylib, featuring 100,000 interactive particles and progressive gameplay across 10 challenging stages.

## Features

### Core Gameplay
- **100,000 Real-time Particles**: Physics-based particle simulation with attraction mechanics
- **10 Progressive Stages**: Each stage introduces new enemy types and challenges
- **Event-Driven Architecture**: Decoupled component system using publish-subscribe pattern
- **Memory Pool Optimization**: High-performance memory management for smooth gameplay.

### Game Mechanics
- **Particle Attraction System**: Player can attract and control massive particle swarms
- **Boost System**: Dual-gauge system for enhanced particle attraction and movement speed
- **Dynamic Combat**: Use particles as weapons to defeat various enemy types
- **Boss Battles**: Multi-phase boss encounters at stages 6 and 10

### Enemy Variety
- **16 Unique Enemy Types**: Each with distinct behaviors and visual styles
  - Basic, Tracker, Speedy, Splitter, Orbiter
  - Teleporter, Repulsor, Cluster, Blackhole
  - Shield Generator, Bomber, Snake (Head/Segment)
  - Boss enemies including Nightmare Lord and Final Boss

### Visual Effects
- **Explosion System**: Dynamic particle-based explosion effects
- **Color-coded Feedback**: Visual indicators for enemy health, damage, and special states
- **Screen Effects**: Impact feedback and stage transition effects

### Technical Features
- **Efficient Collision Detection**: Batch processing and spatial optimization
- **Scalable Event System**: Easily extensible architecture for new features
- **Performance Monitoring**: Built-in debugging and performance tracking tools
- **Save System**: Automatic score tracking and leaderboard

## Dependencies

### Required
- **Operating System**: macOS, Windows, or Linux
- **Raylib**: Version 5.5 or higher
- **Compiler**: GCC 9.0+ or compatible C compiler
- **Build System**: GNU Make

### Development Tools
- **Git**: For version control
- **Text Editor/IDE**: Any C-compatible editor

## Setup Steps

### 1. Install Raylib

#### macOS (using Homebrew)
```bash
brew install raylib
```

#### macOS (from source)
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make
sudo make install
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make
sudo make install
```

#### Windows
- Download pre-compiled binaries from [Raylib releases](https://github.com/raysan5/raylib/releases)
- Or use MinGW:
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
mingw32-make
```

### 2. Clone the Repository
```bash
git clone https://github.com/NAMYUNWOO/particles.git
cd particles
```

### 3. Build the Project
```bash
# Clean any previous builds
make clean

# Build the project
make

# Run the game
make run

# Or do everything in one command
make clean && make && make run
```

### 4. Verify Installation
If the build is successful, you should see:
- `bin/game` executable created
- No compilation errors
- Game window opens when running `make run`

## Troubleshooting

### Common Build Issues

#### "raylib.h not found"
- Ensure Raylib is properly installed
- Check include paths in Makefile
- On macOS with Homebrew: `export CPATH=/opt/homebrew/include:$CPATH`

#### Linking errors
- Verify Raylib libraries are in the system path
- Check library paths in Makefile
- On Linux, you may need to run: `sudo ldconfig`

#### Performance issues
- Ensure you're building in release mode
- Check that your system meets minimum requirements
- Try reducing particle count in `src/entities/particle.h` if needed

## Quick Start

After successful setup:
1. Run the game with `make run` or `./bin/game`
2. Use WASD or arrow keys to move
3. Hold Space to boost particle attraction
4. Hold Left Shift to boost movement speed
5. Survive through all 10 stages!

## Web Build (Play in Browser)

Particle Storm can be compiled to WebAssembly and played in your web browser!

**Quick Start:**
```bash
# 1. Install Emscripten
brew install emscripten  # macOS

# 2. Download Raylib for WebAssembly
wget https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_webassembly.zip
unzip raylib-5.0_webassembly.zip -d raylib-wasm

# 3. Build for web
make -f Makefile.web

# 4. Run local server
make -f Makefile.web serve

# 5. Open http://localhost:8000/particle_storm.html
```

**Full Instructions**: See [WEB_BUILD_GUIDE.md](WEB_BUILD_GUIDE.md) for complete setup and deployment guide.

**Performance Note**: Web version runs at ~50-60% native speed. Recommended to reduce particle count to 50,000 for smooth 60 FPS gameplay in browser.

## License

This project is distributed under the MIT License. See LICENSE file for details.