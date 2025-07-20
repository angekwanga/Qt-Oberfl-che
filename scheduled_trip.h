#pragma once

#include "types.h"
#include <iterator>
#include <vector>
#include <string>

namespace bht {

/**
 * Defines a class representing a single trip with convenient accessor get methods to 
 * related route, stop and stop times information
 */
class NetworkScheduledTrip {
  private:
    std::string tripId;
    std::vector<StopTime> stopTimes;

  public:
    // Define properties for this iterator
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = int;
    using value_type        = const StopTime;
    using pointer           = const StopTime*; 
    using reference         = const StopTime&;
    
    /**
     * Constructor
     */
    NetworkScheduledTrip(const std::string& tripId, const std::vector<StopTime>& stopTimes);
    
    /**
     * Define an iterator to navigate the stops of a scheduled trip
     */
    class iterator {
      private:
        const NetworkScheduledTrip* trip;
        size_t index;
        
      public:
        // Iterator traits
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = int;
        using value_type        = const StopTime;
        using pointer           = const StopTime*; 
        using reference         = const StopTime&;

        /**
         * Constructor
         */
        iterator(const NetworkScheduledTrip* trip, size_t index);

        /**
         * Move the iterator to the next stop in this trip
         */
        iterator& operator++();
        iterator operator++(int);
        
        /**
         * Move the iterator to the previous stop in this trip
         */
        iterator& operator--();
        iterator operator--(int);
        
        /**
         * Return the current stop time this iterator points to
         * HINT operator* is the dereference operator; NOT the multiplication operator
        */
        const StopTime& operator*() const;

        /**
         * Return a pointer to the stop time the current iterator points to
         */
        const StopTime* operator->() const;

        /**
         * Return the referenced trip of the current stop 
         */
        const std::string& getTripId() const;

        /**
         * return the current stop sequence number on the trip
         * HINT: This sequence number might be invalid to indicate the end of a trip
         */
        unsigned int getStopSequence() const;

        /**
         * Two stops are considered equal if they are in the same trip
         * and represent the same stop in the sequence of stops
         */
        friend bool operator==(const iterator& a, const iterator& b);
        friend bool operator!= (const iterator& a, const iterator& b);
    };

    /**
     * Create a new iterator representing the start of a trip
     */
    iterator begin() const;

    /**
     * Create a new iterator pointing beyond the last element of the trip
     */
    iterator end() const;
};

}