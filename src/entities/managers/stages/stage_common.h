/**
 * @file stage_common.h
 * @brief Common interface and types for all stage definitions
 *
 * This header provides the common interface that all stage implementation
 * files must follow. Each stage (stage_1.c, stage_2.c, etc.) implements
 * a CreateStageN() function that returns a fully configured Stage struct.
 */

#ifndef STAGE_COMMON_H
#define STAGE_COMMON_H

#include "../stage_manager.h"

/**
 * Stage creation function signature
 * Each stage file must implement this pattern:
 * - Stage CreateStage1(void);
 * - Stage CreateStage2(void);
 * - etc.
 */

// Stage 1: Blackhole Enemy Introduction
Stage CreateStage1(void);

// Stage 2: Tracker enemies
Stage CreateStage2(void);

// Stage 3: Speed challenge
Stage CreateStage3(void);

// Stage 4: Splitter enemies
Stage CreateStage4(void);

// Stage 5: Boss 1
Stage CreateStage5(void);

// Stage 6: Advanced enemies
Stage CreateStage6(void);

// Stage 7: Chaos stage
Stage CreateStage7(void);

// Stage 8: Snake enemies
Stage CreateStage8(void);

// Stage 9: Nightmare stage
Stage CreateStage9(void);

// Stage 10: Final boss
Stage CreateStage10(void);

// Stage Test: Developer test mode
Stage CreateStageTest(void);

#endif // STAGE_COMMON_H
