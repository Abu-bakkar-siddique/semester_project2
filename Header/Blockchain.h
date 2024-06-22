#pragma once
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <sqlite3.h>

using namespace std;
class Blockchain {

    map<string, vector<string>> chain; // string indicates the column name, the vector holds the values from fetched data
    sqlite3* db;  // Database connection
    void readBlockchainData();

public:
    // Constructor to open the database and read data into the chain map
    Blockchain(const char* db_name);

    // Destructor to close the database
    ~Blockchain();
    map<string, vector<string>> get_chain();
      
};