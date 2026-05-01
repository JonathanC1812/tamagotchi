#ifndef events.h
#define events.h

#include "common.h"
#include <string>

std::string rollEvent(GameState& gs);
void applyNegativeEvent(Pet& pet, DifficultyLevel diff, std::string& msg);
void applyPositiveEvent(Pet& pet, std::string& msg);
void applyNeutralEvent(Pet& pet, std::string& msg);
bool applyStageEvent(Pet& pet, std::string& msg);

#endif // EVENTS_H
