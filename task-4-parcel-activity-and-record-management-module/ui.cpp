#include <string>
#include "parcel.cpp"
#include <iostream>
#include "activity-management.cpp"

using namespace std;

void ActivityModuleUI() {
    
    int choice;

    do {
    
        cout << "\n============================================\n";
        cout << " Task 4: Parcel & Record Management Module \n";
        cout << "============================================\n";
        cout << "1. Record Parcel Activity\n";
        cout << "2. Display Parcel Activity History\n";
        cout << "3. Undo Last Activity\n";
        cout << "4. Search a Parcel Activity\n";
        cout << "0. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

    } while (choice != 0);
}