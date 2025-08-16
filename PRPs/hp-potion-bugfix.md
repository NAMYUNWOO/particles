name: "HP Potion Healing Amount Bugfix"
description: |

## Purpose
Fix the HP potion healing mechanic to restore exactly 1 health point instead of fully restoring health, while ensuring it doesn't exceed the player's maximum health.

## Core Principles
1. **Minimal Change**: Only modify the healing calculation logic
2. **Preserve Existing Behavior**: Keep all other HP potion functionality intact
3. **Test Coverage**: Update tests to verify the new healing behavior
4. **Event Accuracy**: Ensure event data reflects actual healing amount

---

## Goal
Modify the HP potion to heal exactly 1 health point when collected, capping at maximum health (3).

## Why
- **Game Balance**: Full healing from a single potion is too powerful
- **Player Strategy**: Encourages careful health management
- **Consistent Mechanics**: Aligns with typical RPG healing item behavior

## What
When a player collects an HP potion:
- Player gains exactly 1 health point
- Health cannot exceed maximum (3)
- Event system reports actual amount healed (0 or 1)

### Success Criteria
- [x] HP potion heals exactly 1 health point
- [x] Health never exceeds maximum (3)
- [x] Healing amount is correctly reported in events
- [x] All existing tests pass with updated expectations
- [x] No regression in other HP potion behaviors

## All Needed Context

### Documentation & References
```yaml
# Current Implementation Files
- file: src/entities/managers/item_manager.c
  why: Contains the healing logic that needs to be fixed (line 62)
  current: player->health = 3; // Full restore
  
- file: src/entities/player.h
  why: Defines MAX_HEALTH (3) and player health structure
  
- file: src/core/event/event_types.h
  why: Defines HealthRestoredEventData structure
  
- file: tests/unit/test_item_manager.c
  why: Contains health-related tests that need updating

# Test Framework
- file: src/minunit/minunit.h
  why: Unit testing framework used in the project
  
- file: examples/minunit/
  why: Examples of MinUnit test patterns

# Raylib Documentation
- url: https://www.raylib.com/cheatsheet/cheatsheet.html
  why: Reference for any rendering changes (if needed)
```

### Current Codebase Structure
```bash
particles/
├── src/
│   ├── entities/
│   │   ├── items/
│   │   │   ├── hp_potion.h
│   │   │   └── hp_potion.c
│   │   ├── managers/
│   │   │   ├── item_manager.h
│   │   │   └── item_manager.c
│   │   └── player.h
│   └── core/
│       └── event/
│           └── event_types.h
└── tests/
    └── unit/
        ├── test_hp_potion.c
        └── test_item_manager.c
```

### Current Healing Logic (item_manager.c:60-72)
```c
if (CheckCircleCollision(g_itemManager.hpPotion.position, 
                         g_itemManager.hpPotion.radius,
                         player->position, 
                         player->radius) &&
    player->health < 3) {  // Only collect if health not full
    
    int oldHealth = player->health;
    player->health = 3;  // Full restore <-- THIS NEEDS TO CHANGE
    g_itemManager.hpPotion.isActive = false;
    
    // Publish events...
}
```

## Common Integration Challenges

### External Dependencies
- **Player Health System**: Ensure we respect the MAX_HEALTH constant
- **Event System**: Update event data to reflect actual healing amount
- **Test Dependencies**: Update test expectations for 1-point healing

### Making Code Testable
- The healing logic is already isolated in CheckItemCollisions()
- Tests already exist and just need updating
- No new stubs or mocks required

# Implementation Blueprint

## Implementation Tasks (in order)

### 1. Fix the Healing Logic
**File**: `src/entities/managers/item_manager.c`
**Line**: 62
**Change**:
```c
// OLD:
player->health = 3;  // Full restore

// NEW:
player->health = (player->health + 1 > 3) ? 3 : player->health + 1;  // Heal 1 point, cap at max
```

### 2. Update Unit Tests
**File**: `tests/unit/test_item_manager.c`

Update test expectations for healing behavior:
```c
MU_TEST(test_item_collision_with_low_health) {
    // Setup player with 1 health
    testPlayer.health = 1;
    
    // ... collision test ...
    
    // OLD: mu_assert_int_eq(3, testPlayer.health);
    mu_assert_int_eq(2, testPlayer.health);  // Should heal 1 point
}

// Add new test for boundary condition
MU_TEST(test_item_collision_heals_one_point) {
    // Test health 2 -> 3
    testPlayer.health = 2;
    // ... collision test ...
    mu_assert_int_eq(3, testPlayer.health);
    
    // Test health 1 -> 2
    testPlayer.health = 1;
    // ... collision test ...
    mu_assert_int_eq(2, testPlayer.health);
}
```

### 3. Verify Event Data
Ensure the HealthRestoredEventData correctly reports healing amount:
- oldHealth: previous health value
- newHealth: new health value (oldHealth + 1, capped at 3)
- amountRestored: should be 1 (or 0 if already at max)

## Build System Integration
No Makefile changes needed - all files already included in build.

## Incremental Validation Strategy

### 1. Compile the change
```bash
make clean
make
# Should compile without warnings
```

### 2. Run specific unit tests
```bash
# Compile and run item manager tests
gcc -o test_item_manager tests/unit/test_item_manager.c src/entities/managers/item_manager.c \
    src/entities/items/hp_potion.c src/entities/player.c src/core/event/event_system.c \
    -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit
./test_item_manager
```

### 3. Manual testing
- Run the game
- Take damage (health < 3)
- Collect HP potion
- Verify only 1 health restored
- Verify cannot exceed max health

### 4. Regression testing
- Verify HP potion still spawns every 25 seconds
- Verify 10-second lifetime with 3-second blink
- Verify collision detection works correctly
- Verify visual appearance unchanged

## Validation Gates

### Syntax/Style Check
```bash
# Check for basic C errors and style issues
cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem src/entities/managers/item_manager.c
```

### Compilation Check
```bash
# Ensure no compilation errors or warnings
make clean && make 2>&1 | grep -E "(error|warning):" && echo "Build failed" || echo "Build successful"
```

### Unit Test Execution
```bash
# Run the updated tests
./test_item_manager || echo "Tests failed"
```

## Unit Test Template

Add this test to `tests/unit/test_item_manager.c`:

```c
MU_TEST(test_hp_potion_heals_exactly_one_point) {
    InitItemManager();
    
    // Test healing from different health levels
    int test_cases[][2] = {
        {1, 2},  // 1 -> 2
        {2, 3},  // 2 -> 3
        {3, 3},  // 3 -> 3 (no healing at max)
    };
    
    for (int i = 0; i < 3; i++) {
        // Reset potion
        g_itemManager.hpPotion.isActive = true;
        g_itemManager.hpPotion.position = (Vector2){400, 300};
        
        // Set player health and position
        testPlayer.health = test_cases[i][0];
        testPlayer.position = (Vector2){400, 300};  // Same as potion
        
        // Check collision
        CheckItemCollisions(&testPlayer);
        
        // Verify healing
        mu_assert_int_eq(test_cases[i][1], testPlayer.health);
        
        // Verify potion consumed only if healing occurred
        if (test_cases[i][0] < 3) {
            mu_assert(!g_itemManager.hpPotion.isActive, "Potion should be consumed");
        }
    }
}

// Add to test suite
MU_TEST_SUITE(item_manager_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    // ... existing tests ...
    MU_RUN_TEST(test_hp_potion_heals_exactly_one_point);
}
```

---

## Anti-Patterns to Avoid
- ❌ Don't use magic numbers - reference MAX_HEALTH if available
- ❌ Don't forget to update test expectations
- ❌ Don't modify other HP potion behaviors
- ❌ Don't break the event system data

## Summary
This is a simple one-line fix with test updates. The main change is replacing the full heal (`player->health = 3`) with incremental healing (`player->health + 1`), ensuring it caps at maximum health. All existing HP potion functionality remains unchanged.

**Confidence Score: 9/10**
- Simple, localized change
- Clear test coverage
- No complex dependencies
- Existing code structure supports the change well