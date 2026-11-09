#pragma once
#include <iostream>
#include <limits>
#include <string>
#include "queue.cpp"

using namespace std;

void updateServiceRequestInQueue(Queue& serviceQueue) {
    if (serviceQueue.isEmpty()) {
        cout << "No active service requests to process." << endl;
        return;
    }

    int selection = 0;
    cout << "Processing request: " << serviceQueue.getFront() << endl;
    cout << "Enter one of the option below: " << endl;
    cout << "1. Mark as In Progress" << endl;
    cout << "2. Mark as Completed" << endl;
    cout << "3. Cancel Request" << endl;
    cout << "4. Delete Request" << endl;
    cout << "Enter your selection (1-4): ";
    if (!(cin >> selection)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid selection. No changes made." << endl;
        return;
    }

    switch (selection) {
        case 1:
            serviceQueue.front->status = IN_PROGRESS;
            cout << "Request " << serviceQueue.getFront() << " marked as In Progress." << endl;
            break;
        case 2:
            serviceQueue.front->status = COMPLETED;
            cout << "Request " << serviceQueue.getFront() << " marked as Completed." << endl;
            break;
        case 3:
            serviceQueue.front->status = CANCELED;
            cout << "Request " << serviceQueue.getFront() << " has been canceled." << endl;
            break;
        case 4:
            serviceQueue.deleteSpecificRequest(serviceQueue.getFront());
            return; // deleteSpecificRequest already removed the front node.
        default:
            cout << "Invalid selection. No changes made." << endl;
            return;
    }
    serviceQueue.dequeue();
}
