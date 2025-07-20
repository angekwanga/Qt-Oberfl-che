#include "stoptimestablemodel.h"

StopTimesTableModel::StopTimesTableModel(const bht::Network& network, QObject *parent)
    : QAbstractTableModel(parent), network(network), displayMode(SCHEDULE_MODE)
{
    headerFont.setBold(true);
    sequenceNumberFont.setItalic(true);
}

void StopTimesTableModel::setDisplayMode(DisplayMode mode) {
    beginResetModel();
    displayMode = mode;
    endResetModel();
}

QVariant StopTimesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Ignore all other orientations besides horizontal
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    // Return data matching the desired role
    if (role == Qt::FontRole) {
        // We want bold headers
        return headerFont;
    }
    else if (role == Qt::TextAlignmentRole) {
        // We want left alight, vertically centered headers
        return (int)(Qt::AlignLeft | Qt::AlignVCenter);
    }
    else if (role == Qt::DisplayRole) {
        // Return header names based on display mode
        if (displayMode == SCHEDULE_MODE) {
            static std::string scheduleHeaders[] = {"Nr.", "Name Haltestelle", "Zeit"};
            if (section >= 0 && section < 3) {
                return QString(scheduleHeaders[section].c_str());
            }
        } else {
            static std::string routeHeaders[] = {"Nr.", "Name Haltestelle", "Ankunftszeit", "Abfahrtszeit"};
            if (section >= 0 && section < 4) {
                return QString(routeHeaders[section].c_str());
            }
        }
        return QVariant();
    }
    else {
        return QVariant();
    }
}

int StopTimesTableModel::rowCount(const QModelIndex &) const
{
    return displayedStopTimes.size();
}

int StopTimesTableModel::columnCount(const QModelIndex &) const
{
    if (displayMode == SCHEDULE_MODE) {
        return 3; // Nr, Name, Zeit
    } else {
        return 4; // Nr, Name, Ankunft, Abfahrt
    }
}

QVariant StopTimesTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(displayedStopTimes.size())) {
        return QVariant();
    }

    // Return data matching the desired role
    if (role == Qt::FontRole) {
        // We want a special font for the sequence number
        return index.column() == 0 ? sequenceNumberFont : contentFont;
    }
    else if (role == Qt::TextAlignmentRole) {
        // We want left alight, vertically centered headers
        return (int)(Qt::AlignLeft | Qt::AlignVCenter);
    }
    else if (role == Qt::DisplayRole) {
        // Return cell data
        const bht::StopTime &item = displayedStopTimes[index.row()];
        
        if (displayMode == SCHEDULE_MODE) {
            switch (index.column()) {
            case 0:
                return QString::number(item.stopSequence);
            case 1:
                return QString(network.getStopById(item.stopId).name.c_str());
            case 2:
                // Afficher l'heure de départ (plus logique pour un horaire)
                return QString("%1:%2")
                    .arg(static_cast<int>(item.departureTime.hour), 2, 10, QLatin1Char('0'))
                    .arg(static_cast<int>(item.departureTime.minute), 2, 10, QLatin1Char('0'));
            default:
                return QVariant();
            }
        } else {
            switch (index.column()) {
            case 0:
                return QString::number(item.stopSequence);
            case 1:
                return QString(network.getStopById(item.stopId).name.c_str());
            case 2:
                // Ankunftszeit
                return QString("%1:%2")
                    .arg(static_cast<int>(item.arrivalTime.hour), 2, 10, QLatin1Char('0'))
                    .arg(static_cast<int>(item.arrivalTime.minute), 2, 10, QLatin1Char('0'));
            case 3:
            default:
                // Abfahrtszeit
                return QString("%1:%2")
                    .arg(static_cast<int>(item.departureTime.hour), 2, 10, QLatin1Char('0'))
                    .arg(static_cast<int>(item.departureTime.minute), 2, 10, QLatin1Char('0'));
            }
        }
    }
    else {
        return QVariant();
    }
}

void StopTimesTableModel::setDisplayedStopTimes(const std::vector<bht::StopTime>& data) {
    beginResetModel();
    displayedStopTimes = data;
    endResetModel();
}

void StopTimesTableModel::clear() {
    beginResetModel();
    displayedStopTimes.clear();
    endResetModel();
}