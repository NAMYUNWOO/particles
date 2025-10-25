## develop "HP potion item" ✓ [COMPLETED 2025-07-13]
- HPPOTION_BASE_SIZE = 10.0f
- It appears on the map in a random location visible to the player.
- It is still and not moving. 
- This can appear once every 25 seconds and there can only be one on the map at a time.
- It will disappear after 10 seconds of appearing. For the last 3 seconds, it will blink to inform the player that it will disappear soon.
- When this item comes into contact with a player, the player's HP will be restored. However, if the player's HP is full, HP will not increase.

## Remove Toroidal World Concept ✓ [COMPLETED 2025-09-06]
- Remove wrap-around behavior where particles cross screen edges
- Implement boundary enforcement - no objects can go over screen boundaries
- Particles should bounce off walls like enemies do
- Remove toroidal distance calculations and functions
- Maintain performance with 100,000 particles
- PRP: PRPs/remove-toroidal-world.md

## Implement Stage-Based Version Control System ✓ [COMPLETED 2025-10-25]
- Split `stage_manager.c` into modular stage files (10 individual files)
- Create `src/entities/managers/stages/` directory structure
- Each stage in separate file: `stage_1.c` through `stage_10.c`
- Create `stage_common.h` interface for all stages
- Update Makefile with stage-specific build targets (`make test-stage-N`, `make compile-stage-N`)
- Create Git workflow documentation (`.github/STAGE_WORKFLOW.md`)
- Create comprehensive stage development guide (`.github/STAGE_DEVELOPMENT.md`)
- Update `PLANNING.md` with new file structure and workflow
- All stages compile successfully without warnings
- Build system fully supports independent stage development

**Benefits:**
- Independent stage development and testing
- Clean Git history with stage-specific commits
- Easy to track changes per stage
- Parallel development on different stages possible
- Each stage file < 200 lines, well-documented

**New Commands:**
```bash
make test-stage-N        # Test specific stage
make compile-stage-N     # Compile specific stage
```

## Add WebAssembly Stage Selection for GitHub Pages ✓ [COMPLETED 2025-10-25]
- Created interactive dropdown menu in web interface for stage selection
- Modified `shell.html` with modern UI and stage selector
- Added command-line argument parsing in `main.c` (`--start-stage N`)
- Updated `Makefile.web` to include all 10 stage files
- Implemented URL parameter support (`?stage=N`)
- Each stage can be tested independently on GitHub Pages
- Updated `WEB_BUILD_GUIDE.md` with stage testing instructions

**Features:**
- Dropdown selector: Choose from "Full Game" or any of 10 stages
- URL parameters: Direct links to specific stages
- Stage descriptions shown when stage is selected
- Modern gradient UI with smooth transitions
- Works in both local builds and GitHub Pages deployment

**Usage:**
```bash
# Web (GitHub Pages)
https://namyunwoo.github.io/particles/          # Full game
https://namyunwoo.github.io/particles/?stage=3  # Jump to Stage 3
https://namyunwoo.github.io/particles/?stage=10 # Jump to Final Boss

# Local testing
http://localhost:8000/particle_storm.html?stage=6  # Test Boss stage locally
```

**Benefits:**
- Quick stage testing without playing through entire game
- Share direct links to specific stages for feedback
- Easier debugging and balancing of individual stages
- Professional testing environment for playtesting
