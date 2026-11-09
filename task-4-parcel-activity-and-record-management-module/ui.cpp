#include <cerrno>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <string>
#include "activity-management-stack.cpp"

using namespace std;

static bool readInteger(const string& prompt, int& value) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) return false;

    char *end = nullptr;
    errno = 0;
    const long parsed = strtol(line.c_str(), &end, 10);
    if (end == line.c_str() || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) {
        return false;
    }
    while (*end == ' ' || *end == '\t' || *end == '\r') ++end;
    if (*end != '\0') return false;
    value = static_cast<int>(parsed);
    return true;
}

static bool readTimestamp(string& timestamp) {
    cout << "Enter Timestamp (24-hour HHMM, e.g., 1530): ";
    if (!getline(cin, timestamp) || timestamp.size() != 4) return false;
    for (char digit : timestamp) {
        if (digit < '0' || digit > '9') return false;
    }
    const int hours = (timestamp[0] - '0') * 10 + (timestamp[1] - '0');
    const int minutes = (timestamp[2] - '0') * 10 + (timestamp[3] - '0');
    return hours < 24 && minutes < 60;
}

void ActivityModuleUI() {
    ActivityStack myActivityStack;
    int choice = -1;

    do {
        cout << "\n============================================\n";
        cout << " Task 4: Parcel & Record Management Module \n";
        cout << "============================================\n";
        cout << "1. Record Parcel Activity\n";
        cout << "2. Display Parcel Activity History\n";
        cout << "3. Undo Last Activity\n";
        cout << "4. Search a Parcel Activity\n";
        cout << "0. Exit\n";

        if (!readInteger("\nEnter your choice: ", choice)) {
            if (cin.eof()) return;
            cout << "[!] Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            int parcelID = 0, prevStatus = -1, newStatus = -1;
            string action, description, timestamp;

            if (!readInteger("Enter Parcel ID: ", parcelID) || parcelID <= 0) {
                cout << "[!] Parcel ID must be a positive number.\n";
                break;
            }
            cout << "Enter Activity Type: ";
            if (!getline(cin, action) || action.empty()) {
                cout << "[!] Activity type cannot be empty.\n";
                break;
            }
            cout << "Enter Activity Description: ";
            if (!getline(cin, description) || description.empty()) {
                cout << "[!] Activity description cannot be empty.\n";
                break;
            }
            if (!readInteger("Enter Previous Status (0=ORDERED, 1=SHIPPED, 2=DELIVERED): ", prevStatus) ||
                prevStatus < ORDERED || prevStatus > DELIVERED) {
                cout << "[!] Invalid previous status.\n";
                break;
            }
            if (!readInteger("Enter New Status (0=ORDERED, 1=SHIPPED, 2=DELIVERED): ", newStatus) ||
                newStatus < ORDERED || newStatus > DELIVERED) {
                cout << "[!] Invalid new status.\n";
                break;
            }
            if (!readTimestamp(timestamp)) {
                cout << "[!] Invalid timestamp. Use a valid 24-hour HHMM value.\n";
                break;
            }

            if (myActivityStack.push(parcelID, action, description,
                                     static_cast<ParcelStatus>(prevStatus),
                                     static_cast<ParcelStatus>(newStatus), timestamp)) {
                cout << "Activity recorded successfully!\n";
            }
            break;
        }
        case 2:
            myActivityStack.displayHistory();
            break;
        case 3: {
            ActivityNode lastActivity = myActivityStack.pop();
            if (lastActivity.activityID != -1) {
                cout << "Last activity undone successfully!\n"
                     << "Activity ID: " << lastActivity.activityID << '\n'
                     << "Parcel ID: " << lastActivity.parcelID << '\n'
                     << "Activity Type: " << lastActivity.activityType << '\n'
                     << "Activity Description: " << lastActivity.activityDescription << '\n'
                     << "Timestamp: " << lastActivity.timestamp << '\n';
            } else {
                cout << "[!] No activity to undo.\n";
            }
            break;
        }
        case 4: {
            int parcelID = 0;
            if (!readInteger("Enter Parcel ID to search: ", parcelID) || parcelID <= 0) {
                cout << "[!] Parcel ID must be a positive number.\n";
                break;
            }
            myActivityStack.searchByParcelID(parcelID);
            break;
        }
        case 0:
            cout << "Exiting Parcel Activity and Record Management Module.\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
}
