autotest:
	g++ -I. -I/usr/local/include -std=c++17 -o test_runner /usr/local/lib/libgtest_main.a /usr/local/lib/libgtest.a tester.cpp network.cpp scheduled_trip.cpp csv.cpp