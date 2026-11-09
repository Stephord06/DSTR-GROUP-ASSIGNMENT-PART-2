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
  ON_HOLD
};

inline string status_to_string(ParcelStatus status)
{
  switch (status)
  {
  case ORDERED:
    return "Ordered";
  case SHIPPED:
    return "Shipped";
  case DELIVERED:
    return "Delivered";
  case COLLECTED:
    return "Collected";
  case RETURNED:
    return "Returned";
  case CANCELLED:
    return "Cancelled";
  case ON_HOLD:
    return "On hold";
  default:
    return "Unknown";
  }
}

enum ParcelType
{
  DOCUMENT,
  ELECTRONIC,
  FRAGILE_GOODS,
  GENERAL_PARCEL
};

inline string type_to_string(ParcelType type)
{
  switch (type)
  {
  case DOCUMENT:
    return "Document";
  case ELECTRONIC:
    return "Electronic";
  case FRAGILE_GOODS:
    return "Fragile Goods";
  case GENERAL_PARCEL:
    return "General Parcel";
  default:
    return "Unknown";
  }
}

inline string fragile_to_string(bool is_fragile)
{
  return is_fragile ? "Yes" : "No";
}

struct Parcel
{
  string recipient_name, delivery_address, arrival_date;
  int recipientID, parcelID;
  bool fragile;
  ParcelType type;
  ParcelStatus status = ParcelStatus::ORDERED;
};