#include "ParcelCategoryTree.hpp"
#include <iomanip>

using namespace std;

// Constructor for initializing empty tree with root pointing to null and 0 count
ParcelCategoryTree::ParcelCategoryTree()
{
    this->root = nullptr;
    this->count = 0;
}

// Destructor which frees all heap memory when the object is removed
ParcelCategoryTree::~ParcelCategoryTree()
{
    clear();
}

// Post-order traversal to delete children first before deleting parent node
void ParcelCategoryTree::destroyTree(CategoryTreeNode *node)
{
    if (node != nullptr)
    {
        destroyTree(node->left);  // Delete left subtree first
        destroyTree(node->right); // Delete right subtree next
        delete node;              // Free current node memory
    }
}

// Clear the whole tree and reset the root
void ParcelCategoryTree::clear()
{
    destroyTree(root);
    root = nullptr;
    count = 0;
}

// Check if tree has no nodes
bool ParcelCategoryTree::isEmpty() const
{
    return root == nullptr;
}

// Return the total records in the tree
int ParcelCategoryTree::getCount() const
{
    return count;
}

// Helper to convert category enum to String
string ParcelCategoryTree::getCategoryName(ParcelType type)
{
    switch (type)
    {
    case DOCUMENT:
        return "Document";

    case ELECTRONIC:
        return "Electronic";

    case FRAGILE_GOODS:
        return "Fragile Goods";

    case GENERAL_PARCEL:
        return "General Parcel";

    default:
        return "Unknown";
    }
}

// Helper to convert status enum to String
string ParcelCategoryTree::getStatusName(ParcelStatus status)
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

    case ON_HOLD:
        return "On hold";

    default:
        return "Unknown";
    }
}

// Check if parcel IDs are positive and strings are not empty before inserting or updating
bool ParcelCategoryTree::validateParcelData(const Parcel &p)
{
    // ID must be a positive integer
    if (p.parcelID <= 0 || p.recipientID <= 0)
    {
        cout << "[Validation Error] Parcel ID and Recipient ID must be greater than 0.\n";
        return false;
    }
    // String fields cannot be blank
    if (p.recipient_name.empty() || p.delivery_address.empty() || p.arrival_date.empty())
    {
        cout << "[Validation Error] Recipient Name, Delivery Address, and Arrival Date cannot be blank.\n";
        return false;
    }
    return true;
}

// Recursive insert (left < root < right)
CategoryTreeNode *ParcelCategoryTree::insertNode(CategoryTreeNode *node, Parcel p, bool &success)
{
    // Find an empty spot and allocate new node
    if (node == nullptr)
    {
        success = true;
        count++;
        return new CategoryTreeNode(p);
    }

    // If the new ID is smaller, go to the left subtree
    if (p.parcelID < node->info.parcelID)
    {
        node->left = insertNode(node->left, p, success);
    }
    // If the new ID is bigger, go to the right subtree
    else if (p.parcelID > node->info.parcelID)
    {
        node->right = insertNode(node->right, p, success);
    }
    // Duplicate ID found - not allowed in BST
    else
    {
        cout << "[Duplicate Error] Parcel ID " << p.parcelID << " already exists in the Category Tree!\n";
        success = false;
    }

    return node;
}

// Insert function
bool ParcelCategoryTree::insert(Parcel p)
{
    // Step 1: Validate input fields
    if (!validateParcelData(p))
    {
        return false;
    }

    // Step 2: Insert into BST recursively
    bool success = false;
    root = insertNode(root, p, success);

    if (success)
    {
        cout << "[Success] Parcel ID " << p.parcelID << " has been added to the Category Tree.\n";
    }
    return success;
}

// Search by Parcel ID
CategoryTreeNode *ParcelCategoryTree::searchById(int id)
{
    CategoryTreeNode *curr = root;

    // Loop through the tree until the target is found or hit null
    while (curr != nullptr)
    {
        if (id == curr->info.parcelID)
        {
            return curr; // Found the parcel
        }
        else if (id < curr->info.parcelID)
        {
            curr = curr->left; // Go left if smaller
        }
        else
        {
            curr = curr->right; // Go right if bigger
        }
    }
    return nullptr; // Parcel ID not found
}

// Helper to find the in-order successor (smallest node in right subtree)
CategoryTreeNode *ParcelCategoryTree::findMinNode(CategoryTreeNode *node)
{
    // Keep moving left until reaching the leftmost leaf
    while (node && node->left != nullptr)
    {
        node = node->left;
    }
    return node;
}

// BST deletion handling 3 cases: 0 child, 1 child, and 2 children
CategoryTreeNode *ParcelCategoryTree::deleteNode(CategoryTreeNode *node, int parcelID, bool &success)
{
    // Base case: tree is empty or ID not found
    if (node == nullptr)
    {
        success = false;
        return nullptr;
    }

    // Search for the node to delete
    if (parcelID < node->info.parcelID)
    {
        node->left = deleteNode(node->left, parcelID, success);
    }
    else if (parcelID > node->info.parcelID)
    {
        node->right = deleteNode(node->right, parcelID, success);
    }
    else
    {
        // Target node found!
        success = true;
        count--;

        // Case 1: Node has NO children (leaf node)
        if (node->left == nullptr && node->right == nullptr)
        {
            delete node;
            return nullptr;
        }
        // Case 2a: Node has only RIGHT child
        else if (node->left == nullptr)
        {
            CategoryTreeNode *temp = node->right;
            delete node;
            return temp; // Replace current node with right child
        }
        // Case 2b: Node has only LEFT child
        else if (node->right == nullptr)
        {
            CategoryTreeNode *temp = node->left;
            delete node;
            return temp; // Replace current node with left child
        }
        // Case 3: Node has TWO children
        else
        {
            // Find in-order successor (smallest in right subtree)
            CategoryTreeNode *successor = findMinNode(node->right);

            // Copy successor's data over to this node
            node->info = successor->info;

            // Delete the successor from the right subtree
            bool dummy = false;
            node->right = deleteNode(node->right, successor->info.parcelID, dummy);
            count++; // Rebalance count since recursive call deducted an extra 1
        }
    }
    return node;
}

// Public remove function
bool ParcelCategoryTree::remove(int id)
{
    if (isEmpty())
    {
        cout << "[Error] Tree is empty. Nothing to delete.\n";
        return false;
    }

    bool success = false;
    root = deleteNode(root, id, success);

    if (success)
    {
        cout << "[Success] Parcel ID " << id << " deleted from Category Tree.\n";
    }
    else
    {
        cout << "[Error] Parcel ID " << id << " was not found in the tree.\n";
    }
    return success;
}

// Update existing parcel while keeping parcelID unchanged to protect BST order
bool ParcelCategoryTree::update(int id, const Parcel &updatedData)
{
    // Search target node first
    CategoryTreeNode *target = searchById(id);
    if (!target)
    {
        cout << "[Error] Parcel ID " << id << " not found. Cannot update.\n";
        return false;
    }

    // Validate the new values
    if (!validateParcelData(updatedData))
    {
        return false;
    }

    // Update parcel fields (keep target->info.parcelID unchanged)
    target->info.recipientID = updatedData.recipientID;
    target->info.recipient_name = updatedData.recipient_name;
    target->info.delivery_address = updatedData.delivery_address;
    target->info.arrival_date = updatedData.arrival_date;
    target->info.type = updatedData.type;
    target->info.status = updatedData.status;
    target->info.fragile = updatedData.fragile;

    cout << "[Success] Parcel ID " << id << " updated successfully!\n";
    return true;
}

// In-order traversal (Left -> Node -> Right): naturally displays parcels sorted by parcelID
void ParcelCategoryTree::inOrderTraversal(CategoryTreeNode *node) const
{
    if (node == nullptr)
        return;

    inOrderTraversal(node->left); // Visit left subtree

    // Print current node in table row
    cout << left << setw(10) << node->info.parcelID
         << setw(14) << node->info.recipientID
         << setw(20) << node->info.recipient_name
         << setw(16) << getCategoryName(node->info.type)
         << setw(14) << getStatusName(node->info.status)
         << setw(10) << (node->info.fragile ? "Yes" : "No")
         << setw(14) << node->info.arrival_date
         << node->info.delivery_address << "\n";

    inOrderTraversal(node->right); // Visit right subtree
}

// Display all parcels in a formatted table
void ParcelCategoryTree::displayAll() const
{
    if (isEmpty())
    {
        cout << "[Notice] Category Tree is currently empty. No parcels to show.\n";
        return;
    }

    cout << "\n============================================ ALL PARCELS (BST IN-ORDER) ============================================\n";
    cout << left << setw(10) << "ParcelID"
         << setw(14) << "RecipientID"
         << setw(20) << "Recipient Name"
         << setw(16) << "Category"
         << setw(14) << "Status"
         << setw(10) << "Fragile"
         << setw(14) << "Arrival Date"
         << "Delivery Address\n";
    cout << string(125, '-') << "\n";

    inOrderTraversal(root);

    cout << string(125, '=') << "\n";
    cout << "Total Parcels in Tree: " << count << "\n";
}

// Traverse tree and print only parcels matching category
void ParcelCategoryTree::filterCategoryTraversal(CategoryTreeNode *node, ParcelType type, int &matchCount) const
{
    if (node == nullptr)
        return;

    filterCategoryTraversal(node->left, type, matchCount);

    if (node->info.type == type)
    {
        cout << left << setw(10) << node->info.parcelID
             << setw(14) << node->info.recipientID
             << setw(20) << node->info.recipient_name
             << setw(14) << getStatusName(node->info.status)
             << setw(10) << (node->info.fragile ? "Yes" : "No")
             << setw(14) << node->info.arrival_date
             << node->info.delivery_address << "\n";
        matchCount++;
    }

    filterCategoryTraversal(node->right, type, matchCount);
}

// Search and filter by parcel category
void ParcelCategoryTree::searchByCategory(ParcelType type) const
{
    if (isEmpty())
    {
        cout << "[Notice] Cannot filter category as the tree is currently empty.\n";
        return;
    }

    int matchCount = 0;
    cout << "\n================================ PARCELS IN CATEGORY: " << getCategoryName(type) << " ================================\n";
    cout << left << setw(10) << "ParcelID"
         << setw(14) << "RecipientID"
         << setw(20) << "Recipient Name"
         << setw(14) << "Status"
         << setw(10) << "Fragile"
         << setw(14) << "Arrival Date"
         << "Delivery Address\n";
    cout << string(120, '-') << "\n";

    filterCategoryTraversal(root, type, matchCount);

    cout << string(120, '=') << "\n";
    if (matchCount == 0)
        cout << "No parcels found under category: " << getCategoryName(type) << "\n";
    else
        cout << "Total records found: " << matchCount << "\n";
}

// Traverse tree and print only parcels matching delivery status
void ParcelCategoryTree::filterStatusTraversal(CategoryTreeNode *node, ParcelStatus status, int &matchCount) const
{
    if (node == nullptr)
        return;

    filterStatusTraversal(node->left, status, matchCount);

    if (node->info.status == status)
    {
        cout << left << setw(10) << node->info.parcelID
             << setw(14) << node->info.recipientID
             << setw(20) << node->info.recipient_name
             << setw(16) << getCategoryName(node->info.type)
             << setw(10) << (node->info.fragile ? "Yes" : "No")
             << setw(14) << node->info.arrival_date
             << node->info.delivery_address << "\n";
        matchCount++;
    }

    filterStatusTraversal(node->right, status, matchCount);
}

// Search and filter by parcel status
void ParcelCategoryTree::searchByStatus(ParcelStatus status) const
{
    if (isEmpty())
    {
        cout << "[Notice] Cannot filter status as the tree is currently empty.\n";
        return;
    }

    int matchCount = 0;
    cout << "\n================================ PARCELS WITH STATUS: " << getStatusName(status) << " ================================\n";
    cout << left << setw(10) << "ParcelID"
         << setw(14) << "RecipientID"
         << setw(20) << "Recipient Name"
         << setw(16) << "Category"
         << setw(10) << "Fragile"
         << setw(14) << "Arrival Date"
         << "Delivery Address\n";
    cout << string(120, '-') << "\n";

    filterStatusTraversal(root, status, matchCount);

    cout << string(120, '=') << "\n";
    if (matchCount == 0)
        cout << "No parcels found with status: " << getStatusName(status) << "\n";
    else
        cout << "Total records found: " << matchCount << "\n";
}

// Recursive helper to print 2D tree shape horizontally (Right subtree top, Left subtree bottom)
void ParcelCategoryTree::printHierarchy(CategoryTreeNode *node, int space) const
{
    if (node == nullptr)
        return;

    // Increase distance between tree levels
    space += 8;

    // Process right child first so it appears at top
    printHierarchy(node->right, space);

    // Print spaces followed by the node's parcelID and category
    cout << endl;
    for (int i = 8; i < space; i++)
        cout << " ";
    cout << "[" << node->info.parcelID << ":" << getCategoryName(node->info.type) << "]\n";

    // Process left child
    printHierarchy(node->left, space);
}

// Visual 2D hierarchy display to demo tree structure to the lecturer
void ParcelCategoryTree::displayTreeShape() const
{
    if (isEmpty())
    {
        cout << "[Notice] Tree is empty.\n";
        return;
    }
    cout << "\n--- 2D Category BST Structure (Right Subtree on Top, Left on Bottom) ---\n";
    printHierarchy(root, 0);
    cout << "-------------------------------------------------------------------------\n";
}

// Display single parcel in card format
void ParcelCategoryTree::displayParcel(const Parcel &p) const
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