#include "resourceTask3.hpp"
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

static bool menuInputClosed = false;

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
  char line[RESOURCE_TEXT_SIZE];
  if (!fgets(line, sizeof(line), stdin)) {
    menuInputClosed = true;
    return false;
  }
  if (!strchr(line, '\n') && !feof(stdin)) {
    clearInputBuffer();
    return false;
  }
  char *end = nullptr;
  errno = 0;
  long parsed = strtol(line, &end, 10);
  if (end == line || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) {
    return false;
  }
  while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') {
    ++end;
  }
  if (*end != '\0') {
    return false;
  }
  if (value) {
    *value = static_cast<int>(parsed);
  }
  return true;
}

bool readTextLine(const char *prompt, char *buffer, size_t maxLen) {
  if (!buffer || maxLen == 0)
    return false;
  if (prompt) {
    printf("%s", prompt);
    fflush(stdout);
  }
  if (!fgets(buffer, static_cast<int>(maxLen), stdin)) {
    menuInputClosed = true;
    return false;
  }
  if (!strchr(buffer, '\n') && !feof(stdin)) {
    clearInputBuffer();
  }
  buffer[strcspn(buffer, "\r\n")] = '\0';
  return true;
}

static void normalizeRequestId(char *text) {
  const char *start = text;
  while (*start == ' ' || *start == '\t') {
    ++start;
  }
  size_t len = strlen(start);
  while (len > 0 && (start[len - 1] == ' ' || start[len - 1] == '\t')) {
    --len;
  }
  for (size_t i = 0; i < len; ++i) {
    text[i] = static_cast<char>(toupper(static_cast<unsigned char>(start[i])));
  }
  text[len] = '\0';
}

static int lookupResource(int id) {
  const int idx = findResourceIndex(&resourcePool, id);
  if (idx == -1) {
    cout << "Invalid Resource ID. No resource found with ID " << id << "."
         << endl;
  }
  return idx;
}

static void showAssignOutcome(int result, const char *requestId, int resourceId) {
  switch (result) {
  case ASSIGN_OK: {
    char typeText[RESOURCE_TEXT_SIZE];
    getResourceTypeText(
        resourcePool.records[findResourceIndex(&resourcePool, resourceId)].type,
        typeText, sizeof(typeText));
    printf("Request %s assigned to %s resource %d.\n", requestId, typeText,
           resourceId);
    break;
  }
  case ASSIGN_INVALID_REQUEST:
    cout << "Invalid Request ID. No active service request found." << endl;
    break;
  case ASSIGN_REQUEST_CLOSED:
    cout << "Request is already completed or cancelled." << endl;
    break;
  case ASSIGN_DUPLICATE:
    cout << "Request (or its parcel) already has an active assignment."
         << endl;
    break;
  case ASSIGN_NO_SUITABLE:
    cout << "No suitable resource is registered for this service type."
         << endl;
    break;
  default:
    cout << "All suitable resources are occupied or unavailable." << endl;
    break;
  }
}

int displayResourceMenu(Queue &serviceQueue) {
  static bool poolReady = false; 
  if (!poolReady) {
    initResourceQueue(&resourcePool);
    poolReady = true;
  }
  int choice = 0;
  do {
    cout << "\n=========================================\n";
    cout << " Task 3: Collection & Delivery Resource Management\n";
    cout << "=========================================\n";
    cout << "1. Register Resource\n";
    cout << "2. Assign Resource\n";
    cout << "3. View Next Available\n";
    cout << "4. Search Resource\n";
    cout << "5. Release Resource\n";
    cout << "6. Update Status\n";
    cout << "7. Remove Resource\n";
    cout << "8. Show Resource Board\n";
    cout << "9. Show Activity History\n";
    cout << "0. Exit\n";
    cout << "=========================================\n";
    if (!readInt("Enter your choice: ", &choice)) {
      if (menuInputClosed)
        break;
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
      } else if (isResourceQueueFull(&resourcePool)) {
        cout << "Resource pool is full." << endl;
      } else if (findResourceIndex(&resourcePool, id) != -1) {
        cout << "Duplicate Resource ID." << endl;
      } else {
        cout << "Invalid Resource ID or type." << endl;
      }
      break;
    }
    case 2: {
      char requestId[RESOURCE_TEXT_SIZE];
      if (!readTextLine("Service Request ID (e.g. REQ1): ", requestId,
                        sizeof(requestId))) {
        cout << "Invalid assignment input." << endl;
        break;
      }
      normalizeRequestId(requestId);
      int resourceId = 0;
      const int result =
          assignRequestToResource(&resourcePool, serviceQueue, requestId,
                                  &resourceId);
      showAssignOutcome(result, requestId, resourceId);
      break;
    }
    case 3: {
      int type = 0;
      if (!readInt("Resource type (1=Counter, 2=Delivery): ", &type)) {
        cout << "Invalid type." << endl;
        break;
      }
      const int idx = findNextAvailableIndex(&resourcePool, type);
      if (idx == -1) {
        cout << "No available resource found for that type." << endl;
      } else {
        printf("Next available resource: %d\n", resourcePool.records[idx].id);
      }
      break;
    }
    case 4: {
      int id = 0;
      if (!readInt("Resource ID to search: ", &id)) {
        cout << "Invalid ID." << endl;
        break;
      }
      const int idx = lookupResource(id);
      if (idx == -1)
        break;
      char typeText[RESOURCE_TEXT_SIZE];
      char statusText[RESOURCE_TEXT_SIZE];
      char assignmentText[RESOURCE_TEXT_SIZE];
      getResourceTypeText(resourcePool.records[idx].type, typeText,
                          sizeof(typeText));
      getResourceStatusText(resourcePool.records[idx].status, statusText,
                            sizeof(statusText));
      getAssignmentText(&resourcePool, idx, assignmentText,
                        sizeof(assignmentText));
      printf("Resource ID: %d | Type: %s | Status: %s | Assignment: %s\n",
             resourcePool.records[idx].id, typeText, statusText,
             assignmentText);
      break;
    }
    case 5: {
      int id = 0;
      if (!readInt("Resource ID to release: ", &id)) {
        cout << "Invalid ID." << endl;
        break;
      }
      const int idx = lookupResource(id);
      if (idx == -1)
        break;
      if (resourcePool.records[idx].status != RESOURCE_BUSY) {
        cout << "Resource is not occupied; nothing to release." << endl;
      } else if (releaseResource(&resourcePool, id)) {
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
      if (status < RESOURCE_BUSY || status > RESOURCE_UNAVAILABLE) {
        cout << "Invalid status value." << endl;
        break;
      }
      const int idx = lookupResource(id);
      if (idx == -1)
        break;
      if (resourcePool.records[idx].status == status) {
        cout << "Resource already has that status." << endl;
        break;
      }
      
      char droppedRequest[RESOURCE_REQUEST_ID_SIZE];
      snprintf(droppedRequest, sizeof(droppedRequest), "%s",
               status == RESOURCE_BUSY ? "" : resourcePool.records[idx].requestId);
      if (updateResourceStatus(&resourcePool, id, status)) {
        cout << "Resource status updated." << endl;
        if (droppedRequest[0] != '\0') {
          printf("Request %s is now unassigned; assign it again.\n",
                 droppedRequest);
        }
      } else {
        cout << "Status update failed." << endl;
      }
      break;
    }
    case 7: {
      int id = 0;
      if (!readInt("Resource ID to remove: ", &id)) {
        cout << "Invalid ID." << endl;
        break;
      }
      const int idx = lookupResource(id);
      if (idx == -1)
        break;
      if (resourcePool.records[idx].status == RESOURCE_BUSY) {
        cout << "Cannot remove an occupied resource. Release it first."
             << endl;
      } else if (removeResource(&resourcePool, id)) {
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
      showResourceActivityHistory();
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
