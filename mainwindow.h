#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QComboBox>
#include <QTimeEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QMessageBox>
#include "network.h"
#include "stoptimestablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_searchTextEdit_textChanged();
    void on_routeComboBox_currentIndexChanged(int index);
    void on_tripComboBox_currentIndexChanged(int index);
    
    // New slots for Aufgabe 5
    void on_fromStopComboBox_currentIndexChanged(int index);
    void on_toStopComboBox_currentIndexChanged(int index);
    void on_departureTimeEdit_timeChanged(const QTime &time);
    void on_calculateRouteButton_clicked();

private:
    Ui::MainWindow *ui;

    /// The network we fetched
    bht::Network myNetwork;

    /// Model for displayed routes
    QStringListModel routesModel;

    /// Model for displayed trips
    QStringListModel tripsModel;

    /// Model for displayed stop times in Fahrplan tab
    StopTimesTableModel scheduleModel;
    
    /// Model for displayed route results in Routenplaner tab
    StopTimesTableModel routeModel;
    
    /// Model for stop selection
    QStringListModel fromStopsModel;
    QStringListModel toStopsModel;

    /// Shown routes with route id and to route display name pairs
    std::vector<std::pair<std::string, std::string>> routes;

    /// Shown trips with trip id and to trip display name pairs
    std::vector<std::pair<std::string, std::string>> trips;
    
    /// Available stops for selection
    std::vector<std::pair<std::string, std::string>> availableStops;

    /**
     * @brief getStringListForDisplayNames Extract display names from the second item out of a vector of pairs
     * @param items Vector of pairs where the second item is the display name
     * @return Resulting string list
     */
    QStringList getStringListForDisplayNames(const std::vector<std::pair<std::string, std::string>>& items);

    /**
     * @brief updateStopTimesTable Update the displayed items in the stop times table
     * @param needle Search word to apply for filtering
     * @param tripId Currently selected trip id
     */
    void updateStopTimesTable(std::string needle, std::string tripId);
    
    /**
     * @brief updateUIStates Update UI element states based on selections
     */
    void updateUIStates();
    
    /**
     * @brief populateStopComboBoxes Fill the stop selection combo boxes
     */
    void populateStopComboBoxes();
    
    /**
     * @brief displayTravelPlan Display calculated travel plan in table
     * @param travelPlan Vector of StopTime objects representing the route
     */
    void displayTravelPlan(const std::vector<bht::StopTime>& travelPlan);
};

#endif // MAINWINDOW_H