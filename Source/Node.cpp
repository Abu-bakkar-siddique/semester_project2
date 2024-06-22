#include "Node.h"

using namespace std;

// Static member initialization
int Node::difficulty = 1000;
Node::Node(const char* db_name) {
    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK) {
        login_check = false;
        throw runtime_error("Can't open database: " + string(sqlite3_errmsg(db)));
    }
} 

Node::~Node() {
    sqlite3_close(db);
}

bool Node::sign_transaction(string key)
{
    //read private key from the db
    //check for matchings 
    sqlite3_stmt* stmnt;
    string check_query = "SELECT private_key FROM nodes WHERE username = '" + username + "'";
    int rc = sqlite3_prepare_v2(db, check_query.c_str(), -1, &stmnt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement, error in sign_transaction process: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    rc = sqlite3_step(stmnt);
    if (rc == SQLITE_ROW) {
        string signature_key = reinterpret_cast<const char*>(sqlite3_column_text(stmnt, 0));
        if (signature_key != key)
        {
            return false;
        }
    }
    return true;
   
}

Node::Node(string un, string pubk, string prik, double w, int dif, string iden)
    : username(un), public_key(pubk), private_key(prik), wallet(w) {}

string Node::generate_public_key(const string& username) {
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100000, 999999);

    int random_number = dis(gen);

    stringstream ss;
    ss << username << "_" << now_ms << "_" << random_number;

    return ss.str();
}

string Node::generate_private_key(const string& username, const string& password) {
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100003300, 993339999);
    int random_number = dis(gen);

    string hashed_password;
    for (char c : password) {
        hashed_password += static_cast<char>(c + 10);
    }

    stringstream ss;
    ss << username << "_" << now_ms << "_" << random_number << "_" << hashed_password;

    return ss.str();
}

bool Node::set_values_to_attributes(const string& usern, const string& pass) {
    string query = "SELECT identifier, private_key, wallet_balance, difficulty, password FROM nodes WHERE username = '" + usern + "' AND password = '" + pass + "';";
    sqlite3_stmt* stmnt;

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmnt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    rc = sqlite3_step(stmnt);
    if (rc == SQLITE_ROW) {
       this->username = usern;
       this->public_key = reinterpret_cast<const char*>(sqlite3_column_text(stmnt, 0));
       this->private_key = reinterpret_cast<const char*>(sqlite3_column_text(stmnt, 1));
       this->wallet = sqlite3_column_double(stmnt, 2);
       this->difficulty = sqlite3_column_int(stmnt, 3);

        sqlite3_finalize(stmnt);
        return true;
    }
    else {
        cerr << "No matching data found or query execution failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmnt);
        return false;
    }
}

bool Node::signup(string username, string password) {
    sqlite3_stmt* stmt;

    // Check if the username already exists
    string check_query = "SELECT COUNT(*) FROM nodes WHERE username = ?";
    int rc = sqlite3_prepare_v2(db, check_query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Bind the username parameter to the SQL statement
    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Failed to bind parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Execute the SQL statement
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int user_count = sqlite3_column_int(stmt, 0);
        if (user_count > 0) {
            cerr << "Username already exists" << endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }
    else {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Generate public and private keys
    string public_key = generate_public_key(username);
    string private_key = generate_private_key(username, password);

    // Insert new user into the database
    string insert_query = "INSERT INTO nodes (username, identifier, private_key, wallet_balance, difficulty, password) VALUES (?, ?, ?, ?, ?, ?);";
    rc = sqlite3_prepare_v2(db, insert_query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Bind parameters to the SQL statement
    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Failed to bind username parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_text(stmt, 2, public_key.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Failed to bind public_key parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_text(stmt, 3, private_key.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Failed to bind private_key parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    double default_balance = 100;
    rc = sqlite3_bind_double(stmt, 4, default_balance);
    if (rc != SQLITE_OK) {
        cerr << "Failed to bind wallet_balance parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    int default_difficulty = 1000;
    rc = sqlite3_bind_int(stmt, 5, default_difficulty);
    if (rc != SQLITE_OK) {
        cerr << "Failed to bind difficulty parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_text(stmt, 6, password.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Failed to bind password parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Execute the SQL statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    // Display user information
    cout << "User created successfully:" << endl;
    cout << "Username: " << username << endl;
    cout << "Public Key: " << public_key << endl;
    cout << "Private Key: " << private_key << endl;
    return true;
}

bool Node::view_chain() {
    map<string, vector<string>> chain_data;
    Blockchain chain("dbb.db");
    chain_data = chain.get_chain();

    // Check if chain_data is empty
    if (chain_data.empty()) {
        cout << "No data available to display." << endl;
        return false;
    }

    // Check for inconsistent column sizes
    size_t num_rows = chain_data.begin()->second.size();
    for (const auto& pair : chain_data) {
        if (pair.second.size() != num_rows) {
            cerr << "Inconsistent column sizes detected." << endl;
            return false;
        }
    }

    // Determine the width of each column
    map<string, size_t> column_widths;
    for (const auto& pair : chain_data) {
        size_t max_width = pair.first.size();
        for (const auto& value : pair.second) {
            max_width = max(max_width, value.size());
        }
        column_widths[pair.first] = max_width;
    }

    // Print the headers
    for (const auto& pair : chain_data) {
        cout << setw(column_widths[pair.first] + 2) << pair.first << " | ";
    }
    cout << endl;

    // Print a separator line
    for (const auto& pair : chain_data) {
        cout << string(column_widths[pair.first] + 2, '-') << "-+-";
    }
    cout << endl;

    // Print the rows of data
    for (size_t i = 0; i < num_rows; ++i) {
        for (const auto& pair : chain_data) {
            cout << setw(column_widths[pair.first] + 2) << pair.second[i] << " | ";
        }
        cout << endl;
    }

    return true;
}double Node::get_wallet_balance() {
    return wallet;
}

bool Node::login(string username, string password) {
    int rc;
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    string query = "SELECT password FROM nodes WHERE username = ?";

    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt,nullptr);
    if (rc != SQLITE_OK) 
    {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Bind the username parameter to the SQL statement
    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Failed to bind parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Execute the SQL statement
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW){
        // Username exists, check if password matches
        string stored_password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (password == stored_password) {
            // Password matches, close database and return true
            sqlite3_finalize(stmt);
            // assign valeus here
            set_values_to_attributes(username, password);
            this->login_check = true;
            this->username = username;
            return true;
        }

        else 
        {
            // Password doesn't match, close database and return false
            cerr << "Incorrect password" << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return false;
        }
    }
    else {
        // Username doesn't exist, close database and return false
        cerr << "User does not exist" << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }
}

//bool Node::view_transaction_history()
//{
//    int rc;
//    sqlite3_stmt* stmt;
//    map<string, vector<string>> history;
//    string get_transactions = "SELECT * FROM transactions WHERE sender = ? ;";
//
//    rc = sqlite3_prepare_v2(db,get_transactions.c_str(), -1, &stmt, nullptr);
//    if (rc != SQLITE_OK)
//    {
//        cout << "error viewing history: " << string(sqlite3_errmsg(db)) << endl;
//        return false;       
//    }
//    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
//    //int rc = sqlite3_prepare_v2(, get_transactions.c_str(), -1, &stmt, SQLITE_STATIC);
//    if (rc != SQLITE_OK) 
//    {
//        cout<<"error viewing history: " << string(sqlite3_errmsg(db)) << endl;
//        return false;
//    }
//    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
//        const char* sender_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
//        const char* receiver_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
//        double amount = sqlite3_column_double(stmt, 2);
//
//        // Insert data into the chain map
//        history["sender"].push_back(sender_name ? sender_name : "");
//        history["receiver"].push_back(receiver_name ? receiver_name : "");
//        history["amount"].push_back(to_string(amount));
//    }
//
//    if (rc != SQLITE_DONE) {
//        sqlite3_finalize(stmt);
//        sqlite3_close(db);
//        cout<<"Error fetching data: " + string(sqlite3_errmsg(db));
//        return false;
//    }
//
//    // Finalize the statement
//    sqlite3_finalize(stmt);
//    if (history.empty()) {
//        cout << "No data available to display." << endl;
//        return false;
//    }
//
//    // Check for inconsistent column sizes
//    size_t num_rows = history.begin()->second.size();
//    for (const auto& pair : history) {
//        if (pair.second.size() != num_rows) {
//            cerr << "Inconsistent column sizes detected." << endl;
//            return false;
//        }
//    }
//
//    // Determine the width of each column
//    map<string, size_t> column_widths;
//    for (const auto& pair : history) {
//        size_t max_width = pair.first.size();
//        for (const auto& value : pair.second) {
//            max_width = max(max_width, value.size());
//        }
//        column_widths[pair.first] = max_width;
//    }
//
//    // Print the headers
//    for (const auto& pair : history) {
//        cout << setw(column_widths[pair.first] + 2) << pair.first << " | ";
//    }
//    cout << endl;
//
//    // Print a separator line
//    for (const auto& pair : history) {
//        cout << string(column_widths[pair.first] + 2, '-') << "-+-";
//    }
//    cout << endl;
//
//    // Print the rows of data
//    for (size_t i = 0; i < num_rows; ++i) {
//        for (const auto& pair : history) {
//            cout << setw(column_widths[pair.first] + 2) << pair.second[i] << " | ";
//        }
//        cout << endl;
//    }
//    return true;
//}

bool Node::view_transaction_history()
{
    int rc;
    sqlite3_stmt* stmt;
    map<string, vector<string>> history;
    string get_transactions = "SELECT sender, reciever, amount FROM transactions WHERE sender = ?;";

    rc = sqlite3_prepare_v2(db, get_transactions.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        cout << "error viewing history: " << string(sqlite3_errmsg(db)) << endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        cout << "error viewing history: " << string(sqlite3_errmsg(db)) << endl;
        return false;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* sender_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* receiver_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double amount = sqlite3_column_double(stmt, 2);

        // Insert data into the history map
        history["sender"].push_back(sender_name ? sender_name : "");
        history["receiver"].push_back(receiver_name ? receiver_name : "");
        history["amount"].push_back(to_string(amount));
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        cout << "Error fetching data: " + string(sqlite3_errmsg(db)) << endl;
        return false;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    if (history.empty()) {
        cout << "No data available to display." << endl;
        return false;
    }

    // Check for inconsistent column sizes
    size_t num_rows = history.begin()->second.size();
    for (const auto& pair : history) {
        if (pair.second.size() != num_rows) {
            cerr << "Inconsistent column sizes detected." << endl;
            return false;
        }
    }

    // Determine the width of each column
    map<string, size_t> column_widths;
    for (const auto& pair : history) {
        size_t max_width = pair.first.size();
        for (const auto& value : pair.second) {
            max_width = max(max_width, value.size());
        }
        column_widths[pair.first] = max_width;
    }

    // Print the headers
    for (const auto& pair : history) {
        cout << setw(column_widths[pair.first] + 2) << pair.first << " | ";
    }
    cout << endl;

    // Print a separator line
    for (const auto& pair : history) {
        cout << string(column_widths[pair.first] + 2, '-') << "-+-";
    }
    cout << endl;

    // Print the rows of data
    for (size_t i = 0; i < num_rows; ++i) {
        for (const auto& pair : history) {
            cout << setw(column_widths[pair.first] + 2) << pair.second[i] << " | ";
        }
        cout << endl;
    }

    return true;
}

bool Node::send_money(string receiver_username, double amount, string privKey) {
    // Check balance
    if (wallet < 0 || amount < 0 || wallet < amount) {
        cout << "Transaction failed: insufficient balance" << endl;
        return false;
    }

    // Database variables
    sqlite3_stmt* stmt = nullptr;
    int rc = SQLITE_OK;

    // Prepare and bind parameters for SELECT statement to get receiver details
    const char* check_query = "SELECT id, identifier FROM nodes WHERE username = ?";
    rc = sqlite3_prepare_v2(db, check_query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    rc = sqlite3_bind_text(stmt, 1, receiver_username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cerr << "Failed to bind parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Execute the SELECT statement
    rc = sqlite3_step(stmt);
    int receiver_id = -1;
    const unsigned char* receiver_public_key = nullptr;
    if (rc == SQLITE_ROW) {
        receiver_id = sqlite3_column_int(stmt, 0);
        receiver_public_key = sqlite3_column_text(stmt, 1);
    }
    else {
        cerr << "No username: " << receiver_username << " exists" << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    cout << "Making Transaction against Reciever's Public Key :     " << receiver_public_key << "\n";
    cout << "Are you sure you want to Continue? (Type 'Y' to Continue or any other Character to Cancel Transaction )";
    char ynchoice;
    cin >> ynchoice;
    if (ynchoice == 'Y' || ynchoice == 'y') {
        
        sqlite3_finalize(stmt); // Finalize SELECT statement

        // Begin transaction or sign transaction
        if (!sign_transaction(privKey)) {
            cout << "Invalid private key, cannot proceed with this transaction" << endl;
            return false;
        }

        // Begin transaction
        rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to begin transaction: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Prepare and bind parameters for transaction queries
        string transaction_query1 =
            "UPDATE nodes SET wallet_balance = wallet_balance - " + to_string(amount) + " WHERE username = '" + username + "';";
        string transaction_query2 =
            "UPDATE nodes SET wallet_balance = wallet_balance + " + to_string(amount) + " WHERE username = '" + receiver_username + "';";
        string transaction_query3 =
            "UPDATE notifications SET numberOfNotifications = numberOfNotifications + 1 WHERE node_id = (SELECT id FROM nodes WHERE username = '" + receiver_username + "');";
        rc = sqlite3_exec(db, transaction_query1.c_str(), nullptr, nullptr, nullptr);

        if (rc != SQLITE_OK) {
            cerr << "Failed to Update Sender's Wallet: " << sqlite3_errmsg(db) << endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        rc = sqlite3_exec(db, transaction_query2.c_str(), nullptr, nullptr, nullptr);

        if (rc != SQLITE_OK) {
            cerr << "Failed to Update Reciever's Wallet: " << sqlite3_errmsg(db) << endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        rc = sqlite3_exec(db, transaction_query3.c_str(), nullptr, nullptr, nullptr);

        if (rc != SQLITE_OK) {
            cerr << "Failed to Update Notifications Table : " << sqlite3_errmsg(db) << endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        // Insert transaction into transactions table
        const char* insertion_query = "INSERT INTO transactions (sender, reciever, amount) VALUES (?, ?, ?)";
        rc = sqlite3_prepare_v2(db, insertion_query, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }
        rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            cerr << "Failed to bind parameter: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }
        rc = sqlite3_bind_text(stmt, 2, receiver_username.c_str(), -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            cerr << "Failed to bind parameter: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }
        rc = sqlite3_bind_double(stmt, 3, amount);
        if (rc != SQLITE_OK) {
            cerr << "Failed to bind parameter: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }
        sqlite3_finalize(stmt); // Finalize INSERT statement

        // Commit transaction
        rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to commit transaction: " << sqlite3_errmsg(db) << endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        // Get the latest transaction ID
        const char* query_trans_id = "SELECT id FROM transactions ORDER BY id DESC LIMIT 1";
        rc = sqlite3_prepare_v2(db, query_trans_id, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        int latest_id = -1;
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            latest_id = sqlite3_column_int(stmt, 0);
        }
        else if (rc != SQLITE_DONE) {
            cerr << "Process failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }
        sqlite3_finalize(stmt); // Finalize SELECT statement

        //Create and commit the block
        Block block(latest_id, db);
        //cout << "debug print before commiting block in send_money" << endl;
        if (!block.commit_block()) {
            cout << "Failed to add block to the chain" << endl;
            return false;
        }

        // //Update wallet balance in memory if transaction succeeded
        wallet -= amount;
        return true;

    }
    else {

        cout << "\nTransaction Cancelled...";
        return false;

    }
    
}

bool  Node::set_notfications() {

    int rc;
    sqlite3_stmt* stmt;
    string get_notifications = "SELECT numberOfNotifications FROM notifications WHERE node_id = (SELECT id FROM nodes WHERE username = ?);";

    rc = sqlite3_prepare_v2(db, get_notifications.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        cout << "error fetching notifications: " << string(sqlite3_errmsg(db)) << endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        cout << "error binding username for fetching notifications: " << string(sqlite3_errmsg(db)) << endl;
        return false;
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Assign the result from the query to the notifications variable
        notifications = sqlite3_column_int(stmt, 0);
    }
    else if (rc == SQLITE_DONE) {
        cerr << "No Notifications exist" << endl;
        notifications = 0;
        sqlite3_finalize(stmt);
        return false;
    }
    else {
        cerr << "Failed to execute fetch Notifications query: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    return true;
   
}


bool  Node::reset_notfications() {

    int rc;
    sqlite3_stmt* stmt;
    string reset_notifications = "UPDATE notifications SET numberOfNotifications = 0 WHERE node_id = (SELECT id FROM nodes WHERE username = ?);";

    rc = sqlite3_prepare_v2(db, reset_notifications.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        cout << "error resetting notifications: " << string(sqlite3_errmsg(db)) << endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        cout << "error binding username for reset notifications: " << string(sqlite3_errmsg(db)) << endl;
        return false;
    }
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Failed to execute reset notifications statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    else {
        notifications = 0;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    return true;

}

bool Node::get_notifications() {
    int rc;
    sqlite3_stmt* stmt;
    map<string, vector<string>> history;
    string get_transactions = "SELECT sender, reciever, amount FROM transactions WHERE reciever = ? ORDER BY id DESC LIMIT " + to_string(notifications) + " ;";

    rc = sqlite3_prepare_v2(db, get_transactions.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        cout << "error viewing notifications: " << string(sqlite3_errmsg(db)) << endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        cout << "error viewing notifications: " << string(sqlite3_errmsg(db)) << endl;
        return false;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* sender_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* receiver_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double amount = sqlite3_column_double(stmt, 2);

        // Insert data into the history map
        history["sender"].push_back(sender_name ? sender_name : "");
        history["receiver"].push_back(receiver_name ? receiver_name : "");
        history["amount"].push_back(to_string(amount));
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        cout << "Error fetching data: " + string(sqlite3_errmsg(db)) << endl;
        return false;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    if (history.empty()) {
        cout << "No data available to display." << endl;
        return false;
    }

    // Check for inconsistent column sizes
    size_t num_rows = history.begin()->second.size();
    for (const auto& pair : history) {
        if (pair.second.size() != num_rows) {
            cerr << "Inconsistent column sizes detected." << endl;
            return false;
        }
    }

    // Determine the width of each column
    map<string, size_t> column_widths;
    for (const auto& pair : history) {
        size_t max_width = pair.first.size();
        for (const auto& value : pair.second) {
            max_width = max(max_width, value.size());
        }
        column_widths[pair.first] = max_width;
    }

    // Print the headers
    for (const auto& pair : history) {
        cout << setw(column_widths[pair.first] + 2) << pair.first << " | ";
    }
    cout << endl;

    // Print a separator line
    for (const auto& pair : history) {
        cout << string(column_widths[pair.first] + 2, '-') << "-+-";
    }
    cout << endl;

    // Print the rows of data
    for (size_t i = 0; i < num_rows; ++i) {
        for (const auto& pair : history) {
            cout << setw(column_widths[pair.first] + 2) << pair.second[i] << " | ";
        }
        cout << endl;
    }
    
    Node::reset_notfications();

    return true;
}

bool Node::approve_block(int block_id) {

    int rc;
    sqlite3_stmt* stmt;
    int approvedvalue = 1;
    string approveBlockQuery = "UPDATE node_approvals SET approve = 1 WHERE node_id = (SELECT id FROM nodes WHERE username = '" + username + "') AND block_id = " + to_string(block_id) + ";";
    
    rc = sqlite3_prepare_v2(db, approveBlockQuery.c_str(), -1, &stmt, nullptr); 
    if (rc != SQLITE_OK) {
        cout << "statement prep error\n";
        cout << "Error preparing block approval query: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    //// Bind parameters
    //rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    //if (rc != SQLITE_OK) {
    //    cout << "usrname bind error\n";
    //    cout << "Error binding username parameter: " << sqlite3_errmsg(db) << endl;
    //    sqlite3_finalize(stmt);
    //    return false;
    //}

    //rc = sqlite3_bind_int(stmt, 2, block_id);
    //if (rc != SQLITE_OK) {
    //    cout << "blockid bind error\n";
    //    cout << "Error binding block_id parameter: " << sqlite3_errmsg(db) << endl;
    //    sqlite3_finalize(stmt);
    //    return false;
    //}

    rc = sqlite3_step(stmt); 
    if (rc != SQLITE_DONE) {
        cout << "statemet exeuteon error\n";
        std::cerr << "Error Approving Block : " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    cout << "\nApproved Block with Block ID : " << block_id << endl;
    sqlite3_finalize(stmt);
    return true;

}

bool Node::view_pending_approvals() {
    int rc;
    sqlite3_stmt* stmt;

    string get_pending_approvals = "SELECT block_id  FROM node_approvals WHERE node_id = (SELECT id from nodes WHERE username = ? ) AND approve = 0 ;";

    rc = sqlite3_prepare_v2(db, get_pending_approvals.c_str(), -1, &stmt, nullptr); 
    if (rc != SQLITE_OK)
    {
        cout << "error preparing get_pending_approvals statement: " << string(sqlite3_errmsg(db)) << endl;
        return false;
    }

    // Bind the username parameter
    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        cout << "Error binding username parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool hasPendingApprovals = false;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        hasPendingApprovals = true; // Set the flag to true if at least one row is returned

        // Get the block_id and approved status from the result
        int block_id = sqlite3_column_int(stmt, 0);
        int approved = sqlite3_column_int(stmt, 1);

        // Print the block_id and its approval status
        cout << "\nBlock ID: " << block_id << ", Approval Status: " << (approved ? "true" : "false");
    }

    // Check the flag after the loop
    if (!hasPendingApprovals) {
        cout << "Woohoo, No pending Approvals!";
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        cout << "Error fetching pending approvals data: " + string(sqlite3_errmsg(db)) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    return true;
}

