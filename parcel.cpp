#pragma once
#include <string>

using namespace std;

enum ParcelStatus
{
  ORDERED,
  SHIPPED,
  DELIVERED,
  COLLECTED,
  RETURNED,
  CANCELLED,
  ON_HOLD,
};

enum ParcelType
{
  DOCUMENT,
  ELECTRONIC,
  FRAGILE_GOODS,
  GENERAL_PARCEL,
};

struct Parcel
{
  string recipient_name, delivery_address, arrival_date;
  // The IDs are integers to be easier to handle with trees
  int recipientID, parcelID;
  bool fragile;
  ParcelType type;
  ParcelStatus status = ParcelStatus::ORDERED;
};
