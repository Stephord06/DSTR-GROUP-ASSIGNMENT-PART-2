#include "resourceTask3.hpp"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

static bool menuInputClosed = false;
static bool resourceQueueInitialized = false;

static void clearInputBuffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

bool readInt(const char *prompt, int *value) {
    if (prompt) { printf("%s", prompt); fflush(stdout); }
    char line[RESOURCE_TEXT_SIZE];
    if (!fgets(line, sizeof(line), stdin)) { menuInputClosed = true; return false; }
    if (!strchr(line, '\n') && !feof(stdin)) { clearInputBuffer(); return false; }
    char *end = nullptr;
    errno = 0;
    long parsed = strtol(line, &end, 10);
    if (end == line || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) return false;
    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') ++end;
    if (*end != '\0') return false;
    if (value) *value = static_cast<int>(parsed);
    return true;
}

bool readTextLine(const char *prompt, char *buffer, size_t maxLen) {
    if (!buffer || maxLen == 0 || maxLen > static_cast<size_t>(INT_MAX)) return false;
    if (prompt) { printf("%s", prompt); fflush(stdout); }
    if (!fgets(buffer, static_cast<int>(maxLen), stdin)) { menuInputClosed = true; return false; }
    if (!strchr(buffer, '\n') && !feof(stdin)) { clearInputBuffer(); buffer[0] = '\0'; return false; }
    buffer[strcspn(buffer, "\r\n")] = '\0';
    return true;
}

static void printAssignmentResult(int result, const char *requestId, int resourceId) {
    if (result == ASSIGN_OK) {
        char typeText[RESOURCE_TEXT_SIZE];
        const int index = findResourceIndex(&resourcePool, resourceId);
        getResourceTypeText(index >= 0 ? resourcePool.records[index].type : 0,
                            typeText, sizeof(typeText));
        printf("Request %s assigned to %s resource %d.\n", requestId, typeText, resourceId);
    } else if (result == ASSIGN_INVALID_REQUEST) {
        cout << "The selected request is no longer active, its parcel is unavailable, or its ID cannot be stored.\n";
    } else if (result == ASSIGN_DUPLICATE) {
        cout << "This request or parcel already has an active resource assignment.\n";
    } else if (result == ASSIGN_NO_SUITABLE) {
        cout << "No suitable resource of the required type is registered.\n";
    } else {
        cout << "All suitable resources are busy or unavailable.\n";
    }
}

int displayResourceMenu(TreeNode& parcels, Queue& serviceRequests) {
    if (!resourceQueueInitialized) {
        initResourceQueue(&resourcePool);
        resourceQueueInitialized = true;
    }
    int choice = 0;
    do {
        cout << "\n=========================================\n"
             << " Task 3: Collection & Delivery Resource Management\n"
             << "=========================================\n"
             << "1. Register Resource\n2. Assign Existing Service Request\n3. View Next Available\n"
             << "4. Search Resource\n5. Release Resource\n6. Update Status\n"
             << "7. Remove Resource\n8. Show Resource Board\n9. Show Activity History\n0. Exit\n"
             << "=========================================\n";
        if (!readInt("Enter your choice: ", &choice)) {
            if (menuInputClosed) break;
            cout << "Invalid input. Please enter a valid number.\n";
            continue;
        }
        switch (choice) {
        case 1: {
            int id = 0, type = 0;
            if (!readInt("Resource ID: ", &id) || !readInt("Type (1=Counter, 2=Delivery): ", &type)) {
                cout << "Invalid resource input.\n"; break;
            }
            if (registerResource(&resourcePool, id, type)) cout << "Resource registered successfully.\n";
            else cout << "Could not register resource (invalid/duplicate ID, type, or full pool).\n";
            break;
        }
        case 2: {
            showAssignableRequests(&serviceRequests);
            int position = 0, assignedResourceId = 0;
            char requestId[RESOURCE_REQUEST_ID_SIZE] = {};
            if (serviceRequests.isEmpty()) break;
            if (!readInt("Select a request by its list number: ", &position)) {
                cout << "Invalid request selection.\n"; break;
            }
            const int result = assignRequestByPosition(&resourcePool, &serviceRequests, &parcels,
                                                       position, &assignedResourceId,
                                                       requestId, sizeof(requestId));
            printAssignmentResult(result, requestId, assignedResourceId);
            break;
        }
        case 3: {
            int type = 0;
            if (!readInt("Resource type (1=Counter, 2=Delivery): ", &type)) { cout << "Invalid type.\n"; break; }
            const int index = findNextAvailableIndex(&resourcePool, type);
            if (index < 0) cout << "No available resource found for that type.\n";
            else printf("Next available resource: %d\n", resourcePool.records[index].id);
            break;
        }
        case 4: {
            int id = 0;
            if (!readInt("Resource ID to search: ", &id)) { cout << "Invalid ID.\n"; break; }
            const int index = findResourceIndex(&resourcePool, id);
            if (index < 0) { cout << "Resource not found.\n"; break; }
            char typeText[RESOURCE_TEXT_SIZE], statusText[RESOURCE_TEXT_SIZE], assignmentText[RESOURCE_TEXT_SIZE];
            getResourceTypeText(resourcePool.records[index].type, typeText, sizeof(typeText));
            getResourceStatusText(resourcePool.records[index].status, statusText, sizeof(statusText));
            getAssignmentText(&resourcePool, index, assignmentText, sizeof(assignmentText));
            printf("Resource ID: %d | Type: %s | Status: %s | Assignment: %s\n",
                   resourcePool.records[index].id, typeText, statusText, assignmentText);
            break;
        }
        case 5: {
            int id = 0;
            if (!readInt("Resource ID to release: ", &id)) { cout << "Invalid ID.\n"; break; }
            if (releaseResource(&resourcePool, id)) cout << "Resource released successfully.\n";
            else cout << "Release failed; resource may not be busy.\n";
            break;
        }
        case 6: {
            int id = 0, status = 0;
            if (!readInt("Resource ID: ", &id) || !readInt("Status (2=Available, 3=Unavailable): ", &status)) {
                cout << "Invalid status input.\n"; break;
            }
            if (status != RESOURCE_AVAILABLE && status != RESOURCE_UNAVAILABLE) {
                cout << "Use release to make a busy resource available; busy status is assigned with a request.\n"; break;
            }
            if (updateResourceStatus(&resourcePool, id, status)) cout << "Resource status updated.\n";
            else cout << "Status update failed.\n";
            break;
        }
        case 7: {
            int id = 0;
            if (!readInt("Resource ID to remove: ", &id)) { cout << "Invalid ID.\n"; break; }
            if (removeResource(&resourcePool, id)) cout << "Resource removed successfully.\n";
            else cout << "Resource removal failed; resource may be missing or busy.\n";
            break;
        }
        case 8: showResourceBoard(&resourcePool); break;
        case 9: showResourceActivityHistory(); break;
        case 0: cout << "Exiting Task 3 module.\n"; break;
        default: cout << "Invalid option. Please select again.\n"; break;
        }
    } while (choice != 0);
    return 0;
}
