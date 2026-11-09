#include "task1.cpp"
#include "task2/serviceMenu.cpp"
#include "task3/resourceTask3.cpp"
#include "task3/resourceTask3Menu.cpp"
#include "task4/ui.cpp"
#include "task5/ParcelCategoryList.hpp"
#include "tree.cpp"
#include <iostream>
#include <string>

using namespace std;

void runTask5Menu();

int main()
{
  // Stack allocation: Parcel constructor initializes root dummy node
  TreeNode parcels{Parcel{"", "", "", 0, 0, false, ParcelType::DOCUMENT}};
  Queue serviceQueue;
  int selection = 6;

  do
  {
    cout << "Welcome to the Parcel Management System!" << endl;
    cout << "Please select an option:" << endl;
    cout << "1. Task 1 - Parcel Intake, Sorting and Processing Module" << endl;
    cout << "2. Task 2 - Parcel Service and Collection Management Module" << endl;
    cout << "3. Task 3 - Collection Counter and Delivery Resource Management Module" << endl;
    cout << "4. Task 4 - Parcel Activity and Record Management Module" << endl;
    cout << "5. Task 5 - Parcel Category Organization Module" << endl;
    cout << "6. Exit" << endl;
    cout << "Enter your selection (1-6): ";
    if (!(cin >> selection))
    {
      if (cin.eof())
        break;
      cin.clear();
      cin.ignore(10000, '\n');
      cout << "Invalid selection. Please enter a number.\n";
      continue;
    }
    cin.ignore(10000, '\n');

    switch (selection)
    {
    case 1:
      full_menu(parcels);
      break;
    case 2:
      displayServiceMenu(parcels, serviceQueue);
      break;
    case 3:
      displayResourceMenu(parcels, serviceQueue);
      break;
    case 4:
      ActivityModuleUI();
      break;
    case 5:
      cout << "Task 5 selected." << endl;
      runTask5Menu();
      break;
    case 6:
      cout << "Exiting the program. Goodbye!" << endl;
      break;
    default:
      cout << "Invalid selection. Please try again." << endl;
    }
  } while (selection != 6);

  return 0;
}
