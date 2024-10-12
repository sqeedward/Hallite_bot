# Halite Bot - Strategic Expansion and Combat

## Author: Bruma George-Sebastian  
## Topic: Halite Bot Strategy

---

## Overview

This project implements a competitive bot for the game **Halite**. The bot uses strategic expansion and combat tactics to conquer the game map, leveraging enemy detection, resource management, and optimal movement decisions. This bot was designed to identify and capture high-production areas, target weak opponents, and make informed movement decisions.

---

## Key Features and Strategies

### 1. **Enemy Detection and Targeting**
The bot employs a robust system to detect nearby enemies and make informed decisions based on their strength:

- **`is_enemy()` and `real_enemy()`**: These functions determine if neighboring sites are controlled by an enemy or neutral territory.
- **`better_enemy()`**: This function checks if a nearby enemy has a strength less than or equal to the bot’s current strength, allowing it to decide whether to attack.
- **`poor_enemy()`**: Identifies the weakest enemy in the neighboring directions and prioritizes attacking them to minimize risk.

### 2. **Strength and Power Evaluation**
To ensure successful attacks and strategic expansions, the bot constantly evaluates its surroundings:

- **`get_strength()`**: Retrieves the strength of a specific site to help determine whether it’s safe to attack.
- **`get_power_neighbor()`**: Calculates the total production value of all neighboring sites, aiding the bot in identifying high-value areas for expansion.

### 3. **Resource-Driven Expansion**
The bot prioritizes capturing high-production areas, ensuring it controls the most valuable parts of the map for long-term growth:

- **`find_highest_prod()`**: Scans the map for the site with the highest production and chooses that as the target for expansion. If two sites have the same production, it uses the neighboring production values to break ties and select the optimal location.

### 4. **Zone Management and Movement Optimization**
To avoid getting stuck on the map edges, the bot ensures all movements remain within the map boundaries:

- **`normalizeAxe()`**: Normalizes movement to keep the bot within the bounds of the game map.
- **`get_zone()`**: Adjusts the bot’s position based on map constraints to prevent illegal movements.

---

## Bot Strategy

### 1. **Selective Attacking**
The bot identifies nearby enemies and only attacks when it has a clear advantage, minimizing unnecessary losses. By evaluating enemy strength, it can choose the safest direction for expansion or combat.

### 2. **Focusing on High-Production Zones**
A key aspect of this bot's strategy is capturing the most productive areas on the map. This ensures that the bot can sustain its strength and expand faster than its opponents. High-production zones are prioritized for initial expansion to gain an early game advantage.

### 3. **Balanced Expansion and Combat**
The bot balances between expanding into high-production neutral territories and attacking weaker enemies. By managing this balance, it maximizes both resource gain and territorial control while avoiding unnecessary conflicts with stronger opponents.

---

## Code Structure

- **`test_enemy()`**: Checks if an enemy exists in a given direction.
- **`is_enemy()`**: Verifies if a neighboring site is controlled by an enemy.
- **`real_enemy()`**: Determines if an enemy truly occupies a neighboring location.
- **`get_strength()`**: Returns the strength of a neighboring site.
- **`better_enemy()`**: Checks if a nearby enemy is weak enough to attack.
- **`poor_enemy()`**: Identifies the weakest neighboring enemy.
- **`hasEnemies()`**: Checks if any enemies are adjacent.
- **`find_highest_prod()`**: Locates the highest production site on the map.
- **`normalizeAxe()`**: Ensures that movements stay within the map boundaries.

---

## How to Run the Bot

1. Clone the repository to your local machine.
2. Compile the code using a C++ compiler.
   ```bash
   g++ MyBot.cpp -o MyBot
