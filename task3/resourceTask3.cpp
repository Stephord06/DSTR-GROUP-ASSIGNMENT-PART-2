#include "resourceTask3.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
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
    if (!queue || resourceId <= 0 || queue->size < 0 || queue->size > RESOURCE_MAX_SIZE) return -1;
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        if (index >= 0 && queue->records[index].id == resourceId) return index;
    }
    return -1;
}

int findNextAvailableIndex(const ResourceQueue *queue, int resourceType) {
    if (!queue || (resourceType != COUNTER && resourceType != DELIVERY) ||
        queue->size < 0 || queue->size > RESOURCE_MAX_SIZE) return -1;
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        if (index >= 0 && queue->records[index].type == resourceType &&
            queue->records[index].status == RESOURCE_AVAILABLE) return index;
    }
    return -1;
}

int countResourceStatus(const ResourceQueue *queue, int resourceType, int status) {
    if (!queue || queue->size < 0 || queue->size > RESOURCE_MAX_SIZE) return 0;
    int count = 0;
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        if (index < 0) continue;
        const ResourceRecord &record = queue->records[index];
        if ((resourceType == 0 || record.type == resourceType) && record.status == status) {
            ++count;
        }
    }
    return count;
}

void getResourceTypeText(int type, char *out, size_t len) {
    if (!out || len == 0) return;
    if (type == COUNTER) snprintf(out, len, "Counter");
    else if (type == DELIVERY) snprintf(out, len, "Delivery");
    else snprintf(out, len, "Unknown");
}

void getResourceStatusText(int status, char *out, size_t len) {
    if (!out || len == 0) return;
    if (status == RESOURCE_AVAILABLE) snprintf(out, len, "Available");
    else if (status == RESOURCE_BUSY) snprintf(out, len, "Busy");
    else if (status == RESOURCE_UNAVAILABLE) snprintf(out, len, "Unavailable");
    else snprintf(out, len, "Unknown");
}

void getAssignmentText(const ResourceQueue *queue, int index, char *out, size_t len) {
    if (!out || len == 0) return;
    if (!queue || index < 0 || index >= RESOURCE_MAX_SIZE || queue->size < 0 ||
        queue->size > RESOURCE_MAX_SIZE) {
    if (!queue || index < 0 || index >= RESOURCE_MAX_SIZE) {
        snprintf(out, len, "-");
        return;
    }

    bool indexIsActive = false;
    for (int offset = 0; offset < queue->size; ++offset) {
        if ((queue->head + offset) % RESOURCE_MAX_SIZE == index) {
            indexIsActive = true;
            break;
        }
    }
    if (!indexIsActive) {
        snprintf(out, len, "-");
        return;
    }

    bool indexIsActive = false;
    for (int offset = 0; offset < queue->size; ++offset) {
        if ((queue->head + offset) % RESOURCE_MAX_SIZE == index) {
            indexIsActive = true;
            break;
        }
    }
    if (!indexIsActive) {
        snprintf(out, len, "-");
        return;
    }

    const ResourceRecord &record = queue->records[index];
    if (record.kind == ASSIGN_PARCEL) snprintf(out, len, "Parcel #%d", record.parcelId);
    else if (record.kind == ASSIGN_REQUEST)
        snprintf(out, len, "Request #%d / Parcel #%d", record.itemId, record.parcelId);
    else snprintf(out, len, "-");
}

void recordResourceActivity(const char *message) {
    if (!message) return;
    if (activityCount >= RESOURCE_LOG_LIMIT) {
        for (int i = 1; i < RESOURCE_LOG_LIMIT; ++i) {
            memcpy(activityHistory[i - 1], activityHistory[i], RESOURCE_TEXT_SIZE);
        }
        activityCount = RESOURCE_LOG_LIMIT - 1;
    }
    snprintf(activityHistory[activityCount], RESOURCE_TEXT_SIZE, "%s", message);
    ++activityCount;
}

bool registerResource(ResourceQueue *queue, int resourceId, int resourceType) {
    if (!queue || queue->size < 0 || queue->size > RESOURCE_MAX_SIZE ||
        resourceId <= 0 || (resourceType != COUNTER && resourceType != DELIVERY) ||
        isResourceQueueFull(queue) || findResourceIndex(queue, resourceId) != -1) return false;

    const int insertIndex = (queue->size == 0) ? 0 : (queue->tail + 1) % RESOURCE_MAX_SIZE;
    queue->records[insertIndex] = {resourceId, resourceType, RESOURCE_AVAILABLE, NO_ASSIGNMENT, 0, 0};
    if (queue->size == 0) queue->head = insertIndex;
    queue->tail = insertIndex;
    ++queue->size;

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d registered as %s",
             resourceId, resourceType == COUNTER ? "Counter" : "Delivery");
    recordResourceActivity(message);
    return true;
}

bool assignResource(ResourceQueue *queue, int resourceId, int assignmentKind,
                    int itemId, int parcelId) {
    if (!queue || resourceId <= 0 || itemId <= 0 ||
        (assignmentKind != ASSIGN_PARCEL && assignmentKind != ASSIGN_REQUEST)) return false;
    if (assignmentKind == ASSIGN_REQUEST && parcelId <= 0) return false;

    const int index = findResourceIndex(queue, resourceId);
    if (index == -1 || queue->records[index].status != RESOURCE_AVAILABLE) return false;

    for (int offset = 0; offset < queue->size; ++offset) {
        const int other = (queue->head + offset) % RESOURCE_MAX_SIZE;
        const ResourceRecord &record = queue->records[other];
        if (record.status == RESOURCE_BUSY && record.kind == assignmentKind && record.itemId == itemId)
            return false;
    }

    ResourceRecord &record = queue->records[index];
    record.status = RESOURCE_BUSY;
    record.kind = assignmentKind;
    record.itemId = itemId;
    record.parcelId = assignmentKind == ASSIGN_PARCEL ? itemId : parcelId;

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d assigned to %s #%d", resourceId,
             assignmentKind == ASSIGN_REQUEST ? "Request" : "Parcel", itemId);
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
    snprintf(message, sizeof(message), "Resource %d released %s #%d", resourceId,
             kind == ASSIGN_REQUEST ? "Request" : "Parcel", itemId);
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
    if (record.status == newStatus || (record.status == RESOURCE_BUSY && newStatus == RESOURCE_UNAVAILABLE))
    if (record.status == newStatus) return false;
    if (record.status == RESOURCE_BUSY && newStatus == RESOURCE_UNAVAILABLE) {
        return false;

    if (newStatus != RESOURCE_BUSY) {
        record.kind = NO_ASSIGNMENT;
        record.itemId = 0;
        record.parcelId = 0;
    }
    record.status = newStatus;

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d marked %s", resourceId,
             newStatus == RESOURCE_AVAILABLE ? "Available" :
             newStatus == RESOURCE_BUSY ? "Busy" : "Unavailable");
    recordResourceActivity(message);
    return true;
}

bool removeResource(ResourceQueue *queue, int resourceId) {
    if (!queue || resourceId <= 0 || queue->size < 0 || queue->size > RESOURCE_MAX_SIZE) return false;
    const int index = findResourceIndex(queue, resourceId);
    if (index == -1 || queue->records[index].status == RESOURCE_BUSY) return false;

    int logicalOffset = -1;
    for (int offset = 0; offset < queue->size; ++offset) {
        if ((queue->head + offset) % RESOURCE_MAX_SIZE == index) {
            logicalOffset = offset;
            break;
        }
    }
    if (logicalOffset == -1) return false;

    for (int offset = logicalOffset; offset < queue->size - 1; ++offset) {
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

    cout << "\nResource board\nID  Type      Status        Assignment\n";
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        const ResourceRecord &record = queue->records[index];
        char typeText[RESOURCE_TEXT_SIZE], statusText[RESOURCE_TEXT_SIZE], assignmentText[RESOURCE_TEXT_SIZE];
        getResourceTypeText(record.type, typeText, sizeof(typeText));
        getResourceStatusText(record.status, statusText, sizeof(statusText));
        getAssignmentText(queue, index, assignmentText, sizeof(assignmentText));
        printf("%-3d %-9s %-13s %s\n", record.id, typeText, statusText, assignmentText);
    }

    printf("Available: %d | Busy: %d | Unavailable: %d\n",
           countResourceStatus(queue, 0, RESOURCE_AVAILABLE),
           countResourceStatus(queue, 0, RESOURCE_BUSY),
           countResourceStatus(queue, 0, RESOURCE_UNAVAILABLE));
}

void showResourceActivityHistory() {
    if (activityCount == 0) {
        cout << "No activity recorded.\n";
        return;
    }
    cout << "\nResource activity log\n";
    for (int i = activityCount - 1; i >= 0; --i) {
        cout << (activityCount - i) << ". " << activityHistory[i] << '\n';
    }
}
