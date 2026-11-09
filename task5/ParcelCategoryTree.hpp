#pragma once
#include "../parcel.cpp"
#include <iostream>
#include <string>

using namespace std;

// Node Structure for Task 5 Category Tree
// Left child stores smaller parcelID, right child stores larger parcelID
struct CategoryTreeNode
{
    Parcel info;             // Holds the parcel record
    CategoryTreeNode *left;  // Pointer to left child in the tree
    CategoryTreeNode *right; // Pointer to right child in the tree

    // Constructor to set parcel info and initialize child pointers to null
    CategoryTreeNode(Parcel p)
    {
        info = p;
        left = nullptr;
        right = nullptr;
    }
};

// Task 5: Parcel Category Organisation Module using Binary Search Tree (BST)
class ParcelCategoryTree
{
private:
    CategoryTreeNode *root; // Root pointer of the BST
    int count;              // Track total parcels stored in the tree

    // Helper functions for recursive tree operations
    void destroyTree(CategoryTreeNode *node);
    CategoryTreeNode *insertNode(CategoryTreeNode *node, Parcel p, bool &success);
    CategoryTreeNode *findMinNode(CategoryTreeNode *node);
    CategoryTreeNode *deleteNode(CategoryTreeNode *node, int parcelID, bool &success);
    void inOrderTraversal(CategoryTreeNode *node) const;
    void filterCategoryTraversal(CategoryTreeNode *node, ParcelType type, int &matchCount) const;
    void filterStatusTraversal(CategoryTreeNode *node, ParcelStatus status, int &matchCount) const;
    void printHierarchy(CategoryTreeNode *node, int space) const;

    // Input validation check
    bool validateParcelData(const Parcel &p);

public:
    // Constructor and destructor
    ParcelCategoryTree();
    ~ParcelCategoryTree();

    // Utility getters
    bool isEmpty() const;
    int getCount() const;

    // Enum to text conversion helpers for clean display
    static string getCategoryName(ParcelType type);
    static string getStatusName(ParcelStatus status);

    // Core BST operations
    bool insert(Parcel p);
    CategoryTreeNode *searchById(int id);
    bool remove(int id);
    bool update(int id, const Parcel &updatedData);

    // Viewing and filtering functions
    void displayAll() const;
    void searchByCategory(ParcelType type) const;
    void searchByStatus(ParcelStatus status) const;
    void displayTreeShape() const; // Visual 2D tree layout for presentation demo
    void displayParcel(const Parcel &p) const;

    // Clean up all allocated memory
    void clear();
};