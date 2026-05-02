#include "ui.h"
#include "pet.h"
#include "actions.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

using namespace std;

constexpr int UI_WIDTH = 48;

/* ---------- helpers ---------- */

void clearScreen() {
    cout << "\033[2J\033[H";
}

void drawLine(char left, char mid, char right) {
    cout << "  " << left;
    for (int i = 0; i < UI_WIDTH - 2; ++i) cout << mid;
    cout << right << "\n";
}

void printCentered(const string& text) {
    int padding = (UI_WIDTH - text.size()) / 2;
    if (padding < 0) padding = 0;

    cout << "  |"
         << string(padding - 1, ' ')
         << text
         << string(UI_WIDTH - padding - text.size() - 1, ' ')
         << "|\n";
}

static string bar(int v) {
    string b = "[";
    for (int i = 0; i < 10; ++i)
        b += (i < v / 10) ? '#' : '.';
    return b + "]";
}

/* ---------- art ---------- */

void drawPetArt(LifeStage s) {
    cout << "\n";
    switch (s) {
    case EGG:
        cout << "       .--.\n"
                "      /    \\\n"
                "      |˶•  •˶|\n"
                "      \\  ⤙ /\n"
                "       `--'\n";
        break;
    case BABY:
        cout << "      (^_^)\n"
                "     /|  |\\\n"
                "       /  \\\n";
        break;
    case CHILD:
        cout << "    \\(^_^)/\n"
                "      |   |\n"
                "      / \\\n";
        break;
    case TEEN:
        cout << "     (>_<)/\n"
                "     /|   |\n"
                "      / \\\n";
        break;
    case ADULT:
        cout << "     (*v*)\n"
                "     /|   |\\\n"
                "      / \\\n";
        break;
    case SENIOR:
        cout << "     (u_u)~\n"
                "     /|   |\\\n"
                "      / \\\n";
        break;
    case DEAD:
        cout << "     (x_x)\n"
                "      /|\\\n"
                "      / \\\n";
        break;
    }
    cout << "\n";
}

/* ---------- screens ---------- */

void showWelcomeScreen() {
    clearScreen();
    drawLine('+', '-', '+');
    printCentered("T A M A G O T C H I");
    printCentered("T E R M I N A L");
    drawLine('+', '-', '+');
    printCentered("Raise your pet from egg to senior.");
    printCentered("Stats decay in real time even");
    printCentered("while the game is closed!");
    drawLine('+', '-', '+');
    cout << "\n";
}

void drawStatBars(const Pet& p) {
    cout << "  Hunger:    " << bar(p.hunger)    << " " << setw(3) << p.hunger    << "%\n";
    cout << "  Happiness: " << bar(p.happiness) << " " << setw(3) << p.happiness << "%\n";
    cout << "  Health:    " << bar(p.health)    << " " << setw(3) << p.health    << "%\n";
    cout << "  Energy:    " << bar(p.energy)    << " " << setw(3) << p.energy    << "%\n";
    cout << "  Clean:     " << bar(p.cleanliness) << " " << setw(3) << p.cleanliness << "%\n";
}

void drawMainScreen(const GameState& gs) {
    clearScreen();
    const Pet& p = gs.pet;

    string diff =
        gs.difficulty == EASY ? "EASY" :
        gs.difficulty == HARD ? "HARD" : "NORMAL";

    drawLine('+', '-', '+');
    printCentered("TAMAGOTCHI TERMINAL [" + diff + "]");
    drawLine('+', '-', '+');

    drawPetArt(p.stage);

    cout << "  Name: " << p.name
         << "   Stage: " << stageName(p.stage)
         << "   Age: " << p.age << "\n";

    cout << "  Weight: " << p.weight << "g"
         << "   Gold: " << p.gold << "\n";

    if (p.isSick)
        cout << "  !! PET IS SICK !! Use medicine!\n";

    cout << "\n";
    drawStatBars(p);
    cout << "\n";

    if (!gs.message.empty())
        cout << "  >> " << gs.message << "\n\n";

    drawLine('+', '-', '+');
}

void showMainMenu(const GameState&) {
    cout << "  [1] Feed    [2] Play    [3] Sleep   [4] Heal\n";
    cout << "  [5] Work    [6] Bath    [7] Shop    [8] Invent\n";
    cout << "  [9] Status  [10] Leaderboard  [11] Save  [0] Quit\n";
    cout << "  Choice: ";
}

DifficultyLevel chooseDifficulty() {
    cout << "  Select Difficulty:\n";
    cout << "  [1] Easy   - Slow decay, rare events, no permadeath\n";
    cout << "  [2] Normal - Standard, permadeath on\n";
    cout << "  [3] Hard   - Fast decay, frequent events, expensive shop\n";
    cout << "  Choice: ";

    int c;
    while (!(cin >> c) || c < 1 || c > 3) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Enter 1-3: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return c == 1 ? EASY : c == 3 ? HARD : NORMAL;
}

void showDeathScreen(const Pet& p, const string& cause) {
    clearScreen();
    drawLine('+', '-', '+');
    printCentered("YOUR PET HAS DIED");
    drawLine('+', '-', '+');
    drawPetArt(DEAD);
    cout << "  " << p.name << " has passed away.\n";
    cout << "  Age: " << p.age << " day(s)\n";
    cout << "  Cause: " << cause << "\n\n";
}

void showEvolutionScreen(const Pet& p) {
    clearScreen();
    drawLine('+', '-', '+');
    printCentered(p.name + " EVOLVED!");
    printCentered("New stage: " + stageName(p.stage));
    drawLine('+', '-', '+');
    drawPetArt(p.stage);
    pressEnterToContinue();
}

void printMessage(const string& msg) {
    if (!msg.empty())
        cout << "\n  *** " << msg << " ***\n";
}

void pressEnterToContinue() {
    cout << "  [Press Enter to continue]";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
