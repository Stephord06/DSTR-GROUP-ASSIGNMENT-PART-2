#include "resourceTask3.hpp"
#include "../task2/queue.cpp"
#include "../tree.cpp"

#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

ResourceQueue resourcePool = {};
static char activityHistory[RESOURCE_LOG_LIMIT][RESOURCE_TEXT_SIZE] = {};
static int activityCount = 0;

static bool validQueue(const ResourceQueue *queue) {
    if (!queue || queue->size < 0 || queue->size > RESOURCE_MAX_SIZE) return false;
    if (queue->size == 0) return queue->head == 0 && queue->tail == -1;
    if (queue->head < 0 || queue->head >= RESOURCE_MAX_SIZE ||
        queue->tail < 0 || queue->tail >= RESOURCE_MAX_SIZE) return false;
    return queue->tail == (queue->head + queue->size - 1) % RESOURCE_MAX_SIZE;
}

static int activeOffsetForIndex(const ResourceQueue *queue, int index) {
    if (!validQueue(queue) || index < 0 || index >= RESOURCE_MAX_SIZE) return -1;
    for (int offset = 0; offset < queue->size; ++offset)
        if ((queue->head + offset) % RESOURCE_MAX_SIZE == index) return offset;
    return -1;
}

static bool validRequestId(const string& requestId) {
    return !requestId.empty() && requestId.size() < RESOURCE_REQUEST_ID_SIZE;
}

static bool isActiveRequest(const Node *request) {
    return request && (request->status == PENDING || request->status == IN_PROGRESS);
}

static int requiredResourceType(ServiceType serviceType) {
    // Allocation rule: recipient collection uses a counter; other services use delivery.
    return serviceType == RECIPIENT_COLLECTION ? COUNTER : DELIVERY;
}

static bool hasActiveAssignment(const ResourceQueue *queue, const char *requestId, int parcelId) {
    if (!validQueue(queue)) return false;
    for (int offset = 0; offset < queue->size; ++offset) {
        const ResourceRecord &record = queue->records[(queue->head + offset) % RESOURCE_MAX_SIZE];
        if (record.status == RESOURCE_BUSY && record.requestId[0] != '\0' &&
            (strcmp(record.requestId, requestId) == 0 || record.parcelId == parcelId)) return true;
    }
    return false;
}

static Node *getActiveRequestAtPosition(const Queue *requests, int position) {
    if (!requests || position <= 0) return nullptr;
    int currentPosition = 0;
    for (Node *request = requests->front; request; request = request->next) {
        if (isActiveRequest(request) && ++currentPosition == position) return request;
    }
    return nullptr;
}

void initResourceQueue(ResourceQueue *queue) {
    if (!queue) return;
    queue->head = 0;
    queue->tail = -1;
    queue->size = 0;
    for (int i = 0; i < RESOURCE_MAX_SIZE; ++i)
        queue->records[i] = ResourceRecord{0, 0, RESOURCE_AVAILABLE, "", 0};
}

bool isResourceQueueEmpty(const ResourceQueue *queue) {
    return !validQueue(queue) || queue->size == 0;
}

bool isResourceQueueFull(const ResourceQueue *queue) {
    return validQueue(queue) && queue->size == RESOURCE_MAX_SIZE;
}

int findResourceIndex(const ResourceQueue *queue, int resourceId) {
    if (!validQueue(queue) || resourceId <= 0) return -1;
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        if (queue->records[index].id == resourceId) return index;
    }
    return -1;
}

int findNextAvailableIndex(const ResourceQueue *queue, int resourceType) {
    if (!validQueue(queue) || (resourceType != COUNTER && resourceType != DELIVERY)) return -1;
    for (int offset = 0; offset < queue->size; ++offset) {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        if (queue->records[index].type == resourceType &&
            queue->records[index].status == RESOURCE_AVAILABLE) return index;
    }
    return -1;
}

int countResourceStatus(const ResourceQueue *queue, int resourceType, int status) {
    if (!validQueue(queue)) return 0;
    int count = 0;
    for (int offset = 0; offset < queue->size; ++offset) {
        const ResourceRecord &record = queue->records[(queue->head + offset) % RESOURCE_MAX_SIZE];
        if ((resourceType == 0 || record.type == resourceType) && record.status == status) ++count;
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
    if (activeOffsetForIndex(queue, index) < 0) {
        snprintf(out, len, "-");
        return;
    }
    const ResourceRecord &record = queue->records[index];
    if (record.status != RESOURCE_BUSY || record.requestId[0] == '\0')
        snprintf(out, len, "-");
    else
        snprintf(out, len, "Request %s / Parcel #%d", record.requestId, record.parcelId);
}

void recordResourceActivity(const char *message) {
    if (!message) return;
    if (activityCount >= RESOURCE_LOG_LIMIT) {
        for (int i = 1; i < RESOURCE_LOG_LIMIT; ++i)
            memcpy(activityHistory[i - 1], activityHistory[i], RESOURCE_TEXT_SIZE);
        activityCount = RESOURCE_LOG_LIMIT - 1;
    }
    snprintf(activityHistory[activityCount++], RESOURCE_TEXT_SIZE, "%s", message);
}

bool registerResource(ResourceQueue *queue, int resourceId, int resourceType) {
    if (!validQueue(queue) || resourceId <= 0 ||
        (resourceType != COUNTER && resourceType != DELIVERY) ||
        isResourceQueueFull(queue) || findResourceIndex(queue, resourceId) != -1) return false;

    const int insertIndex = queue->size == 0 ? 0 : (queue->tail + 1) % RESOURCE_MAX_SIZE;
    queue->records[insertIndex] = ResourceRecord{resourceId, resourceType, RESOURCE_AVAILABLE, "", 0};
    if (queue->size == 0) queue->head = insertIndex;
    queue->tail = insertIndex;
    ++queue->size;

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d registered as %s", resourceId,
             resourceType == COUNTER ? "Counter" : "Delivery");
    recordResourceActivity(message);
    return true;
}

int assignRequestByPosition(ResourceQueue *resources, const Queue *requests,
                            TreeNode *parcels, int activePosition,
                            int *assignedResourceId, char *assignedRequestId,
                            size_t requestIdCapacity) {
    if (!validQueue(resources) || !requests || !parcels || activePosition <= 0 ||
        !assignedRequestId || requestIdCapacity == 0) return ASSIGN_INVALID_REQUEST;
    assignedRequestId[0] = '\0';
    if (assignedResourceId) *assignedResourceId = 0;

    Node *request = getActiveRequestAtPosition(requests, activePosition);
    if (!request || !validRequestId(request->requestID) || request->parcelID <= 0)
        return ASSIGN_INVALID_REQUEST;

    TreeNode *parcelNode = parcels->search_by_id(request->parcelID);
    if (!parcelNode || parcelNode->parcel.status == ParcelStatus::DELIVERED)
        return ASSIGN_INVALID_REQUEST;

    const int neededType = requiredResourceType(request->serviceType);
    if (hasActiveAssignment(resources, request->requestID.c_str(), request->parcelID))
        return ASSIGN_DUPLICATE;

    bool suitableResourceExists = false;
    for (int offset = 0; offset < resources->size; ++offset) {
        const ResourceRecord &resource = resources->records[(resources->head + offset) % RESOURCE_MAX_SIZE];
        if (resource.type == neededType) suitableResourceExists = true;
    }
    if (!suitableResourceExists) return ASSIGN_NO_SUITABLE;

    const int availableIndex = findNextAvailableIndex(resources, neededType);
    if (availableIndex < 0) return ASSIGN_ALL_OCCUPIED;
    if (request->requestID.size() >= requestIdCapacity ||
        request->requestID.size() >= RESOURCE_REQUEST_ID_SIZE) return ASSIGN_INVALID_REQUEST;

    ResourceRecord &resource = resources->records[availableIndex];
    resource.status = RESOURCE_BUSY;
    snprintf(resource.requestId, sizeof(resource.requestId), "%s", request->requestID.c_str());
    resource.parcelId = request->parcelID;
    if (assignedResourceId) *assignedResourceId = resource.id;
    snprintf(assignedRequestId, requestIdCapacity, "%s", request->requestID.c_str());

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d assigned to Request %s / Parcel #%d",
             resource.id, resource.requestId, resource.parcelId);
    recordResourceActivity(message);
    return ASSIGN_OK;
}

bool releaseResource(ResourceQueue *queue, int resourceId) {
    const int index = findResourceIndex(queue, resourceId);
    if (index < 0 || queue->records[index].status != RESOURCE_BUSY) return false;
    ResourceRecord &record = queue->records[index];
    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d released Request %s / Parcel #%d",
             resourceId, record.requestId, record.parcelId);
    record.status = RESOURCE_AVAILABLE;
    record.requestId[0] = '\0';
    record.parcelId = 0;
    recordResourceActivity(message);
    return true;
}

bool updateResourceStatus(ResourceQueue *queue, int resourceId, int newStatus) {
    if (newStatus != RESOURCE_AVAILABLE && newStatus != RESOURCE_UNAVAILABLE) return false;
    const int index = findResourceIndex(queue, resourceId);
    if (index < 0) return false;
    ResourceRecord &record = queue->records[index];
    // Use releaseResource to clear an assignment; don't allow status changes to orphan it.
    if (record.status == RESOURCE_BUSY || record.status == newStatus) return false;
    record.status = newStatus;
    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d marked %s", resourceId,
             newStatus == RESOURCE_AVAILABLE ? "Available" : "Unavailable");
    recordResourceActivity(message);
    return true;
}

bool removeResource(ResourceQueue *queue, int resourceId) {
    const int index = findResourceIndex(queue, resourceId);
    if (index < 0 || queue->records[index].status == RESOURCE_BUSY) return false;
    const int position = activeOffsetForIndex(queue, index);
    if (position < 0) return false;

    for (int offset = position; offset < queue->size - 1; ++offset) {
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

void showAssignableRequests(const Queue *requests) {
    if (!requests) {
        cout << "Service request list is unavailable.\n";
        return;
    }
    int position = 0;
    for (Node *request = requests->front; request; request = request->next) {
        if (!isActiveRequest(request)) continue;
        cout << ++position << ". Request ID: " << request->requestID
             << " | Parcel ID: " << request->parcelID
             << " | Service: ";
        switch (request->serviceType) {
        case RECIPIENT_COLLECTION: cout << "Recipient Collection"; break;
        case CAMPUS_DELIVERY: cout << "Campus Delivery"; break;
        case SPECIAL_HANDLING: cout << "Special Handling"; break;
        case RE_DELIVERY: cout << "Re-Delivery"; break;
        case RETURN_TO_SENDER: cout << "Return to Sender"; break;
        default: cout << "Unknown"; break;
        }
        cout << " | Status: " << (request->status == PENDING ? "Pending" : "In Progress") << '\n';
    }
    if (position == 0) cout << "No pending or in-progress service requests are available.\n";
}

void showResourceActivityHistory() {
    if (activityCount == 0) {
        cout << "No activity recorded.\n";
        return;
    }
    cout << "\nResource activity log (most recent first)\n";
    for (int i = activityCount - 1; i >= 0; --i)
        cout << (activityCount - i) << ". " << activityHistory[i] << '\n';
}
