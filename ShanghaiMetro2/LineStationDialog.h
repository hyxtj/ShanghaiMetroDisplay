/***************************************************************************
  文件名称：LineStationDialog.h
  功    能：线路站点选择对话框的头文件
  说    明：定义按线路选择站点的对话框界面和功能
***************************************************************************/

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
    explicit LineStationDialog(const MetroGraph& metroGraph, QWidget* parent = nullptr); //构造函数

    QString  getSelectedStation() const;   //获取选择的站点
    QString  getSelectedLine()    const;   //获取选择的路线

private slots:
    /*槽函数定义*/
    void onLineSelected(const QString& line);       //线路选择
    void onStationSelected(QListWidgetItem* item);  //站点选择
    void onOkClicked();                             //点击确认
    void onCancelClicked();                         //点击取消

private:
    /*基础函数*/
    void setupUI();                                 //UI设置
    void updateStationsList(const QString& line);   //更新站点列表

	const MetroGraph& metroGraph;      //全局地铁线路图对象

    /*控件定义*/
    QComboBox*   lineComboBox;       //线路选择框
    QListWidget* stationsListWidget; //站点列表控件
    QPushButton* okButton;           //确认键
    QPushButton* cancelButton;       //取消键
    QString      selectedStation;    //当前所选站点
	QString      selectedLine;       //当前所选线路
};

#endif // LINESTATIONDIALOG_HS