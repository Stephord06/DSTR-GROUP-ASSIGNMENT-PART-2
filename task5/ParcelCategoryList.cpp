#include "ParcelCategoryList.hpp"
#include <iomanip>

using namespace std;

// Constructor for initializing list with empty pointers and zero size
ParcelCategoryList::ParcelCategoryList()
{
    this->head = NULL;
    this->tail = NULL;
    this->size = 0;
}

// Destructor which free all heap memory when the object is removed
ParcelCategoryList::~ParcelCategoryList()
{
    clear();
}

// Helper to convert category enum to String
string ParcelCategoryList::getCategoryName(ParcelType type)
{
    switch (type)
    {
    case DOCUMENT:
        return "DOCUMENT";

    case ELECTRONIC:
        return "ELECTRONIC";

    case FRAGILE_GOODS:
        return "Fragile Goods";

    case GENERAL_PARCEL:
        return "General Parcel";

    default:
        return "Unknown";
    }
}

// Helper to convert status enum to String
string ParcelCategoryList::getStatusName(ParcelStatus status)
{
    switch (status)
    {
    case ORDERED:
        return "Ordered";

    case SHIPPED:
        return "Shipped";

    case DELIVERED:
        return "Delivered";

    case COLLECTED:
        return "Collected";

    case RETURNED:
        return "Returned";

    case CANCELLED:
        return "Cancelled";

    default:
        return "Unknown";
    }
}

// Check if linked list has no elements
bool ParcelCategoryList::isEmpty()
{
    return this->head == NULL;
}

// Return the current number of stored parcels
int ParcelCategoryList::getSize()
{
    return this->size;
}

// Check if parcel ids are positive and strings are not empty
bool ParcelCategoryList::validateParcelData(Parcel p)
{
    if (p.parcelID <= 0 || p.recipientID <= 0)
    {
        cout << "[Validation Error] ID must be positive integers greater than 0.\n";
        return false;
    }
    if (p.recipient_name == "" || p.delivery_address == "" || p.arrival_date == "")
    {
        cout << "[Validation Error] Name, Address, and Arrival Date cannot be blank.\n";
        return false;
    }
    return true;
}

// Insert new parcel at the end of the list
bool ParcelCategoryList::insertParcel(Parcel p)
{
    // Step 1: Validate the input fields
    if (!validateParcelData(p))
    {
        return false;
    }

    // Step 2: Prevent duplicate parcel ID
    if (searchById(p.parcelID) != NULL)
    {
        cout << "[Duplication Error] Parcel ID " << p.parcelID << " is already existed in the system!\n";
        return false;
    }

    // Step 3: Dynamically allocte new node using pointer
    NodeType *newNode = new NodeType(p);

    // Step 4: Insert node into the Singly Linked List
    if (isEmpty())
    {
        // If the list is empty, both head and tail point to the first node
        head = newNode;
        tail = newNode;
    }
    else
    {
        // Link new node after tail and update the tail pointer
        tail->link = newNode;
        tail = newNode;
    }

    size++;
    cout << "[Success] Parcel ID " << p.parcelID << " has been added successfully.\n";
    return true;
}

// Linear Search to find the parcel by its ID
NodeType *ParcelCategoryList::searchById(int id)
{
    NodeType *current = head;

    // Start searching from head until reaching NULL
    while (current != NULL)
    {
        if (current->info.parcelID == id)
        {
            return current; // Found the parcel
        }
        current = current->link; // Move to the next node
    }
    return NULL; // Return NULL if not found
}

// Filter and print all parcels belong to the selected category
void ParcelCategoryList::searchByCategory(ParcelType type)
{
    // Check if list is empty
    if (isEmpty())
    {
        cout << "[Notice] Cannot filter category as the list is currently empty.\n";
        return;
    }

    int matchCount = 0;
    NodeType *current = head;

    cout << "\n================================ PARCEL UNDER CATEGORY: " << getCategoryName(type) << " ================================\n";
    cout << left << setw(10) << "ParcelID"
         << setw(14) << "RecipientID"
         << setw(20) << "Recipient Name"
         << setw(16) << "Status"
         << setw(10) << "Fragile"
         << setw(14) << "Arrival Date"
         << "Delivery Address\n";
    cout << string(120, '-') << "\n";

    // Loop through all Nodes and print the matching ones
    while (current != NULL)
    {
        if (current->info.type == type)
        {
            cout << left << setw(10) << current->info.parcelID
                 << setw(14) << current->info.recipientID
                 << setw(20) << current->info.recipient_name
                 << setw(16) << getStatusName(current->info.status)
                 << setw(10) << (current->info.fragile ? "Yes" : "No")
                 << setw(14) << current->info.arrival_date
                 << current->info.delivery_address << "\n";
            matchCount++;
        }
        current = current->link;
    }

    cout << string(120, '=') << "\n";

    if (matchCount == 0)
    {
        cout << "No parcels found matching this category.\n";
    }
    else
    {
        cout << "Total records found: " << matchCount << "\n";
    }
}

// Filter and print all the parcels matching a selected delivery status
void ParcelCategoryList::searchByStatus(ParcelStatus status)
{
    if (isEmpty())
    {
        cout << "[Notice] Cannot filter status as the list is currently empty.\n";
        return;
    }

    int matchCount = 0;
    NodeType *current = head;

    cout << "\n================================ PARCEL WITH STATUS: " << getStatusName(status) << " ================================\n";
    cout << left << setw(10) << "ParcelID"
         << setw(16) << "Category"
         << setw(20) << "Recipient Name"
         << setw(10) << "Fragile"
         << setw(14) << "Arrival Date"
         << "Delivery Address\n";
    cout << string(120, '-') << "\n";

    while (current != NULL)
    {
        if (current->info.status == status)
        {
            cout << left << setw(10) << current->info.parcelID
                 << setw(16) << getCategoryName(current->info.type)
                 << setw(20) << current->info.recipient_name
                 << setw(10) << (current->info.fragile ? "Yes" : "No")
                 << setw(14) << current->info.arrival_date
                 << current->info.delivery_address << "\n";
            matchCount++;
        }

        current = current->link;
    }

    cout << string(120, '=') << "\n";

    if (matchCount == 0)
    {
        cout << "No parcels found with this status\n";
    }
    else
    {
        cout << "Total records found: " << matchCount << "\n";
    }
}

// Display single parcel in card format
void ParcelCategoryList::displayParcel(Parcel p)
{
    cout << "\n------------- Parcel Details -------------\n";
    cout << "Parcel ID        : " << p.parcelID << "\n";
    cout << "Recipient ID     : " << p.recipientID << "\n";
    cout << "Recipient Name   : " << p.recipient_name << "\n";
    cout << "Delivery Address : " << p.delivery_address << "\n";
    cout << "Arrival Date     : " << p.arrival_date << "\n";
    cout << "Category (Type)  : " << getCategoryName(p.type) << "\n";
    cout << "Current Status   : " << getStatusName(p.status) << "\n";
    cout << "Fragile Handling : " << (p.fragile ? "Yes" : "No") << "\n";
    cout << "------------------------------------------\n";
}

// Display all stored parcels in a formatted table
void ParcelCategoryList::displayAll()
{
    if (isEmpty())
    {
        cout << "[Notice] The list is empty. No parcels to be shown.\n";
        return;
    }

    cout << "\n============================================ ALL STORED PARCEL RECORDS ============================================\n";
    cout << left << setw(10) << "ParcelID"
         << setw(14) << "RecipientID"
         << setw(18) << "Recipient"
         << setw(16) << "Category"
         << setw(14) << "Status"
         << setw(9) << "Fragile"
         << setw(13) << "Arrival"
         << "Address\n";
    cout << string(125, '-') << "\n";

    NodeType *current = head;
    while (current != NULL)
    {
        cout << left << setw(10) << current->info.parcelID
             << setw(14) << current->info.recipientID
             << setw(18) << current->info.recipient_name
             << setw(16) << getCategoryName(current->info.type)
             << setw(14) << getStatusName(current->info.status)
             << setw(9) << (current->info.fragile ? "Yes" : "No")
             << setw(13) << current->info.arrival_date
             << current->info.delivery_address << "\n";

        current = current->link;
    }

    cout << string(125, '=') << "\n";
    cout << "Total Records: " << size << "\n";
}

// Update existing parcel while preserving their parcelId
bool ParcelCategoryList::updateParcel(int id, Parcel updatedData)
{
    if (isEmpty())
    {
        cout << "[Error] List is empty. Cannot be updated.\n";
        return false;
    }

    // Locate the parcel node
    NodeType *targetNode = searchById(id);
    if (targetNode == NULL)
    {
        cout << "[Error] Parcel with ID " << id << " can't be found.\n";
        return false;
    }

    // Validate the new values to avoid data appending error
    if (!validateParcelData(updatedData))
    {
        return false;
    }

    // Update parcel properties (ParcelId remain unchanged)
    targetNode->info.recipientID = updatedData.recipientID;
    targetNode->info.recipient_name = updatedData.recipient_name;
    targetNode->info.delivery_address = updatedData.delivery_address;
    targetNode->info.arrival_date = updatedData.arrival_date;
    targetNode->info.type = updatedData.type;
    targetNode->info.status = updatedData.status;
    targetNode->info.fragile = updatedData.fragile;

    cout << "[Success] Parcel ID " << id << " has been updated successfully!\n";
    return true;
}

// Remove parcel node from Singly Linked List and reconnect surrounding pointers
bool ParcelCategoryList::removeParcel(int id)
{
    if (isEmpty())
    {
        cout << "[Error] List is empty. There is nothing to delete.\n";
        return false;
    }

    NodeType *toDelete = NULL;

    // Case 1: Target parcel is at the first node
    if (head->info.parcelID == id)
    {
        toDelete = head;
        head = head->link; // Move head pointer to the next node

        // If the list only has 1 Node and is empty after deletion, reset the tail to NULL
        if (head == NULL)
        {
            tail = NULL;
        }

        delete toDelete; // Free up the allocated memory
        size--;
        cout << "[Success] Parcel ID " << id << " has been deleted successfully!\n";
        return true;
    }

    // Case 2: Target parcel is in the middle or in the end of the node
    // Use two pointers (prev and current) to maintain the node linking
    NodeType *prev = head;
    NodeType *current = head->link;

    while (current != NULL && current->info.parcelID != id)
    {
        prev = current;
        current = current->link;
    }

    // If current reached NULL, return ID was not found
    if (current == NULL)
    {
        cout << "[Error] Parcel ID " << id << " was not found in the list.\n";
        return false;
    }

    // Bypass the target Node
    toDelete = current;
    prev->link = current->link;

    // If deleting the tail node, update the tail pointer to previous node
    if (current == tail)
    {
        tail = prev;
    }

    delete toDelete; // free up the allocated memory
    size--;
    cout << "[Success] Parcel ID " << id << " has been deleted successfully!\n";
    return true;
}

// Delete all nodes to free memory
void ParcelCategoryList::clear()
{
    NodeType *current = head;
    while (current != NULL)
    {
        NodeType *nextNode = current->link; // save next pointer before deleting current
        delete current;                     // Delete current Node
        current = nextNode;                 // Advance to next Node
    }
    head = NULL;
    tail = NULL;
    size = 0;
}