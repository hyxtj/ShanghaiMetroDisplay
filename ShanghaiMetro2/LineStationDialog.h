// LineStationDialog.h
#ifndef LINESTATIONDIALOG_H
#define LINESTATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "MetroGraph.h"

class LineStationDialog : public QDialog {
    Q_OBJECT

public:
    explicit LineStationDialog(const MetroGraph& metroGraph, QWidget* parent = nullptr);
    QString getSelectedStation() const;
    QString getSelectedLine() const;

private slots:
    void onLineSelected(const QString& line);
    void onStationSelected(QListWidgetItem* item);
    void onOkClicked();
    void onCancelClicked();

private:
    void setupUI();
    void updateStationsList(const QString& line);

    const MetroGraph& metroGraph;
    QComboBox* lineComboBox;
    QListWidget* stationsListWidget;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QString selectedStation;
    QString selectedLine;
};

#endif // LINESTATIONDIALOG_H