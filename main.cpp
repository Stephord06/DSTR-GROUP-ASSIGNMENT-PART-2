#include "task2/serviceMenu.cpp"
#include "tree.cpp";
#include <iostream>
#include <string>

using namespace std;

int main() {
  TreeNode *parcels = new TreeNode;
  int selection = 6;
  do {
    cout << "Welcome to the Parcel Management System!" << endl;
    cout << "Please select an option:" << endl;
    cout << "1. Task 1 - Parcel Intake, Sorting and Processing Module" << endl;
    cout << "2. Task 2 - Parcel Service and Collection Management Module"
         << endl;
    cout << "3. Task 3 - Collection Counter and Delivery Resource Management "
            "Module"
         << endl;
    cout << "4. Task 4 - Parcel Activity and Record Management Module" << endl;
    cout << "5. Task 5 - Parcel Category Organization Module" << endl;
    cout << "6. Exit" << endl;
    cout << "\n \n";
    cout << "Enter your selection (1,2,3,4,5,6) : ";
    cin >> selection;

    switch (selection) {
    case 1:
      cout << "Task 1 selected." << endl;
      // Call Task 1 function here
      break;
    case 2:
      cout << "Task 2 selected." << endl;
      displayServiceMenu();
      break;
    case 3:
      cout << "Task 3 selected." << endl;
      // Call Task 3 function here
      break;
    case 4:
      cout << "Task 4 selected." << endl;
      // Call Task 4 function here
      break;
    case 5:
      cout << "Task 5 selected." << endl;
      // Call Task 5 function here
      break;
    case 6:
      cout << "Exiting the program. Goodbye!" << endl;
      break;
    default:
      cout << "Invalid selection. Please try again." << endl;
    }
  } while (selection != 6);
