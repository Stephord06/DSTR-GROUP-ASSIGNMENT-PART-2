#include "../tree.cpp"
#include <iostream>
#include <string>

using namespace std;

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
    int nextActivityID;

public:
    ActivityStack() {
        topNode = nullptr;
        nextActivityID = 1;
    }

    ActivityStack(const ActivityStack &) = delete;
    ActivityStack &operator=(const ActivityStack &) = delete;

    ~ActivityStack() {
        while (topNode != nullptr) {
            ActivityNode *next = topNode->next;
            delete topNode;
            topNode = next;
        }
    }

    bool isEmpty() {
        return topNode == nullptr;
    }

    void push(int pID, string action, string desc, ParcelStatus prev, ParcelStatus newStat, string time) {
        ActivityNode *newNode = new ActivityNode{};
        newNode->activityID = nextActivityID++;
        newNode->parcelID = pID;
        newNode->activityType = action;
        newNode->activityDescription = desc;
        newNode->previousStatus = prev;
        newNode->newStatus = newStat;
        newNode->timestamp = time;

        newNode->next = topNode;
        topNode = newNode;
    }

    ActivityNode pop(){
        if (isEmpty()) {
            cerr << "Stack is empty!" << endl;
            return ActivityNode{-1, -1, "", "", (ParcelStatus)0, (ParcelStatus)0, "", nullptr};
        }

        ActivityNode *temp = topNode;
        ActivityNode poppedNode = *temp;
        poppedNode.next = nullptr;

        topNode = topNode->next;
        delete temp;

        return poppedNode;
    }

    void displayHistory(){
        if (isEmpty()){
            cout << "No activities to display." << endl;
            return;
        }

        ActivityNode *current = topNode;
        cout << "Parcel Activity History (Linked List Stack)" << endl;

        while (current != nullptr){
            cout << "\n-------------------------------------" << endl;
            cout << "Activity ID: " << current->activityID;
            cout << " | Parcel ID: " << current->parcelID;
            cout << " | Activity Type: " << current->activityType;
            cout << " | Activity Description: " << current->activityDescription;
            cout << " | Previous Status: " << status_to_string(current->previousStatus);
            cout << " | New Status: " << status_to_string(current->newStatus);
            cout << " | Timestamp: " << current->timestamp;
            cout << "---------------------------------------\n" << endl;

            current = current->next;
        }
    }

    void displayTargetParcelActivity(int targetParcelID) {
        if (isEmpty()) {
            cout << "No activities to display." << endl;
            return;
        }

        ActivityNode *current = topNode;
        bool found = false;

        cout << "Searching for activities related to Parcel ID: " << targetParcelID << endl;

        while (current != nullptr) {
            if (current->parcelID == targetParcelID) {
                found = true;
                cout << "\n-------------------------------------" << endl;
                cout << "Activity ID: " << current->activityID;
                cout << " | Parcel ID: " << current->parcelID;
                cout << " | Activity Type: " << current->activityType;
                cout << " | Activity Description: " << current->activityDescription;
                cout << " | Previous Status: " << status_to_string(current->previousStatus);
                cout << " | New Status: " << status_to_string(current->newStatus);
                cout << " | Timestamp: " << current->timestamp;
                cout << "---------------------------------------\n" << endl;
            }
            current = current->next;
        }

        if (found == false) {
            cout << "[!] No activities found for Parcel ID: " << targetParcelID << endl;
        }
    }
};