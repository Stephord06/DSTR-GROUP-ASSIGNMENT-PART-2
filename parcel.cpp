#include <string>
using namespace std;

enum ParcelStatus {
  ORDERED,
  SHIPPED,
  DELIVERED,
  ON_HOLD,
};
string status_to_string(ParcelStatus status) {
  switch (status) {
  case ORDERED: {
    return "Ordered";
  }
  case SHIPPED: {
    return "Shipped";
  }
  case ON_HOLD: {
    return "On hold";
  }
  case DELIVERED: {
    return "Delivered";
  }
  }
}

enum ParcelType {
  DOCUMENT,
  ELECTRONIC,
};

string type_to_string(ParcelType type) {
  switch (type) {
  case DOCUMENT: {
    return "Document";
  }
  case ELECTRONIC: {
    return "Electronic";
  }
  }
}
string fragile_to_string(bool is_fragile) {
  if (is_fragile) {
    return "Yes";
  } else {
    return "No";
  }
}

struct Parcel {
  string recipient_name, delivery_address, arrival_date;
  // The IDs are integers to be easier to handle with trees
  int recipientID, parcelID;
  bool fragile;
  ParcelType type;
  ParcelStatus status = ParcelStatus::ORDERED;
};
