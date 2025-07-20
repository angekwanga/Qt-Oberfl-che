#include <cstddef>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include "types.h"
#include "scheduled_trip.h"
#include "network.h"

#define TEST(category, name) void category ## name()
#define EXPECT_EQ(a, b) assert((a) == (b));
#define EXPECT_GE(a, b) assert((a) >= (b));
#define EXPECT_GT(a, b) assert((a) > (b));
#define EXPECT_NE(a, b) assert((a) != (b));
#define EXPECT_LE(a, b) assert((a) <= (b));
#define EXPECT_TRUE(a) assert((a) == true);

using namespace bht;

namespace {

inline bool containsStop(const std::vector<Stop>& items, const std::string &needle) {
  for (auto item : items) {
    if (item.id == needle) {
      return true;
    }
  }

  return false;
}

#define contains(c, item) (c.find(item) != c.end())

inline bool hasTrip(Network &network, std::string& stopId1, std::string& stopId2) {
  const std::unordered_set<std::string> neighbors = network.getNeighbors(stopId1);
  return contains(neighbors, stopId2);
}

// Tests for getStopsForTransfer
TEST(Network, getStopsForTransfer) {
  std::string inputDirectory{"/GTFSTest"};
  Network network{inputDirectory};

  // Plessa, Bahnhof
  std::vector<Stop> results = network.getStopsForTransfer("de:12062:900416008:1:50");
  const std::string plessaStops[] = { "de:12062:900416008:1:50", "de:12062:900416008:2:51", "de:12062:900416008" };
  EXPECT_EQ(results.size(), 3) // "transfers at de:12062:900416008:1:50 should contain 3 stops but your size() is " // results.size();
  for (const std::string& stopId : plessaStops) {
    EXPECT_TRUE(containsStop(results, stopId)) // "Stop " // stopId // " not in stops for transfer at stop de:12062:900416008:1:50 but should be";
  }

  // S Potsdam Hauptbahnhof
  results = network.getStopsForTransfer("de:12054:900230999");
  const std::string potsdamStops[] = { "de:12054:900230999:1:50", "de:12054:900230999:2:52", "de:12054:900230999:1:51", "de:12054:900230999:2:53", "de:12054:900230999" };
  for (const std::string& stopId : potsdamStops) {
    EXPECT_TRUE(containsStop(results, stopId)) // "Stop " // stopId // " not in stops for transfer at station de:12054:900230999 but should be";
  }
}

// Tests for getNeighbors
TEST(Network, getNeighbors) {
  std::string inputDirectory{"/GTFSTest"};
  Network network{inputDirectory};

  // Potsdam, Platz der Einheit/West
  std::unordered_set<std::string> results = network.getNeighbors("de:12054:900230014::2");
  const std::string neighbors1[] = { "de:12054:900230064::2", "de:12054:900230205::2", "de:12054:900230014::1" };
  EXPECT_GE(results.size(), 3) // "Stop at de:12054:900230014::2 should contain at least 3 neighbor stops but your size() is " // results.size();
  for (const std::string& stopId : neighbors1) {
    EXPECT_TRUE(contains(results, stopId)) // "Stop " // stopId // " not in neighbor stops for transfer at stop de:12054:900230014::2 but should be";
  }

  // Neubrandenburg, Bahnhof
  results = network.getNeighbors("de:13071:12353");
  const std::string neighbors2[] = { "de:13071:20", "de:13071:31", "de:13071:32", "de:13071:3628", "de:13071:5041" };
  EXPECT_GE(results.size(), 5) // "Stop at de:13071:12353 should contain at least 5 neighbor stops but your size() is " // results.size();
  for (const std::string& stopId : neighbors2) {
    EXPECT_TRUE(contains(results, stopId)) // "Stop " // stopId // " not in neighbor stops for transfer at stop de:13071:12353 but should be";
  }

  // Potsdam, Abzweig nach Eiche
  results = network.getNeighbors("de:12054:900230134::4");
  const std::string neighbors3[] = { "de:12054:900230133::2", "de:12054:900230228::2" };
  for (const std::string& stopId : neighbors3) {
    EXPECT_TRUE(contains(results, stopId)) // "Stop " // stopId // " not in neighbor stops for transfer at stop de:12054:900230134::4 but should be";
  }
}

// Tests for getTravelPath
TEST(Network, getTravelPath) {
  std::string inputDirectory{"/GTFSTest"};
  Network network{inputDirectory};

  std::vector<Stop> results = network.getTravelPath("de:12054:900230999", "de:13071:12353");
  EXPECT_GE(results.size(), 12) // "Path from 'de:12054:900230999' to 'de:13071:12353' should be at least 12 stops long but your size() is " // results.size();
  /*
  const std::string stops1[] = { "de:12054:900230999:2:53", "de:11000:900003201:1:50", "de:11000:900003200:3:55", "de:13071:80001", "de:13071:12353" };
  for (auto item : stops1) {
    EXPECT_TRUE(containsStop(results, item)) // "Stop " // item // " should be in path from from 'de:12054:900230999' to 'de:13071:12353' but is not";
  }
  */
  for (unsigned int index = 0; index < results.size() - 1; index++) {
    EXPECT_TRUE(hasTrip(network, results[index].id, results[index + 1].id)) // "Trip between " // results[index].id // " and " // results[index+1].id // " in your result path not found.";
  }
  
  // Check undefined path
  results = network.getTravelPath("de:12054:900230999:1:50", "de:12054:900230999::3");
  EXPECT_EQ(results.size(), 0) // "Path from 'de:12054:900230999:1:50' to 'de:12054:900230999::' should be empty but your size() is " // results.size();
}

// Tests for getScheduledTrip
TEST(Network, getScheduledTrip) {
  std::string inputDirectory{"/GTFSTest"};
  Network network{inputDirectory};

  std::string tripId = "230348303";
  NetworkScheduledTrip trip1 = network.getScheduledTrip(tripId);
  const std::string stops1[] = { "000005170002", "000005100145", "de:12073:900340004:1:50" };
  int index = 0;
  for (auto iter = trip1.begin(); iter != trip1.end() && index < 3; iter++, index++) {
    EXPECT_EQ(iter->stopSequence, index) // "Stop sequence number of stop " // stops1[index] // " should be " // index // " but was " // iter->stopSequence;
    EXPECT_EQ(iter->stopId, stops1[index]) // "Stop at sequence number " // index // " should be " // stops1[index] // " but was " // iter->stopId;
    EXPECT_EQ(iter->tripId, tripId) // "Stop should belong to trip " // tripId // " but tripId was " // iter->tripId;
  }

  tripId = "230353669";
  NetworkScheduledTrip trip2 = network.getScheduledTrip(tripId);
  const std::string stops2[] = { "de:11000:900058101:3:54", "de:11000:900100020:2:53", "de:11000:900003200:3:54", "de:11000:900007102:5:58", "de:12065:900200005:1:1", "de:12065:900203780:1:50", "de:12065:900203709:2:51", "de:12065:900203749:1:50", "de:12065:900203772:1:50", "de:13071:80001" };
  index = 0;
  for (auto iter = trip2.begin(); iter != trip2.end() && index < 10; iter++, index++) {
    EXPECT_EQ(iter->stopSequence, index) // "Stop sequence number of stop " // stops2[index] // " should be " // index // " but was " // iter->stopSequence;
    EXPECT_EQ(iter->stopId, stops2[index]) // "Stop at sequence number " // index // " should be " // stops2[index] // " but was " // iter->stopId;
    EXPECT_EQ(iter->tripId, tripId) // "Stop should belong to trip " // tripId // " but tripId was " // iter->tripId;
  }
}

// Tests for NetworkScheduledTrip iterators
TEST(Network, NetworkScheduledTrip_iterator) {
  std::string inputDirectory{"/GTFSTest"};
  Network network{inputDirectory};

  std::string tripId = "230348303";
  NetworkScheduledTrip trip = network.getScheduledTrip(tripId);
  EXPECT_EQ(trip.begin(), trip.begin()) // "NetworkScheduledTrip iterators should be comparable";
  EXPECT_EQ(++(trip.begin()), ++(trip.begin())) // "NetworkScheduledTrip iterators should be comparable";
  EXPECT_EQ(trip.end(), trip.end()) // "NetworkScheduledTrip iterators should be comparable";

  auto iter1 = trip.begin();
  auto iter2 = trip.end();
  iter1++;
  iter2--;
  iter2--;
  EXPECT_EQ(iter1, iter2) // "Iterators should compare equal if they point to the same stop";

  EXPECT_EQ(std::distance(trip.begin(), trip.end()), 3) // "std::distance from begin() to end() should return the number of stops in the trip";
}

} // namespace
