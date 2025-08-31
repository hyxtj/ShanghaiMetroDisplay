/***************************************************************************
  函数名称：AddLineDialog::AddLineDialog
  功    能：构造函数，初始化对话框界面
  输入参数：metroGraph - 地铁图数据，parent - 父窗口指针
  返 回 值：
  说    明：初始化界面组件并更新站点列表
***************************************************************************/

#include "AddLineDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QCombobox>
#include <QCollator>

/***************************************************************************
  函数名称：AddLineDialog::AddLineDialog
  功    能：构造函数，初始化对话框界面
  输入参数：const MetroGraph& metroGraph - 地铁图数据
            QWidget           parent     - 父窗口指针
  返 回 值：
  说    明：初始化界面组件并更新站点列表
***************************************************************************/
AddLineDialog::AddLineDialog(const MetroGraph& metroGraph, QWidget* parent)
    : QDialog(parent), metroGraph(metroGraph), lineColor(Qt::black)
{
    setupUI();
    updateStationLists();
}

/***************************************************************************
  函数名称：AddLineDialog::setupUI
  功    能：UI初始化函数
  输入参数：
  返 回 值：
  说    明：初始化界面组件
***************************************************************************/
void AddLineDialog::setupUI() {
    setWindowTitle(QString::fromUtf8("添加地铁线路"));
    setMinimumSize(800, 600);
    setStyleSheet(R"(
        QDialog {
            background-color: #0a0e14;
            color: #ffffff;
        }
        QLabel {
            color: #8b9bb4;
        }
        QLineEdit, QSpinBox, QComboBox {
            background-color: #1e2229;
            border: 1px solid #2a2f3b;
            border-radius: 4px;
            padding: 5px;
            color: #ffffff;
        }
        QListWidget {
            background-color: #1e2229;
            border: 1px solid #2a2f3b;
            border-radius: 4px;
            color: #ffffff;
        }
        QListWidget::item:selected {
            background-color: #00d4ff;
            color: #000000;
        }
    )");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    /*线路信息框*/
    QGroupBox*   infoGroup  = new QGroupBox(QString::fromUtf8("线路信息"), this);
    QHBoxLayout* infoLayout = new QHBoxLayout(infoGroup);

    /*线路名称*/
    infoLayout->addWidget(new QLabel(QString::fromUtf8("线路名称:"), this));
    lineNameEdit = new QLineEdit(this);
    infoLayout->addWidget(lineNameEdit);

    /*线路颜色选择*/
    infoLayout  ->addWidget(new QLabel(QString::fromUtf8("线路颜色:"), this));
    colorButton = new QPushButton(this);
    colorButton ->setFixedSize(30, 30);
    colorButton ->setStyleSheet(QString("background-color: %1").arg(lineColor.name()));
    connect(colorButton, &QPushButton::clicked, this, &AddLineDialog::onColorButtonClicked);
    infoLayout->addWidget(colorButton);

    infoLayout->addStretch();
    mainLayout->addWidget(infoGroup);

    /*站点选择框*/
    QGroupBox*   stationsGroup  = new QGroupBox(QString::fromUtf8("站点选择"), this);
    QHBoxLayout* stationsLayout = new QHBoxLayout(stationsGroup);

    availableStationsList = new QListWidget(this);
    stationsLayout->addWidget(availableStationsList);
    
    /*操作按钮组*/
    QVBoxLayout* buttonsLayout = new QVBoxLayout();
    addStationButton    = new QPushButton(QString::fromUtf8("添加 >>"), this);
    removeStationButton = new QPushButton(QString::fromUtf8("<< 移除"), this);
    moveUpButton        = new QPushButton(QString::fromUtf8("上移"), this);
    moveDownButton      = new QPushButton(QString::fromUtf8("下移"), this);

    buttonsLayout ->addWidget(addStationButton);
    buttonsLayout ->addWidget(removeStationButton);
    buttonsLayout ->addStretch();
    buttonsLayout ->addWidget(moveUpButton);
    buttonsLayout ->addWidget(moveDownButton);
    stationsLayout->addLayout(buttonsLayout);

    selectedStationsList = new QListWidget(this);
    stationsLayout->addWidget(selectedStationsList);

    connect(addStationButton,     &QPushButton::clicked,              this, &AddLineDialog::onAddStationClicked);
    connect(removeStationButton,  &QPushButton::clicked,              this, &AddLineDialog::onRemoveStationClicked);
    connect(moveUpButton,         &QPushButton::clicked,              this, &AddLineDialog::onMoveUpClicked);
    connect(moveDownButton,       &QPushButton::clicked,              this, &AddLineDialog::onMoveDownClicked);
    connect(selectedStationsList, &QListWidget::itemSelectionChanged, this, &AddLineDialog::onStationSelectionChanged);

    mainLayout->addWidget(stationsGroup);

    // 连接设置
    QGroupBox*   connectionGroup  = new QGroupBox(QString::fromUtf8("连接设置"), this);
    QVBoxLayout* connectionLayout = new QVBoxLayout(connectionGroup);

    /*设置连接模式*/
    QHBoxLayout* connectionTypeLayout = new QHBoxLayout();
    connectionTypeLayout->addWidget(new QLabel(QString::fromUtf8("连接类型:"), this));
    connectionTypeCombo = new QComboBox(this);
    connectionTypeCombo ->addItem(QString::fromUtf8("直线连接"));
    connectionTypeCombo ->addItem(QString::fromUtf8("折线连接"));
    connectionTypeLayout->addWidget(connectionTypeCombo);
    connectionTypeLayout->addStretch();

    connectionLayout->addLayout(connectionTypeLayout);

    /*添加转折点和移除转折点*/
    QHBoxLayout* viaPointLayout = new QHBoxLayout();
    viaPointLayout->addWidget(new QLabel(QString::fromUtf8("转折点X:"), this));
    viaPointXSpin = new QSpinBox(this);
    viaPointXSpin ->setRange(0, 2000);
    viaPointLayout->addWidget(viaPointXSpin);

    viaPointLayout->addWidget(new QLabel(QString::fromUtf8("转折点Y:"), this));
    viaPointYSpin = new QSpinBox(this);
    viaPointYSpin ->setRange(0, 2000);
    viaPointLayout->addWidget(viaPointYSpin);

    addViaPointButton = new QPushButton(QString::fromUtf8("添加转折点"), this);
    viaPointLayout->addWidget(addViaPointButton);

    removeViaPointButton = new QPushButton(QString::fromUtf8("移除转折点"), this);
    viaPointLayout->addWidget(removeViaPointButton);

    connectionLayout->addLayout(viaPointLayout);

    viaPointsList = new QListWidget(this);
    connectionLayout->addWidget(viaPointsList);

    connect(connectionTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &AddLineDialog::onConnectionTypeChanged);

    connect(addViaPointButton, &QPushButton::clicked, this, [this]() {
        int x = viaPointXSpin->value();
        int y = viaPointYSpin->value();
        viaPointsList->addItem(QString("(%1, %2)").arg(x).arg(y));
     });
    connect(removeViaPointButton, &QPushButton::clicked, this, [this]() {
        int row = viaPointsList->currentRow();
        if (row >= 0) {
            delete viaPointsList->takeItem(row);
        }
     });

    mainLayout->addWidget(connectionGroup);

    /*确认和取消键*/
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* okButton     = new QPushButton(QString::fromUtf8("确定"), this);
    QPushButton* cancelButton = new QPushButton(QString::fromUtf8("取消"), this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(okButton,     &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    /* 初始状态*/
    onConnectionTypeChanged(0);
    onStationSelectionChanged();
}

/***************************************************************************
  函数名称：AddLineDialog::onAddStationClicked
  功    能：响应触发添加站点按钮
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void AddLineDialog::onAddStationClicked() {
    QList<QListWidgetItem*> selectedItems = availableStationsList->selectedItems();
    for (QListWidgetItem* item : selectedItems) {
        selectedStationsList->addItem(item->text());
        delete availableStationsList->takeItem(availableStationsList->row(item));
    }
}

/***************************************************************************
  函数名称：AddLineDialog::onRemoveStationClicked
  功    能：响应触发移除站点按钮
  输入参数：
  返 回 值：
  说    明：
  ***************************************************************************/
void AddLineDialog::onRemoveStationClicked() {
    QList<QListWidgetItem*> selectedItems = selectedStationsList->selectedItems();
    for (QListWidgetItem* item : selectedItems) {
        availableStationsList->addItem(item->text());
        delete selectedStationsList->takeItem(selectedStationsList->row(item));
    }
}

/***************************************************************************
  函数名称：AddLineDialog::onMoveUpClicked
  功    能：响应触发上移站点按钮
  输入参数：
  返 回 值：
  说    明：
  ***************************************************************************/
void AddLineDialog::onMoveUpClicked() {
    int currentRow = selectedStationsList->currentRow();
    if (currentRow > 0) {
        QListWidgetItem* item = selectedStationsList->takeItem(currentRow);
        selectedStationsList->insertItem(currentRow - 1, item);
        selectedStationsList->setCurrentRow(currentRow - 1);
    }
}

/***************************************************************************
  函数名称：AddLineDialog::onMoveDownClicked
  功    能：响应触发下移站点按钮
  输入参数：
  返 回 值：
  说    明：
  ***************************************************************************/
void AddLineDialog::onMoveDownClicked() {
    int currentRow = selectedStationsList->currentRow();
    if (currentRow >= 0 && currentRow < selectedStationsList->count() - 1) {
        QListWidgetItem* item = selectedStationsList->takeItem(currentRow);
        selectedStationsList->insertItem(currentRow + 1, item);
        selectedStationsList->setCurrentRow(currentRow + 1);
    }
}

/***************************************************************************
  函数名称：AddLineDialog::onColorButtonClicked
  功    能：响应触发选择颜色按钮
  输入参数：
  返 回 值：
  说    明：
  ***************************************************************************/
void AddLineDialog::onColorButtonClicked() {
    QColor color = QColorDialog::getColor(lineColor, this, QString::fromUtf8("选择线路颜色"));
    if (color.isValid()) {
        lineColor = color;
        colorButton->setStyleSheet(QString("background-color: %1").arg(lineColor.name()));
    }
}

/***************************************************************************
  函数名称：AddLineDialog::onStationSelectionChanged
  功    能：响应站点选择变化，更新按钮状态
  输入参数：
  返 回 值：
  说    明：
  ***************************************************************************/
void AddLineDialog::onStationSelectionChanged() {
    bool hasSelection = selectedStationsList->currentRow() >= 0;
    removeStationButton->setEnabled(hasSelection);
    moveUpButton->setEnabled(hasSelection);
    moveDownButton->setEnabled(hasSelection);
}

/***************************************************************************
  函数名称：AddLineDialog::onConnectionTypeChanged
  功    能：响应连接类型变化，更新转折点设置状态
  输入参数：int index - 当前选择的连接类型索引
  返 回 值：
  说    明：
  ***************************************************************************/
void AddLineDialog::onConnectionTypeChanged(int index) {
    bool isViaEnabled = (index == 1); // 折线连接时才启用转折点设置
    viaPointXSpin       ->setEnabled(isViaEnabled);
    viaPointYSpin       ->setEnabled(isViaEnabled);
    addViaPointButton   ->setEnabled(isViaEnabled);
    removeViaPointButton->setEnabled(isViaEnabled);
    viaPointsList       ->setEnabled(isViaEnabled);
}

/***************************************************************************
  函数名称：AddLineDialog::getLine
  功    能：获取用户输入的线路信息
  输入参数：
  返 回 值：MetroLine - 包含线路名称和颜色的结构体
  说    明：
  ***************************************************************************/
MetroLine AddLineDialog::getLine() const {
    MetroLine line;
    line.name  = lineNameEdit->text();
    line.color = lineColor;
    return line;
}

/***************************************************************************
  函数名称：AddLineDialog::getStationsAndConnections
  功    能：获取用户选择的站点及其连接方式
  输入参数：
  返 回 值：QVector<QPair<QString, QVector<QPoint>>> - 每个站点及其连接的转折点列表
  说    明：
  ***************************************************************************/
QVector<QPair<QString, QVector<QPoint>>> AddLineDialog::getStationsAndConnections() const {
    QVector<QPair<QString, QVector<QPoint>>> result;

    /* 获取所有选中的站点*/
    QVector<QString> stations;
    for (int i = 0; i < selectedStationsList->count(); i++) {
        stations.append(selectedStationsList->item(i)->text());
    }

    /* 为每对相邻站点创建连接*/
    for (int i = 0; i < stations.size() - 1; i++) {
        QString from = stations[i];
        QString to = stations[i + 1];

        QVector<QPoint> viaPoints;
        if (connectionTypeCombo->currentIndex() == 1) { // 折线连接
            for (int j = 0; j < viaPointsList->count(); j++) {
                QString viaText = viaPointsList->item(j)->text();
                /* 解析坐标，格式为 "(x, y)"*/
                viaText = viaText.mid(1, viaText.length() - 2); // 去掉括号
                QStringList coords = viaText.split(", ");
                if (coords.size() == 2) {
                    int x = coords[0].toInt();
                    int y = coords[1].toInt();
                    viaPoints.append(QPoint(x, y));
                }
            }
        }

        result.append(qMakePair(to, viaPoints));
    }

    return result;
}

/***************************************************************************
  函数名称：pinyinCompare
  功    能：用于按拼音顺序比较两个QString
  输入参数：a - 第一个字符串，b - 第二个字符串
  返 回 值：bool - 如果a应排在b前面则返回true，否则返回false
  说    明：使用QCollator进行中文拼音排序
  ***************************************************************************/
static bool pinyinCompare(const QString& a, const QString& b) {
    // 使用QCollator进行中文拼音排序
    QCollator collator;
    collator.setNumericMode(false);
    return collator.compare(a, b) < 0;
}

/***************************************************************************
  函数名称：AddLineDialog::updateStationLists
  功    能：更新可选站点列表，按拼音排序
  输入参数：
  返 回 值：
  说    明：
  ***************************************************************************/
void AddLineDialog::updateStationLists() {
    availableStationsList->clear();
    selectedStationsList ->clear();

    /* 获取所有站点名称并按拼音排序*/
    QVector<QString> stationNames = metroGraph.getStationNames();
    std::sort(stationNames.begin(), stationNames.end(), pinyinCompare);

    for (const QString& name : stationNames) {
        availableStationsList->addItem(name);
    }
}

/* AddLineDialog.cpp*/