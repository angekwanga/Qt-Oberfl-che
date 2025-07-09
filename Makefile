CPPFLAGS=-Wall -Wpedantic -std=c++17
CC=g++

main: main.o network.o scheduled_trip.o csv.o

autotest: 
	g++ -I. -I/usr/local/include -std=c++17 -o test_runner /usr/local/lib/libgtest_main.a /usr/local/lib/libgtest.a scheduled_trip.cpp tester.cpp network.cpp csv.cpp