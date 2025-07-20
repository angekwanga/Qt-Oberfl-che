#include "scheduled_trip.h"

namespace bht {

// NetworkScheduledTrip implementation
NetworkScheduledTrip::NetworkScheduledTrip(const std::string& tripId, const std::vector<StopTime>& stopTimes)
    : tripId(tripId), stopTimes(stopTimes) {
}

NetworkScheduledTrip::iterator NetworkScheduledTrip::begin() const {
    return iterator(this, 0);
}

NetworkScheduledTrip::iterator NetworkScheduledTrip::end() const {
    return iterator(this, stopTimes.size());
}

// Iterator implementation
NetworkScheduledTrip::iterator::iterator(const NetworkScheduledTrip* trip, size_t index)
    : trip(trip), index(index) {
}

NetworkScheduledTrip::iterator& NetworkScheduledTrip::iterator::operator++() {
    ++index;
    return *this;
}

NetworkScheduledTrip::iterator NetworkScheduledTrip::iterator::operator++(int) {
    iterator tmp = *this;
    ++index;
    return tmp;
}

NetworkScheduledTrip::iterator& NetworkScheduledTrip::iterator::operator--() {
    --index;
    return *this;
}

NetworkScheduledTrip::iterator NetworkScheduledTrip::iterator::operator--(int) {
    iterator tmp = *this;
    --index;
    return tmp;
}

const StopTime& NetworkScheduledTrip::iterator::operator*() const {
    return trip->stopTimes[index];
}

const StopTime* NetworkScheduledTrip::iterator::operator->() const {
    return &(trip->stopTimes[index]);
}

const std::string& NetworkScheduledTrip::iterator::getTripId() const {
    return trip->tripId;
}

unsigned int NetworkScheduledTrip::iterator::getStopSequence() const {
    if (index >= trip->stopTimes.size()) {
        return static_cast<unsigned int>(-1); // Invalid sequence to indicate end
    }
    return trip->stopTimes[index].stopSequence;
}

bool operator==(const NetworkScheduledTrip::iterator& a, const NetworkScheduledTrip::iterator& b) { 
  return a.getTripId() == b.getTripId() && a.getStopSequence() == b.getStopSequence();
}

bool operator!=(const NetworkScheduledTrip::iterator& a, const NetworkScheduledTrip::iterator& b) { 
  return !(a == b); 
}

}