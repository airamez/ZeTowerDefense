# ZeTowerDefense - 3D Tower Defense Game

A 3D tower defense game built in Unreal Engine 5.7 where players defend their base from waves of enemy helicopters and tanks.

## 🎮 Game Overview

### Core Gameplay
- **Base Defense**: Protect your base located at one extreme of the map
- **Enemy Waves**: Enemies spawn from the opposite side and advance toward your base
- **Unit Types**: Both enemies and defensive units use the same models (HeliBP and TankBP)
- **Wave System**: Progressive difficulty with increasing enemy count and stats

### Map Configuration
- **Dimensions**: 80x40x1 floor
- **Base Position**: Center of the extreme side
- **Enemy Spawn**: Linear spawn at configurable distance from base

## 🌊 Wave Spawner System

### Wave Progression
| Parameter | Description | Default Value |
|-----------|-------------|---------------|
| **Initial Tanks** | Number of tanks in first wave | 5 |
| **Initial Helis** | Number of helicopters in first wave | 2 |
| **Tank Increment** | Additional tanks per wave | Configurable |
| **Heli Increment** | Additional helicopters per wave | Configurable |
| **Speed Increment** | Speed increase per wave | Configurable |
| **Fire Rate Increment** | Rate of fire increase per wave | Configurable |
| **HP Increment** | Health increase per wave | Configurable |
| **Power Increment** | Damage increase per wave | Configurable |

### Spawner Distance Parameters
| Parameter | Description | Default Value |
|-----------|-------------|---------------|
| **Initial Spawn Distance** | Distance from base where enemies spawn | Configurable |
| **Distance Increment** | Additional distance per wave | Configurable |
| **Maximum Distance** | Maximum spawn distance limit | Configurable |
| **Attack Distance** | Distance where enemies stop to attack defending units or the base | Configurable |
| **Unit Height** | Height offset for spawned units | Configurable |
| **Unit Rotation** | Rotation offset to compensate for mesh orientation | Configurable |

## ⚔️ Combat System

### Unit Stats
| Stat | Description | Base Value (Defenders) | Wave-based (Enemies) |
|------|-------------|------------------------|----------------------|
| **Speed** | Movement speed | Default | Wave spawner value |
| **Rate of Fire** | Attack speed | Default | Wave spawner value |
| **HP** | Hit points | Default | Wave spawner value |
| **Power** | Damage dealt | Default | Wave spawner value |
| **Attack Range** | Combat engagement distance | Configurable, upgradeable | Increases per wave |
| **Turn Rate** | Rotation speed to face target | 15 (configurable per BP) | Same |

### Unit Types
| Unit | Can Attack |
|------|-----------|
| **Helicopter (Heli)** | Helis and Tanks |
| **Tank** | Tanks only |

### Combat Rules
- **Base Damage**: 1 damage per enemy shot to base
- **Unit Damage**: Each unit fires at its target using its configured Power stat
- **Targeting Priority**: Enemies target the closest compatible defending unit within their attack range first; if none is in range they advance toward the base
- **Facing**: Every unit rotates to face its current target before firing; turn speed is configurable per Blueprint via the `Turn Rate` property (default 15)
- **Aggro / Divert**: When a defender fires at an enemy outside the enemy's own attack range, that enemy diverts toward the attacking defender instead of continuing toward the base; the enemy resumes heading for the base only if no defender is in range or attacking it
- **Tank Hit by Heli**: An enemy Tank fired upon by a defending Heli will stop advancing toward the base, move toward the Heli, and face it — but cannot return fire (Tank vs Heli targeting rule); it remains pinned until the Heli is destroyed or it dies
- **Defender Behavior**: Defenders are static (do not move); they rotate to face their target and fire when ready
- **Fight to the Death**: Once two units are within each other's attack range they engage and neither retreats until one is destroyed
- **Projectile System**: Each unit type can have a configurable rocket/missile Blueprint assigned; missiles fly toward their target and deal damage on impact; if no Blueprint is assigned the game falls back to instant damage
- **Attack Range**: Each unit has an individually configurable attack range; enemy range increases each wave via the wave spawner; defender range can be upgraded between waves
- **Mesh Orientation**: Each unit Blueprint has a `Mesh Rotation Offset` property (under Mesh category) to fix sideways-facing meshes without changing movement logic; the rocket Blueprint has the same property

## 🏗️ Building System

### Unit Placement
- **Build Menu**: Right-click to open
- **Unit Selection**: Select desired unit type from build menu
- **Unit Placement**: Left-click to place the selected unit on the map
- **Available Units**: Heli and Tank (used as defensive towers)
- **Placement Rules**: Must be on floor, no overlapping units
- **Visual Feedback**: Highlight valid placement areas

### Point System
- **Enemy Points**: 1 point per enemy destroyed
- **Unit Cost**: Points required to build defensive units
- **Upgrade Cost**: Points required to upgrade unit stats

### Real-time HUD
- **Base HP Display**: Current base health shown in top-left corner
- **Points Display**: Current collected points shown in top-left corner
- **Real-time Updates**: Both values update immediately during gameplay

## 📈 Upgrade System

### Fibonacci Progression
Upgrade costs follow Fibonacci sequence: 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89...

### Upgradable Stats
| Stat | Cost Calculation | Effect |
|------|-------------------|--------|
| **Speed** | Fibonacci sequence | Increases movement/turn speed |
| **Rate of Fire** | Fibonacci sequence | Increases attack speed |
| **Power** | Fibonacci sequence | Increases damage dealt |

### Unit Menu
- **Access**: Left-click on defending unit
- **Display**: Current stats and upgrade options
- **Validation**: Upgrade buttons enabled when points sufficient

## 🎯 Game Controls

| Action | Key/Button | Function |
|--------|------------|----------|
| **Move** | WASD/Controller | Free movement |
| **Build Menu** | Right Mouse | Open unit selection |
| **Select Unit** | Left Mouse | Select for upgrade |
| **Pause** | Escape | Pause game |
| **Continue** | C (when paused) | Resume game |
| **Exit** | X (when paused) | Leave game |

## 📊 Game States

### Wave Completion
When all enemies in a wave are destroyed:
- Game pauses
- Summary screen displays:
  - Wave number
  - Enemies destroyed
  - Base HP remaining
  - Points collected

### Base Health
- **Starting HP**: 100
- **Damage**: 1 per enemy hit
- **Game Over**: When HP reaches 0

## 🔧 Technical Requirements

### Unreal Engine 5.7 Components
- **Player Controller**: Free camera movement
- **Blueprints**: HeliBP, TankBP for both enemy and defender units
- **Fabric**: Project uses Fabric instead of C++ classes
- **UI Systems**: Build menu, unit upgrade menu, pause menu, wave summary
- **Spawner System**: Wave-based enemy generation
- **Combat System**: Targeting, damage, health management

### Key Systems to Implement
1. **Wave Spawner** with configurable parameters
2. **Unit Placement** with collision detection
3. **Combat AI** for targeting and engagement
4. **Upgrade System** with Fibonacci cost calculation
5. **UI Management** for all game menus
6. **Game State Management** for pause/resume/wave completion

## 🎮 Game Flow

1. **Start**: Player begins with base and initial points
2. **Build Phase**: Place defensive units using points
3. **Wave Start**: Enemies spawn and advance
4. **Combat**: Units engage automatically
5. **Wave End**: Summary and point collection
6. **Upgrade Phase**: Improve units between waves
7. **Repeat**: Next wave with increased difficulty
8. **Game Over**: Base destroyed or player exits

## 🙏 Credits

### Third-Party Assets
- **Cruise Missile Model**: `/Game/Fab/Cruise_Missile_New_FBX_Sample`
  - Source: Epic Games Fab Marketplace
  - License: Can be used in commercial projects with credit attribution
  - Creator: Thank you to the original creator for providing this model for game development

---

*This README provides the complete specification for implementing the ZeTowerDefense game in Unreal Engine 5.7.*
