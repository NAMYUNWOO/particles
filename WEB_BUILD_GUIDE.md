# Web Build Guide - Particle Storm

This guide explains how to build and play Particle Storm in your web browser using WebAssembly.

## Prerequisites

### 1. Install Emscripten SDK

**macOS:**
```bash
brew install emscripten
```

**Windows:**
```bash
# Download and install from: https://emscripten.org/docs/getting_started/downloads.html
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
emsdk install latest
emsdk activate latest
```

**Linux:**
```bash
# Clone emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# Install and activate
./emsdk install latest
./emsdk activate latest

# Add to PATH (add to ~/.bashrc or ~/.zshrc)
source ./emsdk_env.sh
```

### 2. Download Raylib for WebAssembly

```bash
# In the particles project directory
wget https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_webassembly.zip
unzip raylib-5.0_webassembly.zip -d raylib-wasm
```

**Or manually:**
1. Download from: https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_webassembly.zip
2. Extract to `particles/raylib-wasm/`

### 3. Verify Setup

```bash
# Check emscripten is installed
emcc --version

# Should output something like:
# emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 3.1.x
```

## Building for Web

### Option 1: Using Makefile.web

```bash
# Build
make -f Makefile.web

# Run local web server
make -f Makefile.web serve

# Then open in your browser:
# http://localhost:8000/particle_storm.html
```

### Option 2: Manual Build

```bash
# Create output directory
mkdir -p build/web

# Compile with emcc
emcc -Wall -std=c99 -D_DEFAULT_SOURCE -DPLATFORM_WEB \
  -Iraylib-wasm/include -I./src \
  -o build/web/particle_storm.html \
  src/main.c \
  src/core/game.c \
  src/core/physics.c \
  src/core/input_handler.c \
  src/core/memory_pool.c \
  src/core/event/event_system.c \
  src/entities/player.c \
  src/entities/particle.c \
  src/entities/enemy.c \
  src/entities/explosion.c \
  src/entities/items/hp_potion.c \
  src/entities/managers/enemy_manager.c \
  src/entities/managers/particle_manager.c \
  src/entities/managers/stage_manager.c \
  src/entities/managers/item_manager.c \
  -Lraylib-wasm/lib -lraylib \
  -s USE_GLFW=3 \
  -s ASYNCIFY \
  -s WASM=1 \
  -s GL_ENABLE_GET_PROC_ADDRESS=1 \
  -s ASSERTIONS=1 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=67108864 \
  --shell-file raylib-wasm/shell.html

# Start web server
cd build/web
python3 -m http.server 8000

# Open http://localhost:8000/particle_storm.html
```

## Running the Web Build

### Start Local Server

You **must** use a web server (not just open the HTML file) due to WebAssembly security restrictions.

**Python 3:**
```bash
cd build/web
python3 -m http.server 8000
```

**Python 2:**
```bash
cd build/web
python -m SimpleHTTPServer 8000
```

**Node.js (http-server):**
```bash
npm install -g http-server
cd build/web
http-server -p 8000
```

**PHP:**
```bash
cd build/web
php -S localhost:8000
```

### Open in Browser

Navigate to: http://localhost:8000/particle_storm.html

## Troubleshooting

### Issue: "emcc: command not found"

**Solution**: Emscripten not in PATH

```bash
# macOS/Linux
source /path/to/emsdk/emsdk_env.sh

# Or add to ~/.bashrc or ~/.zshrc permanently
echo 'source /path/to/emsdk/emsdk_env.sh' >> ~/.zshrc
```

### Issue: "raylib.h: No such file or directory"

**Solution**: Raylib WebAssembly not downloaded

```bash
# Download raylib for WebAssembly
wget https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_webassembly.zip
unzip raylib-5.0_webassembly.zip -d raylib-wasm
```

### Issue: "Failed to load .wasm file" in browser

**Solution**: Not using a web server

- Don't open HTML file directly (file:// URLs don't work)
- Must use http://localhost with a web server

### Issue: Black screen or game not rendering

**Solution**: Check browser console for errors

- Press F12 to open Developer Tools
- Check Console tab for errors
- Verify WebGL is supported: https://get.webgl.org/

### Issue: Performance problems (low FPS)

**Optimization Tips**:

1. **Reduce particle count** in `src/entities/particle.h`:
   ```c
   // Reduce from 100,000 to 50,000 for web
   #define PARTICLE_COUNT 50000
   ```

2. **Disable some visual effects** in web build

3. **Use Chrome/Edge** (better WebAssembly performance than Firefox/Safari)

## Deployment to Web Hosting

### Option 1: GitHub Pages

```bash
# 1. Create gh-pages branch
git checkout -b gh-pages

# 2. Copy web build to root
cp -r build/web/* .

# 3. Commit and push
git add .
git commit -m "Deploy web build to GitHub Pages"
git push origin gh-pages

# 4. Enable GitHub Pages in repo settings
# Settings → Pages → Source: gh-pages branch
```

Your game will be available at: `https://NAMYUNWOO.github.io/particles/particle_storm.html`

### Option 2: Netlify

1. Drag and drop `build/web/` folder to https://app.netlify.com/drop
2. Your game gets a URL like: `https://random-name.netlify.app/particle_storm.html`

### Option 3: itch.io

1. Zip the `build/web/` contents
2. Upload to https://itch.io/game/new
3. Set "Kind of project" to "HTML"
4. Check "This file will be played in the browser"

## Performance Comparison

| Platform | Expected FPS | Particle Count | Notes |
|----------|-------------|----------------|-------|
| **Desktop (native)** | 60 FPS | 100,000 | Full performance |
| **Web (Chrome)** | 45-60 FPS | 50,000 | Recommended for web |
| **Web (Firefox)** | 30-45 FPS | 30,000 | Slower WebAssembly |
| **Web (Safari)** | 30-40 FPS | 30,000 | Slower WebGL |
| **Mobile browser** | 20-30 FPS | 10,000 | Limited performance |

## Controls (Web Version)

**Same as desktop:**
- **WASD / Arrow Keys**: Move player
- **Space**: Hold to boost particle attraction
- **Left Shift**: Hold to boost movement speed
- **Enter**: Progress through menus/stages
- **ESC**: Pause game
- **F1-F5**: Debug keys (if enabled)

## Known Web Limitations

1. **No file system access**: Scoreboard won't persist between sessions
   - Solution: Use localStorage (requires code modification)

2. **Sound may require user interaction**: First click enables audio
   - Browser security policy

3. **Lower performance**: ~50-60% of native speed
   - Reduce particle count for smooth gameplay

4. **Memory limits**: Browser tabs have memory restrictions
   - 67MB allocated (should be sufficient)

## Building for Production

For production deployment, optimize the build:

```bash
emcc -O3 \  # Enable optimization
  -DNDEBUG \  # Disable debug code
  -s TOTAL_MEMORY=33554432 \  # Reduce memory if possible
  ... (other flags)
```

Add `-O3` for maximum optimization (slower compile, faster runtime).

## Resources

- **Raylib Web Wiki**: https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)
- **Emscripten Docs**: https://emscripten.org/docs/getting_started/index.html
- **WebAssembly Debugging**: https://developer.chrome.com/blog/wasm-debugging-2020/
- **Raylib Examples (Web)**: https://www.raylib.com/examples.html

## Next Steps

After building successfully:

1. **Test in different browsers** (Chrome, Firefox, Safari, Edge)
2. **Optimize particle count** for web performance
3. **Add loading screen** (web builds take time to load)
4. **Implement localStorage** for persistent scores
5. **Deploy to GitHub Pages** or itch.io for public play

---

**Last Updated**: 2025-10-21
**Raylib Version**: 5.0+
**Emscripten Version**: 3.1.x+
