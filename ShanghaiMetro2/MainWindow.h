/***************************************************************************
  文件名称：MainWindow.h
  功    能：主窗口的头文件
  说    明：定义地铁查询系统的主界面类和功能
***************************************************************************/

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
#include "LineStationDialog.h"

#include <QMediaPlayer>
#include <QAudioOutput>


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr); //构造函数
    ~MainWindow();                         //析构函数

private slots:
    /* 槽函数定义*/
    void onFromStationSelected(const QString& station);  //选择起点站
    void onToStationSelected(const QString& station);    //选择终点站
    void onFindPathClicked();                            //点击查找路线
	void onStationClicked(const QString& station);       //点击站点
    void onStrategyChanged(QAbstractButton* button);     //改变查找策略
    void onClearClicked();                               //点击清除
    void onAddLineClicked();                             //点击添加路线
    void onAddStationClicked();                          //点击添加站点
    void onMousePositionChanged(const QPoint& pos);      //处理鼠标位置变化
    void onAddStationAtPosition(const QPoint& position); //在指定位置添加站点

    void onSelectStartByLine();                          //选择起点站--先选择路线方式
    void onSelectEndByLine();                            //选择终点站--先选择路线方式

protected:
    void keyPressEvent(QKeyEvent* event);                //处理鼠标事件

private:
    /*基础函数*/
    void setupUI();                              //设置UI       
    void loadMetroData();                        //加载站点数据
    void loadSortedStations();                   //加载按拼音排序的站点数据
    void updatePathGuide(const MetroPath& path); //更新换乘攻略
    void refreshUI();                            //刷新UI
    void updateStatusBar();                      //更新状态条

    /*数据处理*/
    MetroGraph     metroGraph;           //全局地铁线路数据
    PathFinder     pathFinder;           //路径查找类
    SearchStrategy selectedStrategy;     //路径搜索策略

    /* UI组件*/
	QSplitter*     mainSplitter;       //主分割器
    StationWidget* stationWidget;      //站点控件
    QScrollArea*   controlScrollArea;  //带有滚轮的区域
    QWidget*       controlPanel;       //控制面板
	QComboBox*     fromComboBox;       //起点选择框
    QComboBox*     toComboBox;         //终点选择框
    QPushButton*   findPathButton;     //查找按键
	QPushButton*   clearButton;        //清除按键
    QPushButton*   addLineButton;      //添加路线按键
    QPushButton*   addStationButton;   //添加站点按键
    QTextEdit*     pathGuideText;      //换乘策略文本框
    QButtonGroup*  strategyButtonGroup;//策略选择栏
    QStatusBar*    statusBar;          //状态栏
    QLabel*        stationCountLabel;  //站点名称标签
    QLabel*        lineCountLabel;     //路线名称标签

    QString selectedFromStation;//起点站名称
    QString selectedToStation;  //终点站名称
    QLabel* mousePosLabel;      //显示鼠标位置的标签

    QPushButton* selectStartByLineButton; //通过路线查找起点站按钮
	QPushButton* selectEndByLineButton;   //通过路线查找终点站按钮

	QMediaPlayer* backgroundPlayer;      //背景音乐播放控制器
    QMediaPlayer* arrivalPlayer;         //成功音乐播放控制器
	QAudioOutput* backgroundAudioOutput; //背景音乐音频输出
    QAudioOutput* arrivalAudioOutput;    //成功音乐音频输出

    void setupAudio();          //多媒体初始化
    void playBackgroundMusic(); //播放背景音乐
    void playArrivalSound();    //播放成功音乐
    void stopBackgroundMusic(); //停止背景音乐
};

#endif // MAINWINDOW_H