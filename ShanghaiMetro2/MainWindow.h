// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QStatusBar>
#include <QLabel>
#include <QSplitter>
#include <QScrollArea>
#include "StationWidget.h"
#include "MetroGraph.h"
#include "PathFinder.h"
#include <QKeyEvent>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onFromStationSelected(const QString& station);
    void onToStationSelected(const QString& station);
    void onFindPathClicked();
    void onStationClicked(const QString& station);
    void onStrategyChanged(QAbstractButton* button);
    void onClearClicked();
    void onAddLineClicked();
    void onAddStationClicked();
    void onMousePositionChanged(const QPoint& pos); // 新增：处理鼠标位置变化
    void onAddStationAtPosition(const QPoint& position); // 新增：在指定位置添加站点

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    void setupUI();
    void loadMetroData();
    void loadSortedStations();
    void updatePathGuide(const MetroPath& path);
    void refreshUI();
    void updateStatusBar();


    MetroGraph metroGraph;
    PathFinder pathFinder;
    SearchStrategy selectedStrategy;

    // UI components
    QSplitter* mainSplitter;
    StationWidget* stationWidget;
    QScrollArea* controlScrollArea;
    QWidget* controlPanel;
    QComboBox* fromComboBox;
    QComboBox* toComboBox;
    QPushButton* findPathButton;
    QPushButton* clearButton;
    QPushButton* addLineButton;
    QPushButton* addStationButton;
    QTextEdit* pathGuideText;
    QButtonGroup* strategyButtonGroup;
    QStatusBar* statusBar;
    QLabel* stationCountLabel;
    QLabel* lineCountLabel;

    QString selectedFromStation;
    QString selectedToStation;
    QLabel* mousePosLabel; // 新增：显示鼠标位置的标签
};

#endif // MAINWINDOW_H