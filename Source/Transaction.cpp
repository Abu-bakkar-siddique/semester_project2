#include "Transaction.h"

Transaction::Transaction(Node sender, string& r_pub_key,double &amount)
    : sender(sender), receiver_public_key(r_pub_key), amount_to_send(amount) {}

void Transaction::compute_transaction() {
   
    if (sender.get_wallet_balance() < amount_to_send || sender.get_wallet_balance() < 0)
    {
        cerr << "Insufficient balance" << endl;
        return;
    }
  
    return;
}

map<string, string> Transaction::get_transaction() {
    return transaction;
}
