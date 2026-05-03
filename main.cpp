#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <ctime>

#include "common.h"
#include "pet.h"
#include "events.h"
#include "actions.h"
#include "items.h"
#include "fileio.h"
#include "ui.h"

using namespace std;

/* ---------------- HANDLERS ---------------- */

static void handleFeed(GameState& gs){
    displayInventory(gs.pet);

    if(gs.pet.inventory.empty()){
        gs.message = "No items! Visit shop.";
        return;
    }

    cout << "  Select food index (-1 cancel): ";
    int idx;
    cin >> idx;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if(idx == -1){
        gs.message = "Cancelled.";
        return;
    }

    gs.message = actionFeed(gs, idx);
}

static void handlePlay(GameState& gs){
    cout << "\n  [0] Free play (no item)\n";

    for(size_t i = 0; i < gs.pet.inventory.size(); i++)
        if(gs.pet.inventory[i].type == TOY)
            cout << "  [" << i + 1 << "] Use: " << gs.pet.inventory[i].name << "\n";

    cout << "  Choice: ";
    int c;
    cin >> c;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if(c == 0){
        gs.message = actionPlay(gs, -1);
    } else {
        int idx = c - 1;

        if(idx >= 0 && idx < (int)gs.pet.inventory.size())
            gs.message = actionPlay(gs, idx);
        else
            gs.message = "Invalid.";
    }
}

static void handleWork(GameState& gs){
    int left = 1 + rand() % 10;
    int right = 1 + rand() % 10;

    char ops[] = {'+', '-', '*'};
    char op = ops[rand() % 3];

    if(op == '-' && right > left){
        int temp = left;
        left = right;
        right = temp;
    }

    cout << "\n  Work task: Solve " << left << " " << op << " " << right << " = ";

    int answer;

    if(!(cin >> answer)){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        gs.message = "Invalid answer.";
        return;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    gs.message = actionWork(gs, left, right, op, answer);
}

static void handleShop(GameState& gs){
    displayShop(gs.difficulty);

    cout << "  Your gold: " << gs.pet.gold << "\n";
    cout << "  Item number to buy (-1 exit): ";

    int idx;
    cin >> idx;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if(idx == -1){
        gs.message = "Left shop.";
        return;
    }

    gs.message = buyItem(gs.pet, idx, gs.difficulty);
}

/* ---------------- TURN END ---------------- */

static void runTurnEnd(GameState& gs, bool& evolved, string& deathCause){
    evolved = false;
    deathCause = "";

    decayStats(gs.pet, gs.difficulty);

    gs.turnCount++;

    if(gs.turnCount % 5 == 0)
        agePet(gs.pet);

    string ev = rollEvent(gs);

    if(!ev.empty())
        gs.message += " | EVENT: " + ev;

    if(checkEvolution(gs.pet))
        evolved = true;

    checkDeath(gs.pet, deathCause);
}

/* ---------------- NEW GAME ---------------- */

static GameState newGame(){
    GameState gs;

    cout << "\n  Enter your pet's name: ";

    string name;
    getline(cin, name);

    if(name.empty())
        name = "Mochi";

    gs.difficulty = chooseDifficulty();
    gs.pet = initPet(name, gs.difficulty);

    gs.turnCount = 0;
    gs.running = true;

    gs.luckyNumber = 1 + rand() % 9;
    gs.luckyTurnBase = 0;

    giveStarterItems(gs.pet);

    return gs;
}

/* ---------------- HATCH MESSAGE ---------------- */

string getHatchMessage(const Pet& p){
    return "Welcome! " + p.name + " has hatched. \nPersonality: "
         + personalityName(p.personality)
         + ". Fav food: " + p.favoriteFoodName + "!";
}

/* ---------------- MAIN ---------------- */

int main(){
    srand((unsigned)time(nullptr));

    showWelcomeScreen();

    cout << "  [1] New Game\n";
    if(saveFileExists()) cout << "  [2] Continue\n";
    cout << "  [3] Leaderboard\n";
    cout << "  [4] Death Log\n";
    cout << "  [0] Quit\n";
    cout << "  Choice: ";

    int sc;
    cin >> sc;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if(sc == 0){
        cout << "  Goodbye!\n";
        return 0;
    }

    if(sc == 3){
        displayLeaderboard();
        pressEnterToContinue();
        return 0;
    }

    if(sc == 4){
        displayDeathLog();
        pressEnterToContinue();
        return 0;
    }

    GameState gs;
    gs.running = false;

    if(sc == 2 && saveFileExists()){
        if(!loadGame(gs)){
            cout << "  Load failed. Starting new game.\n";
            pressEnterToContinue();

            gs = newGame();
            showIntroStory(gs.pet.name);
            gs.message = getHatchMessage(gs.pet);
        }
        else {
            if(gs.luckyNumber < 1 || gs.luckyNumber > 9){
                gs.luckyNumber   = 1 + rand() % 9;
                gs.luckyTurnBase = gs.turnCount - 1; // -1 so re-roll can fire normally
            }
        }
    }
    else {
        gs = newGame();
        showIntroStory(gs.pet.name);
        gs.message = getHatchMessage(gs.pet);
    }

    /* ---------------- GAME LOOP ---------------- */

    while(gs.running && gs.pet.stage != DEAD){
        drawMainScreen(gs);
        showMainMenu(gs);

        int choice;

        if(!(cin >> choice)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        gs.message = "";
        bool skipTurn = false;

        switch(choice){
        case 1: handleFeed(gs); break;
        case 2: handlePlay(gs); break;
        case 3: gs.message = actionSleep(gs); break;

        case 4:{
            displayInventory(gs.pet);
            cout << "  Medicine index (-1 cancel): ";

            int idx;
            cin >> idx;

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if(idx == -1)
                gs.message = "Cancelled.";
            else
                gs.message = actionHeal(gs, idx);

            break;
        }

        case 5: handleWork(gs); break;
        case 6: gs.message = actionBath(gs); break;
        case 7: handleShop(gs); break;

        case 8:
            displayInventory(gs.pet);
            pressEnterToContinue();
            skipTurn = true;
            break;

        case 9:
            cout << actionStatus(gs) << "\n";
            pressEnterToContinue();
            skipTurn = true;
            break;

        case 10:
            displayLeaderboard();
            pressEnterToContinue();
            skipTurn = true;
            break;

        case 11:
            gs.message = saveGame(gs) ? "Game saved!" : "Save failed!";
            skipTurn = true;
            break;

        case 0:{
            cout << "\n  Save before quitting? [y/n]: ";
            char c;
            cin >> c;

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if(c == 'y' || c == 'Y')
                saveGame(gs);

            gs.running = false;
            continue;
        }

        default:
            gs.message = "Unknown command.";
            skipTurn = true;
            break;
        }

        if(!skipTurn){
            if(choice == gs.luckyNumber && choice >= 1 && choice <= 9){
                int goldBonus = 5 + rand() % 6;
                int happyBonus = 5 + rand() % 6;

                gs.pet.gold += goldBonus;
                gs.pet.happiness += happyBonus;

                clampStats(gs.pet);

                gs.message += " | LUCKY [" + to_string(gs.luckyNumber)
                            + "]! +" + to_string(goldBonus)
                            + "g +" + to_string(happyBonus) + " happiness!";
            }

            bool evolved = false;
            string dc;

            runTurnEnd(gs, evolved, dc);

            if(gs.turnCount % 5 == 0 && gs.turnCount != gs.luckyTurnBase){
                int newLucky;
                do { newLucky = 1 + rand() % 9; } while(newLucky == gs.luckyNumber);
                gs.luckyNumber   = newLucky;
                gs.luckyTurnBase = gs.turnCount;

                gs.message += " | New lucky number: [" + to_string(gs.luckyNumber) + "]!";
            }

            if(evolved)
                showEvolutionScreen(gs.pet);

            if(gs.pet.stage == DEAD){
                showDeathScreen(gs.pet, dc);
                logDeath(gs.pet, dc, gs.difficulty);
                updateLeaderboard(gs.pet);

                if(isPermadeath(gs.difficulty)){
                    deleteSaveFile();
                    cout << "  (Permadeath: save deleted)\n";
                }

                pressEnterToContinue();
                gs.running = false;
            }
        }
    }

    if(gs.pet.stage != DEAD && gs.pet.age > 0)
        updateLeaderboard(gs.pet);

    cout << "\n  Thanks for playing Tamagotchi Terminal!\n\n";
    return 0;
}
