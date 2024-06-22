#pragma once
#include <string>
#include <chrono>
#include <random>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <iomanip>
#include <sqlite3.h>
#include "Blockchain.h"
#include "block.h"

using namespace std;
//class Node 
//{
//private:
//    double wallet;
//    static int difficulty;
//    sqlite3* db;
//    bool login_check;
//
//    // Generates a public key
//    string generate_public_key(const string& username);
//
//    // Generates a private key
//    string generate_private_key(const string& username, const string& password);
//
//    // method to be called for new signups
//    bool set_values_to_attributes(const string& usern, const string& pass);
//
//    bool sign_transaction(string priv__key);
//    
//protected:
//    string username;
//    string public_key;
//    string private_key;
//    string password;
      
//
//public:
//    // Constructor
//    Node(const char* db_name);
//
//    // Destructor
//    ~Node();
//   
//    // Constructor for new node registration
//    Node(string un, string pubk, string prik, double w, int dif, string iden);
//    bool login(string username, string password);
//
//    bool signup(string username, string password);
//
//    // Method to view the blockchain
//    bool view_chain();
//
//    // Getter methods 
//    double get_wallet_balance();
//    bool view_transaction_history();
//    bool send_money(string reciever_username, double amount, string privKey);
//};

class Node
{
private:
    double wallet;
    static int difficulty;
    bool login_check;

    // Generates a public key
    string generate_public_key(const string& username);

    // Generates a private key
    string generate_private_key(const string& username, const string& password);

    // method to be called for new signups
    bool set_values_to_attributes(const string& usern, const string& pass);

    bool sign_transaction(string priv__key);

protected:
    string username;
    string public_key;
    string private_key;
    string password;
    
public:

    int notifications;
    string getPrivateKey() {
        return private_key;
    }
    sqlite3* db;
    // Constructor
    Node(const char* db_name);

    // Destructor
    ~Node();

    // Constructor for new node registration
    Node(string un, string pubk, string prik, double w, int dif, string iden);
    bool login(string username, string password);

    bool signup(string username, string password);

    // Method to view the blockchain
    bool view_chain();

    // Getter methods 
    double get_wallet_balance();
    bool view_transaction_history();
    bool send_money(string reciever_username, double amount, string privKey);
    bool set_notfications(); // UPDATE : Method for Setting Notifications
    bool reset_notfications(); // UPDATE : Method for Updating Notifications in the DB
    bool get_notifications();  // UPDATE : Method for Getting Notifications from the DB
    bool approve_block(int block_id);
    bool view_pending_approvals();
};
