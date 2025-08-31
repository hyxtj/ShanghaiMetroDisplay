/***************************************************************************
  文件名称：AddStationDialog.h
  功    能：添加地铁站点对话框的头文件
  说    明：定义添加地铁站点的对话框界面和功能
***************************************************************************/

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
	explicit AddStationDialog(StationWidget* stationWidget, QWidget* parent = nullptr); // 构造函数

	/*获取输入方法*/
    QString getStationName()     const; //获取输入站点名称    
	QPoint  getStationPosition() const; //获取输入站点位置
	QString getStationTag()      const; //获取输入站点标签
	QString getStationType()     const; //获取输入站点类型
	double  getLongitude()       const; //获取输入站点经度
	double  getLatitude()        const; //获取输入站点纬度

	QPushButton* selectPositionButton;  // 选择位置按钮

protected:
	void closeEvent(QCloseEvent* event); // 重载关闭事件，断开信号连接

private:
	StationWidget* stationWidget;  // 关联的站点控件
	
	/*输入组件*/
	QLineEdit* nameEdit;     // 站点名称输入框
	QSpinBox*  xSpin;        // 站点X坐标输入框
    QSpinBox*  ySpin;        // 站点Y坐标输入框
	QComboBox* tagCombo;     // 站点标签选择框
    QComboBox* typeCombo;    // 站点类型选择框
	QLineEdit* longitudeEdit;// 站点经度编辑框
	QLineEdit* latitudeEdit; // 站点纬度编辑框
	
    QMetaObject::Connection positionConnection; // 保存信号连接

    void setupUI(); //设置UI样式  
};

#endif // ADDSTATIONDIALOG_H