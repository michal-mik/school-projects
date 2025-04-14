#include <iostream>
#include <map>
#include <queue>
#include <thread>
#include <chrono>
#include <future>

using namespace std;

struct Contact {
    string firstName;
    string lastName;
    string phoneNumber;
    int answered = 0;
    int missed = 0;
};

struct Trie {
    map<char, Trie*> children;
    map<string, Contact> contacts;
    bool end = false;
};

class PhoneBook {
private:
    Trie* root;
    queue<Contact*> callQueue;

public:
    PhoneBook() {
        root = new Trie();
    }

    void addNumber(string lastName, string firstName, string phoneNumber) {
        Trie* node = root;
        for (char c : lastName) {
            if (!node->children[c]) {
                node->children[c] = new Trie();
            }
            node = node->children[c];
        }

        node->end = true;
        node->contacts[firstName] = {firstName, lastName, phoneNumber};
        cout << "Contact " << firstName << " " << lastName << " - " << phoneNumber << " was saved\n" << endl;
    }

    Contact* findNumber(string lastName, string firstName) {
        Trie* node = root;
        for (char c : lastName) {
            if (!node->children[c]) return nullptr;
            node = node->children[c];
        }
        if (node->end && node->contacts.count(firstName)) return &node->contacts[firstName];
        return nullptr;
    }

    void removeNumber(string lastName, string firstName) {
        Contact* contact = findNumber(lastName, firstName);
        if (contact != nullptr) {
            Trie* node = root;
            for (char c : lastName) node = node->children[c];
            node->contacts.erase(firstName);
            cout << "Contact " << lastName << " was deleted\n" << endl;
        } else {
            cout << "Contact " << lastName << " was not found\n" << endl;
        }
    }

    void addToQueue(string lastName, string firstName) {
        Contact* contact = findNumber(lastName, firstName);
        if (contact != nullptr) {
            callQueue.push(contact);
            cout << "Contact " << firstName << " was added to queue\n" << endl;
        } else {
            cout << "Contact " << firstName << " was not found\n" << endl;
        }
    }

    void startQueueCall() {
        if (callQueue.empty()) {
            cout << "No inserted numbers in queue\n" << endl;
            return;
        }

        while (!callQueue.empty()) {
            Contact* contact = callQueue.front();
            callQueue.pop();

            if (!contact) continue;

            cout << "Calling to " << contact->firstName << " " << contact->lastName << " ..." << endl;
            cout << "Press 'a' to accept the call" << endl;
            cout << "Press 'q' to decline the call" << endl;

            future<char> futureInput = async(launch::async, []() {
                char input;
                cin >> input;
                return input;
            });

            if (futureInput.wait_for(chrono::seconds(10)) == future_status::ready) {
                char input = futureInput.get();
                if (input == 'a') {
                    cout << "Call accepted\n" << endl;
                    contact->answered++;
                } else if (input == 'q') {
                    cout << "Call declined\n" << endl;
                    contact->missed++;
                } else {
                    cout << "Invalid input\n" << endl;
                    return;
                }
            } else {
                cout << "Call timed out\n" << endl;
                cout << "Press any button to continue in queue\n" << endl;
                contact->missed++;
            }

            cout << "Answered: " << contact->answered << ", Missed: " << contact->missed << "\n" << endl;

            if (contact->missed >= 3) {
                cout << "Phone number was deleted due to inactivity\n" << endl;
                removeNumber(contact->lastName, contact->firstName);
            }
        }
    }
};

int main() {
    PhoneBook phoneBook;
    int choice;

    while (true) {
        cout << "[1] Add contact" << endl;
        cout << "[2] Find contact" << endl;
        cout << "[3] Remove contact" << endl;
        cout << "[4] Add to queue" << endl;
        cout << "[5] Start queue call" << endl;
        cout << "[6] Quit" << endl;

        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input\n" << endl;
            continue;
        }

        if (choice == 6) break;

        string lastName, firstName, phoneNumber;

        switch (choice) {
            case 1:
                cout << "Enter last name: ";
                cin >> lastName;
                cout << "Enter first name: ";
                cin >> firstName;
                cout << "Enter phone number: ";
                cin >> phoneNumber;
                phoneBook.addNumber(lastName, firstName, phoneNumber);
                break;

            case 2:
                cout << "Enter last name: ";
                cin >> lastName;
                cout << "Enter first name: ";
                cin >> firstName;
                if (Contact* contact = phoneBook.findNumber(lastName, firstName)) {
                    cout << contact->firstName << " " << contact->lastName << " - " << contact->phoneNumber << endl;
                } else {
                    cout << "Contact not found\n" << endl;
                }
                break;

            case 3:
                cout << "Enter last name: ";
                cin >> lastName;
                cout << "Enter first name: ";
                cin >> firstName;
                phoneBook.removeNumber(lastName, firstName);
                break;

            case 4:
                cout << "Enter last name: ";
                cin >> lastName;
                cout << "Enter first name: ";
                cin >> firstName;
                phoneBook.addToQueue(lastName, firstName);
                break;

            case 5:
                phoneBook.startQueueCall();
                break;
        }
    }
    return 0;
}