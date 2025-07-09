#include "network.h"
#include "csv.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <queue>

namespace bht {

Network::Network(std::string directory) {
  // Fetch data
  readAgencies(directory + "/agency.txt");
  readCalendarDates(directory + "/calendar_dates.txt");
  readCalendars(directory + "/calendar.txt");
  readLevels(directory + "/levels.txt");
  readPathways(directory + "/pathways.txt");
  readRoutes(directory + "/routes.txt");
  readShapes(directory + "/shapes.txt");
  readStopTimes(directory + "/stop_times.txt");
  readStops(directory + "/stops.txt");
  readTransfers(directory + "/transfers.txt");
  readTrips(directory + "/trips.txt");
}

std::vector<Stop> Network::search(std::string needle) {
  std::vector<Stop> result;

  for (auto pair : stops) {
      if (pair.second.name.find(needle) != std::string::npos) {
          result.push_back(pair.second);
      }
  }

  return result;
}

std::vector<Route> Network::getRoutes() {
  std::vector<Route> result;

  for (auto route : routes) {
    result.push_back(route.second);
  }

  std::sort(result.begin(), result.end(), [] (const Route& lhs, const Route& rhs) {
    return lhs.shortName < rhs.shortName;
  });

  return result;
}

std::string Network::getRouteDisplayName(Route route) {
  std::string displayName = route.shortName;
  if (!route.longName.empty()) {
    displayName = displayName + " - " + route.longName;
  }
  return displayName;
}

std::vector<Trip> Network::getTripsForRoute(std::string routeId) {
  std::vector<Trip> result;
  for (auto trip : trips) {
    if (trip.routeId == routeId) {
      result.push_back(trip);
    }
  }
  return result;
}

std::string Network::getTripDisplayName(Trip trip) {
  return trip.shortName + " - " + trip.headsign;
}

std::vector<StopTime> Network::getStopTimesForTrip(std::string tripId) {
  return searchStopTimesForTrip("", tripId);
}

Stop Network::getStopById(std::string stopId) {
  return stops.at(stopId);
}

std::vector<StopTime> Network::searchStopTimesForTrip(std::string needle, std::string tripId) {
    std::vector<StopTime> result;
    for (auto stopTime : stopTimes) {
      // Check if stop belongs to trip and matches filter if given.
      if (stopTime.tripId == tripId && (needle.empty() || stops.at(stopTime.stopId).name.find(needle) != std::string::npos)) {
        result.push_back(stopTime);
      }
    }

    std::sort(result.begin(), result.end(), [] (const StopTime& lhs, const StopTime& rhs) {
      return lhs.stopSequence - rhs.stopSequence;
    });

    return result;
}

// AUFGABE 5a: Extended getStopsForTransfer to include zone_id
std::vector<Stop> Network::getStopsForTransfer(const std::string& stopId) {
  std::vector<Stop> result;
  std::set<std::string> addedStops; // To avoid duplicates
  
  Stop stop = stops.at(stopId);
  
  // Add the original stop
  result.push_back(stop);
  addedStops.insert(stop.id);
  
  // Handle parent station logic (existing from Übungsblatt 4)
  if (stop.locationType == LocationType_Stop && !stop.parentStation.empty()) {
    // This is a generic stop / platform
    stop = stops.at(stop.parentStation);
    if (addedStops.find(stop.id) == addedStops.end()) {
      result.push_back(stop);
      addedStops.insert(stop.id);
    }
  }

  // Add stops that share the same parent station
  auto range = stopStations.equal_range(stop.id);
  for (auto iterator = range.first; iterator != range.second; ++iterator) {
    if (addedStops.find(iterator->second) == addedStops.end()) {
      result.push_back(stops[iterator->second]);
      addedStops.insert(iterator->second);
    }
  }
  
  // NEW: Add stops that share the same zone_id
  Stop originalStop = stops.at(stopId);
  if (!originalStop.zoneId.empty()) {
    auto zoneRange = zoneStops.equal_range(originalStop.zoneId);
    for (auto iterator = zoneRange.first; iterator != zoneRange.second; ++iterator) {
      if (addedStops.find(iterator->second) == addedStops.end()) {
        result.push_back(stops[iterator->second]);
        addedStops.insert(iterator->second);
      }
    }
  }

  return result;
}

std::unordered_set<std::string> Network::getNeighbors(const std::string& stopId) {
  std::unordered_set<std::string> result;

  // Add transfer stations
  std::vector<Stop> transfers = getStopsForTransfer(stopId);
  for (auto transfer : transfers) {
    if (transfer.id != stopId && transfer.locationType == LocationType_Stop) {
      result.insert(transfer.id);
    }
  }

  // Search neigbors by checking trips
  auto tripsRange = stopTrips.equal_range(stopId);
  for (auto iterator = tripsRange.first; iterator != tripsRange.second; ++iterator) {
    auto scheduledTrip = scheduledTrips[iterator->second];
    auto scheduledTripIterator = scheduledTrip->at(stopId);
    if (scheduledTripIterator == scheduledTrip->end()) {
      continue;
    }
    scheduledTripIterator++;
    if (scheduledTripIterator != scheduledTrip->end()) {
      result.insert(scheduledTripIterator->stopId);
    }
  }

  return result;
}

std::vector<Stop> Network::getTravelPath(const std::string& fromStopId, const std::string& toStopId) {
  // Initialization
  std::map<std::string, float> dist;
  std::map<std::string, std::string> previous;
  std::set<std::string> q;
  for (auto stop : stops) {
    // initial distance from source to vertex v is set to infinite
    dist[stop.first] = INFINITY;

    // all nodes in the graph are unoptimized - thus are in Q
    q.insert(stop.first);
  }
  
  dist[fromStopId] = 0;
  auto nextNode = [&dist](const std::string& a, const std::string& b) { 
    float da = dist[a];
    float db = dist[b];

    return (da < db);
  };

  // Dijkstra according to http://www.gitta.info/Accessibiliti/de/html/Dijkstra_learningObject1.html
  while (q.size() > 0) {
    auto nextIter = std::min_element(q.begin(), q.end(), nextNode);
    std::string u = *nextIter;
    q.erase(u);

    if (dist[u] == INFINITY) {
      break;
    }

    for (auto v : getNeighbors(u)) {
      float alt = dist[u] + 1.0; // Distance(u, v) set to 1.0 for now
      if (alt < dist[v]) {
        dist[v] = alt;
        previous[v] = u;
      }
    }
  }

  // Test if we have a result
  std::vector<Stop> result;
  auto p = previous.find(toStopId);
  while (p != previous.end()) {
    result.push_back(stops.at(p->first));
    p = previous.find(p->second);
  }
  std::reverse(result.begin(), result.end());
  return result;
}

// Helper function to compare times
bool Network::isTimeGreaterOrEqual(const GTFSTime& time1, const GTFSTime& time2) {
  if (time1.hour != time2.hour) {
    return time1.hour >= time2.hour;
  }
  if (time1.minute != time2.minute) {
    return time1.minute >= time2.minute;
  }
  return time1.second >= time2.second;
}

// AUFGABE 5b: getTravelPlanDepartingAt implementation
std::vector<StopTime> Network::getTravelPlanDepartingAt(const std::string& fromStopId, 
                                                        const std::string& toStopId, 
                                                        const GTFSTime& departureTime) {
  std::vector<StopTime> result;
  
  // Use modified Dijkstra with time constraints
  std::map<std::string, GTFSTime> earliestArrival;
  std::map<std::string, StopTime> previousStopTime;
  std::priority_queue<std::pair<int, std::string>, 
                     std::vector<std::pair<int, std::string>>, 
                     std::greater<std::pair<int, std::string>>> pq;
  
  // Initialize
  for (auto& stop : stops) {
    earliestArrival[stop.first] = {23, 59, 59}; // Max time
  }
  
  earliestArrival[fromStopId] = departureTime;
  pq.push({departureTime.hour * 3600 + departureTime.minute * 60 + departureTime.second, fromStopId});
  
  while (!pq.empty()) {
    auto current = pq.top();
    pq.pop();
    
    std::string currentStopId = current.second;
    GTFSTime currentTime = earliestArrival[currentStopId];
    
    if (currentStopId == toStopId) {
      break;
    }
    
    // Check all trips passing through this stop
    auto tripsRange = stopTrips.equal_range(currentStopId);
    for (auto iterator = tripsRange.first; iterator != tripsRange.second; ++iterator) {
      std::string tripId = iterator->second;
      
      // Get all stop times for this trip
      std::vector<StopTime> tripStopTimes = getStopTimesForTrip(tripId);
      
      // Find current stop in this trip
      auto currentStopIt = std::find_if(tripStopTimes.begin(), tripStopTimes.end(),
                                       [currentStopId](const StopTime& st) {
                                         return st.stopId == currentStopId;
                                       });
      
      if (currentStopIt != tripStopTimes.end()) {
        // Check if we can catch this trip (departure time must be >= current time)
        if (isTimeGreaterOrEqual(currentStopIt->departureTime, currentTime)) {
          // Check all subsequent stops in this trip
          for (auto nextStopIt = currentStopIt + 1; nextStopIt != tripStopTimes.end(); ++nextStopIt) {
            std::string nextStopId = nextStopIt->stopId;
            
            // If this arrival time is better than what we have
            if (isTimeGreaterOrEqual(earliestArrival[nextStopId], nextStopIt->arrivalTime)) {
              earliestArrival[nextStopId] = nextStopIt->arrivalTime;
              previousStopTime[nextStopId] = *nextStopIt;
              
              int timeInSeconds = nextStopIt->arrivalTime.hour * 3600 + 
                                 nextStopIt->arrivalTime.minute * 60 + 
                                 nextStopIt->arrivalTime.second;
              pq.push({timeInSeconds, nextStopId});
            }
          }
        }
      }
    }
    
    // Also check transfer possibilities
    std::vector<Stop> transfers = getStopsForTransfer(currentStopId);
    for (auto& transfer : transfers) {
      if (transfer.id != currentStopId) {
        if (isTimeGreaterOrEqual(earliestArrival[transfer.id], currentTime)) {
          earliestArrival[transfer.id] = currentTime;
          // For transfers, we don't add to previousStopTime as it's not a real stop time
        }
      }
    }
  }
  
  // Reconstruct path
  if (previousStopTime.find(toStopId) != previousStopTime.end()) {
    std::vector<StopTime> path;
    std::string currentId = toStopId;
    
    while (previousStopTime.find(currentId) != previousStopTime.end()) {
      path.push_back(previousStopTime[currentId]);
      
      // Find previous stop in the same trip
      std::string tripId = previousStopTime[currentId].tripId;
      std::vector<StopTime> tripStopTimes = getStopTimesForTrip(tripId);
      
      auto currentIt = std::find_if(tripStopTimes.begin(), tripStopTimes.end(),
                                   [currentId](const StopTime& st) {
                                     return st.stopId == currentId;
                                   });
      
      if (currentIt != tripStopTimes.begin()) {
        currentIt--;
        currentId = currentIt->stopId;
      } else {
        break;
      }
    }
    
    std::reverse(path.begin(), path.end());
    result = path;
  }
  
  return result;
}

NetworkScheduledTrip Network::getScheduledTrip(const std::string& tripId) const {
  return *scheduledTrips.at(tripId);
}

void Network::readAgencies(std::string source) {
  bht::CSVReader reader{source};
  do {
    std::string id = reader.getField("agency_id");
    if (id.empty() == false) {
      Agency item = {
        id,
        reader.getField("agency_name"),
        reader.getField("agency_url"),
        reader.getField("agency_timezone"),
        reader.getField("agency_lang"),
        reader.getField("agency_phone")
      };
      agencies[id] = item;
    }
  } while (reader.next());
}

void Network::readCalendarDates(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("service_id");
    if (id.empty() == false) {
      CalendarDate item = {
        id,
        parseDate(reader.getField("date")),
        (ECalendarDateException)std::stoi(reader.getField("exception_type"))
      };
      calendarDates.push_back(item);
    }
  } while (reader.next());
}

void Network::readCalendars(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("service_id");
    if (id.empty() == false) {
      Calendar item = {
        id,
        (ECalendarAvailability)std::stoi(reader.getField("monday")),
        (ECalendarAvailability)std::stoi(reader.getField("tuesday")),
        (ECalendarAvailability)std::stoi(reader.getField("wednesday")),
        (ECalendarAvailability)std::stoi(reader.getField("thursday")),
        (ECalendarAvailability)std::stoi(reader.getField("friday")),
        (ECalendarAvailability)std::stoi(reader.getField("saturday")),
        (ECalendarAvailability)std::stoi(reader.getField("sunday")),
        parseDate(reader.getField("start_date")),
        parseDate(reader.getField("end_date"))
      };
      calendars[id] = item;
    }
  } while (reader.next());
}

void Network::readLevels(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("level_id");
    if (id.empty() == false) {
      Level item = {
        id,
        (unsigned int)std::stoi(reader.getField("level_index")),
        reader.getField("level_name")
      };
      levels[id] = item;
    }
  } while (reader.next());
}

void Network::readPathways(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("pathway_id");
    if (id.empty() == false) {
      Pathway item = {
        id,
        reader.getField("from_stop_id"),
        reader.getField("to_stop_id"),
        (EPathwayMode)std::stoi(reader.getField("pathway_mode", "1")),
        reader.getField("is_bidirectional") == "1",
        std::stof(reader.getField("length", "0.0")),
        (unsigned int)std::stoi(reader.getField("traversal_time", "0")),
        (unsigned int)std::stoi(reader.getField("stair_count", "0")),
        std::stof(reader.getField("max_slope", "0.0")),
        std::stof(reader.getField("min_width", "0.0")),
        reader.getField("signposted_as")
      };
      pathways[id] = item;
    }
  } while (reader.next());
}

void Network::readRoutes(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("route_id");
    if (id.empty() == false) {
      Route item = {
        id,
        reader.getField("agency_id"),
        reader.getField("route_short_name"),
        reader.getField("route_long_name"),
        reader.getField("route_desc"),
        (RouteType)std::stoi(reader.getField("route_type", "0")),
        reader.getField("route_color"),
        reader.getField("route_text_color")
      };
      routes[id] = item;
    }
  } while (reader.next());
}

void Network::readShapes(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("shape_id");
    if (id.empty() == false) {
      Shape item = {
        id,
        std::stod(reader.getField("shape_pt_lat")),
        std::stod(reader.getField("shape_pt_lon")),
        (unsigned int)std::stoi(reader.getField("shape_pt_sequence"))
      };
      shapes.push_back(item);
    }
  } while (reader.next());
}

void Network::readStopTimes(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("trip_id");
    if (id.empty() == false) {
      StopTime item = {
        id,
        parseTime(reader.getField("arrival_time")),
        parseTime(reader.getField("departure_time")),
        reader.getField("stop_id"),
        (unsigned int)std::stoi(reader.getField("stop_sequence")),
        (EPickupType)std::stoi(reader.getField("pickup_type")),
        (EDropOffType)std::stoi(reader.getField("drop_off_type")),
        reader.getField("stop_headsign") 
      };
      stopTimes.push_back(item);
      tripStopTimes.insert(std::make_pair(item.tripId, item));
      tripStops.insert(std::make_pair(item.tripId, item.stopId));
      stopTrips.insert(std::make_pair(item.stopId, item.tripId));
    }
  } while (reader.next());
}

void Network::readStops(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("stop_id");
    if (id.empty() == false) {
      Stop item = {
        id,
        reader.getField("stop_code"),
        reader.getField("stop_name"),
        reader.getField("stop_desc"),
        std::stod(reader.getField("stop_lat")),
        std::stod(reader.getField("stop_lon")),
        (LocationType)std::stoi(reader.getField("location_type")),
        reader.getField("parent_station"),
        (WheelchairAccessibility)std::stoi(reader.getField("wheelchair_boarding")),
        reader.getField("platform_code"),
        reader.getField("level_id"),
        reader.getField("zone_id")
      };
      stops[id] = item;
      if (item.locationType == LocationType_Stop && !item.parentStation.empty()) {
        stopStations.insert(std::make_pair(item.parentStation, item.id));
      } 
      else {
        stopStations.insert(std::make_pair(item.id, item.id));
      }
      
      // NEW: Add zone mapping for Übungsblatt 5
      if (!item.zoneId.empty()) {
        zoneStops.insert(std::make_pair(item.zoneId, item.id));
      }
    }
  } while (reader.next());
}

void Network::readTransfers(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("from_stop_id");
    if (id.empty() == false) {
      Transfer item = {
        id,
        reader.getField("to_stop_id"),
        reader.getField("from_route_id"),
        reader.getField("to_route_id"),
        reader.getField("from_trip_id"),
        reader.getField("to_trip_id"),
        (TransferType)std::stoi(reader.getField("transfer_type")),
        (unsigned int)std::stoi(reader.getField("min_transfer_time", "0"))
      };
      transfers.push_back(item);
    }
  } while (reader.next());
}

void Network::readTrips(std::string source) {
  bht::CSVReader reader(source);
  do {
    std::string id = reader.getField("trip_id");
    if (id.empty() == false) {
      Trip item = {
        id,
        reader.getField("route_id"),
        reader.getField("service_id"),
        reader.getField("trip_headsign"),
        reader.getField("trip_short_name"),
        (TripDirection)std::stoi(reader.getField("direction_id")),
        reader.getField("block_id"),
        reader.getField("shape_id"),
        (WheelchairAccessibility)std::stoi(reader.getField("wheelchair_accessible")),
        reader.getField("bikes_allowed") == "1",
      };
      trips.push_back(item);

      std::vector<StopTime> selectedStopTimes;
      auto range = tripStopTimes.equal_range(item.id);
      for (auto iterator = range.first; iterator != range.second; ++iterator) {
        selectedStopTimes.push_back(iterator->second);
      }
      std::sort(selectedStopTimes.begin(), selectedStopTimes.end());
      std::shared_ptr<NetworkScheduledTrip> scheduledTrip{new NetworkScheduledTrip(item, selectedStopTimes)};
      scheduledTrips[item.id] = scheduledTrip;
    }
  } while (reader.next());
}

GTFSDate Network::parseDate(std::string input) {
  GTFSDate result = { 
    .day = (unsigned char)std::stoi(input.substr(6, 2)),
    .month = (unsigned char)std::stoi(input.substr(4, 2)),
    .year = (unsigned short)std::stoi(input.substr(0, 4))
  };

  return result;
}

GTFSTime Network::parseTime(std::string input) {
  size_t first = input.find(":");
  size_t next = input.find(":", first + 1);
  GTFSTime result = { 
    .hour = (unsigned char)std::stoi(input.substr(0, first)),
    .minute = (unsigned char)std::stoi(input.substr(first + 1, next - first)),
    .second = (unsigned char)std::stoi(input.substr(next + 1))
  };

  return result;
}

}