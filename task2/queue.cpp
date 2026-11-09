#pragma once
#include <iostream>
#include <new>
#include <string>
using namespace std;

enum Status {
    CANCELED,
    PENDING,
    IN_PROGRESS,
    COMPLETED
};

enum ServiceType {
    RECIPIENT_COLLECTION,
    CAMPUS_DELIVERY,
    SPECIAL_HANDLING,
    RE_DELIVERY,
    RETURN_TO_SENDER
};

struct Node {
    string requestID, parcelID, recipientName;
    ServiceType serviceType;
    Status status;
    Node* next;

    Node(string reqID, string parID, string recName, ServiceType servType, Status stat)
        : requestID(reqID), parcelID(parID), recipientName(recName),
          serviceType(servType), status(stat), next(nullptr) {}
};

struct Queue {
    Node* front;
    Node* rear;

    Queue() : front(nullptr), rear(nullptr) {}
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    ~Queue() {
        while (front != nullptr) {
            Node* temp = front;
            front = front->next;
            delete temp;
        }
        rear = nullptr;
    }

    bool isEmpty() const {
        return front == nullptr;
    }

    void enqueue(string reqID, string parID, string recName, ServiceType servType, Status stat) {
        Node* newNode = new (nothrow) Node(reqID, parID, recName, servType, stat);
        if (!newNode) {
            cout << "Memory allocation failed!" << endl;
            return;
        }
        if (isEmpty()) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        cout << "Enqueued: " << reqID << endl;
    }

    void dequeue() {
        if (isEmpty()) {
            cout << "Queue is empty!" << endl;
            return;
        }
        Node* temp = front;
        front = front->next;
        cout << "Dequeued: " << temp->requestID << endl;
        delete temp;
        if (front == nullptr) {
            rear = nullptr;
        }
    }

    string getFront() const {
        return isEmpty() ? "" : front->requestID;
    }

    void deleteSpecificRequest(const string& requestID) {
        if (isEmpty()) {
            cout << "Queue is empty!" << endl;
            return;
        }

        Node* curr = front;
        Node* prev = nullptr;
        while (curr != nullptr && curr->requestID != requestID) {
            prev = curr;
            curr = curr->next;
        }

        if (curr == nullptr) {
            cout << "Request ID " << requestID << " not found!" << endl;
            return;
        }

        if (curr == front) {
            front = front->next;
        } else {
            prev->next = curr->next;
        }
        if (curr == rear) {
            rear = prev;
        }

        delete curr;
        cout << "Request ID " << requestID << " has been deleted from the queue." << endl;
    }
};
