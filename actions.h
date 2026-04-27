#ifndef ACTIONS_H
#define ACTIONS_H
#include "common.h"
std::string actionFeed(GameState& gs, int idx);
std::string actionPlay(GameState& gs, int idx);
std::string actionSleep(GameState& gs);
std::string actionHeal(GameState& gs, int idx);
std::string actionStatus(const GameState& gs);
bool canAct(const Pet& pet);
#endif
