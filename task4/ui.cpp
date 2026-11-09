#include <string>
#include <iostream>
#include <limits>
#include <cctype>
#include "activity-management-stack.cpp"

using namespace std;

void ActivityModuleUI(TreeNode &parcels) {
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
                int parcelID, newStatus;
                string action, description, timestamp;

                cout << "Enter Parcel ID: ";
                cin >> parcelID;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "[!] Invalid input. Parcel ID should be a number.\n" << endl;
                    break;
                }

                TreeNode *parcelNode = parcels.search_by_id(parcelID);
                if (!parcelNode) {
                    cout << "[!] Parcel not found. Register it in Task 1 before recording activity.\n" << endl;
                    break;
                }

                cout << "Enter Activity Type (e.g., UPDATED_STATUS, DELIVERED, ORDERED, etc): ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, action);

                cout << "Enter Activity Description: ";
                getline(cin, description);

                while (true) {
                    cout << "Enter New Status (0: ORDERED, 1: SHIPPED, 2: DELIVERED, 3: ON_HOLD): ";
                    cin >> newStatus;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "[!] Invalid input. Status should be a number from 0 to 3.\n";
                        continue;
                    }
                    if (newStatus >= ORDERED && newStatus <= ON_HOLD) {
                        break;
                    }
                    cout << "[!] Invalid status. Enter a number from 0 to 3.\n";
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

                ParcelStatus previousStatus = parcelNode->parcel.status;
                ParcelStatus updatedStatus = static_cast<ParcelStatus>(newStatus);
                myActivityStack.push(parcelID, action, description, previousStatus, updatedStatus, timestamp);
                parcelNode->parcel.status = updatedStatus;
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
                        TreeNode *parcelNode = parcels.search_by_id(lastActivity.parcelID);
                        if (parcelNode) {
                            parcelNode->parcel.status = lastActivity.previousStatus;
                            cout << "Last activity undone successfully!" << endl;
                        } else {
                            cout << "[!] Activity removed from history, but the parcel no longer exists; its status could not be restored." << endl;
                        }
                        cout << "Activity ID: " << lastActivity.activityID << endl;
                        cout << "Parcel ID: " << lastActivity.parcelID << endl;
                        cout << "Activity Type: " << lastActivity.activityType << endl;
                        cout << "Activity Description: " << lastActivity.activityDescription << endl;
                        cout << "Previous Status: " << status_to_string(lastActivity.previousStatus) << endl;
                        cout << "New Status: " << status_to_string(lastActivity.newStatus) << endl;
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
                TreeNode *parcelNode = parcels.search_by_id(targetParcelID);
                if (!parcelNode) {
                    cout << "[!] Parcel not found.\n" << endl;
                    break;
                }
                cout << "\nParcel: " << parcelNode->parcel.recipient_name
                     << " | Current Status: " << status_to_string(parcelNode->parcel.status)
                     << endl;
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