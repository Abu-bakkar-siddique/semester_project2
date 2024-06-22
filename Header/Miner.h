#include <iostream>
#include <string>
using namespace std;

// Abstraction: Miner base class
class Miner {
protected:
    string name;       // Encapsulation (private attribute)
    int miningPower;        // Encapsulation (private attribute)
    double wallet;

public:
    Miner(string name, int miningPower)
        : name(name), miningPower(miningPower) {}

    // Pure virtual function (Abstraction)
    virtual void mine() const = 0;

    // Encapsulation: Getter methods
    string getName() const {
        return name;
    }

    double getWallet() const {
        // IMPLEMENT GETTING WALLET BALANCE FROM THE MINER'S RECORD IN THE DB AND THEN RETURN IT.
        return wallet;
    }

    double setWallet() const {
        // IMPLEMENT UPDATING WALLET BALANCE FROM THE MINER'S RECORD IN THE DB AND THEN RETURN IT.
        return wallet;
    }


    int getMiningPower() const {
        return miningPower;
    }

    // Encapsulation: Setter methods
    void setName(string newName) {
        name = newName;
    }

    void setMiningPower(int power) {
        miningPower = power;
    }

    // Virtual destructor
    virtual ~Miner() {}
};

// Inheritance and Polymorphism: CPU Miner
class CPU_Miner : public Miner {
private:
    int cpuCores;   // Additional attribute for CPU miners

public:
    CPU_Miner(string name, int miningPower, int cpuCores)
        : Miner(name, miningPower), cpuCores(cpuCores) {}

    // Override mine method with specific implementation
    void mine() const override {
        cout << getName() << " is mining with " << cpuCores << " CPU cores at "
            << getMiningPower() << " power.";
    }


    // Virtual destructor
    ~CPU_Miner() override {}
};

// Inheritance and Polymorphism: GPU Miner
class GPU_Miner : public Miner {
private:
    int gpuUnits;   // Additional attribute for GPU miners

public:
    GPU_Miner(string name, int miningPower, int gpuUnits)
        : Miner(name, miningPower), gpuUnits(gpuUnits) {}

    // Override mine method with specific implementation
    void mine() const override {
        cout << getName() << " is mining with " << gpuUnits << " GPU units at "
            << getMiningPower() << " power.";
    }
    // Virtual destructor
    ~GPU_Miner() override {}
};
