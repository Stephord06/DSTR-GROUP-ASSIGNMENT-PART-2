#pragma once
#include <iostream>
#include <string>   
#include "queue.cpp"

using namespace std;

string statusToString(Status status) {
    switch (status) {
        case PENDING: return "Pending";
        case IN_PROGRESS: return "In Progress";
        case COMPLETED: return "Completed";
        default: return "Unknown";
    }
}

string serviceTypeToString(ServiceType type) {
    switch (type) {
        case RECIPIENT_COLLECTION: return "Recipient Collection";
        case CAMPUS_DELIVERY: return "Campus Delivery";
        case SPECIAL_HANDLING: return "Special Handling";
        case RE_DELIVERY: return "Re-Delivery";
        case RETURN_TO_SENDER: return "Return to Sender";
        default: return "Unknown";
    }
}

void display(Queue& serviceQueue) {
    if (serviceQueue.isEmpty()) {
        cout << "No active requests to display." << endl;
        return;
    }

    int index = 0;
    Node* current = serviceQueue.front;
    cout << "\nActive Service Requests (Front to Rear):" << endl;

    while (current != nullptr) {
        index++; // show the order of the request in the queue

        cout << index << ":\tRequest ID: " << current->requestID 
             << ", Parcel ID: " << current->parcelID
             << ", Recipient Name: " << current->recipientName 
             << ", Service Type: " << serviceTypeToString(current->serviceType)
             << ", Status: " << statusToString(current->status) << endl;
             
        current = current->next;
    }
}   

void displayActiveRequests(Queue& serviceQueue) {
    cout << "Displaying active requests..." << endl;
    display(serviceQueue);
}
