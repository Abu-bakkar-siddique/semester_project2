#pragma once
#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include<ctime>
#include <sqlite3.h>


using namespace std;

class Block {
private:
    string  block_hash;
    string previous_block_hash; // this will be NULL for the genesis block
    /* A Merkle root is the hash of all the hashes of all the transactions
    that are part of a block in a blockchain network. */
    static const int difficulty_level;
    int transaction_id;
    sqlite3 *db;

public:
    Block(int trans_id, sqlite3 *db);
    
    // Setter methods
    string calculateBlockHash(); // Method to calculate the block hash
    bool commit_block();

    // Method to mine the block (you can implement your own)

};
