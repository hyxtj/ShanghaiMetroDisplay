/***************************************************************************
  文件名称：AddStationDialog.cpp
  功    能：添加地铁站点对话框的实现文件
  说    明：提供添加新地铁站点的界面，包括站点信息设置和位置选择
***************************************************************************/

#include "AddStationDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include<QFormLayout>
#include <QComboBox>

/***************************************************************************
  函数名称：AddStationDialog::AddStationDialog
  功    能：构造函数，初始化对话框界面
  输入参数：StationWidget* stationWidget - 站点显示部件
            QWidget*       parent        - 父窗口指针
  返 回 值：
  说    明：初始化界面组件并设置对话框属性
***************************************************************************/
AddStationDialog::AddStationDialog(StationWidget* stationWidget, QWidget* parent)
    : QDialog(parent), stationWidget(stationWidget) 
{
    setupUI();

    // 确保对话框模态，这样当它隐藏时用户只能与地图交互
    setModal(true);

    // 确保对话框在显示时位于最前面
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

/***************************************************************************
  函数名称：AddStationDialog::setupUI
  功    能：设置对话框的用户界面
  输入参数：
  返 回 值：
  说    明：创建并布局各个UI组件，包括站点信息输入、位置选择和操作按钮
  ***************************************************************************/
void AddStationDialog::setupUI() {
    setWindowTitle(QString::fromUtf8("添加地铁站点"));
    setMinimumSize(400, 400); // 增加高度以容纳经纬度输入框

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    /*站点基本信息*/
    QGroupBox*   infoGroup  = new QGroupBox(QString::fromUtf8("站点信息"), this);
    QFormLayout* infoLayout = new QFormLayout(infoGroup);

    /*站点名称编辑*/
    nameEdit = new QLineEdit(this);
    infoLayout->addRow(QString::fromUtf8("站点名称:"), nameEdit);

    /*标签位置选择*/
    tagCombo = new QComboBox(this);
    tagCombo->addItems({  QString::fromUtf8("left") , QString::fromUtf8("right") ,
                          QString::fromUtf8("top")  , QString::fromUtf8("bottom"),
                          QString::fromUtf8("topleft")
    });
    infoLayout->addRow(QString::fromUtf8("标签位置:"), tagCombo);

    /*站点类型选择*/
    typeCombo = new QComboBox(this);
    typeCombo->addItems({ QString::fromUtf8("normal"), QString::fromUtf8("transfer") });
    infoLayout->addRow(QString::fromUtf8("站点类型:"), typeCombo);

    mainLayout->addWidget(infoGroup);

    /*站点位置选择*/
    QGroupBox*   positionGroup  = new QGroupBox(QString::fromUtf8("站点位置"), this);
    QHBoxLayout* positionLayout = new QHBoxLayout(positionGroup);

    positionLayout->addWidget(new QLabel(QString::fromUtf8("X坐标:"), this));
    xSpin = new QSpinBox(this);
    xSpin->setRange(0, 2000);
    positionLayout->addWidget(xSpin);

    positionLayout->addWidget(new QLabel(QString::fromUtf8("Y坐标:"), this));
    ySpin = new QSpinBox(this);
    ySpin->setRange(0, 2000);
    positionLayout->addWidget(ySpin);

    selectPositionButton = new QPushButton(QString::fromUtf8("选择位置"), this);
    positionLayout->addWidget(selectPositionButton);

    mainLayout->addWidget(positionGroup);

    /*经纬度信息*/
    QGroupBox*   coordGroup  = new QGroupBox(QString::fromUtf8("经纬度坐标"), this);
    QFormLayout* coordLayout = new QFormLayout(coordGroup);

    longitudeEdit = new QLineEdit(this);
    longitudeEdit->setPlaceholderText(QString::fromUtf8("例如: 121.47"));
    coordLayout->addRow(QString::fromUtf8("经度:"), longitudeEdit);

    latitudeEdit = new QLineEdit(this);
    latitudeEdit->setPlaceholderText(QString::fromUtf8("例如: 31.23"));
    coordLayout->addRow(QString::fromUtf8("纬度:"), latitudeEdit);

    mainLayout->addWidget(coordGroup);

    /*确认按钮*/
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* okButton     = new QPushButton(QString::fromUtf8("确定"), this);
    QPushButton* cancelButton = new QPushButton(QString::fromUtf8("取消"), this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(selectPositionButton, &QPushButton::clicked, this, [this]() {
        /* 启用选择模式*/
        if (stationWidget) {
            stationWidget->setSelectionMode(true);

            /* 连接位置选择信号*/
            positionConnection = connect(stationWidget, &StationWidget::positionSelected,
                this, [this](const QPoint& position) {
                    /* 断开信号连接*/
                    disconnect(positionConnection);

                    /* 退出选择模式*/
                    stationWidget->setSelectionMode(false);

                    /* 更新坐标*/
                    xSpin->setValue(position.x());
                    ySpin->setValue(position.y());
                });
        }
    });

    connect(okButton,     &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

/***************************************************************************
  函数名称：AddStationDialog::getStationName
  功    能：获取输入的站点名称
  输入参数：
  返 回 值：QString - 站点名称
  说    明：
  ***************************************************************************/
QString AddStationDialog::getStationName() const {
    return nameEdit->text();
}

/***************************************************************************
  函数名称：AddStationDialog::getStationPosition
  功    能：获取输入的站点位置
  输入参数：
  返 回 值：QPoint - 站点位置
  说    明：
  ***************************************************************************/
QPoint AddStationDialog::getStationPosition() const {
    return QPoint(xSpin->value(), ySpin->value());
}

/***************************************************************************
  函数名称：AddStationDialog::getStationTag
  功    能：获取输入的站点标签位置
  输入参数：
  返 回 值：QString - 站点标签位置
  说    明：
  ***************************************************************************/
QString AddStationDialog::getStationTag() const {
    return tagCombo->currentText();
}

/***************************************************************************
  函数名称：AddStationDialog::getStationType
  功    能：获取输入的站点类型
  输入参数：
  返 回 值：QString - 站点类型
  说    明：
  ***************************************************************************/
QString AddStationDialog::getStationType() const {
    return typeCombo->currentText();
}

/***************************************************************************
  函数名称：AddStationDialog::cloesEvent
  功    能：处理关闭事件
  输入参数：QCloseEvent* event - 关闭事件指针
  返 回 值：
  说    明：检查站点名称是否为空，若为空则提示用户
  ***************************************************************************/
void AddStationDialog::closeEvent(QCloseEvent* event) {
    if (stationWidget) {
        stationWidget->setSelectionMode(false);
    }
    QDialog::closeEvent(event);
}

/***************************************************************************
  函数名称：AddStationDialog::getLongitude
  功    能：获取经度
  输入参数：
  返 回 值：经度值
  说    明：
  ***************************************************************************/
double AddStationDialog::getLongitude() const {
    return longitudeEdit->text().toDouble();
}

/***************************************************************************
  函数名称：AddStationDialog::getLatitude
  功    能：获取纬度
  输入参数：
  返 回 值：纬度值
  说    明：
  ***************************************************************************/
double AddStationDialog::getLatitude() const {
    return latitudeEdit->text().toDouble();
}

/* AddStationDialog.cpp */