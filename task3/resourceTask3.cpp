#include "resourceTask3.hpp"

#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

ResourceQueue resourcePool = {};
static char activityHistory[RESOURCE_LOG_LIMIT][RESOURCE_TEXT_SIZE] = {};
static int activityCount = 0;

static bool validQueue(const ResourceQueue *queue)
{
    if (!queue || queue->size < 0 || queue->size > RESOURCE_MAX_SIZE)
        return false;
    if (queue->size == 0)
        return queue->head == 0 && queue->tail == -1;
    if (queue->head < 0 || queue->head >= RESOURCE_MAX_SIZE ||
        queue->tail < 0 || queue->tail >= RESOURCE_MAX_SIZE)
        return false;
    return queue->tail == (queue->head + queue->size - 1) % RESOURCE_MAX_SIZE;
}

static int activeOffsetForIndex(const ResourceQueue *queue, int index)
{
    if (!validQueue(queue) || index < 0 || index >= RESOURCE_MAX_SIZE)
        return -1;
    for (int offset = 0; offset < queue->size; ++offset)
    {
        if ((queue->head + offset) % RESOURCE_MAX_SIZE == index)
            return offset;
    }
    return -1;
}

static bool validRequestId(const char *requestId)
{
    if (!requestId || strncmp(requestId, "REQ", 3) != 0)
        return false;
    const size_t length = strlen(requestId);
    if (length < 4 || length >= RESOURCE_REQUEST_ID_SIZE)
        return false;
    for (size_t i = 3; i < length; ++i)
    {
        if (requestId[i] < '0' || requestId[i] > '9')
            return false;
    }
    return true;
}

static int requiredResourceType(int serviceType)
{
    if (serviceType < TASK3_RECIPIENT_COLLECTION || serviceType > TASK3_RETURN_TO_SENDER)
        return 0;
    // Team allocation rule: recipient collection uses a counter; all other listed
    // services use a delivery resource.
    return serviceType == TASK3_RECIPIENT_COLLECTION ? COUNTER : DELIVERY;
}

static bool hasActiveAssignment(const ResourceQueue *queue, const char *requestId, int parcelId)
{
    if (!validQueue(queue))
        return false;
    for (int offset = 0; offset < queue->size; ++offset)
    {
        const ResourceRecord &record = queue->records[(queue->head + offset) % RESOURCE_MAX_SIZE];
        if (record.status == RESOURCE_BUSY && record.requestId[0] != '\0' &&
            (strcmp(record.requestId, requestId) == 0 || record.parcelId == parcelId))
            return true;
    }
    return false;
}

void initResourceQueue(ResourceQueue *queue)
{
    if (!queue)
        return;
    queue->head = 0;
    queue->tail = -1;
    queue->size = 0;
    for (int i = 0; i < RESOURCE_MAX_SIZE; ++i)
    {
        queue->records[i] = ResourceRecord{0, 0, RESOURCE_AVAILABLE, "", 0};
    }
}

bool isResourceQueueEmpty(const ResourceQueue *queue)
{
    return !validQueue(queue) || queue->size == 0;
}

bool isResourceQueueFull(const ResourceQueue *queue)
{
    return validQueue(queue) && queue->size == RESOURCE_MAX_SIZE;
}

int findResourceIndex(const ResourceQueue *queue, int resourceId)
{
    if (!validQueue(queue) || resourceId <= 0)
        return -1;
    for (int offset = 0; offset < queue->size; ++offset)
    {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        if (queue->records[index].id == resourceId)
            return index;
    }
    return -1;
}

int findNextAvailableIndex(const ResourceQueue *queue, int resourceType)
{
    if (!validQueue(queue) || (resourceType != COUNTER && resourceType != DELIVERY))
        return -1;
    for (int offset = 0; offset < queue->size; ++offset)
    {
        const int index = (queue->head + offset) % RESOURCE_MAX_SIZE;
        if (queue->records[index].type == resourceType &&
            queue->records[index].status == RESOURCE_AVAILABLE)
            return index;
    }
    return -1;
}

int countResourceStatus(const ResourceQueue *queue, int resourceType, int status)
{
    if (!validQueue(queue))
        return 0;
    int count = 0;
    for (int offset = 0; offset < queue->size; ++offset)
    {
        const ResourceRecord &record = queue->records[(queue->head + offset) % RESOURCE_MAX_SIZE];
        if ((resourceType == 0 || record.type == resourceType) && record.status == status)
            ++count;
    }
    return count;
}

void getResourceTypeText(int type, char *out, size_t len)
{
    if (!out || len == 0)
        return;
    if (type == COUNTER)
        snprintf(out, len, "Counter");
    else if (type == DELIVERY)
        snprintf(out, len, "Delivery");
    else
        snprintf(out, len, "Unknown");
}

void getResourceStatusText(int status, char *out, size_t len)
{
    if (!out || len == 0)
        return;
    if (status == RESOURCE_AVAILABLE)
        snprintf(out, len, "Available");
    else if (status == RESOURCE_BUSY)
        snprintf(out, len, "Busy");
    else if (status == RESOURCE_UNAVAILABLE)
        snprintf(out, len, "Unavailable");
    else
        snprintf(out, len, "Unknown");
}

void getAssignmentText(const ResourceQueue *queue, int index, char *out, size_t len)
{
    if (!out || len == 0)
        return;
    if (activeOffsetForIndex(queue, index) < 0)
    {
        snprintf(out, len, "-");
        return;
    }
    const ResourceRecord &record = queue->records[index];
    if (record.status != RESOURCE_BUSY || record.requestId[0] == '\0')
    {
        snprintf(out, len, "-");
    }
    else
    {
        snprintf(out, len, "Request %s / Parcel #%d", record.requestId, record.parcelId);
    }
}

void recordResourceActivity(const char *message)
{
    if (!message)
        return;
    if (activityCount >= RESOURCE_LOG_LIMIT)
    {
        for (int i = 1; i < RESOURCE_LOG_LIMIT; ++i)
            memcpy(activityHistory[i - 1], activityHistory[i], RESOURCE_TEXT_SIZE);
        activityCount = RESOURCE_LOG_LIMIT - 1;
    }
    snprintf(activityHistory[activityCount++], RESOURCE_TEXT_SIZE, "%s", message);
}

bool registerResource(ResourceQueue *queue, int resourceId, int resourceType)
{
    if (!validQueue(queue) || resourceId <= 0 ||
        (resourceType != COUNTER && resourceType != DELIVERY) ||
        isResourceQueueFull(queue) || findResourceIndex(queue, resourceId) != -1)
        return false;

    const int insertIndex = queue->size == 0 ? 0 : (queue->tail + 1) % RESOURCE_MAX_SIZE;
    queue->records[insertIndex] = ResourceRecord{resourceId, resourceType, RESOURCE_AVAILABLE, "", 0};
    if (queue->size == 0)
        queue->head = insertIndex;
    queue->tail = insertIndex;
    ++queue->size;

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d registered as %s", resourceId,
             resourceType == COUNTER ? "Counter" : "Delivery");
    recordResourceActivity(message);
    return true;
}

bool assignResource(ResourceQueue *queue, int resourceId, const char *requestId,
                    int parcelId, int requiredType)
{
    if (!validQueue(queue) || resourceId <= 0 || parcelId <= 0 ||
        !validRequestId(requestId) || (requiredType != COUNTER && requiredType != DELIVERY))
        return false;
    const int index = findResourceIndex(queue, resourceId);
    if (index < 0 || queue->records[index].type != requiredType ||
        queue->records[index].status != RESOURCE_AVAILABLE ||
        hasActiveAssignment(queue, requestId, parcelId))
        return false;

    ResourceRecord &record = queue->records[index];
    record.status = RESOURCE_BUSY;
    snprintf(record.requestId, sizeof(record.requestId), "%s", requestId);
    record.parcelId = parcelId;

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d assigned to Request %s / Parcel #%d",
             resourceId, requestId, parcelId);
    recordResourceActivity(message);
    return true;
}

int assignRequestToResource(ResourceQueue *queue, const char *requestId,
                            int parcelId, int serviceType, int *assignedResourceId)
{
    if (!validQueue(queue) || !validRequestId(requestId) || parcelId <= 0)
        return ASSIGN_INVALID_REQUEST;
    const int requiredType = requiredResourceType(serviceType);
    if (requiredType == 0)
        return ASSIGN_INVALID_REQUEST;
    if (hasActiveAssignment(queue, requestId, parcelId))
        return ASSIGN_DUPLICATE;

    bool suitableExists = false;
    for (int offset = 0; offset < queue->size; ++offset)
    {
        const ResourceRecord &record = queue->records[(queue->head + offset) % RESOURCE_MAX_SIZE];
        if (record.type == requiredType)
            suitableExists = true;
    }
    if (!suitableExists)
        return ASSIGN_NO_SUITABLE;

    const int index = findNextAvailableIndex(queue, requiredType);
    if (index < 0)
        return ASSIGN_ALL_OCCUPIED;
    const int resourceId = queue->records[index].id;
    if (!assignResource(queue, resourceId, requestId, parcelId, requiredType))
        return ASSIGN_INVALID_REQUEST;
    if (assignedResourceId)
        *assignedResourceId = resourceId;
    return ASSIGN_OK;
}

bool releaseResource(ResourceQueue *queue, int resourceId)
{
    const int index = findResourceIndex(queue, resourceId);
    if (index < 0 || queue->records[index].status != RESOURCE_BUSY)
        return false;
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

bool updateResourceStatus(ResourceQueue *queue, int resourceId, int newStatus)
{
    if (newStatus != RESOURCE_AVAILABLE && newStatus != RESOURCE_BUSY &&
        newStatus != RESOURCE_UNAVAILABLE)
        return false;
    const int index = findResourceIndex(queue, resourceId);
    if (index < 0)
        return false;
    ResourceRecord &record = queue->records[index];
    // Busy is only valid together with an actual assignment.
    if (newStatus == RESOURCE_BUSY || record.status == newStatus ||
        (record.status == RESOURCE_BUSY && newStatus == RESOURCE_UNAVAILABLE))
        return false;

    record.status = newStatus;
    record.requestId[0] = '\0';
    record.parcelId = 0;
    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d marked %s", resourceId,
             newStatus == RESOURCE_AVAILABLE ? "Available" : "Unavailable");
    recordResourceActivity(message);
    return true;
}

bool removeResource(ResourceQueue *queue, int resourceId)
{
    const int index = findResourceIndex(queue, resourceId);
    if (index < 0 || queue->records[index].status == RESOURCE_BUSY)
        return false;
    const int position = activeOffsetForIndex(queue, index);
    if (position < 0)
        return false;

    for (int offset = position; offset < queue->size - 1; ++offset)
    {
        const int current = (queue->head + offset) % RESOURCE_MAX_SIZE;
        const int next = (queue->head + offset + 1) % RESOURCE_MAX_SIZE;
        queue->records[current] = queue->records[next];
    }
    --queue->size;
    if (queue->size == 0)
    {
        queue->head = 0;
        queue->tail = -1;
    }
    else
    {
        queue->tail = (queue->head + queue->size - 1) % RESOURCE_MAX_SIZE;
    }

    char message[RESOURCE_TEXT_SIZE];
    snprintf(message, sizeof(message), "Resource %d removed", resourceId);
    recordResourceActivity(message);
    return true;
}

void showResourceBoard(const ResourceQueue *queue)
{
    if (isResourceQueueEmpty(queue))
    {
        cout << "No resources registered.\n";
        return;
    }
    cout << "\nResource board\nID  Type      Status        Assignment\n";
    for (int offset = 0; offset < queue->size; ++offset)
    {
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

void showResourceActivityHistory()
{
    if (activityCount == 0)
    {
        cout << "No activity recorded.\n";
        return;
    }
    cout << "\nResource activity log (most recent first)\n";
    for (int i = activityCount - 1; i >= 0; --i)
        cout << (activityCount - i) << ". " << activityHistory[i] << '\n';
}
