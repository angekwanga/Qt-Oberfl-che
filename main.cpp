#include <iostream>
#include "types.h"
#include "network.h"

int main(int argc, char **argv) {
  bht::Network n{argv[1]};

  auto sitems = n.getStopsForTransfer(argv[2]);
  std::cout << sitems.size() << std::endl;
  for (auto item : sitems) {
    std::cout << item.id << std::endl;
  }

  auto items = n.getNeighbors(argv[2]);
  std::cout << items.size() << std::endl;
  for (auto item : items) {
    std::cout << item << " = " << n.stops[item].name << std::endl;
  }

  // auto path = n.getTravelPath(argv[2], argv[3]);
  // std::cout << "Length of path is " << path.size() << std::endl;
  // for (auto stop : path) {
  //   std::cout << stop.id << " -> " << stop.name << std::endl;
  // }

  bht::NetworkScheduledTrip trip = n.getScheduledTrip("230353669");
  for (bht::NetworkScheduledTrip::iterator iter = trip.begin(); iter != trip.end(); iter++) {
    std::cout << iter->stopSequence << ": " << n.stops[iter->stopId].name << " = " << iter->stopId << std::endl;
  }

  std::distance(trip.begin(), trip.end());
}