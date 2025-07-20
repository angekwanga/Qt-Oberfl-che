#ifndef STOPTIMESTABLEMODEL_H
#define STOPTIMESTABLEMODEL_H

#include <QAbstractTableModel>
#include <vector>
#include <QFont>
#include "types.h"
#include "network.h"

class StopTimesTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum DisplayMode {
        SCHEDULE_MODE,      // Mode Fahrplan (une seule colonne temps)
        ROUTE_PLAN_MODE     // Mode Routenplaner (deux colonnes temps)
    };

    /**
     * @brief Create a new StopTimesTableModel to display stop times
     * @param network Network instance to resolve stop ids to names
     * @param parent Object parent
     */
    explicit StopTimesTableModel(const bht::Network &network, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Data display methods
    // See https://doc.qt.io/qt-6/modelview.html
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief update Update displayed data
     * @param data New data to display
     */
    void setDisplayedStopTimes(const std::vector<bht::StopTime>& data);

    /**
     * @brief clear Clear all data in the model
     */
    void clear();

    /**
     * @brief Set the display mode for the table
     * @param mode Display mode to use
     */
    void setDisplayMode(DisplayMode mode);

    /**
     * @brief Get the current display mode
     * @return Current display mode
     */
    DisplayMode getDisplayMode() const { return displayMode; }

private:
    /// Network instance to resolve stop ids to names
    const bht::Network &network;

    /// Displayed stop times
    std::vector<bht::StopTime> displayedStopTimes;

    /// Font to use to display the header
    QFont headerFont;

    /// Font to use to display the content
    QFont contentFont;
    QFont sequenceNumberFont;

    /// Display mode
    DisplayMode displayMode;
};

#endif // STOPTIMESTABLEMODEL_H