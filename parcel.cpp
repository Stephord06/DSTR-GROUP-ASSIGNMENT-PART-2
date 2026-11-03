#include <string>
using namespace std;

enum ParcelStatus {
  ORDERED,
  SHIPED,
  DELIVERED,
};

enum ParcelType {
  DOCUMENT,
  ELECTRONIC,
};

struct Parcel {
  string recipient_name, delivery_address, arrival_date;
  // The IDs are integers to be easier to handle with trees
  int recipientID, parcelID;
  bool fragile;
  ParcelType type;
  ParcelStatus status = ParcelStatus::ORDERED;
};
