#include <iostream>
#include <stdexcept>
#include "Blockchain.h"
#include <sqlite3.h>

using namespace std;

Blockchain::Blockchain(const char* db_name) {
    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK) {
        throw runtime_error("Can't open database: " + string(sqlite3_errmsg(db)));
    }

    // Read the blockchain data from the db into the chain map
    readBlockchainData();
}

Blockchain::~Blockchain() {
    sqlite3_close(db);
}

map<string, vector<string>> Blockchain::get_chain() {
    return chain;
}

// Reads the chain data into the map
void Blockchain::readBlockchainData() {
    string query = "SELECT sender, reciever, amount FROM transactions;";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        throw runtime_error("Failed to prepare statement: " + string(sqlite3_errmsg(db)));
        
    }

    // Execute the SQL statement and process the result rows
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            const char* sender_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* receiver_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            double amount = sqlite3_column_double(stmt, 2);

            // Insert data into the chain map
            chain["sender"].push_back(sender_name ? sender_name : "");
            chain["receiver"].push_back(receiver_name ? receiver_name : "");
            chain["amount"].push_back(to_string(amount));
        }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw runtime_error("Error fetching data: " + string(sqlite3_errmsg(db)));
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}



//#include "Blockchain.h"
//#include <iostream>
//
//using namespace std;
//
//Blockchain::Blockchain(const char* db_name) {
//    int rc = sqlite3_open(db_name, &db);
//    if (rc != SQLITE_OK) {
//        throw runtime_error("Can't open database: " + string(sqlite3_errmsg(db)));
//    }
//
//    // Read the blockchain data from the db into the chain map
//    readBlockchainData();
//}
//
//Blockchain::~Blockchain() {
//    sqlite3_close(db);
//}
//
//map<string, vector<string>> Blockchain::get_chain() {
//    return chain;
//}
//// reads the chain data into the map
//void Blockchain::readBlockchainData() {
//    string query =
//        "SELECT sender, reciever, amount FROM transactions;";
//        
//    sqlite3_stmt* stmt;
//    // Prepare the SQL statement
//    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
//    if (rc != SQLITE_OK) {
//        throw runtime_error("Failed to prepare statement: " + string(sqlite3_errmsg(db)));
//    }
//
//    // Execute the SQL statement and process the result rows
//    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
//        const char* sender_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
//        const char* receiver_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
//        double amount = sqlite3_column_double(stmt, 2);
//
//        chain["sender_name"].push_back(sender_name ? sender_name : "");
//        chain["receiver_name"].push_back(receiver_name ? receiver_name : "");
//        chain["amount"].push_back(to_string(amount));
//    }
//
//    if (rc != SQLITE_DONE) {
//        sqlite3_finalize(stmt);
//        throw runtime_error("couldn't fetch data: 005");
//    }
//
//    // Finalize the statement
//    sqlite3_finalize(stmt);
//}