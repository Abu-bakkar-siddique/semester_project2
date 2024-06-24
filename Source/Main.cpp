#include <iostream>
#include <string>
#include "Node.h"
#include "Blockchain.h"
#include "Transaction.h"
#include<windows.h>
#include<Miner.h>
#include <thread>   // For std::this_thread::sleep_for
#include <chrono>   // For std::chrono::seconds

using namespace std;

string getPassword() {
    string password;
    char ch;
    DWORD mode, count;
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

    std::cout << "Enter your password: ";
    while (true) {
        ReadConsoleA(h, &ch, 1, &count, NULL);
        if (ch == '\r') { // Enter key is pressed
            break;
        }
        else if (ch == '\b') { // Backspace is pressed
            if (!password.empty()) {
                std::cout << "\b \b"; // Erase the last *
                password.pop_back();
            }
        }
        else {
            password.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << std::endl;

    SetConsoleMode(h, mode); // Restore original mode
    return password;
}

char getch() {
    char c;
    DWORD mode, count;
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    ReadConsoleA(h, &c, 1, &count, NULL);
    SetConsoleMode(h, mode);
    return c;
}

int main() {
    const char* db_name = "D:/Uni/2nd Semester/Oop Project/Updated@21Jun/semesterProject2/Project/Project/dbb.db"; // Replace with the absolute path to your db;
    Node node(db_name);
    Blockchain blockchain(db_name);
    CPU_Miner cpuMiner("CPU Miner", 100, 4);
    Miner* gpuMiner = new GPU_Miner("GPU Miner", 200, 2);

    string username, password;
    int choice;

    while (true) {
        system("cls");
        cout << "\n========================================================================\n\n";
        cout << "                       A SIMPLE BLOCKCHAIN SIMULATION                    \n\n";
        cout << "========================================================================\n";

        cout << "Project by : Abubakkar , Saad Ullah , Abdullah Mir , Hanzla Rasheed\n";
        cout << "------------------------------------------------------------------------\n";
        cout << endl;

        cout << "[1]  Create an Account \n";
        cout << "[2]  Login to an existing Account \n";
        cout << "[3]  Exit \n";
        cout << "Enter your choice : ";
        cin >> choice;

        switch (choice) {
        case 1: {
            system("cls");
            cout << "                             SIGN UP                                    \n";
            cout << "------------------------------------------------------------------------\n";
            cout << endl;

            cout << "Enter username: ";
            cin >> username;
            cout << "Set password: ";
            cin >> password;

            if (node.signup(username, password)) {
                cout << "Signup successful! You may now Login.\n";
            }
            else {
                cout << "Signup failed. Please try again.\n";
            }
            cout << "\nPress Any Key to Go Back...";
            if (getch()) {
                break;
            }
        }
        case 2: {
            system("cls");
            cout << "\n                               LOGIN                                    \n";
            cout << "------------------------------------------------------------------------\n";
            cout << endl;
            cout << "Enter username: ";
            cin >> username;
            password = getPassword();

            if (node.login(username, password)) {
                cout << "Login successful!\n";
                bool logged_in = true;
                
                while (logged_in) {
                    int option;
                    system("cls");

                    cout << "\n                              DASHBOARD                                    \n";
                    cout << "------------------------------------------------------------------------\n";
                    cout << endl;
                    if (node.set_notfications() == true && node.notifications != 0) {
                        cout << "\nYou might have " << node.notifications << " new notifications\n";
                        cout << "------------------------------------------------------------------------\n";
                    }
                    cout << "\nChoose an option:\n";
                    cout << "[1]  View Blockchain\n";
                    cout << "[2]  View Transaction History\n";
                    cout << "[3]  Send Money\n";
                    cout << "[4]  View Wallet\n";
                    cout << "[5]  View Notifications\n";
                    cout << "[6]  Approve Transactions\n";
                    cout << "[7]  Logout\n";
                    cout << "Enter your choice: ";
                    cin >> option;

                    switch (option) {
                    case 1:
                        system("cls");
                        cout << "\n                           VIEW BLOCKCHAIN                            \n";
                        cout << "------------------------------------------------------------------------\n";
                        node.view_chain();
                        cout << "\nPress Any Key to Go Back...";
                        if (getch()) {
                            break;
                        }
                    case 2:
                        system("cls");
                        cout << "\n                        TRANSACTION HISTORY                           \n";
                        cout << "------------------------------------------------------------------------\n";
                        node.view_transaction_history();
                        cout << "\nPress Any Key to Go Back...";
                        if (getch()) {
                            break;
                        }
                    case 3: {
                        system("cls");
                        cout << "\n                            SEND MONEY                                \n";
                        cout << "------------------------------------------------------------------------\n";
                        string receiver_username;
                        double amount;
                        cout << "Enter receiver's username: ";
                        cin >> receiver_username;
                        cout << "Enter amount to send: ";
                        cin >> amount;

                        if (node.send_money(receiver_username, amount,node.getPrivateKey())) {

                            cpuMiner.mine();
                            cout << ".";
                            this_thread::sleep_for(chrono::seconds(1));
                            cout << ".";
                            this_thread::sleep_for(chrono::seconds(1));
                            cout << ".";
                            cout << endl;
                            gpuMiner->mine();
                            cout << ".";
                            this_thread::sleep_for(chrono::seconds(1));
                            cout << ".";
                            this_thread::sleep_for(chrono::seconds(1));
                            cout << ".";
                            cout << endl;
                            cout << "Transaction successful!\n";

                        }
                        else {
                            cout << "Transaction failed. Please check your balance and try again.\n";
                        }
                        cout << "\nPress Any Key to Go Back...";
                        if (getch()) {
                            break;
                        }
                    }
                    case 4:
                        system("cls");
                        cout << "\n                              VIEW WALLET                             \n";
                        cout << "------------------------------------------------------------------------\n";
                        cout << "\nCurrent Wallet Balance : " << node.get_wallet_balance();
                        cout << "\nPress Any Key to Go Back...";
                        if (getch()) {
                            break;
                        }
                    case 5:
                        system("cls");
                        cout << "\n                            NOTIFICATIONS                             \n";
                        cout << "------------------------------------------------------------------------\n";
                        node.get_notifications();
                        cout << "\nPress Any Key to Go Back...";
                        if (getch()) {
                            break;
                        }

                        //cout << "\n  ~~~> Dummy Node " << "(Username Here)" << " with Public ID : " << "(XXXXXXXXXX)" << " sent you "  << "(Amount Here)" << " coins.";
                        //cout << "\n........................................................................\n";
                        //cout << "\n  ~~~> Dummy Node " << "(Username Here)" << " with Public ID : " << "(XXXXXXXXXX)" << " sent you " << "(Amount Here)" << " coins.";
                        //cout << "\n........................................................................\n";
                        //cout << "\nPress Any Key to Go Back...";
                        //if (getch()) {
                        //    break;
                        //}
                    case 6:
                        refresh:
                        system("cls");
                        cout << "\n                           APPROVE BLOCKS                             \n";
                        cout << "------------------------------------------------------------------------\n";

                        if (node.view_pending_approvals() == true) {
                            cout << "\n..........................................................................\n";
                            cout << "\nEnter the Block ID you want to Approve or Enter 0 to Exit: ";
                            int transactiontoapprove = -1;
                            while (transactiontoapprove != 0) {
                                cin >> transactiontoapprove;
                                if (transactiontoapprove == 0) {
                                    break;
                                }
                                if (node.approve_transaction(transactiontoapprove) == true) {
                                    cout << "\nBlock Approved Successfully";
                                    goto refresh;
                                }
                                else {
                                    cout << "\nInvalid Block ID! Press 0 to Exit";
                                }
                            }
                        }
                        else {
                            cout << "\nWooohoo! No Pending Approvals Yet.";
                            cout << "\n..........................................................................\n";
                        }
                        cout << "\nPress Any Key to Go Back...";
                        if (getch()) {
                            break;
                        }

                    case 7:
                        logged_in = false;
                        break;
                    default:
                        system("cls");
                        cout << "Invalid option. Please try again.\n";
                        cout << "\nPress Any Key to Go Back...";
                        if (getch()) {
                            break;
                        }
                    }
                }
            }
            else {
                cout << "Login failed. Incorrect username or password.\n";
            }
            break;
        }
        case 3:
            cout << "Exiting...\n";
            delete gpuMiner;
            return 0;
        default:
            cout << "Invalid choice. Please enter a valid option.\n";
            break;
        }
    };

    return 0;
}

