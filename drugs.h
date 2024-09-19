#ifndef DRUGS_H
#define DRUGS_H

#include <iostream>
using namespace std;

class Drug {
public:
    virtual string getName() const = 0;
    virtual int getSellPrice() const = 0;
    virtual int getBuyPrice() const = 0;
    virtual int getExpGiven() const = 0;
    virtual int getNeededLevel() const = 0;
    virtual ~Drug() = default;
};

class Weed : public Drug {
public:
    string getName() const override { return "weed"; }
    int getSellPrice() const override { return 10; }
    int getBuyPrice() const override { return 8; }
    int getExpGiven() const override { return 10; }
    int getNeededLevel() const override { return 1; }
};

class Cocaine : public Drug {
public:
    string getName() const override { return "cocaine"; }
    int getSellPrice() const override { return 20; }
    int getBuyPrice() const override { return 13; }
    int getExpGiven() const override { return 20; }
    int getNeededLevel() const override { return 2; }
};

class Heroin : public Drug {
public:
    string getName() const override { return "heroin"; }
    int getSellPrice() const override { return 30; }
    int getBuyPrice() const override { return 25; }
    int getExpGiven() const override { return 30; }
    int getNeededLevel() const override { return 5; }
};

#endif //DRUGS_H