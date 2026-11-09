#pragma once
#include <iostream>
#include <string>
#include "handleExceptionService.cpp"
#include "queue.cpp"

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

void serviceRequestInput(Queue& serviceQueue) {
    string requestID, parcelID, recipientName;
    ServiceType serviceTypeEnum;
    Status status = PENDING;

    while (true) {
        cout << "Enter Parcel ID: ";
        cin >> parcelID;

        if(!isValidParcelID(parcelID)){
            continue;
        }

        requestID = generateRequestID();
        cout << "Generated Request ID: " << requestID << endl;

        cin.ignore(1000, '\n');
        cout << "Enter Recipient Name: ";
        getline(cin, recipientName);

        if (recipientName.empty()) {
            cout << "Recipient name cannot be empty. Please try again." << endl;
            continue;
        }

        serviceTypeEnum = selectServiceType();
        break;
    }

    cout << "\nAll inputs are valid." << endl;
    cout << "Now proceeding to enqueue the service request..." << endl;

    serviceQueue.enqueue(requestID, parcelID, recipientName, serviceTypeEnum, status);

    cout << "Service request has been successfully added to the queue." << endl;
}
