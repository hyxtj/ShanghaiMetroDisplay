#ifndef ADDSTATIONDIALOG_H
#define ADDSTATIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QMetaObject>
#include "StationWidget.h"

class AddStationDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddStationDialog(StationWidget* stationWidget, QWidget* parent = nullptr);
    QString getStationName() const;
    QPoint getStationPosition() const;
    QString getStationTag() const;
    QString getStationType() const;
    QPushButton* selectPositionButton;

protected:
    void closeEvent(QCloseEvent* event);

private:
    StationWidget* stationWidget;

    QLineEdit* nameEdit;
    QSpinBox* xSpin;
    QSpinBox* ySpin;
    QComboBox* tagCombo;
    QComboBox* typeCombo;

    QMetaObject::Connection positionConnection; // 保存信号连接

    void setupUI();
};

#endif // ADDSTATIONDIALOG_H