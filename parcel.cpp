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
  string recipient_name;
  string delivery_address;
  string arrival_date;
  int recipientID;
  int parcelID;
  bool fragile;
  ParcelType type;
  ParcelStatus status;

  // Default constructor
  Parcel()
      : recipient_name(""), delivery_address(""), arrival_date(""),
        recipientID(0), parcelID(0), fragile(false),
        type(DOCUMENT), status(ORDERED) {}

  // Parameterized constructor (supports both 7-arg and 8-arg initializations)
  Parcel(string name, string addr, string date, int rID, int pID, bool frag,
         ParcelType t, ParcelStatus s = ORDERED)
      : recipient_name(name), delivery_address(addr), arrival_date(date),
        recipientID(rID), parcelID(pID), fragile(frag), type(t), status(s) {}
};