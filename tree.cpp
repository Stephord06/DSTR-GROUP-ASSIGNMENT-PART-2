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

  TreeNode *search_by_id(int id) {
    TreeNode *temp = this;
    while (temp) {
      if (id == temp->parcel.parcelID) {
        return temp;
      }
      if (id > temp->parcel.parcelID) {
        temp = temp->rightnode;
      } else {
        temp = temp->leftnode;
      }
    }
    return nullptr;
  }
};
