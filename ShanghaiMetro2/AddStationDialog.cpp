#include "AddStationDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include<QFormLayout>
#include <QComboBox>

AddStationDialog::AddStationDialog(StationWidget* stationWidget, QWidget* parent)
    : QDialog(parent), stationWidget(stationWidget) {
    setupUI();

    // 确保对话框模态，这样当它隐藏时用户只能与地图交互
    setModal(true);

    // 确保对话框在显示时位于最前面
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

void AddStationDialog::setupUI() {
    setWindowTitle(QString::fromUtf8("添加地铁站点"));
    setMinimumSize(400, 300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 站点基本信息
    QGroupBox* infoGroup = new QGroupBox(QString::fromUtf8("站点信息"), this);
    QFormLayout* infoLayout = new QFormLayout(infoGroup);

    nameEdit = new QLineEdit(this);
    infoLayout->addRow(QString::fromUtf8("站点名称:"), nameEdit);

    tagCombo = new QComboBox(this);
    tagCombo->addItems({ QString::fromUtf8("left"), QString::fromUtf8("right"),
                       QString::fromUtf8("top"), QString::fromUtf8("bottom"),
                       QString::fromUtf8("topleft") });
    infoLayout->addRow(QString::fromUtf8("标签位置:"), tagCombo);

    typeCombo = new QComboBox(this);
    typeCombo->addItems({ QString::fromUtf8("normal"), QString::fromUtf8("transfer") });
    infoLayout->addRow(QString::fromUtf8("站点类型:"), typeCombo);

    mainLayout->addWidget(infoGroup);

    // 站点位置
    QGroupBox* positionGroup = new QGroupBox(QString::fromUtf8("站点位置"), this);
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

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* okButton = new QPushButton(QString::fromUtf8("确定"), this);
    QPushButton* cancelButton = new QPushButton(QString::fromUtf8("取消"), this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(selectPositionButton, &QPushButton::clicked, this, [this]() {
        // 启用选择模式
        if (stationWidget) {
            stationWidget->setSelectionMode(true);

            // 连接位置选择信号
            positionConnection = connect(stationWidget, &StationWidget::positionSelected,
                this, [this](const QPoint& position) {
                    // 断开信号连接
                    disconnect(positionConnection);

                    // 退出选择模式
                    stationWidget->setSelectionMode(false);

                    // 更新坐标
                    xSpin->setValue(position.x());
                    ySpin->setValue(position.y());
                });
        }
        });

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString AddStationDialog::getStationName() const {
    return nameEdit->text();
}

QPoint AddStationDialog::getStationPosition() const {
    return QPoint(xSpin->value(), ySpin->value());
}

QString AddStationDialog::getStationTag() const {
    return tagCombo->currentText();
}

QString AddStationDialog::getStationType() const {
    return typeCombo->currentText();
}
void AddStationDialog::closeEvent(QCloseEvent* event) {
    if (stationWidget) {
        stationWidget->setSelectionMode(false);
    }
    QDialog::closeEvent(event);
}