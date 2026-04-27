#ifndef UI_H
#define UI_H
#include "common.h"
void clearScreen();
void drawMainScreen(const GameState& gs);
void drawPetArt(LifeStage stage);
void drawStatBars(const Pet& pet);
void showMainMenu(const GameState& gs);
void showWelcomeScreen();
DifficultyLevel chooseDifficulty();
void showDeathScreen(const Pet& pet, const std::string& cause);
void showEvolutionScreen(const Pet& pet);
void printMessage(const std::string& msg);
void pressEnterToContinue();
#endif
