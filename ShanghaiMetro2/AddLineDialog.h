/***************************************************************************
  文件名称：AddLineDialog.h
  功    能：添加地铁线路对话框的头文件
  说    明：定义添加地铁线路的对话框界面和功能
***************************************************************************/

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
	explicit AddLineDialog(const MetroGraph& metroGraph, QWidget* parent = nullptr);//构造函数

	MetroLine                                getLine() const;                       // 获取新添加的地铁线路信息
	QVector<QPair<QString, QVector<QPoint>>> getStationsAndConnections() const;     // 获取新添加的站点及其连接信息

private slots:
	/*槽函数定义*/
	void onAddStationClicked();             // 添加站点按钮响应
	void onRemoveStationClicked();          // 移除站点按钮响应
	void onMoveUpClicked();                 // 上移站点按钮响应
    void onMoveDownClicked();               // 下移站点按钮响应
	void onColorButtonClicked();            // 选择颜色按钮响应
    void onStationSelectionChanged();       // 触发选择站点改变响应
	void onConnectionTypeChanged(int index);// 连接类型改变响应

public:
	/*输入框和列表*/
	QComboBox*   connectionTypeCombo;  // 连接类型选择框
	QSpinBox*    viaPointXSpin;        // 途经点X坐标输入框
	QSpinBox*    viaPointYSpin;        // 途经点Y坐标输入框
    QListWidget* viaPointsList;        // 途经点列表
    QListWidget* selectedStationsList; // 选中站点列表

private:
    const MetroGraph& metroGraph;    //全局地铁线路图对象

	/*地铁信息操作*/
    QLineEdit*   lineNameEdit; //地铁名称编辑框
    QPushButton* colorButton;  //颜色选择按钮
    QColor       lineColor;    //线路颜色

    QListWidget* availableStationsList; //可达站点列表

	/*线路调整*/
    QPushButton* addStationButton;   //添加站点按钮
	QPushButton* removeStationButton;//移除站点按钮
    QPushButton* moveUpButton;       //上移站点按钮
    QPushButton* moveDownButton;     //下移站点按钮

	QPushButton* addViaPointButton;    //添加途经点按钮
	QPushButton* removeViaPointButton; //移除途经点按钮

	/*其他函数*/
    void setupUI();            //设置UI
	void updateStationLists(); //更新站点列表
};

#endif // ADDLINEDIALOG_H#ifndef ADDLINEDIALOG_H