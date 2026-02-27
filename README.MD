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

## 🔄 Recent System Updates (February 2026)

### 🎯 HP System Overhaul
**Complete removal of MaxHP - CurrentHP only system**

#### Changes Made:
- ❌ **Removed MaxHP** from all unit classes (ZTDUnitBase, ZTDBase, ZTDGameMode)
- ✅ **CurrentHP only** - Single HP value for all units
- ✅ **HP Upgrades** - Defenders can upgrade HP using same Fibonacci cost system
- ✅ **Direct HP Increase** - Each HP upgrade adds 25 HP directly to CurrentHP
- ✅ **Cleaner UI** - HP display shows "HP: 125" instead of "HP: 125/125"

#### Upgrade System:
- **HP Upgrade Cost**: Fibonacci sequence (1, 1, 2, 3, 5, 8, 13...)
- **HP Upgrade Amount**: +25 HP per level
- **HP Level**: Tracks current upgrade level (0, 1, 2, 3...)

### ⚡ Defender Stat Simplification
**Removed all "Base" variables - direct stat upgrades**

#### Old System (Removed):
```
BaseFireRate + FireRateUpgradeAmount × FireRateLevel = FireRate
BaseSpeed + SpeedUpgradeAmount × SpeedLevel = Speed
BasePower + PowerUpgradeAmount × PowerLevel = Power
```

#### New System (Current):
```
FireRate × 2.0f = New FireRate (doubles each upgrade)
Speed × 2.0f = New Speed (doubles each upgrade)
Power × 2.0f = New Power (doubles each upgrade)
AttackRange × 2.0f = New AttackRange (doubles each upgrade)
CurrentHP × 2.0f = New CurrentHP (doubles each upgrade)
```

#### Benefits:
- ✅ **Much simpler** - No confusing base vs current stats
- ✅ **Exponential growth** - Each upgrade exactly doubles effectiveness
- ✅ **Easy to balance** - Just adjust initial values
- ✅ **Cleaner Blueprints** - Fewer variables to manage

### 🎲 Enemy Spawn System Redesign
**Randomized positions and mixed enemy types**

#### Old System (Predictable):
- Tanks always spawn on left side
- Helis always spawn on right side
- Small line width (4000 units)
- Sequential positioning

#### New System (Random):
- **100% larger spawn line** (8000 units vs 4000)
- **Completely random positions** - Fisher-Yates shuffle algorithm
- **Mixed enemy types** - Tanks and helis spawn at random positions
- **Better tactical variety** - Defenders must cover wider area

#### Visual Comparison:
```
Before: [Tank][Tank][Tank][Tank] | [Heli][Heli][Heli][Heli]
After:  [Heli][Tank][Heli][Tank][Tank][Heli][Tank][Heli]
```

### 🎯 Separate Enemy Stats System
**Individual stats for tanks and helicopters**

#### Tank Stats (Slow & Tough):
- **Speed**: 150 → +15 per wave
- **FireRate**: 0.3 → +0.05 per wave  
- **HP**: 100 → +20 per wave
- **Power**: 10 → +3 per wave
- **Points**: 2 points per kill

#### Helicopter Stats (Fast & Agile):
- **Speed**: 250 → +25 per wave
- **FireRate**: 0.7 → +0.15 per wave
- **HP**: 50 → +10 per wave
- **Power**: 7 → +2.5 per wave
- **Points**: 1 point per kill

#### Wave Progression Examples:
| Wave | Tank Speed | Tank HP | Heli Speed | Heli HP |
|------|------------|---------|------------|---------|
| 1    | 150        | 100     | 250        | 50      |
| 5    | 210        | 180     | 350        | 90      |
| 10   | 285        | 280     | 475        | 140     |

### 💰 Individual Point Rewards
**Enemy-specific point values instead of global rewards**

#### Changes:
- ❌ **Removed global PointsPerKill** system
- ✅ **Individual PointsOnKill** per enemy type
- ✅ **Tanks give 2 points** (harder to kill)
- ✅ **Helis give 1 point** (easier to kill)
- ✅ **Economic decisions** - Players must prioritize targets

### 🏗️ Spawn Cost System
**Individual spawn costs per defender type**

#### Changes:
- ❌ **Removed global build costs** from GameMode
- ✅ **Individual SpawnCost** property per defender Blueprint
- ✅ **Flexible pricing** - Different costs for different defenders
- ✅ **Easy balancing** - Adjust costs per unit type

### 📊 Updated Blueprint Categories

#### Wave Spawner Stats:
```
Wave
├── Units (Initial counts and increments)
├── Tank (All tank-specific stats and rewards)
├── Heli (All helicopter-specific stats and rewards)
├── Distance (Spawn distance parameters)
└── Spawn (Height and rotation settings)
```

#### Defender Stats:
```
Defender Blueprint
├── Stats (Speed, FireRate, Power, AttackRange, CurrentHP, SpawnCost)
└── Upgrade (All upgrade levels - no base variables)
```

### 🎮 Gameplay Impact

#### Strategic Depth:
- **Enemy Diversity** - Tanks and helis have distinct roles
- **Position Randomness** - Unpredictable spawn patterns
- **Economic Choices** - Different point rewards affect target priority
- **Upgrade Strategy** - Doubling system creates powerful late-game defenders

#### Balance Improvements:
- **Clear Progression** - Predictable stat growth per wave
- **Fair Rewards** - Tougher enemies give more points
- **Scalable Difficulty** - Each enemy type scales independently
- **Player Agency** - More meaningful upgrade decisions

### 🔧 Technical Improvements

#### Code Quality:
- **Removed redundant variables** - Cleaner, more maintainable code
- **Better organization** - Logical grouping of related stats
- **Improved performance** - Less unnecessary calculations
- **Enhanced debugging** - Better logging and error handling

#### Blueprint Integration:
- **Cleaner interfaces** - Fewer confusing properties
- **Better categorization** - Logical grouping in Details panel
- **Easier customization** - All values editable in Blueprint editor
- **Future-proof design** - Easy to add new enemy types

### 🎯 Attack Distance vs Attack Range

The tower defense system uses two distinct distance parameters that control enemy behavior and combat engagement:

#### 📍 **AttackDistance** (Wave Spawner)
- **Purpose**: Distance where enemies stop moving and start attacking the **base**
- **Used by**: Enemy units only
- **Applied to**: `Enemy->AttackDistanceToBase`
- **Default Value**: 2000.0 units
- **Function**: "When I'm this close to the base, stop moving and start shooting"

#### 🎯 **AttackRange** (Wave Spawner)
- **Purpose**: Combat engagement distance for **targeting other units**
- **Used by**: Both enemies and defenders
- **Applied to**: `Enemy->AttackRange` (inherited from `ZTDUnitBase`)
- **Default Value**: 1500.0 units
- **Function**: "I can only see and attack targets within this range"

#### 🔄 **How They Work Together**

**Enemy Behavior Flow:**
1. **Spawn** → Move toward base
2. **Check distance to base**: `if (DistToBase > AttackDistanceToBase)` → Keep moving
3. **Stop at AttackDistance**: When `DistToBase <= AttackDistanceToBase` → Stop moving
4. **Look for targets**: Use `AttackRange` to find defenders within range
5. **Attack**: Shoot at defenders or base

**Visual Example:**
```
Base ←─────── AttackDistance (2000) ─────── Enemy ←── AttackRange (1500) ──→ Defender
```

#### 📊 **Current Configuration**
- **AttackDistance**: 2000 units (enemies stop 2000 units from base)
- **AttackRange**: 1500 units (combat engagement range)

#### 🎮 **Gameplay Impact**

**If AttackDistance > AttackRange (Current Setup: 2000 > 1500):**
- ✅ Enemies stop outside defender range
- ✅ Creates a "safe zone" around base  
- ✅ Enemies can only attack base, not defenders
- ✅ More time for defenders to prepare

**If AttackDistance < AttackRange:**
- ⚡ Enemies stop within defender range
- ⚡ Immediate combat engagement
- ⚡ Defenders can attack enemies before they reach base
- ⚡ More intense, faster-paced combat

#### 🔧 **Balance Tuning**

**Increase AttackDistance:**
- Enemies stop further from base
- More reaction time for defenders
- Easier defense difficulty

**Decrease AttackDistance:**
- Enemies get closer to base
- Less reaction time
- Harder defense difficulty

**Increase AttackRange:**
- Units can engage from further away
- More tactical positioning options
- Larger combat area

**Decrease AttackRange:**
- Units must get closer to engage
- More precise positioning required
- Tighter combat zones

#### 💡 **Strategic Considerations**

The relationship between these two parameters creates different tactical scenarios:

**Buffer Zone Setup (AttackDistance > AttackRange):**
- Creates a 500-unit "buffer zone" where enemies are stationary but can't attack
- Allows defenders to attack approaching enemies safely
- Good for traditional tower defense gameplay

**Immediate Engagement Setup (AttackDistance ≤ AttackRange):**
- Enemies engage as soon as they stop moving
- No safe zone - immediate combat
- Better for action-oriented gameplay

These parameters can be adjusted in the Wave Spawner settings to create different difficulty curves and tactical experiences!

---

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
