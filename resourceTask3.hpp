#ifndef RESOURCE_TASK3_HPP
#define RESOURCE_TASK3_HPP

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define RESOURCE_MAX_SIZE 20
#define RESOURCE_NOTE_LIMIT 256
#define RESOURCE_LOG_LIMIT 64
#define RESOURCE_NAME_LIMIT 32

enum ResourceType {
    COUNTER = 1,
    DELIVERY = 2
};

enum ResourceStatus {
    RESOURCE_AVAILABLE = 2,
    RESOURCE_BUSY = 1,
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
    int item;
    int parcel;
};

struct ResourceQueue {
    ResourceRecord data[RESOURCE_MAX_SIZE];
    int head;
    int tail;
    int size;
};

struct ResourceCounts {
    int total;
    int available;
    int busy;
    int unavailable;
};

extern ResourceQueue resourcePool;

void initResourceQueue(ResourceQueue *queue);
bool isResourceQueueEmpty(const ResourceQueue *queue);
bool isResourceQueueFull(const ResourceQueue *queue);
int findResourceIndex(const ResourceQueue *queue, int resourceId);
int checkAvailability(const ResourceQueue *queue, int type);
int findNextAvailableIndex(const ResourceQueue *queue, int type);
void getResourceTypeText(int type, char *out, size_t len);
void getResourceStatusText(int status, char *out, size_t len);
void getAssignmentText(const ResourceQueue *queue, int index, char *out, size_t len);

bool registerResource(ResourceQueue *queue, int resourceId, int type);
bool assignResource(ResourceQueue *queue, int resourceId, int kind, int itemId, int parcelId);
bool releaseResource(ResourceQueue *queue, int resourceId);
bool updateResourceStatus(ResourceQueue *queue, int resourceId, int newStatus);
bool removeResource(ResourceQueue *queue, int resourceId);

void logActivity(const char *message);
void showActivityHistory();
void showResourceBoard(const ResourceQueue *queue);
void runDemoBenchmark();
void traceMessage(const char *action, const char *detail);

bool readInt(const char *prompt, int *value);
bool readTextLine(const char *prompt, char *buffer, size_t maxLen);
int displayResourceMenu();

#endif
