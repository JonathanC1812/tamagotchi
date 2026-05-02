#include "fileio.h"
#include "pet.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <iomanip>

using namespace std;

bool saveGame(const GameState &gs)
{
    ifstream src(SAVE_FILE, ios::binary);
    if (src.is_open()) {
        ofstream dst(BACKUP_FILE, ios::binary);
        dst << src.rdbuf();
    }
    ofstream f(SAVE_FILE);
    if (!f.is_open())
        return false;
    const Pet &p = gs.pet;
    f << "version=" << SAVE_VERSION << "\n";
    f << "name=" << p.name << "\nhunger=" << p.hunger << "\nhappiness=" << p.happiness
      << "\nhealth=" << p.health << "\nenergy=" << p.energy << "\ncleanliness=" << p.cleanliness << "\nweight=" << p.weight
      << "\nage=" << p.age << "\ngold=" << p.gold << "\nisSick=" << p.isSick
      << "\nstage=" << (int)p.stage << "\ndifficulty=" << (int)gs.difficulty
      << "\nturnCount=" << gs.turnCount << "\nsaveTime=" << (long long)time(nullptr) << "\n";
    f << "personality=" << (int)p.personality
      << "\nfavFood=" << p.favoriteFoodName
      << "\ndislikeFood=" << p.dislikedFoodName << "\n";
    f << "trickCount=" << p.tricksLearned.size() << "\n";
    for (auto &t : p.tricksLearned)
        f << "trick=" << t << "\n";
    f << "itemCount=" << p.inventory.size() << "\n";
    for (auto &it : p.inventory)
        f << "iname=" << it.name << "\nitype=" << (int)it.type
          << "\nihng=" << it.hungerMod << "\nihpy=" << it.happyMod
          << "\nihp=" << it.healthMod << "\nien=" << it.energyMod
          << "\niwt=" << it.weightMod << "\nipr=" << it.price << "\n";
    return true;
}
static string rv(ifstream &f)
{
    string l;
    if (!getline(f, l))
        return "";
    size_t e = l.find('=');
    return e == string::npos ? "" : l.substr(e + 1);
}
bool loadGame(GameState &gs)
{
    ifstream f(SAVE_FILE);
    if (!f.is_open())
        return false;
    try
    {
        if (stoi(rv(f)) != SAVE_VERSION)
        {
            cout << "\n  [Save file version mismatch. Starting new game.]\n";
            return false;
        }
        Pet &p = gs.pet;
        long long savedTime = 0;
        p.name = rv(f);
        p.hunger = stoi(rv(f));
        p.happiness = stoi(rv(f));
        p.health = stoi(rv(f));
        p.energy = stoi(rv(f));
        p.cleanliness = stoi(rv(f));
        p.weight = stoi(rv(f));
        p.age = stoi(rv(f));
        p.gold = stoi(rv(f));
        p.isSick = stoi(rv(f));
        p.stage = (LifeStage)stoi(rv(f));
        gs.difficulty = (DifficultyLevel)stoi(rv(f));
        gs.turnCount = stoi(rv(f));
        savedTime = stoll(rv(f));
        p.personality = (PersonalityType)stoi(rv(f));
        p.favoriteFoodName = rv(f);
        p.dislikedFoodName = rv(f);
        p.isSleeping = false;
        p.refusesFood = false;
        p.refusedFoodName = "";
        int tc = stoi(rv(f));
        p.tricksLearned.clear();
        for (int i = 0; i < tc; i++)
            p.tricksLearned.push_back(rv(f));
        int ic = stoi(rv(f));
        p.inventory.clear();
        for (int i = 0; i < ic; i++)
        {
            Item it;
            it.name = rv(f);
            it.type = (ItemType)stoi(rv(f));
            it.hungerMod = stoi(rv(f));
            it.happyMod = stoi(rv(f));
            it.healthMod = stoi(rv(f));
            it.energyMod = stoi(rv(f));
            it.weightMod = stoi(rv(f));
            it.price = stoi(rv(f));
            p.inventory.push_back(it);
        }
        long elapsed = (long)(time(nullptr) - savedTime);
        if (elapsed > 0)
        {
            cout << "\n  [" << elapsed / 60 << " min passed while away]\n";
            applyTimedDecay(p, gs.difficulty, elapsed);
        }
        gs.running = true;
        gs.message = "Welcome back, " + p.name + "!";
        return true;
    }
    catch (...)
    {
        cout << "\n  [Save file corrupted. Could not load.]\n";
        return false;
    }
}
bool saveFileExists()
{
    ifstream f(SAVE_FILE);
    return f.good();
}
bool deleteSaveFile() { return remove(SAVE_FILE) == 0; }
bool restoreBackup()
{
    ifstream src(BACKUP_FILE, ios::binary);
    if (!src.is_open())
        return false;
    ofstream dst(SAVE_FILE, ios::binary);
    dst << src.rdbuf();
    return dst.good();
}
bool logDeath(const Pet &pet, const string &cause, DifficultyLevel diff)
{
    ofstream f(DEATH_LOG_FILE, ios::app);
    if (!f.is_open())
        return false;
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    const char *d = diff == EASY ? "Easy" : diff == HARD ? "Hard"
                                                         : "Normal";
    f << buf << " | " << pet.name << " | Age:" << pet.age << " | Stage:" << stageName(pet.stage)
      << " | Cause:" << cause << " | Diff:" << d << "\n";
    return true;
}
struct LB
{
    string name, date;
    int age, score;
};
static vector<LB> readLB()
{
    vector<LB> v;
    ifstream f(LEADERBOARD_FILE);
    if (!f.is_open())
        return v;
    string l;
    while (getline(f, l))
    {
        if (l.empty())
            continue;
        istringstream ss(l);
        LB e;
        string a, s;
        getline(ss, e.date, '|');
        getline(ss, e.name, '|');
        getline(ss, a, '|');
        getline(ss, s, '|');
        try
        {
            e.age = stoi(a);
            e.score = stoi(s);
        }
        catch (...)
        {
            continue;
        }
        v.push_back(e);
    }
    return v;
}
bool updateLeaderboard(const Pet &pet)
{
    auto v = readLB();
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    LB e;
    e.name = pet.name;
    e.age = pet.age;
    e.score = pet.age * 10 + pet.health;
    e.date = buf;
    v.push_back(e);
    sort(v.begin(), v.end(), [](const LB &a, const LB &b)
              { return a.score > b.score; });
    if ((int)v.size() > MAX_LEADERBOARD)
        v.resize(MAX_LEADERBOARD);
    ofstream f(LEADERBOARD_FILE);
    if (!f.is_open())
        return false;
    for (auto &x : v)
        f << x.date << "|" << x.name << "|" << x.age << "|" << x.score << "\n";
    return true;
}
int getLeaderboardRank(const Pet &pet)
{
    auto v = readLB();
    int score = pet.age * 10 + pet.health;
    int rank = 1;
    for (auto &x : v)
        if (x.score > score) rank++;
    return rank;
}
void displayLeaderboard()
{
    auto v = readLB();
    cout << "\n  === TOP PETS LEADERBOARD ===\n";
    if (v.empty())
    {
        cout << "  (no entries yet)\n\n";
        return;
    }
    for (size_t i = 0; i < v.size(); i++)
        cout << "  " << i + 1 << ". " << left << setw(12) << v[i].name
                  << "  Age:" << setw(4) << v[i].age << "  Score:" << v[i].score << "\n";
    cout << "\n";
}
void displayDeathLog()
{
    ifstream f(DEATH_LOG_FILE);
    cout << "\n  === DEATH LOG ===\n";
    if (!f.is_open())
    {
        cout << "  (no deaths yet)\n\n";
        return;
    }
    string l;
    int c = 0;
    while (getline(f, l) && c < 10)
    {
        cout << "  " << l << "\n";
        c++;
    }
    if (!c)
        cout << "  (no deaths yet)\n";
    cout << "\n";
}
bool clearDeathLog()
{
    ofstream f(DEATH_LOG_FILE, ios::trunc);
    return f.is_open();
}
