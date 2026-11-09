#include "task3/resourceTask3.hpp"

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

    char *end = nullptr;
    errno = 0;
    long parsed = strtol(line, &end, 10);
    if (end == line || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) {
        clearInputBuffer();
        return false;
    }

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') {
        ++end;
    }
    if (*end != '\0') {
        clearInputBuffer();
        return false;
    }

    if (value) {
        *value = static_cast<int>(parsed);
    }
    return true;
}

bool readTextLine(const char *prompt, char *buffer, size_t maxLen) {
    if (!buffer || maxLen == 0) return false;
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }

    if (!fgets(buffer, static_cast<int>(maxLen), stdin)) {
        menuInputClosed = true;
        return false;
    }

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
        cout << "9. Show Activity History" << endl;
        cout << "0. Exit" << endl;
        cout << "=========================================" << endl;

        if (!readInt("Enter your choice: ", &choice)) {
            if (menuInputClosed) break;
            cout << "Invalid input. Please enter a valid number." << endl;
            continue;
        }

        switch (choice) {
        case 1: {
            int id = 0, type = 0;
            if (!readInt("Resource ID: ", &id) || !readInt("Type (1=Counter, 2=Delivery): ", &type)) {
                cout << "Invalid resource input." << endl;
                break;
            }
            if (registerResource(&resourcePool, id, type)) {
                cout << "Resource registered successfully." << endl;
            } else {
                cout << "Could not register resource." << endl;
            }
            break;
        }
        case 2: {
            int id = 0, kind = 0, itemId = 0, parcelId = 0;
            if (!readInt("Resource ID: ", &id) ||
                !readInt("Assignment kind (1=Parcel, 2=Request): ", &kind) ||
                !readInt("Item ID: ", &itemId) ||
                !readInt("Parcel ID: ", &parcelId)) {
                cout << "Invalid assignment input." << endl;
                break;
            }
            if (assignResource(&resourcePool, id, kind, itemId, parcelId)) {
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
            const int idx = findResourceIndex(&resourcePool, id);
            if (idx == -1) {
                cout << "Resource not found." << endl;
                break;
            }

            char typeText[RESOURCE_TEXT_SIZE];
            char statusText[RESOURCE_TEXT_SIZE];
            char assignmentText[RESOURCE_TEXT_SIZE];
            getResourceTypeText(resourcePool.records[idx].type, typeText, sizeof(typeText));
            getResourceStatusText(resourcePool.records[idx].status, statusText, sizeof(statusText));
            getAssignmentText(&resourcePool, idx, assignmentText, sizeof(assignmentText));

            printf("Resource ID: %d | Type: %s | Status: %s | Assignment: %s\n",
                   resourcePool.records[idx].id, typeText, statusText, assignmentText);
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
            if (status == 1) status = RESOURCE_BUSY;
            else if (status == 2) status = RESOURCE_AVAILABLE;
            else if (status == 3) status = RESOURCE_UNAVAILABLE;
            else {
                cout << "Invalid status value." << endl;
                break;
            }
            if (updateResourceStatus(&resourcePool, id, status)) {
                cout << "Resource status updated." << endl;
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
