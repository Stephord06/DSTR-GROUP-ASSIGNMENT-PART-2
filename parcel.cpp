#pragma once
#include <string>
using namespace std;

enum ParcelStatus {
  ORDERED,
  SHIPED,
  DELIVERED,
  ON_HOLD,
};

enum ParcelType {
  DOCUMENT,
  ELECTRONIC,
};

struct Parcel {
  string recipient_name, delivery_address, arrival_date;
  // The IDs are integers to be easier to handle with trees
  int recipientID = 0, parcelID = 0;
  bool fragile = false;
  ParcelType type = ParcelType::DOCUMENT;
  ParcelStatus status = ParcelStatus::ORDERED;
};
