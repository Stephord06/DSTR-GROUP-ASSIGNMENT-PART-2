#include "ParcelCategoryTree.hpp"
#include <iostream>
#include <limits>

using namespace std;

// Helper function to flush the cin buffer if the user enters letters instead of numbers
static void clearInputBufferTask5()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Helper to construct a Parcel struct safely
static Parcel createParcelTask5(string name, string address, string date, int rID, int pID, bool fragile, ParcelType type, ParcelStatus status)
{
    Parcel p;
    p.recipient_name = name;
    p.delivery_address = address;
    p.arrival_date = date;
    p.recipientID = rID;
    p.parcelID = pID;
    p.fragile = fragile;
    p.type = type;
    p.status = status;
    return p;
}

// Mock sample records to test Task 5 for presentation
static void loadSampleDataTask5(ParcelCategoryTree &tree)
{
    // Insert 503 first so it acts as root node, with 501 and 502 on left, 504 and 505 on right
    tree.insert(createParcelTask5("Charlie Wong", "APU Campus Delivery Hub", "2026-11-03", 1003, 503, true, FRAGILE_GOODS, ORDERED));
    tree.insert(createParcelTask5("Alice Tan", "APU Residence Block A-10-2", "2026-11-01", 1001, 501, false, DOCUMENT, ORDERED));
    tree.insert(createParcelTask5("Bob Lee", "Technology Park Malaysia Counter 1", "2026-11-02", 1002, 502, true, ELECTRONIC, SHIPPED));
    tree.insert(createParcelTask5("Diana Ross", "APU Residence Block B-05-1", "2026-11-03", 1004, 504, false, GENERAL_PARCEL, DELIVERED));
    tree.insert(createParcelTask5("Evan Khoo", "APU Enterprise Counter 2", "2026-11-04", 1005, 505, false, DOCUMENT, COLLECTED));
    cout << "[System] 5 sample parcels loaded successfully into Category Tree.\n";
}

// UI for Task 5
void runTask5Menu()
{
    ParcelCategoryTree tree;
    int choice = 0;

    do
    {
        cout << "\n======================================================\n";
        cout << "  SMART CAMPUS PARCEL SYSTEM - TASK 5: CATEGORY MODULE\n";
        cout << "======================================================\n";
        cout << "1. Insert New Parcel Record into Tree\n";
        cout << "2. Search Parcel by ID\n";
        cout << "3. Filter by Category (Parcel Type)\n";
        cout << "4. Filter by Status\n";
        cout << "5. Display All Stored Parcels\n";
        cout << "6. Display Tree Structure\n";
        cout << "7. Update Parcel Record\n";
        cout << "8. Remove Parcel Record\n";
        cout << "9. Load Sample Data\n";
        cout << "0. Return to Main Menu\n";
        cout << "------------------------------------------------------\n";
        cout << "Enter selection (0-9): ";

        // Handle non-numeric input
        if (!(cin >> choice))
        {
            cout << "[Input Error] Please enter numbers only!\n";
            clearInputBufferTask5();
            continue;
        }

        switch (choice)
        {
        case 1:
        {
            Parcel p;
            int typeChoice, statusChoice;

            cout << "\n--- Register New Parcel ---\n";
            // Collect the IDs of parcel and recipient
            cout << "Enter Parcel ID: ";
            cin >> p.parcelID;
            cout << "Enter Recipient ID: ";
            cin >> p.recipientID;
            clearInputBufferTask5();

            // Collect the additional info for the new parcel
            cout << "Enter Recipient Name: ";
            getline(cin, p.recipient_name);
            cout << "Enter Delivery Address: ";
            getline(cin, p.delivery_address);
            cout << "Enter Arrival Date (YYYY-MM-DD): ";
            getline(cin, p.arrival_date);

            // Select the option for category, status and isFragile
            cout << "Select Category (0: DOCUMENT, 1: ELECTRONIC, 2: FRAGILE_GOODS, 3: GENERAL_PARCEL): ";
            cin >> typeChoice;
            if (typeChoice < 0 || typeChoice > 3)
            {
                cout << "[Error] Invalid category selection!\n";
                break;
            }
            p.type = static_cast<ParcelType>(typeChoice);

            cout << "Select Status (0: ORDERED, 1: SHIPPED, 2: DELIVERED, 3: COLLECTED, 4: RETURNED, 5: CANCELLED, 6: ON_HOLD): ";
            cin >> statusChoice;
            if (statusChoice < 0 || statusChoice > 6)
            {
                cout << "[Error] Invalid status selection!\n";
                break;
            }
            p.status = static_cast<ParcelStatus>(statusChoice);

            cout << "Is it fragile? (1 for Yes, 0 for No): ";
            cin >> p.fragile;

            tree.insert(p);
            break;
        }

        case 2:
        {
            // Search by parcel ID
            int searchID;
            cout << "Enter Parcel ID to search: ";
            cin >> searchID;

            CategoryTreeNode *result = tree.searchById(searchID);
            if (result != nullptr)
            {
                tree.displayParcel(result->info);
            }
            else
            {
                cout << "[Error] Parcel with ID " << searchID << " was not found in the list.\n";
            }
            break;
        }

        case 3:
        {
            // Filter by Category
            int typeChoice;
            cout << "\nSelect Category to Filter the parcels:\n";
            cout << "0: DOCUMENT\n1: ELECTRONIC\n2: FRAGILE_GOODS\n3: GENERAL_PARCEL\nEnter choice: ";
            cin >> typeChoice;

            if (typeChoice >= 0 && typeChoice <= 3)
            {
                tree.searchByCategory(static_cast<ParcelType>(typeChoice));
            }
            else
            {
                cout << "[Error] Invalid category selection.\n";
            }
            break;
        }

        case 4:
        {
            // Filter by Status
            int statusChoice;
            cout << "\nSelect Status to Filter the parcels:\n";
            cout << "0: ORDERED\n1: SHIPPED\n2: DELIVERED\n3: COLLECTED\n4: RETURNED\n5: CANCELLED\n6: ON_HOLD\nEnter Choice: ";
            cin >> statusChoice;

            if (statusChoice >= 0 && statusChoice <= 6)
            {
                tree.searchByStatus(static_cast<ParcelStatus>(statusChoice));
            }
            else
            {
                cout << "[Error] Invalid status selection.\n";
            }
            break;
        }

        case 5:
        {
            // Display all records
            tree.displayAll();
            break;
        }

        case 6:
        {
            // Display tree shape
            tree.displayTreeShape();
            break;
        }

        case 7:
        {
            // Update Parcel Info
            int updateId;
            cout << "Enter Parcel ID to update: ";
            cin >> updateId;

            CategoryTreeNode *existing = tree.searchById(updateId);
            if (existing == nullptr)
            {
                cout << "[Error] Parcel ID " << updateId << " was not found in the list.\n";
                break;
            }

            cout << "\n--- Current Record Found ---";
            tree.displayParcel(existing->info);

            Parcel updated = existing->info;
            int updateOption = 0;

            do
            {
                cout << "\n========================================\n";
                cout << "  UPDATE OPTIONS FOR PARCEL ID: " << updateId << "\n";
                cout << "========================================\n";
                cout << "1. Update All Information\n";
                cout << "2. Update Recipient ID\n";
                cout << "3. Update Recipient Name\n";
                cout << "4. Update Delivery Address\n";
                cout << "5. Update Arrival Date\n";
                cout << "6. Update Category (Parcel Type)\n";
                cout << "7. Update Delivery Status\n";
                cout << "8. Update Fragile Handling\n";
                cout << "0. Finish Updating & Return\n";
                cout << "----------------------------------------\n";
                cout << "Enter your choice (0-8): ";

                if (!(cin >> updateOption))
                {
                    cout << "[Input Error] Please enter numbers only!\n";
                    clearInputBufferTask5();
                    continue;
                }

                switch (updateOption)
                {
                case 1:
                {
                    // Update all fields sequentially
                    int newRecID;
                    cout << "Enter new Recipient ID (" << updated.recipientID << "): ";
                    cin >> newRecID;
                    clearInputBufferTask5();

                    string newName, newAddr, newDate;
                    cout << "Enter new Recipient Name (" << updated.recipient_name << "): ";
                    getline(cin, newName);
                    cout << "Enter new Delivery Address (" << updated.delivery_address << "): ";
                    getline(cin, newAddr);
                    cout << "Enter new Arrival Date (YYYY-MM-DD) (" << updated.arrival_date << "): ";
                    getline(cin, newDate);

                    int typeChoice, statusChoice;
                    cout << "Select Category (0: DOCUMENT, 1: ELECTRONIC, 2: FRAGILE_GOODS, 3: GENERAL_PARCEL): ";
                    cin >> typeChoice;
                    if (typeChoice < 0 || typeChoice > 3)
                    {
                        cout << "[Error] Invalid category option. Update cancelled.\n";
                        break;
                    }

                    cout << "Select Status (0: ORDERED, 1: SHIPPED, 2: DELIVERED, 3: COLLECTED, 4: RETURNED, 5: CANCELLED, 6: ON_HOLD): ";
                    cin >> statusChoice;
                    if (statusChoice < 0 || statusChoice > 6)
                    {
                        cout << "[Error] Invalid status option. Update cancelled.\n";
                        break;
                    }

                    bool newFragile;
                    cout << "Is it fragile? (1 for Yes, 0 for No): ";
                    cin >> newFragile;

                    updated.recipientID = newRecID;
                    updated.recipient_name = newName;
                    updated.delivery_address = newAddr;
                    updated.arrival_date = newDate;
                    updated.type = static_cast<ParcelType>(typeChoice);
                    updated.status = static_cast<ParcelStatus>(statusChoice);
                    updated.fragile = newFragile;

                    tree.update(updateId, updated);
                    updateOption = 0; // Return to Task 5 menu after full update
                    break;
                }

                case 2:
                {
                    int newRecID;
                    cout << "Enter new Recipient ID (" << updated.recipientID << "): ";
                    cin >> newRecID;
                    if (newRecID <= 0)
                    {
                        cout << "[Validation Error] Recipient ID must be greater than 0.\n";
                    }
                    else
                    {
                        updated.recipientID = newRecID;
                        tree.update(updateId, updated);
                    }
                    break;
                }

                case 3:
                {
                    clearInputBufferTask5();
                    string newName;
                    cout << "Enter new Recipient Name (" << updated.recipient_name << "): ";
                    getline(cin, newName);
                    if (newName == "")
                    {
                        cout << "[Validation Error] Recipient Name cannot be blank.\n";
                    }
                    else
                    {
                        updated.recipient_name = newName;
                        tree.update(updateId, updated);
                    }
                    break;
                }

                case 4:
                {
                    clearInputBufferTask5();
                    string newAddr;
                    cout << "Enter new Delivery Address (" << updated.delivery_address << "): ";
                    getline(cin, newAddr);
                    if (newAddr == "")
                    {
                        cout << "[Validation Error] Delivery Address cannot be blank.\n";
                    }
                    else
                    {
                        updated.delivery_address = newAddr;
                        tree.update(updateId, updated);
                    }
                    break;
                }

                case 5:
                {
                    clearInputBufferTask5();
                    string newDate;
                    cout << "Enter new Arrival Date (YYYY-MM-DD) (" << updated.arrival_date << "): ";
                    getline(cin, newDate);
                    if (newDate == "")
                    {
                        cout << "[Validation Error] Arrival Date cannot be blank.\n";
                    }
                    else
                    {
                        updated.arrival_date = newDate;
                        tree.update(updateId, updated);
                    }
                    break;
                }

                case 6:
                {
                    int typeChoice;
                    cout << "Select Category (0: DOCUMENT, 1: ELECTRONIC, 2: FRAGILE_GOODS, 3: GENERAL_PARCEL): ";
                    cin >> typeChoice;
                    if (typeChoice >= 0 && typeChoice <= 3)
                    {
                        updated.type = static_cast<ParcelType>(typeChoice);
                        tree.update(updateId, updated);
                    }
                    else
                    {
                        cout << "[Error] Invalid category selection.\n";
                    }
                    break;
                }

                case 7:
                {
                    int statusChoice;
                    cout << "Select Status (0: ORDERED, 1: SHIPPED, 2: DELIVERED, 3: COLLECTED, 4: RETURNED, 5: CANCELLED, 6: ON_HOLD): ";
                    cin >> statusChoice;
                    if (statusChoice >= 0 && statusChoice <= 6)
                    {
                        updated.status = static_cast<ParcelStatus>(statusChoice);
                        tree.update(updateId, updated);
                    }
                    else
                    {
                        cout << "[Error] Invalid status selection.\n";
                    }
                    break;
                }

                case 8:
                {
                    int fragChoice;
                    cout << "Is it fragile? (1 for Yes, 0 for No): ";
                    cin >> fragChoice;
                    if (fragChoice == 0 || fragChoice == 1)
                    {
                        updated.fragile = (fragChoice == 1);
                        tree.update(updateId, updated);
                    }
                    else
                    {
                        cout << "[Error] Please enter 1 or 0.\n";
                    }
                    break;
                }

                case 0:
                    cout << "[Notice] Finished updating Parcel ID " << updateId << ".\n";
                    break;

                default:
                    cout << "[Error] Invalid selection. Choose between 0 and 8.\n";
                    break;
                }
            } while (updateOption != 0);

            break;
        }

        case 8:
        {
            // Delete parcel
            int deleteId;
            int confirmDel;
            while (true)
            {
                cout << "Enter Parcel ID to delete or 0 to Exit: ";
                cin >> deleteId;
                if (deleteId == 0)
                {
                    break;
                }
                cout << "Are you sure you want to delete Parcel with ID " << deleteId << "?\n1: Yes\n0: No\nEnter choice: ";
                cin >> confirmDel;
                if (confirmDel == 1)
                {
                    tree.remove(deleteId);
                    break;
                }
                else if (confirmDel == 0)
                {
                    cout << "[Notice] Deletion has been cancelled.\n";
                    break;
                }
                else
                {
                    cout << "[Error] Please enter a correct number!\n";
                    continue;
                }
            }
            break;
        }

        case 9:
        {
            // Load the sample data
            loadSampleDataTask5(tree);
            break;
        }

        case 0:
        {
            cout << "Exiting [Task 5] - Category Module...\n";
            break;
        }

        default:
        {
            cout << "[Error] Invalid option. Please enter a number between 0 and 9!\n";
            break;
        }
        }
    } while (choice != 0);
}