#pragma once
#include "parcel.cpp"
#include <iostream>
using namespace std;

struct TreeNode
{
  Parcel parcel;
  TreeNode *leftnode = nullptr;
  TreeNode *rightnode = nullptr;

  void push_child(const Parcel &parcel) {
    TreeNode *node = new TreeNode{parcel};
    TreeNode *temp = this;
    while (true)
    {
      if (node->parcel.parcelID > temp->parcel.parcelID)
      {
        if (!temp->rightnode)
        {
          temp->rightnode = node;
          break;
        }
        else
        {
          temp = temp->rightnode;
        }
      }
      else
      {
        if (!temp->leftnode)
        {
          temp->leftnode = node;
          break;
        }
        else
        {
          temp = temp->leftnode;
        }
      }
    }
  }

  TreeNode *search_by_id(int id)
  {
    TreeNode *temp = this;
    while (temp)
    {
      if (id == temp->parcel.parcelID)
      {
        return temp;
      }
      if (id > temp->parcel.parcelID)
      {
        temp = temp->rightnode;
      }
      else
      {
        temp = temp->leftnode;
      }
    }
    return nullptr;
  }
  bool delete_by_id(int id) {
    TreeNode *parent = nullptr;
    TreeNode *current = this;
    while (current && current->parcel.parcelID != id) {
      parent = current;
      current = (id > current->parcel.parcelID) ? current->rightnode
                                                : current->leftnode;
    }

    if (!current)
      return false;
    if (!parent)
      return false;

    if (current->leftnode && current->rightnode) {
      TreeNode *succ_parent = current;
      TreeNode *succ = current->rightnode;
      while (succ->leftnode) {
        succ_parent = succ;
        succ = succ->leftnode;
      }

      current->parcel = succ->parcel;

      if (succ_parent->leftnode == succ) {
        succ_parent->leftnode = succ->rightnode;
      } else {
        succ_parent->rightnode = succ->rightnode;
      }
      delete succ;
      return true;
    }

    TreeNode *child =
        current->leftnode ? current->leftnode : current->rightnode;
    if (parent->leftnode == current) {
      parent->leftnode = child;
    } else {
      parent->rightnode = child;
    }
    delete current;
    return true;
  }
};

// int main(int argc, char *argv[]) {
//   Parcel new_parcel = Parcel{
//       "", "gr", "goie", 4, 5, true, ParcelType::DOCUMENT,
//   };
//   Parcel new_parcel2 = Parcel{
//       "", "gr", "gqwe", 4, 7, true, ParcelType::DOCUMENT,
//   };
//   Parcel new_parcel3 = Parcel{
//       "", "gr", "iriwur", 4, 3, true, ParcelType::DOCUMENT,
//   };
//   Parcel new_parcel4 = Parcel{
//       "", "gr", "gwoegjfi", 4, 16, true, ParcelType::DOCUMENT,
//   };
//   Parcel new_parcel5 = Parcel{
//       "", "gr", "gwoegjfi", 4, 8, true, ParcelType::DOCUMENT,
//   };
//   Parcel new_parcel6 = Parcel{
//       "", "gr", "gwoegjfi", 4, 9, true, ParcelType::DOCUMENT,
//   };
//   Parcel new_parcel7 = Parcel{
//       "", "gr", "gwoegjfi", 4, 10, true, ParcelType::DOCUMENT,
//   };
//   Parcel new_parcel8 = Parcel{
//       "", "gr", "gwoegjfi", 4, 24, true, ParcelType::DOCUMENT,
//   };
//   TreeNode new_node = TreeNode{new_parcel};
//   new_node.push_child(new_parcel2);
//   new_node.push_child(new_parcel3);
//   new_node.push_child(new_parcel4);
//   new_node.push_child(new_parcel5);
//   new_node.push_child(new_parcel6);
//   new_node.push_child(new_parcel7);
//   new_node.push_child(new_parcel8);
//
//   // cout << new_node.rightnode->rightnode->parcel.parcelID;
//   new_node.delete_by_id(25);
//   cout << new_node.rightnode->rightnode->parcel.parcelID;
//
//   // cout << new_node.search_by_id(16)->parcel.parcelID;
//
//   return 0;
// }
