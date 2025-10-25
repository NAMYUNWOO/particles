# Stage Development Documentation

This directory contains documentation for developing and managing game stages in Particle Storm.

## Quick Links

- **[STAGE_WORKFLOW.md](STAGE_WORKFLOW.md)** - Git branching strategy and version control workflow
- **[STAGE_DEVELOPMENT.md](STAGE_DEVELOPMENT.md)** - Complete guide to creating and modifying stages

## Overview

Each of the 10 game stages is managed as an independent file under `src/entities/managers/stages/`. This modular approach enables:

- **Independent Development**: Modify stages in isolation
- **Parallel Work**: Multiple developers on different stages
- **Clean History**: Track changes per-stage with Git
- **Easy Testing**: Test individual stages without full playthrough

## Stage Files

```
src/entities/managers/stages/
├── stage_common.h    # Common interface
├── stage_1.c         # Stage 1: Gravitational Anomaly
├── stage_2.c         # Stage 2: The Hunt Begins
├── stage_3.c         # Stage 3: Speed Demons
├── stage_4.c         # Stage 4: Divide and Conquer
├── stage_5.c         # Stage 5: Orbital Mechanics
├── stage_6.c         # Stage 6: Guardian of the Void (Boss)
├── stage_7.c         # Stage 7: Quantum Flux
├── stage_8.c         # Stage 8: Magnetic Storm
├── stage_9.c         # Stage 9: Chain Reaction
└── stage_10.c        # Stage 10: The Particle Overlord (Final Boss)
```

## Quick Start

### Testing a Specific Stage

```bash
# Jump directly to stage 3 for testing
make test-stage-3
```

### Compiling a Single Stage

```bash
# Check if stage 5 compiles without errors
make compile-stage-5
```

### Creating a Branch for Stage Work

```bash
# Create feature branch
git checkout -b stage/stage-4-balance-update

# Make changes to src/entities/managers/stages/stage_4.c

# Test your changes
make test-stage-4

# Commit with descriptive message
git commit -am "Stage 4: Reduce splitter health by 10%"

# Push and create PR
git push origin stage/stage-4-balance-update
```

## Documentation Structure

### STAGE_WORKFLOW.md

Covers:
- Git branch naming conventions
- Commit message format
- Pull request process
- Code review guidelines
- Stage completion checklist
- Release tagging strategy

### STAGE_DEVELOPMENT.md

Covers:
- Stage file structure and templates
- Configuration reference (enemies, waves, multipliers)
- Enemy type characteristics
- Balancing guidelines
- Testing procedures
- Common patterns and examples
- Troubleshooting guide

## Workflow Summary

1. **Branch**: `git checkout -b stage/stage-N-feature`
2. **Develop**: Edit `src/entities/managers/stages/stage_N.c`
3. **Test**: `make test-stage-N`
4. **Commit**: Clear, descriptive commit messages
5. **PR**: Create pull request with checklist
6. **Review**: Address feedback, iterate
7. **Merge**: Merge to `develop` branch
8. **Tag**: Tag completion: `stage-N-v1.0`

## Available Make Targets

```bash
# Build and run
make                 # Build entire project
make run             # Run full game
make clean           # Remove build artifacts

# Stage-specific
make test-stage-1    # Test Stage 1
make test-stage-2    # Test Stage 2
# ... through ...
make test-stage-10   # Test Stage 10

make compile-stage-1 # Compile Stage 1 only
make compile-stage-2 # Compile Stage 2 only
# ... through ...
make compile-stage-10 # Compile Stage 10 only
```

## Stage Development Checklist

Before marking a stage as complete:

- [ ] **Code Quality**
  - [ ] Compiles without warnings (`make compile-stage-N`)
  - [ ] Follows coding style from `CLAUDE.md`
  - [ ] Well-commented and documented

- [ ] **Functionality**
  - [ ] All waves spawn correctly
  - [ ] Enemy behaviors work as designed
  - [ ] Stage completion triggers properly
  - [ ] Smooth transition to next stage

- [ ] **Balance**
  - [ ] Difficulty appropriate for stage position
  - [ ] Multipliers feel fair
  - [ ] Spawn timing creates good rhythm

- [ ] **Testing**
  - [ ] Tested in isolation (`make test-stage-N`)
  - [ ] Tested in sequence (`make run`)
  - [ ] Playtested and balanced

## Common Commands Reference Card

```bash
# Development Cycle
git checkout -b stage/stage-3-update
# ... edit stage_3.c ...
make compile-stage-3
make test-stage-3
git commit -am "Stage 3: Improve wave pacing"
git push origin stage/stage-3-update

# Testing
make test-stage-6     # Test boss stage
make clean && make    # Full rebuild
make run              # Play from start

# Validation
make compile-stage-1  # Check compilation
make clean && make    # Ensure integration
```

## Git Branch Naming

- **Stage Feature**: `stage/stage-N-description`
  - Example: `stage/stage-3-speedy-nerf`

- **Sub-feature**: `feature/stage-N-specific-feature`
  - Example: `feature/stage-6-boss-shield`

- **Hotfix**: `hotfix/stage-N-issue`
  - Example: `hotfix/stage-9-spawn-bug`

## Commit Message Format

```
Stage N: Brief description

Detailed explanation of the change and why it was made.

- Specific change 1
- Specific change 2
- Specific change 3

[Optional] Fixes #issue-number
```

## Getting Help

- **Architecture**: See `../PLANNING.md`
- **Coding Style**: See `../CLAUDE.md`
- **Stage Design**: See `../STAGE_DESIGN.md`
- **Testing**: See `../TESTING_STRATEGY.md`

## Stage Development Best Practices

1. **One Stage Per Branch**: Don't mix changes to multiple stages
2. **Test Before Commit**: Always run `make test-stage-N`
3. **Clear Messages**: Explain the "why" in commit messages
4. **Small Changes**: Incremental improvements are easier to review
5. **Playtest**: Balance requires actual gameplay testing
6. **Document**: Update comments when changing stage logic

---

For more detailed information, see:
- [STAGE_WORKFLOW.md](STAGE_WORKFLOW.md) - Complete Git workflow guide
- [STAGE_DEVELOPMENT.md](STAGE_DEVELOPMENT.md) - Comprehensive development guide
