#include "resourceTask3.hpp"

#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

static void clearInputBuffer() {
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF) {
  }
}

bool readInt(const char *prompt, int *value) {
  if (prompt) {
    printf("%s", prompt);
    fflush(stdout);
  }

  int temp = 0;
  if (scanf("%d", &temp) != 1) {
    clearInputBuffer();
    return false;
  }

  if (value)
    *value = temp;
  clearInputBuffer();
  return true;
}

bool readTextLine(const char *prompt, char *buffer, size_t maxLen) {
  if (prompt) {
    printf("%s", prompt);
    fflush(stdout);
  }

  if (!buffer || maxLen == 0)
    return false;

  if (!fgets(buffer, (int)maxLen, stdin))
    return false;

  buffer[strcspn(buffer, "\r\n")] = '\0';
  return true;
}

int displayResourceMenu() {
  int choice = 0;
  initResourceQueue(&resourcePool);

  do {
    cout << "\n=========================================" << endl;
    cout << " Task 3: Collection & Delivery Resource Management" << endl;
    cout << "=========================================" << endl;
    cout << "1. Register Resource" << endl;
    cout << "2. Assign Resource" << endl;
    cout << "3. View Next Available" << endl;
    cout << "4. Search Resource" << endl;
    cout << "5. Release Resource" << endl;
    cout << "6. Update Status" << endl;
    cout << "7. Remove Resource" << endl;
    cout << "8. Show Resource Board" << endl;
    cout << "9. Run Benchmark" << endl;
    cout << "10. Show Activity History" << endl;
    cout << "0. Exit" << endl;
    cout << "=========================================" << endl;

    if (!readInt("Enter your choice: ", &choice)) {
      cout << "Invalid input. Please enter a valid number." << endl;
      continue;
    }

    switch (choice) {
    case 1: {
      int id = 0, type = 0;
      if (!readInt("Resource ID: ", &id) ||
          !readInt("Type (1=Counter, 2=Delivery): ", &type)) {
        cout << "Invalid resource input." << endl;
        break;
      }

      if (registerResource(&resourcePool, id, type)) {
        cout << "Resource registered successfully." << endl;
        traceMessage("Register Resource", "Resource added to pool.");
      } else {
        cout << "Could not register resource." << endl;
      }
      break;
    }

    case 2: {
      int id = 0, kind = 0, item = 0, parcel = 0;

      if (!readInt("Resource ID: ", &id) ||
          !readInt("Assignment kind (1=Parcel, 2=Request): ", &kind) ||
          !readInt("Item ID: ", &item) || !readInt("Parcel ID: ", &parcel)) {
        cout << "Invalid assignment input." << endl;
        break;
      }

      if (assignResource(&resourcePool, id, kind, item, parcel)) {
        cout << "Assignment successful." << endl;
      } else {
        cout << "Assignment failed." << endl;
      }
      break;
    }

    case 3: {
      int type = 0;
      if (!readInt("Resource type (1=Counter, 2=Delivery): ", &type)) {
        cout << "Invalid type." << endl;
        break;
      }

      int idx = findNextAvailableIndex(&resourcePool, type);
      if (idx == -1) {
        cout << "No available resource found for that type." << endl;
        break;
      }

      printf("Next available resource: %d\n", resourcePool.data[idx].id);
      break;
    }

    case 4: {
      int id = 0;
      if (!readInt("Resource ID to search: ", &id)) {
        cout << "Invalid ID." << endl;
        break;
      }

      int idx = findResourceIndex(&resourcePool, id);
      if (idx == -1) {
        cout << "Resource not found." << endl;
        break;
      }

      char typeText[RESOURCE_NAME_LIMIT];
      char statusText[RESOURCE_NAME_LIMIT];
      char assignmentText[RESOURCE_NAME_LIMIT];

      getResourceTypeText(resourcePool.data[idx].type, typeText,
                          sizeof(typeText));
      getResourceStatusText(resourcePool.data[idx].status, statusText,
                            sizeof(statusText));
      getAssignmentText(&resourcePool, idx, assignmentText,
                        sizeof(assignmentText));

      printf("Resource ID: %d | Type: %s | Status: %s | Assignment: %s\n",
             resourcePool.data[idx].id, typeText, statusText, assignmentText);
      break;
    }

    case 5: {
      int id = 0;
      if (!readInt("Resource ID to release: ", &id)) {
        cout << "Invalid ID." << endl;
        break;
      }

      if (releaseResource(&resourcePool, id)) {
        cout << "Resource released successfully." << endl;
      } else {
        cout << "Release failed." << endl;
      }
      break;
    }

    case 6: {
      int id = 0, status = 0;
      if (!readInt("Resource ID: ", &id) ||
          !readInt("Status (1=Busy, 2=Available, 3=Unavailable): ", &status)) {
        cout << "Invalid status input." << endl;
        break;
      }

      if (status == 1)
        status = RESOURCE_BUSY;
      else if (status == 2)
        status = RESOURCE_AVAILABLE;
      else if (status == 3)
        status = RESOURCE_UNAVAILABLE;
      else {
        cout << "Invalid status value." << endl;
        break;
      }

      if (updateResourceStatus(&resourcePool, id, status)) {
        cout << "Resource status updated." << endl;
      } else {
        cout << "Resource status update failed." << endl;
      }
      break;
    }

    case 7: {
      int id = 0;
      if (!readInt("Resource ID to remove: ", &id)) {
        cout << "Invalid ID." << endl;
        break;
      }

      if (removeResource(&resourcePool, id)) {
        cout << "Resource removed successfully." << endl;
      } else {
        cout << "Resource removal failed." << endl;
      }
      break;
    }

    case 8:
      showResourceBoard(&resourcePool);
      break;

    case 9:
      runDemoBenchmark();
      break;

    case 10:
      showActivityHistory();
      break;

    case 0:
      cout << "Exiting Task 3 module." << endl;
      break;

    default:
      cout << "Invalid option. Please select again." << endl;
      break;
    }

  } while (choice != 0);

  return 0;
}

int main() {
  cout << "Parcel Management System - Task 3\n";
  return displayResourceMenu();
}
