#include<iostream> 
#include<string> // For string manipulation
#include<fstream> // For file handling (reading and writing files)
#include<unordered_map> // For storing account-password pairs using a hash map
using namespace std;
// Function to encrypt password
string encrypt(string password, int shift) {
    string encrypted = password;
    for (int i = 0; i < password.length(); i++) {
        encrypted[i] = (password[i] + shift) % 256;
    }
    return encrypted;
}
// Function to decrypt password
string decrypt(string encrypted, int shift) {
    string decrypted = encrypted;
    for (int i = 0; i < encrypted.length(); i++) {
        decrypted[i] = (encrypted[i] - shift + 256) % 256;
    }
    return decrypted;
}
// Class representing an account with a password
class Account {
protected:
    string encrypted_password; // Encrypted password is stored here
public:
    Account(){
        encrypted_password = "";
    }
    // Constructor that encrypts and stores the password
    Account(string password) {
        encrypted_password = encrypt(password, 3);
    }
    // Getter for the encrypted password
    string get_encrypted_password() const {
        return encrypted_password;
    }
    // Setter for the encrypted password
    void set_encrypted_password(const string& encrypted) {
        encrypted_password = encrypted;
    }
    // Decrypts and returns the original password
    string get_original() const {
        return decrypt(encrypted_password, 3);
    }
};
// Class to manage multiple accounts and their associated passwords
class PasswordManager {
    unordered_map<string, Account> accounts; // Stores accounts with passwords
    int original_pin; // Master PIN for access control
    int normalize_pin(int pin) { // Function to normalize a PIN to 4 digits
        return pin % 10000;
    }
public:
    void set_pin(int pin) {
        original_pin = normalize_pin(pin);
    }
    bool verify_pin(int entered_pin) { // Verify if the entered PIN matches the original PIN
        return normalize_pin(entered_pin) == original_pin;
    }
    void loadAccountsFromFile(const string& filename) { // Load existing accounts and passwords from a file
        ifstream file(filename);
        if (file.is_open()) {
            string account, encryptedPassword;
            while (file >> account >> encryptedPassword) {
                Account acc;
                acc.set_encrypted_password(encryptedPassword);
                accounts[account] = acc;
            }
            file.close();
        } else {
            cout << "No existing password file found. A new one will be created.\n";
        }
    }
    void saveAccountsToFile(const string& filename) { // Save all accounts and their passwords to a file
        ofstream file(filename);
        for (const auto& entry : accounts) {
            file << entry.first << " " << entry.second.get_encrypted_password() << endl;
        }
        file.close();
    }
    void addPassword(const string& account, const string& password) { // Add a new password for a given account
        accounts[account] = Account(password);
        cout << "Password added successfully for account: " << account << endl;
    }
    // Retrieve and display the password for a given account if the correct PIN is provided
    void retrievePassword(const string& account, int entered_pin) {
        if (verify_pin(entered_pin)) {
            if (accounts.find(account) != accounts.end()) {
                cout << "Password for " << account << ": " << accounts[account].get_original() << endl;
            } else {
                cout << "No password found for account: " << account << endl;
            }
        } else {
            cout << "Incorrect PIN. Access denied.\n";
        }
    }
    // Delete an account password if the correct PIN is provided
    void deletePassword(const string& account, int entered_pin) {
        if (verify_pin(entered_pin)) {
            if (accounts.erase(account)) { // Erase returns 1 if the account existed and was deleted
                cout << "Account '" << account << "' deleted successfully.\n";
            } else {
                cout << "No account found with the name '" << account << "'.\n";
            }
        } else {
            cout << "Incorrect PIN. Access denied.\n";
        }
    }
    // Display all stored accounts
    void displayAccounts() {
        if (accounts.empty()) {
            cout << "No passwords stored.\n";
        } else {
            cout << "Stored accounts:\n";
            for (const auto& entry : accounts) {
                cout << "- " << entry.first << endl;
            }
        }
    }
};
// Function to validate if a given string is a valid 4-digit PIN
bool is_valid_pin(const string& pin) {
    if (pin.length() != 4) return false; // Check length
    for (char c : pin) {
        if (!isdigit(c)) return false; // Check if all characters are digits
    }
    return true;
}
//Main Function
int main() {
    PasswordManager manager;
    const string FILENAME = "passwords.txt";
    string master_pin_str;
    cout << "Set your 4-digit master PIN: ";// Set the master PIN with validation
    cin >> master_pin_str;
    while (!is_valid_pin(master_pin_str)) {
        cout << "Invalid PIN. Please enter a valid 4-digit PIN: ";
        cin >> master_pin_str;
    }
    int master_pin = stoi(master_pin_str); // Set the master PIN with validation
    manager.set_pin(master_pin);
    manager.loadAccountsFromFile(FILENAME);// Load accounts from the file (if it exists)
    int choice;
    do {
        cout << "\n---- Password Manager Menu ----\n";
        cout << "1. Add New Password\n";
        cout << "2. Retrieve Password\n";
        cout << "3. Display All Accounts\n";
        cout << "4. Delete Account\n";
        cout << "5. Exit and Save\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1: {// Add new password
            string account, password;
            cout << "Enter account name: ";
            cin >> account;
            cout << "Enter password: ";
            cin >> password;
            manager.addPassword(account, password);
            break;
        }
        case 2: {// Retrieve password
            string account, pin_str;
            cout << "Enter account name to retrieve password: ";
            cin >> account;
            cout << "Enter your 4-digit PIN: ";
            cin >> pin_str;
            while (!is_valid_pin(pin_str)) {
                cout << "Invalid PIN. Please enter a valid 4-digit PIN: ";
                cin >> pin_str;
            }
            int enteredPin = stoi(pin_str);
            manager.retrievePassword(account, enteredPin);
            break;
        }
        case 3: // Display all accounts
            manager.displayAccounts();
            break;
        case 4: { // Delete account
            string account, pin_str;
            cout << "Enter account name to delete: ";
            cin >> account;
            cout << "Enter your 4-digit PIN: ";
            cin >> pin_str;
            while (!is_valid_pin(pin_str)) {
                cout << "Invalid PIN. Please enter a valid 4-digit PIN: ";
                cin >> pin_str;
            }
            int enteredPin = stoi(pin_str);
            manager.deletePassword(account, enteredPin);
            break;
        }
        case 5: // Save and exit
            manager.saveAccountsToFile(FILENAME);
            cout << "Passwords saved to file. Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 5);
    return 0;
}