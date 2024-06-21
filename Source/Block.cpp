#include "Block.h"
using namespace std;

// Initialize the static member
const int Block::difficulty_level = 4;

Block::Block(int trans_id, sqlite3 *db)
    : transaction_id(trans_id), db(db) {

    
}

string Block::calculateBlockHash() {
    long hash = 5381;
    string str = previous_block_hash + to_string(difficulty_level);

    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    str += std::to_string(current_time);

    for (char c : str) {
        hash = hash * 33 + c;
    }

    return to_string(hash);
}

bool Block::commit_block() 
{
    cout << "debug print in commint block" << endl;
    int rc;
    sqlite3_stmt* stmt;

     //Get previous block hash
    string previous_block_hash_query = "SELECT block_hash FROM blocks ORDER BY id DESC LIMIT 1";
    sqlite3_prepare_v2(db, previous_block_hash_query.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    
    // Cast the result to const char* and assign to previous_block_hash
    previous_block_hash = (const char*)sqlite3_column_text(stmt, 0);
    sqlite3_finalize(stmt); // Finalize after use

    // Calculate the new block hash
    block_hash = calculateBlockHash();
    // Insert new block
    string insert_block_query = "INSERT INTO blocks(previous_block_hash, block_hash, transaction_id) VALUES(?, ?, ?)";
    sqlite3_prepare_v2(db, insert_block_query.c_str(), -1, &stmt, nullptr);
    

    // Bind the values to the prepared statement
    sqlite3_bind_text(stmt, 1, previous_block_hash.c_str(), -1, SQLITE_STATIC);
    
    sqlite3_bind_text(stmt, 2, block_hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, transaction_id);
    //bug here
    // Execute the insert statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    // Finalize the statement and close the database
    sqlite3_finalize(stmt);

    return true;
}
