#include "network.h"
#include "csv.h"
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <tuple>
#include <cctype>
#include <locale>

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
  readStops(directory + "/stops.txt");
  readStopTimes(directory + "/stop_times.txt");
  readTransfers(directory + "/transfers.txt");
  readTrips(directory + "/trips.txt");
}

std::vector<StopTime> Network::getTravelPlanDepartingAt(const std::string& fromStopId, 
                                                        const std::string& toStopId, 
                                                        const GTFSTime& departureTime) const {
    // Check if stops exist
    if (stops.find(fromStopId) == stops.end() || stops.find(toStopId) == stops.end()) {
        return {}; // Return empty if either stop doesn't exist
    }
    
    if (fromStopId == toStopId) {
        return {};
    }

    // Structure pour stocker les informations de chemin
    struct PathInfo {
        int arrivalTime;
        std::string stopId;
        std::vector<StopTime> path;
        
        // Opérateur de comparaison pour priority_queue (min-heap basé sur le temps d'arrivée)
        bool operator>(const PathInfo& other) const {
            return arrivalTime > other.arrivalTime;
        }
    };

    // Priority queue pour Dijkstra avec contraintes de temps
    std::priority_queue<PathInfo, std::vector<PathInfo>, std::greater<PathInfo>> pq;

    std::unordered_map<std::string, int> bestTime; // meilleur temps d'arrivée pour chaque arrêt
    
    // Convertir le temps de départ en minutes pour faciliter la comparaison
    int departureMinutes = departureTime.hour * 60 + departureTime.minute;
    
    // Commencer avec l'arrêt initial
    pq.push({departureMinutes, fromStopId, {}});
    bestTime[fromStopId] = departureMinutes;
    
    while (!pq.empty()) {
        PathInfo current = pq.top();
        pq.pop();
        
        // Ignorer si nous avons trouvé un meilleur chemin vers cet arrêt
        if (bestTime.count(current.stopId) && bestTime[current.stopId] < current.arrivalTime) {
            continue;
        }
        
        // Vérifier si nous avons atteint la destination
        if (current.stopId == toStopId) {
            return current.path;
        }
        
        // Obtenir tous les prochains départs possibles depuis l'arrêt actuel
        GTFSTime currentGTFSTime = {
            static_cast<unsigned char>(current.arrivalTime / 60),
            static_cast<unsigned char>(current.arrivalTime % 60),
            0
        };
        
        auto nextDepartures = getNextDeparturesFrom(current.stopId, currentGTFSTime);
        
        for (const auto& departure : nextDepartures) {
            // Obtenir tous les arrêts sur ce voyage après l'arrêt actuel
            auto range = tripStopTimes.equal_range(departure.tripId);
            std::vector<StopTime> tripStops;
            
            for (auto it = range.first; it != range.second; ++it) {
                tripStops.push_back(it->second);
            }
            
            std::sort(tripStops.begin(), tripStops.end(), 
                      [](const StopTime& a, const StopTime& b) {
                          return a.stopSequence < b.stopSequence;
                      });
            
            // Trouver la position de l'arrêt actuel dans le voyage
            for (size_t i = 0; i < tripStops.size(); ++i) {
                if (tripStops[i].stopId == current.stopId && 
                    tripStops[i].stopSequence == departure.stopSequence) {
                    
                    // Ajouter tous les arrêts suivants comme destinations possibles
                    for (size_t j = i + 1; j < tripStops.size(); ++j) {
                        const auto& nextStop = tripStops[j];
                        int arrivalTime = nextStop.arrivalTime.hour * 60 + nextStop.arrivalTime.minute;
                        
                        // Ne considérer que si c'est un meilleur temps d'arrivée
                        if (!bestTime.count(nextStop.stopId) || bestTime[nextStop.stopId] > arrivalTime) {
                            bestTime[nextStop.stopId] = arrivalTime;
                            
                            // Construire le nouveau chemin
                            std::vector<StopTime> newPath = current.path;
                            
                            // Ajouter le point de départ si c'est le premier segment
                            if (newPath.empty()) {
                                newPath.push_back(departure);
                            }
                            
                            // Ajouter tous les arrêts intermédiaires jusqu'au suivant
                            for (size_t k = i + 1; k <= j; ++k) {
                                newPath.push_back(tripStops[k]);
                            }
                            
                            pq.push({arrivalTime, nextStop.stopId, newPath});
                        }
                    }
                    break;
                }
            }
        }
        
        // Considérer aussi les transferts à l'arrêt actuel
        auto transferStops = getStopsForTransfer(current.stopId);
        for (const auto& transferStop : transferStops) {
            if (transferStop.id != current.stopId) {
                // Aucun temps de transfert supposé, peut immédiatement prendre le prochain départ
                if (!bestTime.count(transferStop.id) || bestTime[transferStop.id] > current.arrivalTime) {
                    bestTime[transferStop.id] = current.arrivalTime;
                    pq.push({current.arrivalTime, transferStop.id, current.path});
                }
            }
        }
    }
    
    // Aucun chemin trouvé
    return {};
}

bool Network::isTimeGreaterOrEqual(const GTFSTime& time1, const GTFSTime& time2) const {
    int minutes1 = time1.hour * 60 + time1.minute;
    int minutes2 = time2.hour * 60 + time2.minute;
    return minutes1 >= minutes2;
}

std::vector<StopTime> Network::getNextDeparturesFrom(const std::string& stopId, const GTFSTime& afterTime) const {
    std::vector<StopTime> departures;
    
    // Get all trips that stop at this station
    auto tripRange = stopTrips.equal_range(stopId);
    for (auto tripIt = tripRange.first; tripIt != tripRange.second; ++tripIt) {
        const std::string& tripId = tripIt->second;
        
        // Get all stop times for this trip
        auto stopRange = tripStopTimes.equal_range(tripId);
        for (auto stopIt = stopRange.first; stopIt != stopRange.second; ++stopIt) {
            const StopTime& stopTime = stopIt->second;
            
            // Check if this is the right stop and departure is after required time
            if (stopTime.stopId == stopId && isTimeGreaterOrEqual(stopTime.departureTime, afterTime)) {
                departures.push_back(stopTime);
            }
        }
    }
    
    // Sort by departure time
    std::sort(departures.begin(), departures.end(), 
              [](const StopTime& a, const StopTime& b) {
                  int timeA = a.departureTime.hour * 60 + a.departureTime.minute;
                  int timeB = b.departureTime.hour * 60 + b.departureTime.minute;
                  return timeA < timeB;
              });
    
    return departures;
}

std::vector<Stop> Network::search(std::string needle) const {
  std::vector<Stop> result;

  for (const std::pair<const std::string, Stop>& pair : stops) {
      if (pair.second.name.find(needle) != std::string::npos) {
          result.push_back(pair.second);
      }
  }

  return result;
}

std::vector<Route> Network::getRoutes() const {
    std::vector<Route> result;

    for (auto &route : routes) {
        result.push_back(route.second);
    }

    std::sort(result.begin(), result.end(), [] (const Route& lhs, const Route& rhs) {
        return lhs.shortName < rhs.shortName;
    });

    return result;
}

std::string Network::getRouteDisplayName(Route route) const {
    std::string displayName = route.shortName;
    if (!route.longName.empty()) {
        displayName = displayName + " - " + route.longName;
    }
    return displayName;
}

std::vector<Trip> Network::getTripsForRoute(std::string routeId) const {
    std::vector<Trip> result;
    for (auto &trip : trips) {
        if (trip.routeId == routeId) {
            result.push_back(trip);
        }
    }
    return result;
}

std::string Network::getTripDisplayName(Trip trip) const {
    return trip.shortName + " - " + trip.headsign;
}

std::vector<StopTime> Network::getStopTimesForTrip(std::string tripId) const {
    return searchStopTimesForTrip("", tripId);
}

Stop Network::getStopById(std::string stopId) const {
    auto stopIt = stops.find(stopId);
    if (stopIt != stops.end()) {
        return stopIt->second;
    }
    // Return a default/empty stop if not found
    return Stop{};
}

std::vector<StopTime> Network::searchStopTimesForTrip(std::string needle, std::string tripId) const {
    std::vector<StopTime> result;
    
    // Convert needle to lowercase for case-insensitive search
    std::string lowerNeedle = needle;
    std::transform(lowerNeedle.begin(), lowerNeedle.end(), lowerNeedle.begin(), ::tolower);
    
    // Use optimized multimap for faster lookup
    auto range = tripStopTimes.equal_range(tripId);
    for (auto it = range.first; it != range.second; ++it) {
        const StopTime& stopTime = it->second;
        auto stopIt = stops.find(stopTime.stopId);
        if (stopIt != stops.end()) {
            if (needle.empty()) {
                result.push_back(stopTime);
            } else {
                // Convert stop name to lowercase for comparison
                std::string lowerStopName = stopIt->second.name;
                std::transform(lowerStopName.begin(), lowerStopName.end(), lowerStopName.begin(), ::tolower);
                
                if (lowerStopName.find(lowerNeedle) != std::string::npos) {
                    result.push_back(stopTime);
                }
            }
        }
    }

    std::sort(result.begin(), result.end(), [](const StopTime& lhs, const StopTime& rhs) {
        return lhs.stopSequence < rhs.stopSequence;
    });

    return result;
}

// New methods for Aufgabe 5

std::vector<Stop> Network::getStopsForTransfer(const std::string& stopId) const {
    std::vector<Stop> result;
    
    // Check if stop exists
    auto stopIt = stops.find(stopId);
    if (stopIt == stops.end()) {
        return result; // Return empty vector if stop doesn't exist
    }
    
    // Add the stop itself
    result.push_back(stopIt->second);
    
    const Stop& stop = stopIt->second;
    
    // Method 1: Use parent_station relationship
    if (stop.locationType == LocationType_Station) {
        // If it's a station, find all platforms/tracks using multimap
        auto range = stopsForTransferMap.equal_range(stopId);
        for (auto it = range.first; it != range.second; ++it) {
            auto childStopIt = stops.find(it->second);
            if (childStopIt != stops.end()) {
                const Stop& childStop = childStopIt->second;
                if (childStop.id != stopId) {
                    result.push_back(childStop);
                }
            }
        }
    } else if (!stop.parentStation.empty()) {
        // If it's a platform/track, find the parent station and all sibling platforms
        // Add parent station
        auto parentStopIt = stops.find(stop.parentStation);
        if (parentStopIt != stops.end()) {
            const Stop& parentStop = parentStopIt->second;
            bool alreadyAdded = false;
            for (const auto& existingStop : result) {
                if (existingStop.id == parentStop.id) {
                    alreadyAdded = true;
                    break;
                }
            }
            if (!alreadyAdded) {
                result.push_back(parentStop);
            }
        }
        
        // Add all sibling platforms using multimap
        auto range = stopsForTransferMap.equal_range(stop.parentStation);
        for (auto it = range.first; it != range.second; ++it) {
            auto siblingStopIt = stops.find(it->second);
            if (siblingStopIt != stops.end()) {
                const Stop& siblingStop = siblingStopIt->second;
                if (siblingStop.id != stopId) {
                    bool alreadyAdded = false;
                    for (const auto& existingStop : result) {
                        if (existingStop.id == siblingStop.id) {
                            alreadyAdded = true;
                            break;
                        }
                    }
                    if (!alreadyAdded) {
                        result.push_back(siblingStop);
                    }
                }
            }
        }
    }
    
    // Method 2: Find stops with common prefix (for GTFS data without proper parent_station)
    // Extract base station ID (everything before the first colon after the main ID)
    std::string baseStationId = stopId;
    size_t colonPos = stopId.find(':');
    if (colonPos != std::string::npos) {
        // Find the position after the third colon (typical pattern: de:region:station:...)
        size_t count = 0;
        for (size_t i = 0; i < stopId.length() && count < 3; ++i) {
            if (stopId[i] == ':') {
                count++;
                if (count == 3) {
                    baseStationId = stopId.substr(0, i);
                    break;
                }
            }
        }
    }
    
    // Find all stops that start with the base station ID
    for (const auto& pair : stops) {
        const std::string& otherStopId = pair.first;
        const Stop& otherStop = pair.second;
        
        // Check if this stop shares the same base station ID and isn't already added
        if (otherStopId != stopId && otherStopId.find(baseStationId) == 0) {
            // Additional check: make sure it's really the same station
            // (starts with baseStationId and next char is ':' or end of string)
            if (otherStopId.length() == baseStationId.length() || 
                (otherStopId.length() > baseStationId.length() && otherStopId[baseStationId.length()] == ':')) {
                
                bool alreadyAdded = false;
                for (const auto& existingStop : result) {
                    if (existingStop.id == otherStopId) {
                        alreadyAdded = true;
                        break;
                    }
                }
                if (!alreadyAdded) {
                    result.push_back(otherStop);
                }
            }
        }
    }
    
    // Method 3: Zone-based transfers (Aufgabe 5a) - MOST IMPORTANT according to the assignment
    if (!stop.zoneId.empty()) {
        auto range = zoneStops.equal_range(stop.zoneId);
        for (auto it = range.first; it != range.second; ++it) {
            auto zoneStopIt = stops.find(it->second);
            if (zoneStopIt != stops.end()) {
                const Stop& zoneStop = zoneStopIt->second;
                if (zoneStop.id != stopId) {
                    // Check if not already added
                    bool alreadyAdded = false;
                    for (const auto& existingStop : result) {
                        if (existingStop.id == zoneStop.id) {
                            alreadyAdded = true;
                            break;
                        }
                    }
                    if (!alreadyAdded) {
                        result.push_back(zoneStop);
                    }
                }
            }
        }
    }
    
    return result;
}

std::unordered_set<std::string> Network::getNeighbors(const std::string& stopId) const {
    std::unordered_set<std::string> neighbors;
    
    // Use optimized multimap for faster lookup
    auto tripRange = stopTrips.equal_range(stopId);
    for (auto tripIt = tripRange.first; tripIt != tripRange.second; ++tripIt) {
        const std::string& tripId = tripIt->second;
        
        // Get all stops for this trip in order using multimap
        std::vector<StopTime> tripStops;
        auto stopRange = tripStopTimes.equal_range(tripId);
        for (auto stopIt = stopRange.first; stopIt != stopRange.second; ++stopIt) {
            tripStops.push_back(stopIt->second);
        }
        
        // Sort by stop sequence
        std::sort(tripStops.begin(), tripStops.end(), 
                  [](const StopTime& a, const StopTime& b) {
                      return a.stopSequence < b.stopSequence;
                  });
        
        // Find current stop and add next/previous stops
        for (size_t i = 0; i < tripStops.size(); ++i) {
            if (tripStops[i].stopId == stopId) {
                // Add next stop
                if (i + 1 < tripStops.size()) {
                    neighbors.insert(tripStops[i + 1].stopId);
                }
                // Add previous stop
                if (i > 0) {
                    neighbors.insert(tripStops[i - 1].stopId);
                }
                break;
            }
        }
    }
    
    // Add transfer possibilities (same station)
    std::vector<Stop> transferStops = getStopsForTransfer(stopId);
    for (const auto& transferStop : transferStops) {
        if (transferStop.id != stopId) {
            neighbors.insert(transferStop.id);
        }
    }
    
    return neighbors;
}

std::vector<Stop> Network::getTravelPath(const std::string& fromStopId, const std::string& toStopId) const {
    // Check if stops exist
    if (stops.find(fromStopId) == stops.end() || stops.find(toStopId) == stops.end()) {
        return {}; // Return empty if either stop doesn't exist
    }
    
    if (fromStopId == toStopId) {
        return {stops.at(fromStopId)};
    }
    
    // BFS for shortest path
    std::queue<std::string> queue;
    std::unordered_map<std::string, std::string> parent;
    std::unordered_set<std::string> visited;
    
    queue.push(fromStopId);
    visited.insert(fromStopId);
    parent[fromStopId] = "";
    
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop();
        
        if (current == toStopId) {
            // Reconstruct path
            std::vector<Stop> path;
            std::string node = toStopId;
            while (!node.empty()) {
                auto stopIt = stops.find(node);
                if (stopIt != stops.end()) {
                    path.push_back(stopIt->second);
                }
                node = parent[node];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Get neighbors using optimized method
        std::unordered_set<std::string> neighbors = getNeighbors(current);
        for (const std::string& neighbor : neighbors) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                parent[neighbor] = current;
                queue.push(neighbor);
            }
        }
    }
    
    // No path found
    return {};
}

NetworkScheduledTrip Network::getScheduledTrip(const std::string& tripId) const {
    std::vector<StopTime> tripStopTimes;
    
    // Use optimized multimap for faster lookup
    auto range = this->tripStopTimes.equal_range(tripId);
    for (auto it = range.first; it != range.second; ++it) {
        tripStopTimes.push_back(it->second);
    }
    
    std::sort(tripStopTimes.begin(), tripStopTimes.end(), 
              [](const StopTime& a, const StopTime& b) {
                  return a.stopSequence < b.stopSequence;
              });
    
    return NetworkScheduledTrip(tripId, tripStopTimes);
}

void Network::readAgencies(std::string source) {
  CSVReader reader{source};
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
  CSVReader reader(source);
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
  CSVReader reader(source);
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
  CSVReader reader(source);
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
  CSVReader reader(source);
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
  CSVReader reader(source);
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
  CSVReader reader(source);
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
  CSVReader reader(source);
  do {
    std::string id = reader.getField("trip_id");
    if (id.empty() == false) {
      StopTime item = {
        id,
        parseTime(reader.getField("arrival_time")),
        parseTime(reader.getField("departure_time")),
        reader.getField("stop_id"),
        (unsigned int)std::stoi(reader.getField("stop_sequence")),
        (EPickupType)std::stoi(reader.getField("pickup_type", "0")),
        (EDropOffType)std::stoi(reader.getField("drop_off_type", "0")),
        reader.getField("stop_headsign") 
      };
      stopTimes.push_back(item);
      
      // Build optimized data structures (as recommended by professor)
      stopTrips.insert({item.stopId, item.tripId});
      tripStops.insert({item.tripId, item.stopId});
      tripStopTimes.insert({item.tripId, item});
    }
  } while (reader.next());
}

void Network::readStops(std::string source) {
  CSVReader reader(source);
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
        (LocationType)std::stoi(reader.getField("location_type", "0")),
        reader.getField("parent_station"),
        (WheelchairAccessibility)std::stoi(reader.getField("wheelchair_boarding", "0")),
        reader.getField("platform_code"),
        reader.getField("level_id"),
        reader.getField("zone_id")
      };
      stops[id] = item;
      
      // Build transfer data structures (as recommended by professor)
      if (item.locationType == LocationType_Station) {
        stopsForTransferMap.insert({id, id});
      } else if (!item.parentStation.empty()) {
        stopsForTransferMap.insert({item.parentStation, id});
      }
      
      // Aufgabe 5a: Build zone-based transfer data structures
      if (!item.zoneId.empty()) {
        zoneStops.insert({item.zoneId, id});
      }
    }
  } while (reader.next());
}

void Network::readTransfers(std::string source) {
  CSVReader reader(source);
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
        (TransferType)std::stoi(reader.getField("transfer_type", "0")),
        (unsigned int)std::stoi(reader.getField("min_transfer_time", "0"))
      };
      transfers.push_back(item);
    }
  } while (reader.next());
}

void Network::readTrips(std::string source) {
  CSVReader reader(source);
  do {
    std::string id = reader.getField("trip_id");
    if (id.empty() == false) {
      Trip item = {
        id,
        reader.getField("route_id"),
        reader.getField("service_id"),
        reader.getField("trip_headsign"),
        reader.getField("trip_short_name"),
        (TripDirection)std::stoi(reader.getField("direction_id", "0")),
        reader.getField("block_id"),
        reader.getField("shape_id"),
        (WheelchairAccessibility)std::stoi(reader.getField("wheelchair_accessible", "0")),
        reader.getField("bikes_allowed") == "1",
      };
      trips.push_back(item);
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