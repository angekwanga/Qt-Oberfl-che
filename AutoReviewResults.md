BEWERTUNG: 4

# Auswertung der Abgabe

- Status: Abgabe wurde bewertet
- Punkte: **4** von **100**

## Test 1

- Test: *Erforderliche Dateien sind vorhanden*
- Beschreibung: Das erforderliche Makefile muss vorhanden sein.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000017
##### Testschritte
- Makefile soll vorhanden sein: OK

## Test 2

- Test: *Qt-Projektdatei ist vorhanden*
- Beschreibung: Eine Projektdatei für das Qt-Projekt zum erstellen mit qmake ist vorhanden.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000170
- Punkte: **1**
##### Testschritte
- Für ./**/*.pro wurde ./project.pro gefunden: OK

## Test 3

- Test: *Makefile-Target autotest existiert*
- Beschreibung: Das Makefile besitzt ein Target zum kompilieren der Tests.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000043
##### Testschritte
- Inhalt von Makefile prüfen: OK

## Test 4

- Test: *tester.cpp-Datei ist unverändert*
- Beschreibung: Die Datei tester.cpp wurde nicht verändert und ist genau so wie im Original-Repository.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000057
##### Testschritte
- Hash-Test von tester.cpp auf ['bfb51d75114c6432faa78b69c397273aaaff2ee4']: OK

## Test 5

- Test: *Kompilieren der Tests*
- Beschreibung: Mit dem Befehl `make autotest` können die Tests kompiliert werden.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:04.171223
- Punkte: **1**
- Return-Code / Fehlercode: `0`
- Kommandozeile: `make autotest`
##### Testschritte
- Rückgabe-Code ist `0`: OK

##### Ausgabe

```g++ -I. -I/usr/local/include -std=c++17 -o test_runner /usr/local/lib/libgtest_main.a /usr/local/lib/libgtest.a tester.cpp network.cpp scheduled_trip.cpp csv.cpp
```

## Test 6

- Test: *Ausführbare Anwendung der Unit-Tests wurde erstellt*
- Beschreibung: Die Anwendung wird mit Unit-Tests kompiliert und erzeugt die geforderte Datei `/repo/test_runner`.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.000019
##### Testschritte
- test_runner soll vorhanden sein: OK

## Test 7

- Test: *Ausführbare Anwendung der Unit-Tests enthält die Testfälle*
- Beschreibung: Die geforderte Datei `/repo/test_runner` enthält die `tester.cpp`-Datei.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.185296
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
- Erfolgreich: **Nein**
- Laufzeit: 0:00:15.387480
- Punkte: **0**
- Return-Code / Fehlercode: `139`
- Kommandozeile: `/repo/test_runner`
##### Testschritte
- Rückgabe-Code ist `0`: fehlgeschlagen

##### Ausgabe

```Running main() from /usr/src/googletest/googletest/src/gtest_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from Network
[ RUN      ] Network.getStopsForTransfer
[       OK ] Network.getStopsForTransfer (7614 ms)
[ RUN      ] Network.getTravelPlanDepartingAt
tester.cpp:66: Failure
Expected: (results.size()) > (2), actual: 0 vs 2
Path from 'de:12054:900230999' to 'de:13071:12353' should be available but your size() is 0
```

##### Fehlerausgabe

```/usr/local/include/c++/15.1.0/bits/stl_vector.h:1263: std::vector<_Tp, _Alloc>::reference std::vector<_Tp, _Alloc>::operator[](size_type) [with _Tp = bht::SStopTime; _Alloc = std::allocator<bht::SStopTime>; reference = bht::SStopTime&; size_type = long unsigned int]: Assertion '__n < this->size()' failed.
```

## Test 9

- Test: *QMake kann ausgeführt werden*
- Beschreibung: qmake erzeugt ein Makefile für die Anwendung
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:00.255116
- Punkte: **1**
- Return-Code / Fehlercode: `0`
- Kommandozeile: `/usr/bin/qmake6 /repo/./project.pro`
##### Testschritte
- Rückgabe-Code ist `0`: OK

##### Ausgabe

```Info: creating stash file /repo/build/.qmake.stash
```

## Test 10

- Test: *Erzeugtes Makefile kann die Anwendung erstellen*
- Beschreibung: Das erzeugte Makefile für die Anwendung kann kompiliert werden.
- Status: ausgeführt
- Erfolgreich: **Ja**
- Laufzeit: 0:00:11.840883
- Punkte: **1**
- Return-Code / Fehlercode: `0`
- Kommandozeile: `make`
##### Testschritte
- Rückgabe-Code ist `0`: OK

##### Ausgabe

```g++ -c -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -D_REENTRANT -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/repo -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o main.o ../main.cpp
g++ -c -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -D_REENTRANT -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/repo -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o mainwindow.o ../mainwindow.cpp
g++ -c -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -D_REENTRANT -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/repo -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o network.o ../network.cpp
g++ -c -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -D_REENTRANT -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/repo -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o scheduled_trip.o ../scheduled_trip.cpp
g++ -c -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -D_REENTRANT -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/repo -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o csv.o ../csv.cpp
g++ -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -dM -E -o moc_predefs.h /usr/lib/x86_64-linux-gnu/qt6/mkspecs/features/data/dummy.cpp
/usr/lib/qt6/libexec/moc -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB --include /repo/build/moc_predefs.h -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -I/repo -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I/usr/local/include/c++/15.1.0 -I/usr/local/include/c++/15.1.0/x86_64-linux-gnu -I/usr/local/include/c++/15.1.0/backward -I/usr/local/lib/gcc/x86_64-linux-gnu/15.1.0/include -I/usr/local/include -I/usr/local/lib/gcc/x86_64-linux-gnu/15.1.0/include-fixed -I/usr/include/x86_64-linux-gnu -I/usr/include ../mainwindow.h -o moc_mainwindow.cpp
g++ -c -pipe -O2 -std=gnu++1z -Wall -Wextra -fPIC -D_REENTRANT -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I/repo -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o moc_mainwindow.o moc_mainwindow.cpp
g++ -Wl,-O1 -Wl,-rpath-link,/usr/lib/x86_64-linux-gnu -o gtfs_route_planner  main.o mainwindow.o network.o scheduled_trip.o csv.o moc_mainwindow.o   /usr/lib/x86_64-linux-gnu/libQt6Widgets.so /usr/lib/x86_64-linux-gnu/libQt6Gui.so /usr/lib/x86_64-linux-gnu/libGLX.so /usr/lib/x86_64-linux-gnu/libOpenGL.so /usr/lib/x86_64-linux-gnu/libQt6Core.so -lpthread -lGLX -lOpenGL
```

