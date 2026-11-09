#include "resourceTask3.hpp"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

static bool menuInputClosed = false;

static bool readMenuInt(const char *prompt, int *value) {
    char line[RESOURCE_TEXT_SIZE];
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }
    if (!fgets(line, sizeof(line), stdin)) {
        menuInputClosed = true;
        return false;
    }
    char *end = nullptr;
    errno = 0;
    long parsed = strtol(line, &end, 10);
    if (end == line || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) return false;
    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') ++end;
    if (*end != '\0') return false;
    *value = static_cast<int>(parsed);
    return true;
}

static bool promptPositiveInt(const char *prompt, int *value) {
    if (!readMenuInt(prompt, value)) return false;
    if (*value <= 0) {
        cout << "Enter a positive number.\n";
        return false;
    }
    return true;
}

int displayResourceMenu() {
    int choice = 0;

    do {
        cout << "\n=== Task 3: Collection Counter and Delivery Resource Management ===\n"
             << "1. Register resource\n"
             << "2. Assign parcel or service request\n"
             << "3. View next available resource\n"
             << "4. Search resource\n"
             << "5. Release resource\n"
             << "6. Update resource status\n"
             << "7. Remove resource\n"
             << "8. Display availability\n"
             << "9. View recent activity\n"
             << "0. Return to main menu\n";

        if (!readMenuInt("Select: ", &choice)) {
            if (menuInputClosed) return 0;
            cout << "Invalid selection. Enter a menu number.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            int id, type;
            if (!promptPositiveInt("Resource ID: ", &id) ||
                !readMenuInt("Type (1=Counter, 2=Delivery): ", &type)) {
                if (!menuInputClosed) cout << "Invalid resource details.\n";
                break;
            }
            if (registerResource(&resourcePool, id, type)) cout << "Resource registered.\n";
            else cout << "Registration rejected (invalid, duplicate, or full pool).\n";
            break;
        }
        case 2: {
            int kind, itemId, parcelId, type, resourceId;
            if (!readMenuInt("Assignment (1=Parcel, 2=Service request): ", &kind) ||
                !promptPositiveInt("Item ID: ", &itemId)) {
                if (!menuInputClosed) cout << "Invalid assignment details.\n";
                break;
            }
            if (kind == ASSIGN_PARCEL) {
                parcelId = itemId;
            } else if (kind == ASSIGN_REQUEST) {
                if (!promptPositiveInt("Associated parcel ID: ", &parcelId)) {
                    cout << "Invalid parcel ID.\n";
                    break;
                }
            } else {
                cout << "Choose 1 for parcel or 2 for service request.\n";
                break;
            }
            if (!readMenuInt("Resource type (1=Counter, 2=Delivery): ", &type) ||
                (type != COUNTER && type != DELIVERY)) {
                cout << "Invalid resource type.\n";
                break;
            }
            if (!readMenuInt("Resource ID (0=select next available): ", &resourceId) || resourceId < 0) {
                cout << "Invalid resource ID.\n";
                break;
            }
            if (resourceId == 0) {
                const int index = findNextAvailableIndex(&resourcePool, type);
                if (index == -1) {
                    cout << "No suitable resource is available.\n";
                    break;
                }
                resourceId = resourcePool.records[index].id;
            } else {
                const int index = findResourceIndex(&resourcePool, resourceId);
                if (index == -1 || resourcePool.records[index].type != type) {
                    cout << "Resource not found or type does not match.\n";
                    break;
                }
            }
            if (assignResource(&resourcePool, resourceId, kind, itemId, parcelId))
                cout << "Assignment recorded on resource " << resourceId << ".\n";
            else
                cout << "Assignment rejected (resource unavailable, occupied, or duplicate work).\n";
            break;
        }
        case 3: {
            int type;
            if (!readMenuInt("Type (1=Counter, 2=Delivery): ", &type) ||
                (type != COUNTER && type != DELIVERY)) {
                cout << "Invalid resource type.\n";
                break;
            }
            const int index = findNextAvailableIndex(&resourcePool, type);
            if (index == -1) cout << "No available resource of that type.\n";
            else cout << "Next available resource: " << resourcePool.records[index].id << '\n';
            break;
        }
        case 4: {
            int id;
            if (!promptPositiveInt("Resource ID: ", &id)) {
                if (!menuInputClosed) cout << "Invalid resource ID.\n";
                break;
            }
            const int index = findResourceIndex(&resourcePool, id);
            if (index == -1) {
                cout << "Resource not found.\n";
                break;
            }
            ResourceQueue one = {};
            one.head = one.tail = 0;
            one.size = 1;
            one.records[0] = resourcePool.records[index];
            showResourceBoard(&one);
            break;
        }
        case 5: {
            int id;
            if (!promptPositiveInt("Resource ID: ", &id)) {
                if (!menuInputClosed) cout << "Invalid resource ID.\n";
                break;
            }
            if (releaseResource(&resourcePool, id)) cout << "Resource released.\n";
            else cout << "Release rejected (resource not found or not occupied).\n";
            break;
        }
        case 6: {
            int id, status;
            if (!promptPositiveInt("Resource ID: ", &id) ||
                !readMenuInt("Status (1=Occupied, 2=Available, 3=Unavailable): ", &status) ||
                status < RESOURCE_BUSY || status > RESOURCE_UNAVAILABLE) {
                if (!menuInputClosed) cout << "Invalid status details.\n";
                break;
            }
            if (updateResourceStatus(&resourcePool, id, status)) cout << "Resource status updated.\n";
            else cout << "Status update rejected.\n";
            break;
        }
        case 7: {
            int id;
            if (!promptPositiveInt("Resource ID: ", &id)) {
                if (!menuInputClosed) cout << "Invalid resource ID.\n";
                break;
            }
            if (removeResource(&resourcePool, id)) cout << "Resource removed.\n";
            else cout << "Removal rejected (not found or currently occupied).\n";
            break;
        }
        case 8:
            showResourceBoard(&resourcePool);
            break;
        case 9:
            showResourceActivityHistory();
            break;
        case 0:
            break;
        default:
            cout << "Invalid selection.\n";
            break;
        }
    } while (choice != 0 && !menuInputClosed);

    return 0;
}
