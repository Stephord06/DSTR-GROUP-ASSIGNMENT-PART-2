#pragma once
#include <iostream>
#include <string>
#include "queue.cpp"
#include "createServiceRequest.cpp"
#include "maintainActiveRequests.cpp"
#include "updateServiceRequest.cpp"
#include "../tree.cpp"

using namespace std;

// Single shared queue: defined once in resourceTask3.cpp so Task 3 sees the same requests.
extern Queue serviceQueue;

void displayServiceMenu(TreeNode* parcelTree) {
    while (true) {
        int selection = 0;
        cout << "\n========================================================" << endl;
        cout << "Welcome to the Parcel Service and Collection Management Module!" << endl;
        cout << "========================================================" << endl;
        cout << "Please select an option:" << endl;
        cout << "1. Add a new service request" << endl;
        cout << "2. View all service requests" << endl;
        cout << "3. Process a service request" << endl;
        cout << "4. Exit to main menu" << endl;

        cout << "Enter your selection (1, 2, 3, 4): ";
        cin >> selection;
        if (cin.fail()) {
            cout << "Invalid input. Please enter a valid option." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        switch (selection) {
            case 1:
                serviceRequestInput(parcelTree, serviceQueue);
                break;
            case 2:
                displayActiveRequests(serviceQueue);
                break;
            case 3:
                updateServiceRequestInQueue(serviceQueue);
                break;
            case 4:
                return; // Exit to main menu
            default:
                cout << "Invalid selection. Please try again." << endl;
        }
    }
}
