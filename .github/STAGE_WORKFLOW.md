# Stage Development Workflow

This document defines the Git workflow for developing and managing individual game stages in the Particle Storm project.

## Overview

Each of the 10 game stages is managed independently in its own file under `src/entities/managers/stages/`. This modular approach allows for:

- **Independent Development**: Work on stages in isolation without affecting others
- **Parallel Development**: Multiple developers can work on different stages simultaneously
- **Clean History**: Stage-specific commits make it easy to track changes per stage
- **Easy Testing**: Test individual stages without running the entire game sequence

## Branch Strategy

### Main Branches

- **`main`**: Production-ready code. All stages are complete and tested.
- **`develop`**: Integration branch where stage features are merged before release.

### Stage Feature Branches

Branch naming convention for stage development:
```
stage/stage-<N>-<description>
```

Examples:
- `stage/stage-1-blackhole-mechanics`
- `stage/stage-6-boss-shield-system`
- `stage/stage-10-final-boss-phases`

### Sub-feature Branches

For complex features within a stage:
```
feature/stage-<N>-<specific-feature>
```

Examples:
- `feature/stage-1-gravity-field-visual`
- `feature/stage-9-chain-explosion-physics`

## Workflow Process

### 1. Starting a New Stage

```bash
# Ensure you're on the latest develop branch
git checkout develop
git pull origin develop

# Create your stage branch
git checkout -b stage/stage-1-blackhole-mechanics

# Work on your stage file
# Edit: src/entities/managers/stages/stage_1.c
```

### 2. Development Cycle

```bash
# Compile just your stage to check for errors
make compile-stage-1

# Test your stage in the game
make test-stage-1

# Commit your changes with descriptive messages
git add src/entities/managers/stages/stage_1.c
git commit -m "Stage 1: Implement blackhole invulnerability mechanic

- Blackhole is now invulnerable while other enemies exist
- Added transformation logic when last enemy dies
- Updated wave timing for better pacing"
```

### 3. Commit Message Convention

Format:
```
Stage <N>: <Brief description>

<Detailed explanation>
- <Specific change 1>
- <Specific change 2>
- <Specific change 3>

[Optional] Fixes #<issue-number>
```

Examples:
```
Stage 3: Adjust speedy enemy health and spawn timing

The speedy enemies were too difficult for new players.

- Reduced health multiplier from 1.2x to 1.0x
- Increased spawn delay from 0.8s to 1.0s
- Updated wave 2 timing from 7s to 8s

Balancing based on playtest feedback.
```

### 4. Testing Before PR

```bash
# Test your specific stage
make clean && make test-stage-1

# Run full build to ensure no integration issues
make clean && make run

# Check for compilation warnings
make clean && make 2>&1 | grep -i "stage_1"
```

### 5. Creating a Pull Request

```bash
# Push your branch
git push origin stage/stage-1-blackhole-mechanics

# Create PR on GitHub with the template
# Target branch: develop
```

Use the PR template below:

```markdown
## Stage <N>: <Title>

### Description
Brief description of the stage and what makes it unique.

### Changes Made
- [ ] Stage configuration (enemies, waves, timing)
- [ ] Special mechanics implementation
- [ ] Visual theme (colors, backgrounds)
- [ ] Balance testing and adjustments

### Testing Checklist
- [ ] Stage compiles without warnings: `make compile-stage-<N>`
- [ ] Stage runs independently: `make test-stage-<N>`
- [ ] Stage integrates with game flow: `make run`
- [ ] Enemy spawn timing is correct
- [ ] Victory condition works properly
- [ ] Transition to next stage is smooth

### Stage Metrics
- Target kills: XX
- Expected duration: XX seconds
- Difficulty rating: [Easy/Medium/Hard/Boss]

### Screenshots/Videos
[If applicable, add screenshots or short gameplay clips]

### Related Issues
Closes #XX
```

### 6. Code Review and Merge

After PR approval:
```bash
# Reviewer merges to develop
git checkout develop
git merge stage/stage-1-blackhole-mechanics
git push origin develop

# Tag stage completion
git tag stage-1-v1.0 -m "Stage 1 (Gravitational Anomaly) complete"
git push origin stage-1-v1.0
```

## Stage Completion Checklist

Before marking a stage as complete:

- [ ] **Code Quality**
  - [ ] No compiler warnings
  - [ ] Follows project coding style (see `CLAUDE.md`)
  - [ ] Properly documented with comments

- [ ] **Functionality**
  - [ ] All waves spawn correctly
  - [ ] Enemy types behave as designed
  - [ ] Stage completion triggers properly
  - [ ] Transition to next stage works

- [ ] **Balance**
  - [ ] Difficulty is appropriate for stage position (1-10)
  - [ ] Health/speed/size multipliers feel fair
  - [ ] Spawn timing allows for gameplay rhythm

- [ ] **Testing**
  - [ ] Tested in isolation (`make test-stage-N`)
  - [ ] Tested in full game sequence (`make run`)
  - [ ] Playtest feedback incorporated

## Working on Multiple Stages

If you need to work on multiple stages:

```bash
# Work on stage 1
git checkout -b stage/stage-1-feature
# ... make changes to stage_1.c ...
git commit -am "Stage 1: Update"

# Switch to stage 5 without finishing stage 1
git checkout develop
git checkout -b stage/stage-5-feature
# ... make changes to stage_5.c ...
git commit -am "Stage 5: Update"

# Return to stage 1
git checkout stage/stage-1-feature
```

## Hotfixes for Released Stages

If a bug is found in a completed stage:

```bash
# Create hotfix branch from develop
git checkout develop
git checkout -b hotfix/stage-3-enemy-spawn

# Fix the issue in stage_3.c
git commit -am "Hotfix: Stage 3 enemy spawn timing

Fixed issue where enemies would spawn too quickly in wave 2."

# Create PR targeting develop
git push origin hotfix/stage-3-enemy-spawn
```

## Git Tags for Stage Milestones

Tag each stage completion:
```bash
# First completion
git tag stage-1-v1.0 -m "Stage 1: Gravitational Anomaly - Initial release"

# Major revision
git tag stage-1-v2.0 -m "Stage 1: Complete rebalance after feedback"

# Minor fix
git tag stage-1-v1.1 -m "Stage 1: Fix blackhole timing issue"
```

## Stage Release Process

When all stages 1-10 are complete:

```bash
# Merge develop to main
git checkout main
git merge develop

# Tag major version
git tag v1.0.0 -m "Particle Storm v1.0.0 - All 10 stages complete"
git push origin main --tags
```

## Best Practices

1. **One Stage Per Branch**: Don't mix changes to multiple stages in one branch
2. **Small, Focused Commits**: Each commit should represent one logical change
3. **Test Before Push**: Always run `make test-stage-N` before pushing
4. **Clear Commit Messages**: Future you will thank you for descriptive messages
5. **Regular Rebasing**: Keep your feature branch up to date with develop
6. **Delete Merged Branches**: Clean up after PR is merged

## Emergency Rollback

If a stage breaks the game:

```bash
# Find the last working commit
git log --oneline src/entities/managers/stages/stage_5.c

# Revert to specific commit
git checkout <commit-hash> -- src/entities/managers/stages/stage_5.c
git commit -m "Revert Stage 5 to working state from <commit-hash>"
```

## Questions?

See `PLANNING.md` for project architecture or `CLAUDE.md` for general development guidelines.
