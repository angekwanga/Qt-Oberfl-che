BEWERTUNG: 99

# Auswertung der Abgabe

- Status: Abgabe wurde bewertet
- Punkte: **99** von **100**

## Test 1

- Test: *Erforderliche Dateien sind vorhanden*
- Beschreibung: Das erforderliche Makefile muss vorhanden sein.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000027
##### Testschritte
- Makefile soll vorhanden sein: OK

## Test 2

- Test: *Qt-Projektdatei ist vorhanden*
- Beschreibung: Eine Projektdatei für das Qt-Projekt zum erstellen mit qmake ist vorhanden.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000591
- Punkte: **1**
##### Testschritte
- Für ./**/*.pro wurde ./Uebung05.pro gefunden: OK

## Test 3

- Test: *Makefile-Target autotest existiert*
- Beschreibung: Das Makefile besitzt ein Target zum kompilieren der Tests.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000155
##### Testschritte
- Inhalt von Makefile prüfen: OK

## Test 4

- Test: *tester.cpp-Datei ist unverändert*
- Beschreibung: Die Datei tester.cpp wurde nicht verändert und ist genau so wie im Original-Repository.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000135
##### Testschritte
- Hash-Test von tester.cpp auf ['bfb51d75114c6432faa78b69c397273aaaff2ee4']: OK

## Test 5

- Test: *Kompilieren der Tests*
- Beschreibung: Mit dem Befehl `make autotest` können die Tests kompiliert werden.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:30.266590
- Punkte: **1**
- Return-Code / Fehlercode: `0`
- Kommandozeile: `make autotest`
##### Testschritte
- Rückgabe-Code ist `0`: OK

##### Ausgabe

```g++ -I. -I/usr/local/include -std=c++17 -Wall -Wextra -c network.cpp -o network.o
g++ -I. -I/usr/local/include -std=c++17 -Wall -Wextra -c csv.cpp -o csv.o
g++ -I. -I/usr/local/include -std=c++17 -Wall -Wextra -c scheduled_trip.cpp -o scheduled_trip.o
g++ -I. -I/usr/local/include -std=c++17 -Wall -Wextra -o test_runner /usr/local/lib/libgtest_main.a /usr/local/lib/libgtest.a tester.cpp network.cpp csv.cpp scheduled_trip.cpp -lpthread
./test_runner
Running main() from /usr/src/googletest/googletest/src/gtest_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from Network
[ RUN      ] Network.getStopsForTransfer
[       OK ] Network.getStopsForTransfer (8076 ms)
[ RUN      ] Network.getTravelPlanDepartingAt
[       OK ] Network.getTravelPlanDepartingAt (14102 ms)
[----------] 2 tests from Network (22179 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (22179 ms total)
[  PASSED  ] 2 tests.
```

##### Fehlerausgabe

```tester.cpp: In member function 'virtual void {anonymous}::Network_getTravelPlanDepartingAt_Test::TestBody()':
tester.cpp:91:29: warning: comparison of integer expressions of different signedness: 'int' and 'std::vector<bht::SStopTime>::size_type' {aka 'long unsigned int'} [-Wsign-compare]
   91 |   for (int index = 1; index < results.size(); index++) {
      |                       ~~~~~~^~~~~~~~~~~~~~~~
```

## Test 6

- Test: *Ausführbare Anwendung der Unit-Tests wurde erstellt*
- Beschreibung: Die Anwendung wird mit Unit-Tests kompiliert und erzeugt die geforderte Datei `/repo/test_runner`.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000027
##### Testschritte
- test_runner soll vorhanden sein: OK

## Test 7

- Test: *Ausführbare Anwendung der Unit-Tests enthält die Testfälle*
- Beschreibung: Die geforderte Datei `/repo/test_runner` enthält die `tester.cpp`-Datei.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.312371
- Return-Code / Fehlercode: `0`
- Kommandozeile: `readelf -s --wide /repo/test_runner`
##### Testschritte
- Ausgabe ist korrekt: OK
- Rückgabe-Code ist `0`: OK

##### Ausgabe

```<AUSGABE WIRD NICHT ANGEZEIGT>
```

## Test 8

- Test: *Testfälle werden korrekt ausgeführt*
- Beschreibung: Die Unit-Tests der Anwendung werden korrekt ausgeführt (max. Laufzeit 60 Sekunden).
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:22.255635
- Punkte: **96**
- Return-Code / Fehlercode: `0`
- Kommandozeile: `/repo/test_runner`
##### Testschritte
- Rückgabe-Code ist `0`: OK

##### Ausgabe

```Running main() from /usr/src/googletest/googletest/src/gtest_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from Network
[ RUN      ] Network.getStopsForTransfer
[       OK ] Network.getStopsForTransfer (7959 ms)
[ RUN      ] Network.getTravelPlanDepartingAt
[       OK ] Network.getTravelPlanDepartingAt (14016 ms)
[----------] 2 tests from Network (21975 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (21975 ms total)
[  PASSED  ] 2 tests.
```

## Test 9

- Test: *QMake kann ausgeführt werden*
- Beschreibung: qmake erzeugt ein Makefile für die Anwendung
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.355036
- Punkte: **1**
- Return-Code / Fehlercode: `0`
- Kommandozeile: `/usr/bin/qmake6 /repo/./Uebung05.pro`
##### Testschritte
- Rückgabe-Code ist `0`: OK

##### Ausgabe

```Info: creating stash file /repo/build/.qmake.stash
```

##### Fehlerausgabe

```WARNING: Failure to find: config.h
```

## Test 10

- Test: *Erzeugtes Makefile kann die Anwendung erstellen*
- Beschreibung: Das erzeugte Makefile für die Anwendung kann kompiliert werden.
- Status: ausgeführt
- Erfolgreich: **Nein**
- Laufzeit: 0:00:02.677153
- Punkte: **0**
- Return-Code / Fehlercode: `2`
- Kommandozeile: `make`
##### Testschritte
- Rückgabe-Code ist `0`: fehlgeschlagen

##### Ausgabe

```/usr/lib/qt6/libexec/uic ../mainwindow.ui -o ui_mainwindow.h
g++ -c -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -D_REENTRANT -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/repo -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o csv.o ../csv.cpp
g++ -c -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -D_REENTRANT -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/repo -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o main_qt.o ../main_qt.cpp
g++ -c -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -D_REENTRANT -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/repo -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o mainwindow.o ../mainwindow.cpp
```

##### Fehlerausgabe

```../mainwindow.cpp:4:10: fatal error: config.h: No such file or directory
    4 | #include "config.h"
      |          ^~~~~~~~~~
compilation terminated.
make: *** [Makefile:415: mainwindow.o] Error 1
```

