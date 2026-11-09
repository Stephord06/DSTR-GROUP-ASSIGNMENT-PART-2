#include <iostream>
#include <new>
#include <string>
#include "parcel.cpp"

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

    static const char *statusText(ParcelStatus status) {
        switch (status) {
            case ORDERED: return "Ordered";
            case SHIPED: return "Shipped";
            case DELIVERED: return "Delivered";
            default: return "Unknown";
        }
    }

public:
    ActivityStack() : topNode(nullptr), nextActivityID(1) {}
    ActivityStack(const ActivityStack&) = delete;
    ActivityStack& operator=(const ActivityStack&) = delete;

    ~ActivityStack() {
        while (topNode != nullptr) {
            ActivityNode *temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }

    bool push(int pID, const string& action, const string& desc,
              ParcelStatus prev, ParcelStatus newStat, const string& time) {
        ActivityNode *newNode = new (nothrow) ActivityNode;
        if (!newNode) {
            cerr << "Memory allocation failed; activity was not recorded." << endl;
            return false;
        }

        newNode->activityID = nextActivityID++;
        newNode->parcelID = pID;
        newNode->activityType = action;
        newNode->activityDescription = desc;
        newNode->previousStatus = prev;
        newNode->newStatus = newStat;
        newNode->timestamp = time;
        newNode->next = topNode;
        topNode = newNode;
        return true;
    }

    ActivityNode pop() {
        if (isEmpty()) {
            cerr << "Stack is empty!" << endl;
            return {-1, -1, "", "", ORDERED, ORDERED, "", nullptr};
        }

        ActivityNode *temp = topNode;
        ActivityNode poppedNode = *temp;
        topNode = topNode->next;
        poppedNode.next = nullptr;
        delete temp;
        return poppedNode;
    }

    void displayHistory() const {
        if (isEmpty()) {
            cout << "No activities to display." << endl;
            return;
        }

        const ActivityNode *current = topNode;
        cout << "Parcel Activity History (Linked List Stack)" << endl;
        while (current != nullptr) {
            cout << "\n-------------------------------------" << endl;
            cout << "Activity ID: " << current->activityID
                 << " | Parcel ID: " << current->parcelID
                 << " | Activity Type: " << current->activityType
                 << " | Activity Description: " << current->activityDescription
                 << " | Previous Status: " << statusText(current->previousStatus)
                 << " | New Status: " << statusText(current->newStatus)
                 << " | Timestamp: " << current->timestamp
                 << "\n---------------------------------------\n";
            current = current->next;
        }
    }

    void displayTargetParcelActivity(int targetParcelID) const {
        const ActivityNode *current = topNode;
        bool found = false;
        while (current != nullptr) {
            if (current->parcelID == targetParcelID) {
                if (!found) {
                    cout << "Searching for activities related to Parcel ID: " << targetParcelID << endl;
                    found = true;
                }
                cout << "Activity ID: " << current->activityID
                     << " | Parcel ID: " << current->parcelID
                     << " | Activity Type: " << current->activityType
                     << " | Activity Description: " << current->activityDescription
                     << " | Previous Status: " << statusText(current->previousStatus)
                     << " | New Status: " << statusText(current->newStatus)
                     << " | Timestamp: " << current->timestamp << endl;
            }
            current = current->next;
        }
        if (!found) {
            cout << "[!] No activities found for Parcel ID: " << targetParcelID << endl;
        }
    }
};
