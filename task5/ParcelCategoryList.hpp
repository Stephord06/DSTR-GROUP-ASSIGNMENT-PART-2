#include "../parcel.cpp"
#include <iostream>

using namespace std;

// Stores data in "info" and pointer to next node in "link"
struct NodeType
{
    Parcel info;    // Holds parcel record
    NodeType *link; // Pointer to the next node in the list

    // Constructor to initialize the node data and set the next pointer to NULL
    NodeType(Parcel p)
    {
        info = p;
        link = NULL;
    }
};

// Task 5: Parcel Category Organisation Module using Singly Linked List
class ParcelCategoryList
{
private:
    NodeType *head; // point to the first node in the list
    NodeType *tail; // point to the last node for O(1) quick insert
    int size;

    // Helper function to help validate user input values
    bool validateParcelData(Parcel p);

public:
    ParcelCategoryList();
    ~ParcelCategoryList();

    // Helper functions to convert enums into String
    string getCategoryName(ParcelType type);
    string getStatusName(ParcelStatus status);

    bool isEmpty();
    int getSize();

    // 2. Insert new parcel with duplicate ID validation
    bool insertParcel(Parcel p);

    // 3. Search function
    NodeType *searchById(int id);
    void searchByCategory(ParcelType type);
    void searchByStatus(ParcelStatus status);

    // 4. Display parcels
    void displayAll();
    void displayParcel(Parcel p);

    // 5. Update and delete functions
    bool updateParcel(int id, Parcel updatedData);
    bool removeParcel(int id);

    // Clean up memory
    void clear();
};