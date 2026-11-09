#ifndef RESOURCE_TASK3_CPP
#define RESOURCE_TASK3_CPP

#include "resourceTask3.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

ResourceQueue resourcePool;

static struct {
    int id[RESOURCE_MAX_SIZE];
    int type[RESOURCE_MAX_SIZE];
    int status[RESOURCE_MAX_SIZE];
    int kind[RESOURCE_MAX_SIZE];
    int item[RESOURCE_MAX_SIZE];
    int parcel[RESOURCE_MAX_SIZE];
    int count;
} activityLogData;

static void resetActivityLog() {
    memset(activityLogData.id, 0, sizeof(activityLogData.id));
    memset(activityLogData.type, 0, sizeof(activityLogData.type));
    memset(activityLogData.status, 0, sizeof(activityLogData.status));
    memset(activityLogData.kind, 0, sizeof(activityLogData.kind));
    memset(activityLogData.item, 0, sizeof(activityLogData.item));
    memset(activityLogData.parcel, 0, sizeof(activityLogData.parcel));
    activityLogData.count = 0;
}

void initResourceQueue(ResourceQueue *queue) {
    if (!queue) return;
    queue->head = 0;
    queue->tail = -1;
    queue->size = 0;
    for (int i = 0; i < RESOURCE_MAX_SIZE; ++i) {
        queue->data[i].id = 0;
        queue->data[i].type = 0;
        queue->data[i].status = RESOURCE_AVAILABLE;
        queue->data[i].kind = NO_ASSIGNMENT;
        queue->data[i].item = 0;
        queue->data[i].parcel = 0;
    }
}

bool isResourceQueueEmpty(const ResourceQueue *queue) {
    return queue == nullptr || queue->size == 0;
}

bool isResourceQueueFull(const ResourceQueue *queue) {
    return queue != nullptr && queue->size >= RESOURCE_MAX_SIZE;
}

int findResourceIndex(const ResourceQueue *queue, int resourceId) {
    if (!queue || resourceId <= 0) return -1;
    for (int i = 0; i < queue->size; ++i) {
        int index = (queue->head + i) % RESOURCE_MAX_SIZE;
        if (queue->data[index].id == resourceId) return index;
    }
    return -1;
}

int checkAvailability(const ResourceQueue *queue, int type) {
    if (!queue || type <= 0) return -1;
    int total = 0;
    int available = 0;
    int busy = 0;
    for (int i = 0; i < queue->size; ++i) {
        int idx = (queue->head + i) % RESOURCE_MAX_SIZE;
        if (queue->data[idx].type != type) continue;
        total++;
        if (queue->data[idx].status == RESOURCE_AVAILABLE) available++;
        else if (queue->data[idx].status == RESOURCE_BUSY) busy++;
    }
    if (total == 0) return -4;
    if (available == 0) return busy > 0 ? -6 : -5;
    return 1;
}

int findNextAvailableIndex(const ResourceQueue *queue, int type) {
    if (!queue || type <= 0) return -1;
    for (int i = 0; i < queue->size; ++i) {
        int idx = (queue->head + i) % RESOURCE_MAX_SIZE;
        if (queue->data[idx].type == type && queue->data[idx].status == RESOURCE_AVAILABLE) {
            return idx;
        }
    }
    return -1;
}

void getResourceTypeText(int type, char *out, size_t len) {
    if (!out || len == 0) return;
    if (type == COUNTER) {
        snprintf(out, len, "Counter");
    } else if (type == DELIVERY) {
        snprintf(out, len, "Delivery");
    } else {
        snprintf(out, len, "Unknown");
    }
}

void getResourceStatusText(int status, char *out, size_t len) {
    if (!out || len == 0) return;
    if (status == RESOURCE_AVAILABLE) {
        snprintf(out, len, "Available");
    } else if (status == RESOURCE_BUSY) {
        snprintf(out, len, "Busy");
    } else if (status == RESOURCE_UNAVAILABLE) {
        snprintf(out, len, "Unavailable");
    } else {
        snprintf(out, len, "Unknown");
    }
}

void getAssignmentText(const ResourceQueue *queue, int index, char *out, size_t len) {
    if (!out || len == 0) return;
    if (!queue || index < 0 || index >= RESOURCE_MAX_SIZE) {
        snprintf(out, len, "None");
        return;
    }

    const ResourceRecord &r = queue->data[index];
    if (r.kind == NO_ASSIGNMENT) {
        snprintf(out, len, "None");
        return;
    }
    if (r.kind == ASSIGN_PARCEL) {
        snprintf(out, len, "Parcel #%d", r.item);
    } else if (r.kind == ASSIGN_REQUEST) {
        snprintf(out, len, "REQ%d -> Parcel #%d", r.item, r.parcel);
    } else {
        snprintf(out, len, "None");
    }
}

bool registerResource(ResourceQueue *queue, int resourceId, int type) {
    if (!queue) return false;
    if (resourceId <= 0 || (type != COUNTER && type != DELIVERY)) return false;
    if (findResourceIndex(queue, resourceId) != -1) return false;
    if (isResourceQueueFull(queue)) return false;

    int insertIndex = (queue->tail + 1) % RESOURCE_MAX_SIZE;
    queue->data[insertIndex].id = resourceId;
    queue->data[insertIndex].type = type;
    queue->data[insertIndex].status = RESOURCE_AVAILABLE;
    queue->data[insertIndex].kind = NO_ASSIGNMENT;
    queue->data[insertIndex].item = 0;
    queue->data[insertIndex].parcel = 0;
    queue->tail = insertIndex;
    if (queue->size == 0) queue->head = insertIndex;
    queue->size++;
    return true;
}

bool assignResource(ResourceQueue *queue, int resourceId, int kind, int itemId, int parcelId) {
    if (!queue || resourceId <= 0) return false;
    int idx = findResourceIndex(queue, resourceId);
    if (idx == -1) return false;
    if (queue->data[idx].status == RESOURCE_UNAVAILABLE) return false;
    if (queue->data[idx].status == RESOURCE_BUSY) return false;
    if (kind != ASSIGN_PARCEL && kind != ASSIGN_REQUEST) return false;

    queue->data[idx].kind = kind;
    queue->data[idx].item = itemId;
    queue->data[idx].parcel = parcelId;
    queue->data[idx].status = RESOURCE_BUSY;
    return true;
}

bool releaseResource(ResourceQueue *queue, int resourceId) {
    if (!queue || resourceId <= 0) return false;
    int idx = findResourceIndex(queue, resourceId);
    if (idx == -1) return false;
    if (queue->data[idx].status != RESOURCE_BUSY) return false;

    queue->data[idx].status = RESOURCE_AVAILABLE;
    queue->data[idx].kind = NO_ASSIGNMENT;
    queue->data[idx].item = 0;
    queue->data[idx].parcel = 0;
    return true;
}

bool updateResourceStatus(ResourceQueue *queue, int resourceId, int newStatus) {
    if (!queue || resourceId <= 0) return false;
    if (newStatus != RESOURCE_AVAILABLE && newStatus != RESOURCE_BUSY && newStatus != RESOURCE_UNAVAILABLE) return false;

    int idx = findResourceIndex(queue, resourceId);
    if (idx == -1) return false;

    queue->data[idx].status = newStatus;
    if (newStatus != RESOURCE_BUSY) {
        queue->data[idx].kind = NO_ASSIGNMENT;
        queue->data[idx].item = 0;
        queue->data[idx].parcel = 0;
    }
    return true;
}

bool removeResource(ResourceQueue *queue, int resourceId) {
    if (!queue || resourceId <= 0) return false;
    int idx = findResourceIndex(queue, resourceId);
    if (idx == -1) return false;
    if (queue->data[idx].status == RESOURCE_BUSY) return false;

    if (queue->size == 1) {
        initResourceQueue(queue);
        return true;
    }

    for (int i = 0; i < queue->size - 1; ++i) {
        int current = (queue->head + i) % RESOURCE_MAX_SIZE;
        int next = (current + 1) % RESOURCE_MAX_SIZE;
        queue->data[current] = queue->data[next];
    }

    queue->tail = (queue->tail - 1 + RESOURCE_MAX_SIZE) % RESOURCE_MAX_SIZE;
    queue->size--;
    if (queue->size == 0) {
        initResourceQueue(queue);
    }
    return true;
}

void logActivity(const char *message) {
    if (!message) return;
    if (activityLogData.count >= RESOURCE_MAX_SIZE) return;
    activityLogData.id[activityLogData.count] = activityLogData.count + 1;
    activityLogData.type[activityLogData.count] = 0;
    activityLogData.status[activityLogData.count] = 0;
    activityLogData.kind[activityLogData.count] = 0;
    activityLogData.item[activityLogData.count] = 0;
    activityLogData.parcel[activityLogData.count] = 0;
    activityLogData.count++;
    printf("[ACTIVITY] %s\n", message);
}

void showActivityHistory() {
    if (activityLogData.count == 0) {
        cout << "No activity history available." << endl;
        return;
    }
    cout << "\nActivity History" << endl;
    for (int i = 0; i < activityLogData.count; ++i) {
        cout << "- Event " << i + 1 << ": " << activityLogData.id[i] << "\n";
    }
}

void showResourceBoard(const ResourceQueue *queue) {
    if (!queue || queue->size == 0) {
        cout << "No resources registered." << endl;
        return;
    }
    cout << "\nResource Board" << endl;
    cout << "ID | Type | Status | Assignment" << endl;
    for (int i = 0; i < queue->size; ++i) {
        int idx = (queue->head + i) % RESOURCE_MAX_SIZE;
        char typeText[RESOURCE_NAME_LIMIT];
        char statusText[RESOURCE_NAME_LIMIT];
        char assignmentText[RESOURCE_NAME_LIMIT];
        getResourceTypeText(queue->data[idx].type, typeText, sizeof(typeText));
        getResourceStatusText(queue->data[idx].status, statusText, sizeof(statusText));
        getAssignmentText(queue, idx, assignmentText, sizeof(assignmentText));
        printf("%2d | %-7s | %-11s | %s\n",
               queue->data[idx].id,
               typeText,
               statusText,
               assignmentText);
    }
}

void runDemoBenchmark() {
    ResourceQueue temp;
    initResourceQueue(&temp);
    registerResource(&temp, 10, COUNTER);
    registerResource(&temp, 20, DELIVERY);
    assignResource(&temp, 10, ASSIGN_REQUEST, 1, 101);
    releaseResource(&temp, 10);
    showResourceBoard(&temp);
    cout << "Benchmark completed." << endl;
}

void traceMessage(const char *action, const char *detail) {
    if (!action) action = "TRACE";
    if (detail && detail[0] != '\0') {
        printf("[%s] %s\n", action, detail);
    } else {
        printf("[%s]\n", action);
    }
}

bool readInt(const char *prompt, int *value) {
    if (prompt) printf("%s", prompt);
    fflush(stdout);
    int temp = 0;
    if (scanf("%d", &temp) != 1) {
        while (getchar() != '\n' && getchar() != EOF) {}
        return false;
    }
    if (value) *value = temp;
    while (getchar() != '\n' && getchar() != EOF) {}
    return true;
}

bool readTextLine(const char *prompt, char *buffer, size_t maxLen) {
    if (prompt) printf("%s", prompt);
    fflush(stdout);
    if (!buffer || maxLen == 0) return false;
    if (!fgets(buffer, (int)maxLen, stdin)) return false;
    buffer[strcspn(buffer, "\r\n")] = '\0';
    return true;
}

int displayResourceMenu() {
    int choice = 0;
    initResourceQueue(&resourcePool);

    do {
        cout << "\n=========================================" << endl;
        cout << " Task 3: Resource Management Module" << endl;
        cout << "=========================================" << endl;
        cout << "1. Register Resource" << endl;
        cout << "2. Assign Request/Parcel" << endl;
        cout << "3. View Next Available Resource" << endl;
        cout << "4. Search Resource" << endl;
        cout << "5. Release Resource" << endl;
        cout << "6. Update Resource Status" << endl;
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
                if (!readInt("Resource ID: ", &id) || !readInt("Type (1=Counter, 2=Delivery): ", &type)) {
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
                    !readInt("Item ID: ", &item) ||
                    !readInt("Parcel ID: ", &parcel)) {
                    cout << "Invalid assignment input." << endl;
                    break;
                }
                if (assignResource(&resourcePool, id, kind, item, parcel)) {
                    cout << "Assignment successful." << endl;
                    traceMessage("Assign Resource", "Resource assigned to parcel/request.");
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
                char typeText[RESOURCE_NAME_LIMIT], statusText[RESOURCE_NAME_LIMIT], assignmentText[RESOURCE_NAME_LIMIT];
                getResourceTypeText(resourcePool.data[idx].type, typeText, sizeof(typeText));
                getResourceStatusText(resourcePool.data[idx].status, statusText, sizeof(statusText));
                getAssignmentText(&resourcePool, idx, assignmentText, sizeof(assignmentText));
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
                if (!readInt("Resource ID: ", &id) || !readInt("Status (1=Busy, 2=Available, 3=Unavailable): ", &status)) {
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
                    cout << "Resource removed." << endl;
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

#endif
