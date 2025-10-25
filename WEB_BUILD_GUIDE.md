# Web Build Guide - Particle Storm

This guide explains how the WebAssembly version of Particle Storm is built and deployed using GitHub Actions.

## 🎮 Stage Testing on GitHub Pages

The web version now supports **stage selection** via dropdown menu!

Visit: **https://namyunwoo.github.io/particles/**

### How to Test Stages

1. Open the web version in your browser
2. Use the **"Select Stage"** dropdown to choose:
   - **Full Game** - Play all 10 stages from the beginning
   - **Stage 1-10** - Jump directly to any specific stage

3. The page will reload and start at your selected stage

### URL Parameters

You can also link directly to a specific stage:
- Full game: `https://namyunwoo.github.io/particles/`
- Stage 3: `https://namyunwoo.github.io/particles/?stage=3`
- Stage 6 (Boss): `https://namyunwoo.github.io/particles/?stage=6`
- Stage 10 (Final Boss): `https://namyunwoo.github.io/particles/?stage=10`

## 🚀 Automatic Deployment (Recommended)

The project uses **GitHub Actions** for automatic CI/CD deployment to GitHub Pages.

### How It Works

1. **Push to main branch**:
   ```bash
   git push origin main
   ```

2. **GitHub Actions automatically**:
   - Installs Emscripten SDK
   - Extracts Raylib WebAssembly library
   - Builds optimized WASM version with all 10 stages
   - Deploys to GitHub Pages

3. **Test the game**:
   - Visit: https://namyunwoo.github.io/particles/
   - Select any stage from the dropdown

### Viewing Build Status

- Check workflow runs: https://github.com/NAMYUNWOO/particles/actions
- Green checkmark = successful deployment
- Red X = build failed (check logs)

### Manual Trigger

You can manually trigger a deployment from GitHub:
1. Go to **Actions** tab
2. Select **"Deploy to GitHub Pages"** workflow
3. Click **"Run workflow"**

## 🛠️ Local Development Build

For testing locally before pushing:

### Prerequisites

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
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh  # Add to ~/.bashrc or ~/.zshrc
```

### Building Locally

```bash
# Build with all stages
make -f Makefile.web

# Run local web server
cd build/web
python3 -m http.server 8000

# Open in browser:
# http://localhost:8000/particle_storm.html

# Test specific stages locally:
# Full game: http://localhost:8000/particle_storm.html
# Stage 3:   http://localhost:8000/particle_storm.html?stage=3
# Stage 6:   http://localhost:8000/particle_storm.html?stage=6
```

The dropdown selector will work in local builds too!

## 📋 Build Configuration

### Optimization Flags

The build is optimized for 60 FPS performance in web browsers:

**Compiler flags** (`Makefile.web`):
- `-O3`: Maximum optimization
- `-flto`: Link-Time Optimization

**Emscripten flags**:
- `-s TOTAL_MEMORY=134217728`: 128MB memory for 100k particles
- `-s INITIAL_MEMORY=134217728`: Pre-allocate memory
- `--closure 1`: Google Closure Compiler for JS optimization
- `-s ENVIRONMENT=web`: Web-only build (smaller size)

### Custom Shell Template

The build uses `raylib-wasm/shell.html` which includes:
- Custom keyboard event handling
- Prevents browser default actions (e.g., Space doesn't scroll)
- Game controls: Arrow Keys, WASD, Space, Shift, Enter, ESC

## 🎮 Controls (Web Version)

- **WASD / Arrow Keys**: Move player
- **Space**: Hold to boost particle attraction
- **Left Shift**: Hold to boost movement speed
- **Enter**: Progress through menus/stages
- **ESC**: Pause game
- **F1-F5**: Debug keys (if enabled)

## 📊 Performance

| Build Type | File Sizes | Notes |
|------------|-----------|-------|
| **Optimized** | HTML: 3.6KB<br>JS: 61KB<br>WASM: 198KB | Current production build |
| **Debug** | JS: ~377KB<br>WASM: ~396KB | With assertions enabled |

**Expected FPS**: 55-60 FPS on modern browsers (Chrome, Edge recommended)

## 🔧 Troubleshooting

### Build Fails in GitHub Actions

1. Check **Actions** tab for error logs
2. Common issues:
   - Syntax errors in C code
   - Missing source files
   - Emscripten compilation errors

### Game Doesn't Load

1. Check browser console (F12) for errors
2. Verify WebGL support: https://get.webgl.org/
3. Try different browser (Chrome/Edge recommended)

### Low FPS / Performance Issues

The game is optimized for 100,000 particles at 60 FPS. If performance is poor:
- Use Chrome or Edge (better WebAssembly performance)
- Close other browser tabs
- Check browser console for warnings

### Keyboard Controls Not Working

The custom shell template prevents default browser behavior. If keys still trigger browser actions:
- Click on the game canvas first
- Check browser console for JavaScript errors

## 📁 Project Structure

```
particles/
├── .github/workflows/
│   └── deploy-pages.yml          # GitHub Actions deployment
├── src/                           # C source code
├── raylib-wasm/
│   ├── raylib-5.0_webassembly/   # Raylib library
│   └── shell.html                # Custom HTML template
├── build/web/                     # Local build output (gitignored)
│   ├── particle_storm.html
│   ├── particle_storm.js
│   └── particle_storm.wasm
├── Makefile.web                   # Web build configuration
└── raylib-5.0_webassembly.zip    # Raylib WebAssembly package
```

## 🌐 GitHub Pages Configuration

The site is deployed using GitHub Actions (not gh-pages branch).

**Settings** → **Pages**:
- **Source**: GitHub Actions
- **Custom domain**: (optional)

The workflow creates a `github-pages` environment automatically.

## 🔄 Updating the Web Build

1. Make changes to source code
2. Test locally: `make -f Makefile.web`
3. Commit and push to main:
   ```bash
   git add .
   git commit -m "Update game logic"
   git push origin main
   ```
4. GitHub Actions automatically rebuilds and deploys

## 📚 Resources

- **GitHub Actions Workflow**: `.github/workflows/deploy-pages.yml`
- **Raylib Web**: https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)
- **Emscripten Docs**: https://emscripten.org/docs/getting_started/index.html
- **WebAssembly**: https://webassembly.org/

---

**Last Updated**: 2025-10-25
**Raylib Version**: 5.0
**Emscripten Version**: Latest (auto-installed by GitHub Actions)
