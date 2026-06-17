# Space-Invaders-Game


Space Invaders Game is a feature-rich 2D arcade shooter developed in C++ using SFML. Inspired by the classic Space Invaders concept, this project introduces modern gameplay mechanics, multiple game modes, unique enemy behaviors, challenging boss fights, power-up systems, animated backgrounds, sound effects, and a complete HUD interface.

The game was designed with a strong focus on object-oriented programming principles, efficient memory management, and engaging gameplay progression. All major bosses, enemies, and visual assets were integrated using custom PNG resources to create a unique gameplay experience.



#Game Modes

Survival Mode

Survival Mode challenges players to survive against endless waves of enemies for as long as possible. Enemy pressure continuously increases, testing the player’s reflexes, positioning, and resource management skills.

Arcade Mode

Arcade Mode consists of three progressively challenging levels. Each level introduces new enemy encounters and concludes with a unique boss battle. Defeating the boss is required to advance to the next stage.



#Enemy Types

Drone

The Drone is the most common enemy unit.

* Moves straight downward at a constant speed.
* Fires at random intervals.
* Serves as the primary introductory threat.

Viper

The Viper introduces unpredictable movement patterns.

* Descends while continuously moving left and right.
* Follows a sine-wave style flight path.
* Makes targeting significantly more challenging.

Seeker

The Seeker is designed to pressure player positioning.

* Does not fire projectiles.
* Locks onto the player’s current horizontal position when spawned.
* Aggressively dives downward toward the player.



#Boss Battles

Level 1 Boss – Cruiser

The Cruiser is a heavily armored battleship that patrols the battlefield.

Abilities:

* Sweeps left and right across the screen.
* Launches horizontal volleys of laser attacks.
* Creates continuous firing pressure on the player.
* Requires strategic movement and timing to defeat.



Level 2 Boss – Twin Cannons

The Twin Cannons boss introduces a multi-stage encounter.

Abilities:

* Consists of two heavily armed side cannons.
* The central core remains protected initially.
* Players must destroy both side cannons before exposing the core.
* Once exposed, the core becomes vulnerable and can be destroyed to complete the level.



Level 3 Boss – Mothership

The Mothership serves as the final challenge of Arcade Mode.

Abilities:

* Periodically spawns Seeker enemies.
* Maintains constant battlefield pressure through additional enemy support.
* Fires powerful sweeping laser attacks.
* Projects a screen-wide laser beam attack.
* Displays a visible warning flash before executing major attacks.
* Demands precise movement and resource management.



#Power-Up System

Throughout gameplay, players can collect various power-up drops that enhance offensive and defensive capabilities.

Spread Module

* Upgrades the player’s weapon to fire multiple projectiles simultaneously.
* Increases area coverage and crowd-control effectiveness.

Piercing Module

* Allows projectiles to penetrate multiple enemies.
* Significantly improves damage output against dense enemy formations.

Energy Shield

* Provides temporary defensive protection.
* Absorbs incoming damage and increases survivability during intense encounters.



#Additional Features

* Three fully playable arcade levels
* Endless Survival Mode
* Multiple enemy behaviors and attack patterns
* Progressive difficulty scaling
* Boss encounters at the end of each level
* Power-up drop system
* Animated gameplay
* Real-time collision detection
* Dynamic scoring system
* Scrolling background environment
* Asteroid field hazards
* Aggressive dash mechanics
* Sound effects and gameplay audio
* Main Menu interface
* Pause Menu system
* Complete Heads-Up Display (HUD)
* Shield status display
* Weapon name display
* Power-up indicators
* Boss warning effects and attack telegraphs



#Programming Concepts Used

This project was developed using core Computer Science and Object-Oriented Programming concepts, including:

* Classes and Objects
* Inheritance
* Polymorphism
* Object Composition
* Dynamic Memory Management
* Pointers
* Arrays
* Encapsulation
* Modular Game Architecture
* Event-Driven Programming
* Collision Detection Systems
* Resource Management Techniques



#Technologies Used

* C++
* SFML (Simple and Fast Multimedia Library)

#Project Highlights

* Custom boss encounters for every level
* Unique enemy AI behaviors
* Integrated power-up progression system
* Fully animated gameplay experience
* Professional HUD implementation
* Multiple game modes
* Object-oriented architecture
* Custom visual assets and PNG integrations
* Engaging arcade-style combat mechanics

#Authors

Developed as a Computer Science project using C++ and SFML.
