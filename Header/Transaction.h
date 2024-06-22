#pragma once
#include <iostream>
#include <string>
#include "Node.h"
#include <map>


using namespace std;

class Transaction {
protected:
    map<string, string> transaction; // This will be calculated
    Node sender;
    
    string receiver_public_key;
    double amount_to_send;

public:
    Transaction(Node sender, string& r_pub_key, double &amount);

    void compute_transaction();

    map<string, string> get_transaction();
};
