#pragma once
#include "types.h"
#include "scheduled_trip.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>

namespace bht {

class Network {
  private:
    void readAgencies(std::string source);
    void readCalendarDates(std::string source);
    void readCalendars(std::string source);
    void readLevels(std::string source);
    void readPathways(std::string source);
    void readRoutes(std::string source);
    void readShapes(std::string source);
    void readStopTimes(std::string source);
    void readStops(std::string source);
    void readTransfers(std::string source);
    void readTrips(std::string source);

    /**
     * Parse a date string to a date component struct
     */
    GTFSDate parseDate(std::string input);

    /**
     * Parse a time string to a time component struct
     */
    GTFSTime parseTime(std::string input);

    // Optimized data structures for faster lookups (as recommended by professor)
    std::multimap<std::string, std::string> stopTrips; // stop_id -> trip_id
    std::multimap<std::string, std::string> tripStops; // trip_id -> stop_id
    std::multimap<std::string, StopTime> tripStopTimes; // trip_id -> StopTime
    std::multimap<std::string, std::string> stopsForTransferMap; // station_id -> stop_id
    std::multimap<std::string, std::string> zoneStops; // zone_id -> stop_id (Aufgabe 5a)

  public:
    /// @brief Properties fetched from GTFS files
    std::unordered_map<std::string, Agency> agencies;
    std::vector<CalendarDate> calendarDates;
    std::unordered_map<std::string, Calendar> calendars;
    std::unordered_map<std::string, Level> levels;
    std::unordered_map<std::string, Pathway> pathways;
    std::unordered_map<std::string, Route> routes;
    std::vector<Shape> shapes;
    std::vector<StopTime> stopTimes;
    std::unordered_map<std::string, Stop> stops;
    std::vector<Transfer> transfers;
    std::vector<Trip> trips;

    /**
     * Create a new network and read all data from files
     * located in the given directory
     */
    Network(std::string directory);

    /**
     * @brief search Search for stops matching the given search string
     * @param needle Search string to use to find stops
     * @return Result vector with matching stops
     */
    std::vector<Stop> search(std::string needle) const;

    /**
     * @brief Return a vector of all routes in the network
     * @return Ordered result vector of routes
     */
    std::vector<Route> getRoutes() const;

    /**
     * @brief Return the display name of a route to show to the user
     * @param route Route object to return the display name for
     * @return String to display for the route
     */
    std::string getRouteDisplayName(Route route) const;

    /**
     * Return a vector of all trips associated with the given route
     * @param routeId ID of the route to get trips for
     * @return Ordered result vector of trips
     */
    std::vector<Trip> getTripsForRoute(std::string routeId) const;

    /**
     * @brief Return the display name of a trip to show to the user
     * @param trip Trip object to return the display name for
     * @return String to display for the trip
     */
    std::string getTripDisplayName(Trip trip) const;

    /**
     * @brief Return a vector of all stops and their times associated with the given trip
     * @param tripId ID of the trip to get stop times for
     * @return Ordered result vector of stop times
     */
    std::vector<StopTime> getStopTimesForTrip(std::string tripId) const;

    /**
     * @brief Return the stop object for the object identified by the given id
     * @param stopId ID of the stop to get the datastructure for
     * @return Stop data structure
     */
    Stop getStopById(std::string stopId) const;

    /**
     * @brief Return a vector of all stops and their times associated with the given trip
     * filtered by the given search word
     * @param needle Search string to use to find stops
     * @param tripId ID of the trip to get stop times for
     * @return Ordered result vector of stop times matching the given search query
     */
    std::vector<StopTime> searchStopTimesForTrip(std::string needle, std::string tripId) const;

    // Methods from Aufgabe 4

    /**
     * @brief Get all stops that belong to the same station as the given stop
     * @param stopId ID of the stop or station
     * @return Vector of all stops belonging to the same station
     */
    std::vector<Stop> getStopsForTransfer(const std::string& stopId) const;

    /**
     * @brief Get all neighboring stops for a given stop (connected by trips and transfers)
     * @param stopId ID of the stop
     * @return Set of neighboring stop IDs
     */
    std::unordered_set<std::string> getNeighbors(const std::string& stopId) const;

    /**
     * @brief Calculate the shortest path between two stops
     * @param fromStopId ID of the starting stop
     * @param toStopId ID of the destination stop
     * @return Vector of stops representing the path (empty if no path exists)
     */
    std::vector<Stop> getTravelPath(const std::string& fromStopId, const std::string& toStopId) const;

    /**
     * @brief Get a scheduled trip object for iteration
     * @param tripId ID of the trip
     * @return NetworkScheduledTrip object
     */
    NetworkScheduledTrip getScheduledTrip(const std::string& tripId) const;

    // New methods for Aufgabe 5

    /**
     * @brief Calculate travel plan with departure time constraints (Aufgabe 5b)
     * @param fromStopId ID of the starting stop
     * @param toStopId ID of the destination stop  
     * @param departureTime Desired departure time
     * @return Vector of StopTime objects representing the travel plan with times
     */
    std::vector<StopTime> getTravelPlanDepartingAt(const std::string& fromStopId, 
                                                   const std::string& toStopId, 
                                                   const GTFSTime& departureTime) const;

private:
    /**
     * Helper function to compare GTFSTime objects
     */
    bool isTimeGreaterOrEqual(const GTFSTime& time1, const GTFSTime& time2) const;
    
    /**
     * Helper function to get next available departure from a stop after given time
     */
    std::vector<StopTime> getNextDeparturesFrom(const std::string& stopId, const GTFSTime& afterTime) const;
};

}