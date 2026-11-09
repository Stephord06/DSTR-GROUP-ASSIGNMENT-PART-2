#pragma once
#include "parcel.cpp"
#include <iostream>
using namespace std;

struct TreeNode
{
  Parcel parcel;
  TreeNode *leftnode = nullptr;
  TreeNode *rightnode = nullptr;

  // Constructors to support C++11 allocation
  TreeNode() : leftnode(nullptr), rightnode(nullptr) {}
  TreeNode(const Parcel &p) : parcel(p), leftnode(nullptr), rightnode(nullptr) {}

  void push_child(const Parcel &parcel)
  {
    TreeNode *node = new TreeNode(parcel);
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

  bool delete_by_id(int id)
  {
    TreeNode *parent = nullptr;
    TreeNode *current = this;
    while (current && current->parcel.parcelID != id)
    {
      parent = current;
      current = (id > current->parcel.parcelID) ? current->rightnode
                                                : current->leftnode;
    }

    if (!current)
      return false;
    if (!parent)
      return false;

    if (current->leftnode && current->rightnode)
    {
      TreeNode *succ_parent = current;
      TreeNode *succ = current->rightnode;
      while (succ->leftnode)
      {
        succ_parent = succ;
        succ = succ->leftnode;
      }

      current->parcel = succ->parcel;

      if (succ_parent->leftnode == succ)
      {
        succ_parent->leftnode = succ->rightnode;
      }
      else
      {
        succ_parent->rightnode = succ->rightnode;
      }
      delete succ;
      return true;
    }

    TreeNode *child =
        current->leftnode ? current->leftnode : current->rightnode;
    if (parent->leftnode == current)
    {
      parent->leftnode = child;
    }
    else
    {
      parent->rightnode = child;
    }
    delete current;
    return true;
  }
};