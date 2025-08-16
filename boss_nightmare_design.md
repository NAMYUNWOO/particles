# BOSS DESIGN: NIGHTMARE LORD

## Overview
The Nightmare Lord is an ultimate boss enemy designed to be the most challenging and terrifying encounter in the game. This boss combines multiple mechanics from previous enemies and introduces new, unique patterns that require mastery of all game systems.

## Basic Statistics
- **Type**: ENEMY_TYPE_NIGHTMARE
- **Size**: 8x larger than basic enemies (largest boss in game)
- **Base Health**: 2000 HP
- **Movement Speed**: Variable (1x - 3x based on phase)
- **Color Scheme**: Black core with red energy tendrils
- **Special Feature**: Health regeneration (5 HP/second when not taking damage for 3 seconds)

## Phase System

### Phase 1: Shadow Form (100% - 66% HP)
**Theme**: Deception and confusion

**Abilities**:
1. **Shadow Clones**
   - Spawns 3 identical clones at 90% HP
   - Each clone has 200 HP and deals 50% damage
   - Clones independently track and attack the player
   - When a clone dies, it explodes into 20 hostile particles

2. **Phase Shift**
   - Every 2 seconds, becomes semi-transparent
   - Takes 50% reduced damage while phased
   - Can pass through particles without taking damage
   - Lasts for 1 second

3. **Dark Wave**
   - Every 5 seconds, releases omnidirectional dark waves
   - Waves push particles away and damage the player
   - 8 waves in a spiral pattern
   - Wave speed increases with lower HP

### Phase 2: Chaos Storm (66% - 33% HP)
**Theme**: Environmental control and particle manipulation

**Abilities**:
1. **Chaos Vortexes**
   - Creates 3-5 random gravity wells on the map
   - Each vortex pulls/pushes particles in alternating patterns
   - Vortexes last for 10 seconds
   - New vortexes spawn every 7 seconds

2. **Particle Theft**
   - Steals 30% of player's particles within a 300-pixel radius
   - Stolen particles orbit the boss as a shield
   - Stolen particles deal damage to the player if they touch them
   - Particles return to player control after 5 seconds

3. **Teleport Barrage**
   - Teleports every 0.5 seconds for 3 seconds
   - Each teleport creates a shockwave that damages nearby entities
   - Shockwave radius: 100 pixels
   - Pattern becomes predictable after observing

### Phase 3: Final Desperation (33% - 0% HP)
**Theme**: Overwhelming power and last stand

**Abilities**:
1. **Berserk Mode**
   - Size increases by 1.5x
   - Movement speed triples
   - Leaves a damaging trail behind
   - All attacks have 50% shorter cooldowns

2. **Death Spiral**
   - Creates a massive black hole at boss position
   - Pulls everything (player, particles, other enemies) inward
   - Pull strength increases over 5 seconds
   - Player must maintain distance or take massive damage

3. **Final Gambit**
   - Triggered at 5% HP
   - Boss becomes invulnerable and starts a 5-second countdown
   - Visual: Boss glows brighter each second
   - At 0, explodes dealing damage based on distance
   - Player must escape to screen edges to survive

## Special Mechanics

### Adaptive Defense
- Boss learns from player attack patterns
- After taking 100 damage from one direction, gains 25% resistance to attacks from that angle
- Resistance visualized by directional shields
- Forces player to constantly reposition

### Fear Aura
- Particles within 150 pixels of boss move 50% slower
- Creates a "dead zone" around the boss
- Makes close-range attacks risky
- Aura pulses visually to show range

### Second Wind
- Upon first death, boss resurrects with 50% HP
- Resurrection takes 3 seconds with dramatic animation
- All phases unlocked after resurrection
- Can use abilities from any phase randomly

## Visual Effects
- Constantly shifting shadow beneath boss
- Red lightning arcs between boss and nearby particles
- Screen shakes increase with boss proximity
- Darkness effect at screen edges during phase 3

## Audio Cues (Planned)
- Unique theme music with 3 variations for each phase
- Heartbeat sound that speeds up as boss HP decreases
- Distinctive sound for each ability activation
- Warning sirens for Final Gambit

## Balancing Notes
- Designed for players who have mastered all 10 stages
- Requires understanding of all game mechanics
- Average fight duration: 3-5 minutes
- Recommended particle count: 100,000 for full experience

## Implementation Priority
1. Core boss entity and stats
2. Phase transition system
3. Phase 1 abilities
4. Phase 2 abilities  
5. Phase 3 abilities
6. Special mechanics
7. Visual polish
8. Audio integration

## Future Expansion Ideas
- Nightmare mode: Boss starts at phase 2
- Twin Nightmare: Fight two bosses simultaneously
- Endless Nightmare: Boss continuously resurrects with increasing stats