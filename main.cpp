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


static void handleFeed(GameState& gs){
    displayInventory(gs.pet);
    if(gs.pet.inventory.empty()){gs.message="No items! Visit shop.";return;}
    std::cout<<"  Select food index (-1 cancel): ";
    int idx; std::cin>>idx;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    if(idx==-1){gs.message="Cancelled.";return;}
    gs.message=actionFeed(gs,idx);
}

static void handlePlay(GameState& gs){
    std::cout<<"\n  [0] Free play (no item)\n";
    for(size_t i=0;i<gs.pet.inventory.size();i++)
        if(gs.pet.inventory[i].type==TOY)
            std::cout<<"  ["<<i+1<<"] Use: "<<gs.pet.inventory[i].name<<"\n";
    std::cout<<"  Choice: ";
    int c; std::cin>>c;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    if(c==0) gs.message=actionPlay(gs,-1);
    else {
        int idx=c-1;
        if(idx>=0&&idx<(int)gs.pet.inventory.size())
            gs.message=actionPlay(gs,idx);
        else gs.message="Invalid.";
    }
}

static void handleWork(GameState& gs){
    int left = 1 + rand() % 10;
    int right = 1 + rand() % 10;
    char ops[] = {'+', '-', '*'};
    char op = ops[rand() % 3];

    // Keep subtraction questions easy and avoid negative answers.
    if(op == '-' && right > left){
        int temp = left;
        left = right;
        right = temp;
    }

    std::cout << "\n  Work task: Solve " << left << " " << op << " " << right << " = ";
    int answer;
    if(!(std::cin >> answer)){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        gs.message = "Invalid answer.";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    gs.message = actionWork(gs, left, right, op, answer);
}

static void handleShop(GameState& gs){
    displayShop(gs.difficulty);
    std::cout<<"  Your gold: "<<gs.pet.gold<<"\n";
    std::cout<<"  Item number to buy (-1 exit): ";
    int idx; std::cin>>idx;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    if(idx==-1){gs.message="Left shop.";return;}
    gs.message=buyItem(gs.pet,idx,gs.difficulty);
}

static void runTurnEnd(GameState& gs,bool& evolved,std::string& deathCause){
    evolved=false; deathCause="";
    decayStats(gs.pet,gs.difficulty);
    gs.turnCount++;
    if(gs.turnCount%5==0) agePet(gs.pet);
    std::string ev=rollEvent(gs);
    if(!ev.empty()) gs.message+=" | EVENT: "+ev;
    if(checkEvolution(gs.pet)) evolved=true;
    checkDeath(gs.pet,deathCause);
}

static GameState newGame(){
    GameState gs;
    std::cout<<"\n  Enter your pet's name: ";
    std::string name; std::getline(std::cin,name);
    if(name.empty()) name="Mochi";
    gs.difficulty=chooseDifficulty();
    gs.pet=initPet(name,gs.difficulty);
    gs.turnCount=0; gs.running=true;
    gs.message = "Welcome! " + name + " has hatched. " + "Personality: " + personalityName(gs.pet.personality) + ". Fav food: " + gs.pet.favoriteFoodName + "!";
    giveStarterItems(gs.pet);
    return gs;
}

int main(){
    srand((unsigned)time(nullptr));
    showWelcomeScreen();
    std::cout<<"  [1] New Game\n";
    if(saveFileExists()) std::cout<<"  [2] Continue\n";
    std::cout<<"  [3] Leaderboard\n  [4] Death Log\n  [0] Quit\n  Choice: ";
    int sc; std::cin>>sc;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

    if(sc==0){std::cout<<"  Goodbye!\n";return 0;}
    if(sc==3){displayLeaderboard();pressEnterToContinue();return 0;}
    if(sc==4){displayDeathLog();pressEnterToContinue();return 0;}

    GameState gs; gs.running=false;
    if(sc==2&&saveFileExists()){
        if(!loadGame(gs)){
            std::cout<<"  Load failed. Starting new game.\n";
            pressEnterToContinue(); gs=newGame();
        }
    } else gs=newGame();

    // Main game loop
    while(gs.running&&gs.pet.stage!=DEAD){
        drawMainScreen(gs);
        showMainMenu(gs);
        int choice;
        if(!(std::cin>>choice)){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        gs.message="";
        bool skipTurn=false;

        switch(choice){
        case 1: handleFeed(gs); break;
        case 2: handlePlay(gs); break;
        case 3: gs.message=actionSleep(gs); break;
        case 4: {
            displayInventory(gs.pet);
            std::cout<<"  Medicine index (-1 cancel): ";
            int idx; std::cin>>idx;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            if(idx==-1) gs.message="Cancelled.";
            else gs.message=actionHeal(gs,idx);
            break;
        }
        case 5: handleWork(gs); break;
        case 6: gs.message=actionBath(gs); break;
        case 7: handleShop(gs); break;
        case 8: displayInventory(gs.pet); pressEnterToContinue(); skipTurn=true; break;
        case 9: std::cout<<actionStatus(gs)<<"\n"; pressEnterToContinue(); skipTurn=true; break;
        case 10: displayLeaderboard(); pressEnterToContinue(); skipTurn=true; break;
        case 11:
            gs.message=saveGame(gs)?"Game saved!":"Save failed!";
            skipTurn=true; break;
        case 0: {
            std::cout<<"\n  Save before quitting? [y/n]: ";
            char c; std::cin>>c;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            if(c=='y'||c=='Y') saveGame(gs);
            gs.running=false; continue;
        }
        default: gs.message="Unknown command."; skipTurn=true; break;
        }

        if(!skipTurn){
            bool evolved=false; std::string dc;
            runTurnEnd(gs,evolved,dc);
            if(evolved) showEvolutionScreen(gs.pet);
            if(gs.pet.stage==DEAD){
                showDeathScreen(gs.pet,dc);
                logDeath(gs.pet,dc,gs.difficulty);
                updateLeaderboard(gs.pet);
                if(isPermadeath(gs.difficulty)){
                    deleteSaveFile();
                    std::cout<<"  (Permadeath: save deleted)\n";
                }
                pressEnterToContinue();
                gs.running=false;
            }
        }
    }
    if(gs.pet.stage!=DEAD&&gs.pet.age>0) updateLeaderboard(gs.pet);
    std::cout<<"\n  Thanks for playing Tamagotchi Terminal!\n\n";
    return 0;
}
