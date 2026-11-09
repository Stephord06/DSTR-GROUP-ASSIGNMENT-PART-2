#ifndef RESOURCE_TASK3_HPP
#define RESOURCE_TASK3_HPP
#include <cstddef>

#define RESOURCE_MAX_SIZE 20
#define RESOURCE_TEXT_SIZE 128
#define RESOURCE_LOG_LIMIT 40
#define RESOURCE_REQUEST_ID_SIZE 16

struct Queue; // Task 2 service request queue (task2/queue.cpp)

enum ResourceType { COUNTER = 1, DELIVERY = 2 };

enum ResourceStatus {
  RESOURCE_BUSY = 1,
  RESOURCE_AVAILABLE = 2,
  RESOURCE_UNAVAILABLE = 3
};

enum AssignResult {
  ASSIGN_OK = 0,
  ASSIGN_INVALID_REQUEST, // blank, too long or not in the active request queue
  ASSIGN_REQUEST_CLOSED,  // request already completed or cancelled
  ASSIGN_DUPLICATE,       // request or its parcel already holds a resource
  ASSIGN_NO_SUITABLE,     // no resource of the required type is registered
  ASSIGN_ALL_OCCUPIED     // required type exists but none is available
};

struct ResourceRecord {
  int id;
  int type;
  int status;
  char requestId[RESOURCE_REQUEST_ID_SIZE]; // "" when nothing is assigned
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
bool assignResource(ResourceQueue *queue, int resourceId, const char *requestId, int parcelId);
int assignRequestToResource(ResourceQueue *queue, Queue &serviceQueue, const char *requestId, int *assignedResourceId);
bool releaseResource(ResourceQueue *queue, int resourceId);
bool updateResourceStatus(ResourceQueue *queue, int resourceId, int newStatus);
bool removeResource(ResourceQueue *queue, int resourceId);

void recordResourceActivity(const char *message);
void showResourceActivityHistory();
void showResourceBoard(const ResourceQueue *queue);

bool readInt(const char *prompt, int *value);
bool readTextLine(const char *prompt, char *buffer, size_t maxLen);
int displayResourceMenu(Queue &serviceQueue);

#endif
