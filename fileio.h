#ifndef FILEIO_H
#define FILEIO_H
#include "common.h"
#define SAVE_FILE        "savegame.txt"
#define BACKUP_FILE      "savegame.bak"
#define SAVE_VERSION     1
#define DEATH_LOG_FILE   "deaths.txt"
#define LEADERBOARD_FILE "leaderboard.txt"
#define MAX_LEADERBOARD  10
bool saveGame(const GameState& gs);
bool loadGame(GameState& gs);
bool saveFileExists();
bool deleteSaveFile();
bool restoreBackup();
bool logDeath(const Pet& pet, const std::string& cause, DifficultyLevel diff);
bool updateLeaderboard(const Pet& pet);
int  getLeaderboardRank(const Pet& pet);
void displayLeaderboard();
void displayDeathLog();
bool clearDeathLog();
#endif
