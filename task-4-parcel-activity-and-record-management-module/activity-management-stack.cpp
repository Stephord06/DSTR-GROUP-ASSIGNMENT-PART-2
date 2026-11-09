#include <iostream>
#include <string>
#include "parcel.cpp"

using namespace std;

// Parameter Setup
struct ActivityNode {

    int activityID;
    int parcelID;
    string activityType;
    string activityDescription;

    ParcelStatus previousStatus;
    ParcelStatus newStatus;
    string timestamp;

    ActivityNode *next;

};

class ActivityStack {
private:
    ActivityNode *topNode;
    int count;

public:
    ActivityStack() {
        topNode = nullptr;
        count = 0;
    }

    bool isEmpty() {
        return topNode == nullptr;
    }


    // Push Function: To add the related data into the stack (Record the activity)
    void push(int pID, string action, string desc, ParcelStatus prev, ParcelStatus newStat, string time) {
        ActivityNode *newNode = new ActivityNode();
        newNode->activityID = count + 1;
        newNode->parcelID = pID; // Placeholder, should be set when pushing an activity
        newNode->activityType = action; // Placeholder, should be set when pushing an activity
        newNode->activityDescription = desc; // Placeholder, should be set when pushing an activity
        newNode->previousStatus = prev; // Placeholder, should be set when pushing an
        newNode->newStatus = newStat; // Placeholder, should be set when pushing an activity
        newNode->timestamp = time; // Placeholder, should be set when pushing an activity

        newNode->next = topNode;
        topNode = newNode;
    }

    // Pop Function: To remove the top data from the stack (Undo the last activity)
    ActivityNode pop(){
        if (isEmpty()) {
            cerr << "Stack is empty!" << endl;
            return{-1, -1, "", "", (ParcelStatus) 0, (ParcelStatus) 0, "", nullptr};
        }

        ActivityNode *temp = topNode;
        ActivityNode poppedNode = *temp;

        topNode = topNode->next;
        delete temp;
        count--;

        return poppedNode;
    }

    // Display the activity history in the stack
    void displayHistory(){
        if (isEmpty()){
            cout << "No activities to display." << endl;
            return;
        }

        ActivityNode *current = topNode;
        cout << "Parcel Activity History (Linked List Stack)" << endl;

        // Header for the activity history
        while (current != nullptr){
            cout << "\n-------------------------------------" << endl;
            cout << "Activity ID: " << current->activityID;
            cout << " | Parcel ID: " << current->parcelID;
            cout << " | Activity Type: " << current->activityType;
            cout << " | Activity Description: " << current->activityDescription;
            cout << " | Previous Status: " << current->previousStatus;
            cout << " | New Status: " << current->newStatus;
            cout << " | Timestamp: " << current->timestamp;
            cout << "---------------------------------------\n" << endl;

            current = current->next;
        }
    }


};