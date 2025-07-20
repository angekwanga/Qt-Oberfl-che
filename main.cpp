#include "network.h"
#include "scheduled_trip.h"
#include <iostream>

int main(int argc, char **argv) {
  bht::Network n{argv[1]};
  bht::NetworkScheduledTrip trip = n.getScheduledTrip("230419258");
  for (bht::NetworkScheduledTrip::iterator iter = trip.begin(); iter != trip.end(); iter++) {
    std::cout << iter->stopSequence << ": " << n.stops[iter->stopId].name << std::endl;
  }
}