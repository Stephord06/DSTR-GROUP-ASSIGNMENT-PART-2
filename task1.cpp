#include "tree.cpp"
#include <cstdlib>
#include <iostream>
#include <istream>
#include <ostream>

ParcelType type_select_menu() {
  int choice;
  while (true) {

    cout << "\nSelect type:";
    cout << "\n--------------------------------";

    cout << "\n1.Document";
    cout << "\n2.Electronic";

    cout << "\n--------------------------------";
    cout << "\nChoice: ";
    cin >> choice;
    switch (choice) {
    case 1: {
      return ParcelType::DOCUMENT;
    }
    case 2: {
      return ParcelType::ELECTRONIC;
    }
    default: {
      cout << "\nInvalid input";
      continue;
    }
    }
  }
}
ParcelStatus status_select_menu() {
  int choice;
  while (true) {
    cout << "\nSelect status:";
    cout << "\n--------------------------------";

    cout << "\n1.Ordered";
    cout << "\n2.Shipped";
    cout << "\n3.On hold";
    cout << "\n4.Delivered";

    cout << "\n--------------------------------";
    cout << "\nChoice: ";
    cin >> choice;
    switch (choice) {
    case 1: {
      return ParcelStatus::ORDERED;
    }
    case 2: {
      return ParcelStatus::SHIPPED;
    }
    case 3: {
      return ParcelStatus::ON_HOLD;
    }
    case 4: {
      return ParcelStatus::DELIVERED;
    }
    default: {
      cout << "\nInvalid input";
      continue;
    }
    }
  }
}

void register_parcel(TreeNode &parcels) {
  string recipient_name = "", delivery_address = "", arrival_date = "";
  bool fragile;
  string fragile_string = "";
  ParcelType type;
  // TODO: need varification
  cout << "\nEnter recipient name:";
  getline(cin >> ws, recipient_name);
  cout << "\nEnter delivery address:";
  cin >> delivery_address;
  cout << "\nEnter arrival date:";
  cin >> arrival_date;
  type = type_select_menu();
  while (true) {
    cout << "\nIs it fragile?(y/n):";
    cin >> fragile_string;
    if (fragile_string == "y" || fragile_string == "yes") {
      fragile = true;
      break;
    } else if (fragile_string == "n" || fragile_string == "no") {
      fragile = false;
      break;
    } else {
      cout << "\ninvalid input: enter (y/n)";
    }
  }
  int recipient_id;
  int parcel_id;
  while (true) {
    recipient_id = rand();
    TreeNode *result = parcels.search_by_id(recipient_id);
    if (!result) {
      break;
    }
  }
  while (true) {
    parcel_id = rand();
    TreeNode *result = parcels.search_by_id(parcel_id);
    if (!result) {
      break;
    }
  }

  Parcel new_parcel{
      recipient_name, delivery_address, arrival_date, recipient_id,
      parcel_id,      fragile,          type,
  };

  // TreeNode new_treenode = TreeNode{new_parcel};
  parcels.push_child(new_parcel);
  cout << "--------------------------------" << endl;
  cout << "Parcel has been registered successfully!" << endl;
  cout << "Parcel ID: " << new_parcel.parcelID << endl;
}

void search_parcel(TreeNode &parcels) {
  int parcel_id;
  while (true) {
    cout << "\nEnter parcel ID(0 to quit):";
    cin >> parcel_id;
    if (parcel_id == 0) {
      break;
    }
    TreeNode *result = parcels.search_by_id(parcel_id);
    if (result) {
      Parcel parcel = result->parcel;
      cout << "--------------------------------" << endl;
      cout << "Recipient name:" << parcel.recipient_name << endl;
      cout << "Delivery address:" << parcel.delivery_address << endl;
      cout << "Arrival date:" << parcel.arrival_date << endl;
      cout << "Type:" << type_to_string(parcel.type) << endl;
      cout << "Status:" << status_to_string(parcel.status) << endl;
      cout << "Fragile:" << fragile_to_string(parcel.fragile) << endl;
      cout << "--------------------------------" << endl;
    } else {
      cout << "\nParcel not found.";
    }
  }
}

void update_parcel(TreeNode &parcels) {
  int parcel_id;
  while (true) {
    cout << "\nEnter parcel ID(0 to quit):";
    cin >> parcel_id;
    if (parcel_id == 0) {
      break;
    }
    TreeNode *result = parcels.search_by_id(parcel_id);
    if (result) {
      while (true) {

        int choice;
        cout << "\nselect:";
        cout << "\n--------------------------------";
        cout << "\n1.Recipient name";
        cout << "\n2.Delivery address";
        cout << "\n3.Arrival date";
        cout << "\n4.Type";
        cout << "\n5.Status";
        cout << "\n6.Quit";
        cout << "\n--------------------------------";
        cout << "\nchoice: ";
        cin >> choice;
        switch (choice) {
        case 1: {
          string new_name;
          cout << "\nEnter a new Recipient Name:";
          getline(cin >> ws, new_name);
          result->parcel.recipient_name = new_name;
          continue;
        }
        case 2: {
          string new_address;
          cout << "\nEnter a new Delivery Address:";
          cin >> new_address;
          result->parcel.delivery_address = new_address;
          continue;
        }
        case 3: {
          string new_arrival_date;
          cout << "\nEnter a new Arrival Date:";
          cin >> new_arrival_date;
          result->parcel.arrival_date = new_arrival_date;
          continue;
        }
        case 4: {
          ParcelType new_type = type_select_menu();
          result->parcel.type = new_type;
          continue;
        }
        case 5: {
          ParcelStatus new_status = status_select_menu();
          result->parcel.status = new_status;
          continue;
        }
        case 6: {
          return;
        }
        default: {
          cout << "\ninvalid input.";
          continue;
        }
        }
      }
    } else {
      cout << "\nParcel not found";
    }
  }
}

void remove_parcel(TreeNode &parcels) {
  int parcel_id;
  while (true) {
    cout << "\nEnter parcel ID(0 to quit):";
    cin >> parcel_id;
    if (parcel_id == 0) {
      break;
    }
    bool result = parcels.delete_by_id(parcel_id);
    if (result) {
      cout << "\nParcel deleted successfully";
    } else {
      cout << "\nParcel not found";
    }
  }
}

void full_menu(TreeNode &parcels) {
  int choice;
  while (true) {
    cout << "select:";
    cout << "\n--------------------------------";
    cout << "\n1.Register new parcel";
    cout << "\n2.Search parcel";
    cout << "\n3.update parcel";
    cout << "\n4.Remove parcel";
    cout << "\n5.Quit";
    cout << "\n--------------------------------";
    cout << "\nChoice: ";
    cin >> choice;
    switch (choice) {
    case 1: {
      register_parcel(parcels);
      break;
    }
    case 2: {
      search_parcel(parcels);
      break;
    }
    case 3: {
      update_parcel(parcels);
      break;
    }
    case 4: {
      remove_parcel(parcels);
      break;
    }
    case 5: {
      return;
    }
    default: {
      cout << "\ninvalid input";
    }
    }
  }
}
