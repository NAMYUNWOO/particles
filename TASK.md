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
- PRP: PRPs/remove-toroidal-world.md주인공크기를 다양하게 하는 작엄 
