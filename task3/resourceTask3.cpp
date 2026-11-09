#include "resourceTask3.hpp"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>

using namespace std;

ResourceQueue resourcePool = {};

static char activityHistory[RESOURCE_LOG_LIMIT][RESOURCE_TEXT_SIZE] = {};
static int activityCount = 0;

void initResourceQueue(ResourceQueue *queue) {
    if (!queue) return;
    queue->head = 0;
    queue->tail = -1;
    queue->size = 0;
    for (int i = 0; i < RESOURCE_MAX_SIZE; ++i) {
        queue->records[i] = {0, 0, RESOURCE_AVAILABLE, NO_ASSIGNMENT, 0, 0};
    }
}

bool isResourceQueueEmpty(const ResourceQueue *queue) {
    return !queue || queue->size == 0;
}

bool isResourceQueueFull(const ResourceQueue *queue) {
    return queue && queue->size >= RESOURCE_MAX_SIZE;
}

int findResourceIndex(const ResourceQueue *queue, int resourceId) {
    if (!queue || resourceId <= 0) return -1;
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        if (queue->records[index].id == resourceId) return index;
    }
    return -1;
}

int findNextAvailableIndex(const ResourceQueue *queue, int resourceType) {
    if (!queue || (resourceType != COUNTER && resourceType != DELIVERY)) return -1;
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        if (queue->records[index].type == resourceType &&
            queue->records[index].status == RESOURCE_AVAILABLE) return index;
    }
    return -1;
}

int countResourceStatus(const ResourceQueue *queue, int resourceType, int status) {
    if (!queue) return 0;
    int count = 0;
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        const ResourceRecord &record = queue->records[index];
        if ((resourceType == 0 || record.type == resourceType) && record.status == status) ++count;
    }
    return count;
}

void recordResourceActivity(const char *message) {
    if (!message) return;
    if (activityCount == RESOURCE_LOG_LIMIT) {
        for (int i = 1; i < RESOURCE_LOG_LIMIT; ++i) {
            memcpy(activityHistory[i - 1], activityHistory[i], RESOURCE_TEXT_SIZE);
        }
        --activityCount;
    }
    snprintf(activityHistory[activityCount], RESOURCE_TEXT_SIZE, "%s", message);
    ++activityCount;
}

bool registerResource(ResourceQueue *queue, int resourceId, int resourceType) {
    if (!queue || resourceId <= 0 ||
        (resourceType != COUNTER && resourceType != DELIVERY) ||
        isResourceQueueFull(queue) || findResourceIndex(queue, resourceId) != -1) return false;

    const int index = (queue->size == 0) ? 0 : (queue->tail + 1) % RESOURCE_MAX_SIZE;
    queue->records[index] = {resourceId, resourceType, RESOURCE_AVAILABLE, NO_ASSIGNMENT, 0, 0};
    if (queue->size == 0) queue->head = index;
    queue->tail = index;
    ++queue->size;

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d registered (%s)", resourceId,
             resourceType == COUNTER ? "counter" : "delivery");
    recordResourceActivity(message);
    return true;
}

bool assignResource(ResourceQueue *queue, int resourceId, int assignmentKind,
                    int itemId, int parcelId) {
    if (!queue || resourceId <= 0 || itemId <= 0 ||
        (assignmentKind != ASSIGN_PARCEL && assignmentKind != ASSIGN_REQUEST) ||
        (assignmentKind == ASSIGN_REQUEST && parcelId <= 0)) return false;

    const int index = findResourceIndex(queue, resourceId);
    if (index == -1 || queue->records[index].status != RESOURCE_AVAILABLE) return false;

    // Prevent the same parcel/request from being actively assigned twice.
    for (int offset = 0; offset < queue->size; ++offset) {
        const int other = (queue->head + offset) % RESOURCE_MAX_SIZE;
        const ResourceRecord &record = queue->records[other];
        if (record.status == RESOURCE_BUSY && record.kind == assignmentKind &&
            record.itemId == itemId) return false;
    }

    ResourceRecord &record = queue->records[index];
    record.status = RESOURCE_BUSY;
    record.kind = assignmentKind;
    record.itemId = itemId;
    record.parcelId = assignmentKind == ASSIGN_PARCEL ? itemId : parcelId;

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d assigned %s %d", resourceId,
             assignmentKind == ASSIGN_REQUEST ? "request" : "parcel", itemId);
    recordResourceActivity(message);
    return true;
}

bool releaseResource(ResourceQueue *queue, int resourceId) {
    if (!queue || resourceId <= 0) return false;
    const int index = findResourceIndex(queue, resourceId);
    if (index == -1 || queue->records[index].status != RESOURCE_BUSY) return false;

    ResourceRecord &record = queue->records[index];
    const int itemId = record.itemId;
    const int kind = record.kind;
    record.status = RESOURCE_AVAILABLE;
    record.kind = NO_ASSIGNMENT;
    record.itemId = 0;
    record.parcelId = 0;

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d released %s %d", resourceId,
             kind == ASSIGN_REQUEST ? "request" : (kind == ASSIGN_PARCEL ? "parcel" : "manual hold"), itemId);
    recordResourceActivity(message);
    return true;
}

bool updateResourceStatus(ResourceQueue *queue, int resourceId, int newStatus) {
    if (!queue || resourceId <= 0 ||
        (newStatus != RESOURCE_AVAILABLE && newStatus != RESOURCE_BUSY &&
         newStatus != RESOURCE_UNAVAILABLE)) return false;
    const int index = findResourceIndex(queue, resourceId);
    if (index == -1) return false;

    ResourceRecord &record = queue->records[index];
    if (record.status == newStatus) return false;
    if (newStatus == RESOURCE_BUSY && record.status == RESOURCE_UNAVAILABLE) return false;

    // Changing an assigned resource away from BUSY explicitly clears its assignment.
    if (newStatus != RESOURCE_BUSY) {
        record.kind = NO_ASSIGNMENT;
        record.itemId = 0;
        record.parcelId = 0;
    }
    record.status = newStatus;

    const char *statusText = newStatus == RESOURCE_AVAILABLE ? "available" :
                             newStatus == RESOURCE_BUSY ? "busy" : "unavailable";
    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d marked %s", resourceId, statusText);
    recordResourceActivity(message);
    return true;
}

bool removeResource(ResourceQueue *queue, int resourceId) {
    if (!queue || resourceId <= 0) return false;
    const int index = findResourceIndex(queue, resourceId);
    if (index == -1 || queue->records[index].status == RESOURCE_BUSY) return false;

    const int offsetToRemove = (index - queue->head + RESOURCE_MAX_SIZE) % RESOURCE_MAX_SIZE;
    for (int offset = offsetToRemove; offset < queue->size - 1; ++offset) {
        const int current = (queue->head + offset) % RESOURCE_MAX_SIZE;
        const int next = (queue->head + offset + 1) % RESOURCE_MAX_SIZE;
        queue->records[current] = queue->records[next];
    }

    --queue->size;
    if (queue->size == 0) {
        queue->head = 0;
        queue->tail = -1;
    } else {
        queue->tail = (queue->head + queue->size - 1) % RESOURCE_MAX_SIZE;
    }

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d removed", resourceId);
    recordResourceActivity(message);
    return true;
}

void showResourceBoard(const ResourceQueue *queue) {
    if (isResourceQueueEmpty(queue)) {
        cout << "No resources registered.\n";
        return;
    }

    cout << "\nResource availability (" << queue->size << "/" << RESOURCE_MAX_SIZE << ")\n";
    cout << "ID  Type      Status        Assignment\n";
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        const ResourceRecord &record = queue->records[index];
        const char *type = record.type == COUNTER ? "Counter" : "Delivery";
        const char *status = record.status == RESOURCE_AVAILABLE ? "Available" :
                             record.status == RESOURCE_BUSY ? "Occupied" : "Unavailable";
        char assignment[RESOURCE_TEXT_SIZE];
        if (record.kind == ASSIGN_PARCEL) {
            snprintf(assignment, sizeof(assignment), "Parcel #%d", record.parcelId);
        } else if (record.kind == ASSIGN_REQUEST) {
            snprintf(assignment, sizeof(assignment), "Request #%d / Parcel #%d", record.itemId, record.parcelId);
        } else {
            snprintf(assignment, sizeof(assignment), "-");
        }
        printf("%-3d %-9s %-13s %s\n", record.id, type, status, assignment);
    }
    printf("Available: %d | Occupied: %d | Unavailable: %d\n",
           countResourceStatus(queue, 0, RESOURCE_AVAILABLE),
           countResourceStatus(queue, 0, RESOURCE_BUSY),
           countResourceStatus(queue, 0, RESOURCE_UNAVAILABLE));
}

void showResourceActivityHistory() {
    if (activityCount == 0) {
        cout << "No resource activity recorded.\n";
        return;
    }
    cout << "\nRecent resource activity\n";
    for (int i = activityCount - 1; i >= 0; --i) {
        cout << (i + 1) << ". " << activityHistory[i] << '\n';
    }
}
