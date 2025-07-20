#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"

#ifndef GTFS_DIRECTORY_PATH
#error "Setzen Sie den Pfad zu Ihrem GTFS-Verzeichnis in Zeile 5 in config.h und kommentieren Sie die Zeile ein"
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , myNetwork(GTFS_DIRECTORY_PATH)
    , scheduleModel(myNetwork, this)  // Modèle pour l'onglet Fahrplan
    , routeModel(myNetwork, this)     // Modèle pour l'onglet Routenplaner
{
    ui->setupUi(this);
    setWindowTitle("GUI-Routenplaner");

    // Configurez les modes d'affichage
    scheduleModel.setDisplayMode(StopTimesTableModel::SCHEDULE_MODE);      // Une seule colonne temps
    routeModel.setDisplayMode(StopTimesTableModel::ROUTE_PLAN_MODE);       // Deux colonnes temps

    // Assign models to elements - SÉPARÉS !
    ui->routeComboBox->setModel(&routesModel);
    ui->tripComboBox->setModel(&tripsModel);
    
    // Utilisez des modèles différents pour chaque onglet
    ui->stopTimeTableView->setModel(&routeModel);        // Routenplaner tab
    ui->scheduleTableView->setModel(&scheduleModel);     // Fahrplan tab
    
    // Setup new UI elements for Aufgabe 5
    ui->fromStopComboBox->setModel(&fromStopsModel);
    ui->toStopComboBox->setModel(&toStopsModel);

    // Setup displayed routes
    for (auto &item : myNetwork.getRoutes()) {
        std::string displayName = myNetwork.getRouteDisplayName(item);
        routes.push_back(std::make_pair(item.id, displayName));
    }

    // Sort the results by name
    std::sort(routes.begin(),
              routes.end(),
              [](const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) {
                  return a.second < b.second;
              }
    );
    routesModel.setStringList(getStringListForDisplayNames(routes));
    
    // Setup available stops for route calculation
    populateStopComboBoxes();
    
    // Initialize UI states
    updateUIStates();
    
    // Set initial time to current time
    ui->departureTimeEdit->setTime(QTime::currentTime());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_searchTextEdit_textChanged()
{
    // Skip on inactive UI
    if (!ui->searchTextEdit->isEnabled()) {
        return;
    }

    // Update table - UTILISE LE MODÈLE FAHRPLAN
    int index = ui->tripComboBox->currentIndex();
    if (index >= 0 && index < static_cast<int>(trips.size())) {
        std::string tripId = trips[index].first;
        updateStopTimesTable(ui->searchTextEdit->text().toStdString(), tripId);
    }
}

void MainWindow::on_routeComboBox_currentIndexChanged(int index) {
    std::cout << "DEBUG: Route selection changed, index=" << index << std::endl;
    
    if (index < 0 || index >= static_cast<int>(routes.size())) {
        std::cout << "DEBUG: Invalid route index" << std::endl;
        return;
    }
    
    std::string routeId = routes[index].first;
    std::cout << "DEBUG: Selected route ID: " << routeId << std::endl;

    // Find trips for the selected route
    auto tripsForRoute = myNetwork.getTripsForRoute(routeId);
    std::cout << "DEBUG: Found " << tripsForRoute.size() << " trips for route" << std::endl;
    
    trips.clear();
    for (auto &item : tripsForRoute) {
        std::string displayName = myNetwork.getTripDisplayName(item);
        trips.push_back(std::make_pair(item.id, displayName));
        std::cout << "DEBUG: Trip: " << item.id << " - " << displayName << std::endl;
    }

    // Clear and reset UI - UTILISE LE MODÈLE FAHRPLAN
    ui->searchTextEdit->setEnabled(false);
    ui->searchTextEdit->clear();
    ui->scheduleTableView->setEnabled(false);
    scheduleModel.clear();  // Clear le modèle Fahrplan

    // Enable trip selection
    ui->tripComboBox->clear();
    ui->tripComboBox->setEnabled(true);
    tripsModel.setStringList(getStringListForDisplayNames(trips));
}

void MainWindow::on_tripComboBox_currentIndexChanged(int index) {
    std::cout << "DEBUG: Trip selection changed, index=" << index << std::endl;
    
    // Skip on inactive UI
    if (!ui->tripComboBox->isEnabled() || index < 0 || index >= static_cast<int>(trips.size())) {
        std::cout << "DEBUG: Trip selection invalid, enabled=" << ui->tripComboBox->isEnabled() 
                  << " index=" << index << " trips.size()=" << trips.size() << std::endl;
        return;
    }

    // Clear search field
    ui->searchTextEdit->clear();

    // Update table
    std::string tripId = trips[index].first;
    std::cout << "DEBUG: Selected trip ID: " << tripId << std::endl;
    updateStopTimesTable("", tripId);
}

// New methods for Aufgabe 5

void MainWindow::on_fromStopComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index)
    updateUIStates();
}

void MainWindow::on_toStopComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index)
    updateUIStates();
}

void MainWindow::on_departureTimeEdit_timeChanged(const QTime &time) {
    Q_UNUSED(time)
    updateUIStates();
}

void MainWindow::on_calculateRouteButton_clicked() {
    int fromIndex = ui->fromStopComboBox->currentIndex();
    int toIndex = ui->toStopComboBox->currentIndex();
    
    if (fromIndex < 0 || fromIndex >= static_cast<int>(availableStops.size()) || 
        toIndex < 0 || toIndex >= static_cast<int>(availableStops.size())) {
        QMessageBox::warning(this, "Fehler", "Bitte wählen Sie Start- und Zielhaltestelle aus.");
        return;
    }
    
    if (fromIndex == toIndex) {
        QMessageBox::information(this, "Information", "Start- und Zielhaltestelle sind identisch.");
        return;
    }
    
    std::string fromStopId = availableStops[fromIndex].first;
    std::string toStopId = availableStops[toIndex].first;
    
    // Convert QTime to GTFSTime
    QTime departureTime = ui->departureTimeEdit->time();
    bht::GTFSTime gtfsDepartureTime = {
        static_cast<unsigned char>(departureTime.hour()),
        static_cast<unsigned char>(departureTime.minute()),
        0
    };
    
    // Calculate route
    auto travelPlan = myNetwork.getTravelPlanDepartingAt(fromStopId, toStopId, gtfsDepartureTime);
    
    if (travelPlan.empty()) {
        QMessageBox::information(this, "Keine Route gefunden", 
                                 "Es konnte keine Route zur angegebenen Zeit gefunden werden.");
    } else {
        displayTravelPlan(travelPlan);
    }
}

void MainWindow::updateStopTimesTable(std::string needle, std::string tripId) {
    std::cout << "DEBUG: updateStopTimesTable called with tripId: " << tripId 
              << " needle: '" << needle << "'" << std::endl;
    
    const auto& stopTimes = myNetwork.searchStopTimesForTrip(needle, tripId);
    
    std::cout << "DEBUG: Found " << stopTimes.size() << " stop times for Fahrplan" << std::endl;
    
    // Debug: afficher quelques arrêts
    for (size_t i = 0; i < std::min(stopTimes.size(), size_t(5)); ++i) {
        const auto& stop = stopTimes[i];
        auto stopInfo = myNetwork.getStopById(stop.stopId);
        std::cout << "DEBUG: Stop " << i << ": " << stop.stopId 
                  << " (" << stopInfo.name << ") seq=" << stop.stopSequence 
                  << " arrival=" << static_cast<int>(stop.arrivalTime.hour) << ":" 
                  << static_cast<int>(stop.arrivalTime.minute) << std::endl;
    }
    
    // UTILISE LE MODÈLE FAHRPLAN
    scheduleModel.setDisplayedStopTimes(stopTimes);
    ui->searchTextEdit->setEnabled(true);
    ui->scheduleTableView->setEnabled(true);
    ui->scheduleTableView->resizeColumnsToContents();
    
    std::cout << "DEBUG: Schedule table (Fahrplan) updated and enabled" << std::endl;
}

void MainWindow::populateStopComboBoxes() {
    std::cout << "DEBUG: Populating stop combo boxes" << std::endl;
    
    // Get all stops and sort them by name
    for (const auto& stopPair : myNetwork.stops) {
        const auto& stop = stopPair.second;
        // Only include stations and platforms, not sub-elements
        if (stop.locationType == bht::LocationType_Station || 
            stop.locationType == bht::LocationType_Stop) {
            availableStops.push_back(std::make_pair(stop.id, stop.name));
        }
    }
    
    std::cout << "DEBUG: Found " << availableStops.size() << " available stops" << std::endl;
    
    // Sort by name
    std::sort(availableStops.begin(), availableStops.end(),
              [](const std::pair<std::string, std::string>& a, 
                 const std::pair<std::string, std::string>& b) {
                  return a.second < b.second;
              });
    
    // Set models
    QStringList stopNames = getStringListForDisplayNames(availableStops);
    fromStopsModel.setStringList(stopNames);
    toStopsModel.setStringList(stopNames);
}

void MainWindow::updateUIStates() {
    // Enable calculate button only when all required fields are filled
    bool canCalculate = (ui->fromStopComboBox->currentIndex() >= 0 &&
                        ui->toStopComboBox->currentIndex() >= 0 &&
                        ui->fromStopComboBox->currentIndex() != ui->toStopComboBox->currentIndex());
    
    ui->calculateRouteButton->setEnabled(canCalculate);
}

void MainWindow::displayTravelPlan(const std::vector<bht::StopTime>& travelPlan) {
    std::cout << "DEBUG: Displaying travel plan with " << travelPlan.size() << " stops" << std::endl;
    
    // UTILISE LE MODÈLE ROUTENPLANER
    routeModel.setDisplayedStopTimes(travelPlan);
    ui->stopTimeTableView->setEnabled(true);
    ui->stopTimeTableView->resizeColumnsToContents();
    
    std::cout << "DEBUG: Route table (Routenplaner) updated" << std::endl;
}

QStringList MainWindow::getStringListForDisplayNames(const std::vector<std::pair<std::string, std::string>>& items) {
    QStringList result;
    for (auto &item : items) {
        result << QString(item.second.c_str());
    }
    return result;
}