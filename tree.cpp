#include "parcel.cpp"
#include <iostream>
using namespace std;

struct TreeNode {
  Parcel parcel;
  TreeNode *leftnode = nullptr;
  TreeNode *rightnode = nullptr;

  void push_child(TreeNode *node) {
    TreeNode *temp = this;
    while (true) {
      if (node->parcel.parcelID > temp->parcel.parcelID) {
        if (!temp->rightnode) {
          temp->rightnode = node;
          break;
        } else {
          temp = temp->rightnode;
        }
      } else {
        if (!temp->leftnode) {
          temp->leftnode = node;
          break;
        } else {
          temp = temp->leftnode;
        }
      }
    }
  }

  Parcel *search_by_id(int id) {
    TreeNode *temp = this;
    while (true) {
      if (id == temp->parcel.parcelID) {
        return &temp->parcel;
      }
      if (id > temp->parcel.parcelID) {
        temp = temp->rightnode;

      } else if (id < temp->parcel.parcelID) {
        temp = temp->leftnode;
      } else {
        break;
      }
    }
    return nullptr;
  }
};
