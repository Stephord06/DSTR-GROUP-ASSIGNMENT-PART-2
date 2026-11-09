#include <string>
#include <iostream>
#include <limits>
#include <cctype>
#include "activity-management-stack.cpp"

using namespace std;

void ActivityModuleUI() {
    ActivityStack myActivityStack;
    int choice;

    do {
        cout << "\n============================================\n";
        cout << " Task 4: Parcel & Record Management Module \n";
        cout << "============================================\n";
        cout << "1. Record Parcel Activity\n";
        cout << "2. Display Parcel Activity History\n";
        cout << "3. Undo Last Activity\n";
        cout << "4. Search a Parcel Activity\n";
        cout << "0. Exit\n";

        cout << "\nEnter your choice (0-4): ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[!] Invalid input. Please enter a number.\n" << endl;
            continue;
        }

        switch (choice) {
            case 1: {
                int parcelID, prevStatus, newStatus;
                string action, description, timestamp;

                cout << "Enter Parcel ID: ";
                cin >> parcelID;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "[!] Invalid input. Parcel ID should be a number.\n" << endl;
                    break;
                }

                cout << "Enter Activity Type (e.g., UPDATED_STATUS, DELIVERED, ORDERED, etc): ";
                cin.ignore();
                getline(cin, action);

                cout << "Enter Activity Description: ";
                getline(cin, description);

                cout << "Enter Previous Status (0: ORDERED, 1: ACCEPTED, 2: DELIVERED, 3: REJECTED): ";
                cin >> prevStatus;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "[!] Invalid input. Previous Status should be a number.\n" << endl;
                    break;
                }

                cout << "Enter New Status (0: ORDERED, 1: ACCEPTED, 2: DELIVERED, 3: REJECTED): ";
                cin >> newStatus;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "[!] Invalid input. New Status should be a number.\n" << endl;
                    break;
                }

                while (true) {
                    cout << "Enter Timestamp (24-hour format HHMM (e.g., 1530)): ";
                    cin >> timestamp;

                    if (timestamp.length() == 4 && 
                        isdigit(timestamp[0]) && isdigit(timestamp[1]) && 
                        isdigit(timestamp[2]) && isdigit(timestamp[3]) && 
                        stoi(timestamp.substr(0, 2)) < 24 && stoi(timestamp.substr(2, 2)) < 60) {
                        break;
                    } else {
                        cout << "[!] Invalid timestamp format. Please enter in HHMM format (0000-2359)." << endl;
                    }
                }

                myActivityStack.push(parcelID, action, description, (ParcelStatus)prevStatus, (ParcelStatus)newStatus, timestamp);
                cout << "Activity recorded successfully!\n" << endl;
                break;
            }
            case 2:
                cout << "\n============================================\n";
                cout << " Parcel Activity History (Linked List Stack) \n";
                cout << "============================================\n";
                myActivityStack.displayHistory();
                cout << "\n============================================\n";
                break;

            case 3:
                cout << "\n============================================\n";
                cout << " Undo Last Activity (Linked List Stack) \n";
                cout << "============================================\n";

                {
                    ActivityNode lastActivity = myActivityStack.pop();
                    if (lastActivity.activityID != -1) {
                        cout << "Last activity undone successfully!" << endl;
                        cout << "Activity ID: " << lastActivity.activityID << endl;
                        cout << "Parcel ID: " << lastActivity.parcelID << endl;
                        cout << "Activity Type: " << lastActivity.activityType << endl;
                        cout << "Activity Description: " << lastActivity.activityDescription << endl;
                        cout << "Previous Status: " << lastActivity.previousStatus << endl;
                        cout << "New Status: " << lastActivity.newStatus << endl;
                        cout << "Timestamp: " << lastActivity.timestamp << endl;
                    } else {
                        cout << "[!] No activity to undo." << endl;
                    }
                }
                break;

            case 4: {
                int targetParcelID;
                cout << "Enter Parcel ID to search: ";
                cin >> targetParcelID;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "[!] Invalid input. Parcel ID should be a number.\n" << endl;
                    break;
                }
                myActivityStack.displayTargetParcelActivity(targetParcelID);
                break;
            }
            case 0:
                cout << "Exiting Parcel Activity and Record Management Module." << endl;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);
}