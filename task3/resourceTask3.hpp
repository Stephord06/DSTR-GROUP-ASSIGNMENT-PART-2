#ifndef RESOURCE_TASK3_HPP
#define RESOURCE_TASK3_HPP

#include <cstddef>

#define RESOURCE_MAX_SIZE 20
#define RESOURCE_TEXT_SIZE 128
#define RESOURCE_LOG_LIMIT 40

enum ResourceType {
    COUNTER = 1,
    DELIVERY = 2
};

enum ResourceStatus {
    RESOURCE_BUSY = 1,
    RESOURCE_AVAILABLE = 2,
    RESOURCE_UNAVAILABLE = 3
};

enum AssignmentKind {
    NO_ASSIGNMENT = 0,
    ASSIGN_PARCEL = 1,
    ASSIGN_REQUEST = 2
};

struct ResourceRecord {
    int id;
    int type;
    int status;
    int kind;
    int itemId;
    int parcelId;
};

struct ResourceQueue {
    ResourceRecord records[RESOURCE_MAX_SIZE];
    int head;
    int tail;
    int size;
};

extern ResourceQueue resourcePool;

void initResourceQueue(ResourceQueue *queue);
bool isResourceQueueEmpty(const ResourceQueue *queue);
bool isResourceQueueFull(const ResourceQueue *queue);

int findResourceIndex(const ResourceQueue *queue, int resourceId);
int findNextAvailableIndex(const ResourceQueue *queue, int resourceType);
int countResourceStatus(const ResourceQueue *queue, int resourceType, int status);

void getResourceTypeText(int type, char *out, size_t len);
void getResourceStatusText(int status, char *out, size_t len);
void getAssignmentText(const ResourceQueue *queue, int index, char *out, size_t len);

bool registerResource(ResourceQueue *queue, int resourceId, int resourceType);
bool assignResource(ResourceQueue *queue, int resourceId, int assignmentKind,
                    int itemId, int parcelId);
bool releaseResource(ResourceQueue *queue, int resourceId);
bool updateResourceStatus(ResourceQueue *queue, int resourceId, int newStatus);
bool removeResource(ResourceQueue *queue, int resourceId);

void recordResourceActivity(const char *message);
void showResourceActivityHistory();
void showResourceBoard(const ResourceQueue *queue);

bool readInt(const char *prompt, int *value);
bool readTextLine(const char *prompt, char *buffer, size_t maxLen);
int displayResourceMenu();

#endif
