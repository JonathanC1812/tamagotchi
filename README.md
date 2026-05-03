# Tamagotchi Terminal

A text-based virtual pet game for the Linux terminal.
COMP2113 / ENGG1340 Group Project — 2025-2026 Semester 2

---

## Team Members

| Name | GitHub | Contribution |
|------|--------|------|
| Alexander Gunawan (3036572815) | @billicip | Pet Core (pet.h / pet.cpp) |
| Jovan Mikael Johansen (3036321656) | @JvnJhnsn | Random Events (events.h / events.cpp) |
| Darrel Nathan Prasetia (3036572451) | @darrelpras | Player Actions (actions.h / actions.cpp) |
| Eveline Stephanie Wijaya (3036573364) | @eveline248 | Items + Shop (items.h / items.cpp) |
| Jonathan Chandra (3036572528) | @JonathanC1812 | File I/O (fileio.h / fileio.cpp) |
| Aretha Madeline Budiman (3036572657) | @arethamdline | UI + Integration (ui.h / ui.cpp / main.cpp) |

---

## Game Description

Tamagotchi Terminal is a text-based virtual pet game played entirely in the Linux terminal. The player adopts a pet from an egg and raises it through 6 life stages (Egg, Baby, Child, Teen, Adult, and Senior) by feeding, playing, healing, and caring for it. Neglecting the pet causes their stats to drop and can even lead to the pet's death. This game tracks real elapsed time, meaning stats can decay even when the game is closed. A death log and leaderboard persist across all sessions. You can also add a pet and continue their progress.

---

## Features

| Feature | Description | Coding Element |
|---------|-------------|----------------|
| Real-time stat decay | Stats drop based on real time elapsed since last save. If you close the game for 1 hour, the pet will be hungrier when you return. | File I/O — saves timestamp, computes elapsed seconds on load |
| Random events | Every turn has a chance of a random event firing such as illness, mood swings, item drops, gold finds, and more. Probability scales with difficulty. | Random events — `rand()` with probability tables |
| Dynamic inventory | Players can buy, use, and lose items. Inventory is a dynamically allocated vector that grows and shrinks at runtime. | Dynamic memory — `std::vector<Item>` |
| Pet stats + life stages | Pet has 6 stats (Hunger, Happiness, Health, Energy, Cleanliness, Weight) and evolves through 6 life stages based on age and stat thresholds. | Data structures — `Pet` struct, `Item` struct, `LifeStage` enum |
| Multiple source files | Each module is split into a .h header and .cpp implementation file. | Program in multiple files |
| Difficulty levels | Easy, Normal, and Hard scale stat drain rate, event frequency, shop prices, and enable/disable permadeath. | `DifficultyLevel` enum + inline helper functions in `common.h` (`drainRate`, `eventChance`, `shopDiscount`, `isPermadeath`) — each module calls these instead of hardcoding values |
| Save / load game | Full game state is saved to `savegame.txt` and loaded on resume. Before each save, the previous file is copied to `savegame.bak` as a backup. | File I/O — `ofstream` with binary copy for backup; `restoreBackup()` recovers from `savegame.bak` |
| Crash-safe loading | If `savegame.txt` is corrupted, incomplete, or from an incompatible version, the game catches the error and falls back to a new game instead of crashing. | Exception handling — `try/catch(...)` wrapping all parse calls in `loadGame`; `SAVE_VERSION` header checked on load |
| Death log | Every pet death is recorded to `deaths.txt` with name, age, cause, and date. Players can also clear the log via `clearDeathLog()`. | File I/O — append mode for logging, truncate mode for clearing |
| Leaderboard | Top 10 longest-lived pets are tracked in `leaderboard.txt`. Current live rank is computed in real time via `getLeaderboardRank()`. | File I/O — reads and sorts leaderboard entries to compute live rank |
| Shop system | Players spend gold to buy food, toys, and medicine from a shop. Prices adjust based on difficulty. The shop also shows a recommendations panel highlighting the best item per stat. | Data structures + dynamic memory — iterates `vector<Item>` to find best per category |
| Work action | Players solve a random math puzzle (addition, subtraction, or multiplication) to earn gold. | Random number generation — `rand()` for operands and operator selection |
| Bath action | Players can bathe their pet to restore cleanliness, which affects health over time. | Cleanliness stat — affects `health` decay in stat update logic |
| Personality system | Each pet is assigned one of four personalities (Cheerful, Grumpy, Lazy, Anxious) that alter how stats decay and respond to actions. | `PersonalityType` enum + conditional logic in stat decay and action functions |

---

## Non-Standard Libraries

None. This project uses only the C++11 standard library:
- `<iostream>` — terminal input/output
- `<fstream>` — file save/load
- `<vector>` — dynamic inventory
- `<cstdlib>` — random number generation
- `<ctime>` — real-time timestamp tracking
- `<string>` — string handling
- `<sstream>` — string formatting
- `<algorithm>` — sorting leaderboard

No additional installation is required on the grader's machine.

---

## Compilation

Using Makefile:
```
make
```

Or manually:
```
g++ -std=c++11 -Wall -o tamagotchi main.cpp pet.cpp events.cpp actions.cpp items.cpp fileio.cpp ui.cpp
```

---

## How to Run

```
./tamagotchi
```

---

## Controls

| Input | Action |
|-------|--------|
| 1 | Feed pet (select food from inventory) |
| 2 | Play with pet (free play or use a toy) |
| 3 | Sleep (restore energy) |
| 4 | Heal (administer medicine) |
| 5 | Work (solve a math puzzle to earn gold) |
| 6 | Bath (clean pet to restore cleanliness) |
| 7 | Open shop (buy food, toys, medicine) |
| 8 | View inventory |
| 9 | Full status screen |
| 10 | Leaderboard |
| 11 | Save game |
| 0 | Quit |

---

## Life Stages

| Stage | Age | Notes |
|-------|-----|-------|
| Egg | Day 0 | Hatches after 1 day |
| Baby | Day 1-2 | Can feed and sleep |
| Child | Day 3-5 | Can play and use items |
| Teen | Day 6-9 | Shop unlocked, rebellious events possible |
| Adult | Day 10-14 | All actions available |
| Senior | Day 15+ | Wisdom events, natural aging |
| Dead | — | Health reached 0 from neglect or illness |

---

## Difficulty Levels

| Setting | Easy | Normal | Hard |
|---------|------|--------|------|
| Stat drain | Slow | Standard | Fast |
| Event chance | 5% per turn | 12% per turn | 20% per turn |
| Permadeath | Off | On | On |
| Shop prices | -25% | Standard | +50% |
| Pet lifespan | 30+ days | ~20 days | ~10 days |

---

## Saved Files

| File | Contents |
|------|----------|
| `savegame.txt` | Current pet state; starts with a version header (`version=1`) to detect incompatible saves. Also stores the current lucky number so it persists across sessions. |
| `savegame.bak` | Backup of the previous save; auto-created before each save |
| `deaths.txt` | Log of all pet deaths with cause and date |
| `leaderboard.txt` | Top 10 longest-lived pets sorted by score |

---

## File Structure

| File | Responsibility |
|------|---------------|
| `common.h` | Shared structs (Pet, Item, GameState) and enums |
| `pet.h / pet.cpp` | Pet stats, life stages, evolution, decay logic |
| `events.h / events.cpp` | Random event system with difficulty scaling |
| `actions.h / actions.cpp` | Feed, play, sleep, heal actions |
| `items.h / items.cpp` | Item definitions, shop, dynamic inventory |
| `fileio.h / fileio.cpp` | Save/load, backup save, crash-safe loading, death log, clear death log, leaderboard, live rank |
| `ui.h / ui.cpp` | Terminal display, ASCII art, menus |
| `main.cpp` | Game loop, difficulty selection, turn management |
| `Makefile` | Build instructions |
| `.gitignore` | Excludes the compiled binary (`tamagotchi`) and runtime-generated save files from version control |

---

## Troubleshooting

**Compilation fails with "command not found: g++"**
Install g++ via your package manager (e.g. `sudo apt install g++` on Ubuntu).

**"make: command not found"**
Use the manual compilation command instead:
```
g++ -std=c++11 -Wall -o tamagotchi main.cpp pet.cpp events.cpp actions.cpp items.cpp fileio.cpp ui.cpp
```

**"Load failed. Starting new game." on Continue**
The `savegame.txt` file is corrupted. The game will start fresh automatically. If a backup exists, you can restore it manually:
```
cp savegame.bak savegame.txt
```

**Pet stats look wrong after resuming**
This is expected — the game applies real-time decay based on how long you were away. Stats drop proportionally to the time elapsed since the last save.

**Display looks broken / garbled characters**
Ensure your terminal supports UTF-8 and has a wide enough window (at least 60 columns). On Linux, try `export LANG=en_US.UTF-8` before running.
