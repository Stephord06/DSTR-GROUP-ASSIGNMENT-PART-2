#pragma once
#include <iostream>
#include <string>
#include "queue.cpp"
#include "../tree.cpp"

using namespace std;

string generateRequestID() {
    static int counter = 1;
    string requestID = "REQ" + to_string(counter);
    counter++;
    return requestID;
}

ServiceType selectServiceType() {
    int choice = 0;
    while (true) {
        cout << "\nService Type Options:" << endl;
        cout << "1. Recipient Collection" << endl;
        cout << "2. Campus Delivery" << endl;
        cout << "3. Special Handling" << endl;
        cout << "4. Re-Delivery" << endl;
        cout << "5. Return to Sender" << endl;
        cout << "Enter Service Type (1, 2, 3, 4, 5): ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number between 1 and 5." << endl;
            continue;
        }

        switch (choice) {
            case 1:
                return RECIPIENT_COLLECTION;
            case 2:
                return CAMPUS_DELIVERY;
            case 3:
                return SPECIAL_HANDLING;
            case 4:
                return RE_DELIVERY;
            case 5:
                return RETURN_TO_SENDER;
            default:
                cout << "Invalid selection. Please try again." << endl;
        }
    }
}

void serviceRequestInput(TreeNode* parcelTree, Queue& serviceQueue) {
    string requestID, recipientName;
    int parcelID = 0;
    ServiceType serviceTypeEnum;
    Status status = PENDING;

    while (true) {

        cout << "Enter Parcel ID: ";
        if (!(cin >> parcelID)) {
            cout << "Invalid input. Please enter a valid numerical Parcel ID." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            return;
        }

        Parcel* foundParcel = (parcelTree != nullptr) ? parcelTree->search_by_id(parcelID) : nullptr; // Validate the parcel ID against the parcel tree

        // Check if the parcel exists in the tree
        if(foundParcel == nullptr) {
            cout << "Invalid Parcel ID. No parcel found with ID " << parcelID << "." << endl;
            return;
        }

        // Check if the parcel has already been delivered
        if (foundParcel->status == ParcelStatus::DELIVERED) {
            cout << "The parcel with ID " << parcelID << " has already been delivered. Cannot create a service request for it." << endl;
            return;
        }

        Node* curr = serviceQueue.front;
        while (curr != nullptr) {
            if (curr->parcelID == parcelID) {
                cout << "A service request for Parcel ID " << parcelID << " already exists in the queue. Cannot create a duplicate request." << endl;
                return; // Exit the function to prevent adding a duplicate request
            }
            curr = curr->next; // Move to the next node
        }

        //auto generate a unique request ID for the new service request
        requestID = generateRequestID();
        cout << "Generated Request ID: " << requestID << endl;

        cin.ignore(1000, '\n');
        
        recipientName = foundParcel->recipient_name;

        serviceTypeEnum = selectServiceType();
        break;
    }

    cout << "\nAll inputs are valid." << endl;
    cout << "Now proceeding to enqueue the service request..." << endl;

    serviceQueue.enqueue(requestID, parcelID, recipientName, serviceTypeEnum, status);

    cout << "Service request has been successfully added to the queue." << endl;
}
