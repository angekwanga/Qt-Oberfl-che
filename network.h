#pragma once
#include "types.h"
#include "scheduled_trip.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <string>
#include <memory>

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
    std::multimap<std::string, StopTime> tripStopTimes;
    std::unordered_map<std::string, Stop> stops;
    std::multimap<std::string, std::string> stopStations; // Mapping of a stop id to associated child stops id of statons
    std::multimap<std::string, std::string> stopTrips; // Mapping of a stop id to associated trip ids the stop belongs to
    std::multimap<std::string, std::string> tripStops; // Mapping of a trip id to associated stops id
    std::vector<Transfer> transfers;
    std::vector<Trip> trips;
    std::unordered_map<std::string, std::shared_ptr<NetworkScheduledTrip>> scheduledTrips; // Mapping of trip ids to scheduled trips

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
    std::vector<Stop> search(std::string needle);

    /**
     * @brief Return a vector of all routes in the network
     * @return Ordered result vector of routes
     */
    std::vector<Route> getRoutes();

    /**
     * @brief Return the display name of a route to show to the user
     * @param route Route object to return the display name for
     * @return String to display for the route
     */
    std::string getRouteDisplayName(Route route);

    /**
     * Return a vector of all trips associated with the given route
     * @param routeId ID of the route to get trips for
     * @return Ordered result vector of trips
     */
    std::vector<Trip> getTripsForRoute(std::string routeId);

    /**
     * @brief Return the display name of a trip to show to the user
     * @param trip Trip object to return the display name for
     * @return String to display for the trip
     */
    std::string getTripDisplayName(Trip trip);

    /**
     * @brief Return a vector of all stops and their times associated with the given trip
     * @param tripId ID of the trip to get stop times for
     * @return Ordered result vector of stop times
     */
    std::vector<StopTime> getStopTimesForTrip(std::string tripId);

    /**
     * @brief Return the stop object for the object identified by the given id
     * @param stopId ID of the stop to get the datastructure for
     * @return Stop data structure
     */
    Stop getStopById(std::string stopId);

    /**
     * @brief Return a vector of all stops and their times associated with the given trip
     * filtered by the given search word
     * @param needle Search string to use to find stops
     * @param tripId ID of the trip to get stop times for
     * @return Ordered result vector of stop times matching the given search query
     */
    std::vector<StopTime> searchStopTimesForTrip(std::string needle, std::string tripId);

    /**
     * @brief Find all stops that reflect platforms which share a station / parent station
     * @param stopId The id of the platform at a station or a station to return the related stops for
     * @return result vector of stops for the given station
     */
    std::vector<bht::Stop> getStopsForTransfer(const std::string& stopId);

    /**
     * @brief Find all stops that are neighbors of the given stop
     * @param stopId The id of the stop to get neighbors for
     * @return result set of stop ids reachable from the given station
     */
    std::unordered_set<std::string> getNeighbors(const std::string& stopId);

    /**
     * @brief Search a path starting at the given stop and terminating at the other given stop.
     * If stops are at a station, interchanges to different platforms are supported
     * @param fromStopId The id of the stop to start the route
     * @param toStopId The id of the destination stop
     * @return Stop travel path
     */
    std::vector<bht::Stop> getTravelPath(const std::string& fromStopId, const std::string& toStopId);

    /**
     * @brief Return a iterable object that represents the given trip
     * @param tripId ID of the trip to get the iterable object for
     * @return new NetworkScheduledTrip object
     */
    NetworkScheduledTrip getScheduledTrip(const std::string& tripId) const;
};

}
