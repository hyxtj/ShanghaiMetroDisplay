#ifndef ADDLINEDIALOG_H
#define ADDLINEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QColorDialog>
#include <QSpinBox>
#include "MetroGraph.h"

class AddLineDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddLineDialog(const MetroGraph& metroGraph, QWidget* parent = nullptr);
    MetroLine getLine() const;
    QVector<QPair<QString, QVector<QPoint>>> getStationsAndConnections() const;

private slots:
    void onAddStationClicked();
    void onRemoveStationClicked();
    void onMoveUpClicked();
    void onMoveDownClicked();
    void onColorButtonClicked();
    void onStationSelectionChanged();
    void onConnectionTypeChanged(int index);

public:
    QComboBox* connectionTypeCombo;
    QSpinBox* viaPointXSpin;
    QSpinBox* viaPointYSpin;
    QListWidget* viaPointsList;
    QListWidget* selectedStationsList;


private:
    const MetroGraph& metroGraph;

    QLineEdit* lineNameEdit;
    QPushButton* colorButton;
    QColor lineColor;

    QListWidget* availableStationsList;

    QPushButton* addStationButton;
    QPushButton* removeStationButton;
    QPushButton* moveUpButton;
    QPushButton* moveDownButton;


    QPushButton* addViaPointButton;
    QPushButton* removeViaPointButton;

    void setupUI();
    void updateStationLists();
};

#endif // ADDLINEDIALOG_H#ifndef ADDLINEDIALOG_H