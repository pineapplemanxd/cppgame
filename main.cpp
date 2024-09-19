#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <random>
#include <chrono>
#include <thread>
#include <future>
#include "drugs.h"

using namespace std;

int choice;
int money = 50;
int level = 1;
int experience = 0;
int exp_to_level = 100;
int difficulty;
int max_num;
int min_num;
vector<shared_ptr<Drug>> inventory;
string command;
string argument;
string countStr;

int getRandomNumber(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

void menu(); // Forward declaration of menu function

void lost() {
    cout << "Wrong answer! You got caught by the police." << endl;
    cout << "you lost" << endl;
    inventory.clear();
    money = 50;
    level = 1;
    experience = 0;
    exp_to_level = 100;
    cout << "do you want to play again?" << endl;
    cout << "1. yes" << endl;
    cout << "2. no" << endl;
    cin >> choice;
    if (choice == 1) {
        menu();
    } else if (choice == 2) {
        cout << "game exited" << endl;
        return;
    }
}

bool mathEvent() {
    if (difficulty == 1) {
        min_num = 1;
        max_num = 10;
    } else if (difficulty == 2) {
        min_num = 1;
        max_num = 100;
    } else if (difficulty == 3) {
        min_num = 1;
        max_num = 1000;
    } else if (difficulty == 4) {
        min_num = -10000;
        max_num = 10000;
    }
    int a = getRandomNumber(min_num, max_num);
    int b = getRandomNumber(min_num, max_num);
    int correct_answer = a + b;
    int player_answer;
    cout << "Police is suspicious of you. Solve the math problem to avoid getting caught." << endl;
    cout << "What is " << a << " + " << b << "?" << endl;

    auto future = async(launch::async, [&]() {
        cin >> player_answer;
        return player_answer;
    });

    if (future.wait_for(chrono::seconds(10)) == future_status::timeout) {
        cout << "Time's up! You got caught by the police." << endl;
        lost();
        return false;
    }

    player_answer = future.get();

    if (player_answer == correct_answer) {
        cout << "Correct! You avoided getting caught." << endl;
        return true;
    } else {
        lost();
        return false;
    }
}

void game() {
    cout << "difficulty?" << endl;
    cout << "1. easy" << endl;
    cout << "2. medium" << endl;
    cout << "3. hard" << endl;
    cout << "4. impossible" << endl;
    cin >> difficulty;

    cout << "you are drug dealer in the city" << endl;
    cout << "you have to sell drugs to people" << endl;
    cout << "you have to make money" << endl;
    cout << "you have to make sure you don't get caught by police" << endl;
    cout << "type help to see the commands" << endl;
    while (true) {
        cin >> command;
        if (command == "help") {
            cout << "sell <drug> <count/all>" << endl;
            cout << "buy <drug> <count/all>" << endl;
            cout << "shop" << endl;
            cout << "exit" << endl;
            cout << "inventory" << endl;
            cout << "stats" << endl;
        } else if (command == "inventory") {
            for (const auto& item : inventory) {
                cout << item->getName() << endl;
            }
        } else if (command == "stats") {
            cout << "money: " << money << endl;
            cout << "level: " << level << endl;
            cout << "experience: " << experience << endl;
            cout << "exp to level: " << exp_to_level << endl;
        } else if (command == "exit") {
            cout << "game exited" << endl;
            break;
        } else if (command == "shop") {
            cout << "Available products for purchase:" << endl;
            vector<shared_ptr<Drug>> shopItems = { make_shared<Weed>(), make_shared<Cocaine>(), make_shared<Heroin>() };
            for (const auto& item : shopItems) {
                if (level >= item->getNeededLevel()) {
                    cout << item->getName() << " - " << item->getBuyPrice() << "$" << endl;
                } else {
                    cout << item->getName() << " (Level " << item->getNeededLevel() << ") - " << item->getBuyPrice() << "$" << endl;
                }
            }
        } else if (command == "sell" || command == "buy") {
            cin >> argument >> countStr;
            try {
                if (command == "sell") {
                    int sellCount = 0;
                    if (countStr == "all") {
                        sellCount = count_if(inventory.begin(), inventory.end(), [&](const shared_ptr<Drug>& drug) {
                            return drug->getName() == argument;
                        });
                    } else {
                        sellCount = stoi(countStr);
                    }
                    int totalSold = 0;
                    int totalMoneyEarned = 0;
                    int totalExpGained = 0;
                    for (int i = 0; i < sellCount; ++i) {
                        auto it = find_if(inventory.begin(), inventory.end(), [&](const shared_ptr<Drug>& drug) {
                            return drug->getName() == argument;
                        });
                        if (it != inventory.end()) {
                            auto drug = *it;
                            money += drug->getSellPrice();
                            experience += drug->getExpGiven();
                            totalMoneyEarned += drug->getSellPrice();
                            totalExpGained += drug->getExpGiven();
                            totalSold++;
                            if (experience >= exp_to_level) {
                                level++;
                                experience = 0;
                                exp_to_level += 50;
                            }
                            inventory.erase(it);
                        } else {
                            cout << "you don't have enough " << argument << endl;
                            break;
                        }
                    }
                    if (totalSold > 0) {
                        cout << "you sold " << totalSold << " " << argument << "(s) for " << totalMoneyEarned << "$" << endl;
                        cout << "current money: " << money << endl;
                        cout << "current experience: " << experience << endl;
                        cout << "current level: " << level << endl;
                    }
                    // Random chance of event
                    if (getRandomNumber(1, 100) <= 20 + totalSold) { // Increased chance of event
                        if (!mathEvent()) {
                            cout << "Game over. You got caught by the police." << endl;
                            break;
                        }
                    }
                } else if (command == "buy") {
                    shared_ptr<Drug> drug;
                    if (argument == "weed") {
                        drug = make_shared<Weed>();
                    } else if (argument == "cocaine") {
                        drug = make_shared<Cocaine>();
                    } else if (argument == "heroin") {
                        drug = make_shared<Heroin>();
                    } else {
                        cout << "invalid drug" << endl;
                        continue;
                    }
                    int buyCount = 0;
                    if (countStr == "all") {
                        buyCount = money / drug->getBuyPrice();
                    } else {
                        buyCount = stoi(countStr);
                    }
                    int totalBought = 0;
                    if (level >= drug->getNeededLevel()) {
                        for (int i = 0; i < buyCount; ++i) {
                            if (money >= drug->getBuyPrice()) {
                                money -= drug->getBuyPrice();
                                inventory.push_back(drug);
                                totalBought++;
                            } else {
                                cout << "not enough money" << endl;
                                break;
                            }
                        }
                        if (totalBought > 0) {
                            cout << "you bought " << totalBought << " " << drug->getName() << "(s) for " << totalBought * drug->getBuyPrice() << "$" << endl;
                            cout << "current money: " << money << endl;
                        }
                        // Random chance of event
                        if (getRandomNumber(1, 100) <= 20 + totalBought) { // Increased chance of event
                            if (!mathEvent()) {
                                cout << "Game over. You got caught by the police." << endl;
                                break;
                            }
                        }
                    } else {
                        cout << "you need to be level " << drug->getNeededLevel() << " to buy " << drug->getName() << endl;
                    }
                }
            } catch (const invalid_argument& e) {
                cout << "invalid count argument" << endl;
            }
        } else {
            cout << "invalid command" << endl;
        }
    }
}
void menu() {
    cout << "Welcome to the game!" << endl;
    cout << "1. Start Game" << endl;
    cout << "2. Exit" << endl;
    cin >> choice;
    if (choice == 1) {
        game();
    } else if (choice == 2) {
        cout << "game exited" << endl;
    } else {
        cout << "invalid choice" << endl;
        menu();
    }
}
int main() {
    menu();
    return 0;
}