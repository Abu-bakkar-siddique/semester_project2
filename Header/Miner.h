#include <iostream>
#include <string>
#include  "sqlite3.h"
#include "Block.h"
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
    
    bool checkPendingTransactions(sqlite3 *db) {
        int rc;
        sqlite3_stmt *stmt;

        // Step 1: Retrieve IDs of pending transactions
        string get_pending = "SELECT id FROM transactions WHERE approved = 0;";
        rc = sqlite3_prepare_v2(db, get_pending.c_str(), -1, &stmt, nullptr);
        if ( rc != SQLITE_OK ) {
            cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Step 2: Get count of nodes
        int total_nodes = 0;
        string count_nodes = "SELECT COUNT(*) FROM nodes;";
        sqlite3_stmt *stmt_count_nodes;
        rc = sqlite3_prepare_v2(db, count_nodes.c_str(), -1, &stmt_count_nodes, nullptr);
        if ( rc != SQLITE_OK ) {
            cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }

        if ( sqlite3_step(stmt_count_nodes) == SQLITE_ROW ) {
            total_nodes = sqlite3_column_int(stmt_count_nodes, 0);
        }
        else {
            cout << "Error fetching node count: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            sqlite3_finalize(stmt_count_nodes);
            return false;
        }

        sqlite3_finalize(stmt_count_nodes);

        // Calculate half of the nodes
        int half_nodes = total_nodes / 2;

        // Step 3: Iterate through each pending transaction
        while ( sqlite3_step(stmt) == SQLITE_ROW ) {
            int transaction_id = sqlite3_column_int(stmt, 0);

            // Query node_approvals to count approvals for this transaction_id
            string count_approvals = "SELECT COUNT(*) FROM node_approvals WHERE transaction_id = ? AND approve = 1;";
            sqlite3_stmt *stmt_count;
            rc = sqlite3_prepare_v2(db, count_approvals.c_str(), -1, &stmt_count, nullptr);
            if ( rc != SQLITE_OK ) {
                cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
                sqlite3_finalize(stmt);
                return false;
            }

            sqlite3_bind_int(stmt_count, 1, transaction_id);

            // Execute count_approvals query
            if ( sqlite3_step(stmt_count) == SQLITE_ROW ) {
                int approvals_count = sqlite3_column_int(stmt_count, 0);

                // Compare approvals count with half of nodes
                if ( approvals_count >= half_nodes ) {
                    // Update transactions table to set approved = 1
                    string update_approval = "UPDATE transactions SET approved = 1 WHERE id = ?;";
                    sqlite3_stmt *stmt_update;
                    rc = sqlite3_prepare_v2(db, update_approval.c_str(), -1, &stmt_update, nullptr);
                    if ( rc != SQLITE_OK ) {
                        cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
                        sqlite3_finalize(stmt);
                        sqlite3_finalize(stmt_count);
                        return false;
                    }

                    sqlite3_bind_int(stmt_update, 1, transaction_id);

                    // Execute update_approval query
                    rc = sqlite3_step(stmt_update);
                    if ( rc != SQLITE_DONE ) {
                        cout << "Error updating transaction approval status: " << sqlite3_errmsg(db) << endl;
                        sqlite3_finalize(stmt);
                        sqlite3_finalize(stmt_count);
                        sqlite3_finalize(stmt_update);
                        return false;
                    }

                    cout << "Transaction ID " << transaction_id << " approved." << endl;
                    sqlite3_finalize(stmt_update);

                    Block block(transaction_id, db);
                    if ( !block.commit_block() ) {
                        cout << "Failed to add block to the chain" << endl;
                        return false;
                    }

                }
            }

            sqlite3_finalize(stmt_count);
        }

        sqlite3_finalize(stmt);
        return true;
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

