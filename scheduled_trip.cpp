#include "scheduled_trip.h"

namespace bht {

NetworkScheduledTrip::NetworkScheduledTrip(const Trip& trip, const std::vector<StopTime>& stops) : trip(trip), stops(stops) {

}

NetworkScheduledTrip::NetworkScheduledTrip(const NetworkScheduledTrip& other) : trip(other.trip), stops(other.stops) {
}

NetworkScheduledTrip::NetworkScheduledTrip(NetworkScheduledTrip&& other) : trip(other.trip) {
  stops = std::move(other.stops);
}

NetworkScheduledTrip::iterator NetworkScheduledTrip::begin() const {
  return iterator{this, 0};
}

NetworkScheduledTrip::iterator NetworkScheduledTrip::end() const {
  return iterator{this, stops.size()};
}

NetworkScheduledTrip::iterator NetworkScheduledTrip::at(const std::string& stopId) const {
  for (size_t index = 0; index < stops.size(); index++) {
    if (stops[index].stopId == stopId) {
      return iterator{this, index};
    }
  }

  return end();
}

const Trip& NetworkScheduledTrip::getTrip() const {
  return trip;
}

NetworkScheduledTrip::iterator::iterator(const NetworkScheduledTrip* trip, size_t position) : trip(trip), position(position) {

}

NetworkScheduledTrip::iterator& NetworkScheduledTrip::iterator::operator++() {
  position++;
  return *this;
}

NetworkScheduledTrip::iterator NetworkScheduledTrip::iterator::operator++(int) {
  iterator tmp{trip, position};
  ++(*this);
  return tmp;
}

NetworkScheduledTrip::iterator& NetworkScheduledTrip::iterator::operator--() {
  position--;
  return *this;
}

NetworkScheduledTrip::iterator NetworkScheduledTrip::iterator::operator--(int) {
  iterator tmp{trip, position};
  --(*this);
  return tmp;
}

NetworkScheduledTrip::iterator::reference NetworkScheduledTrip::iterator::operator*() const {
  return trip->stops[position];
}

const std::string& NetworkScheduledTrip::iterator::getTripId() const {
  return trip->stops[0].tripId;
}

unsigned int NetworkScheduledTrip::iterator::getStopSequence() const {
  return position;
}

NetworkScheduledTrip::iterator::pointer NetworkScheduledTrip::iterator::operator->() {
  return &(trip->stops[position]);
}

bool operator==(const NetworkScheduledTrip::iterator& a, const NetworkScheduledTrip::iterator& b) { 
  return a.getTripId() == b.getTripId() && a.getStopSequence() == b.getStopSequence();
}

bool operator!=(const NetworkScheduledTrip::iterator& a, const NetworkScheduledTrip::iterator& b) { 
  return !(a == b); 
}

}