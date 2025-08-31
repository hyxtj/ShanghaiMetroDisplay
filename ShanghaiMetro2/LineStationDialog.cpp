/***************************************************************************
  文件名称：LineStationDialog.cpp
  功    能：线路站点选择对话框的实现文件
  说    明：提供按线路选择站点的界面，用于起点和终点的选择
***************************************************************************/

#include "LineStationDialog.h"
#include <QCollator>

/***************************************************************************
  函数名称：LineStationDialog::LineStationDialog
  功    能：构造函数，初始化对话框界面
  输入参数：const MetroGraph metroGraph - 地铁图数据
            QWidget          parent     - 父窗口指针
  返 回 值：
  说    明：初始化界面组件并加载线路和站点数据
***************************************************************************/
LineStationDialog::LineStationDialog(const MetroGraph& metroGraph, QWidget* parent)
    : QDialog(parent), metroGraph(metroGraph) 
{
    setupUI();
}

/***************************************************************************
  函数名称：LineStationDialog::setupUI
  功    能：初始化界面组件
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void LineStationDialog::setupUI() {
    setWindowTitle(QString::fromUtf8("选择线路和站点"));
    setMinimumSize(400, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    /* 线路选择*/
    QHBoxLayout* lineLayout = new QHBoxLayout();
    lineLayout->addWidget(new QLabel(QString::fromUtf8("选择线路:"), this));

    lineComboBox = new QComboBox(this);

    /* 获取所有线路并按名称排序*/
    QVector<MetroLine> lines = metroGraph.getLines();
    QCollator collator;
    collator.setNumericMode(false);
    std::sort(lines.begin(), lines.end(), [&collator](const MetroLine& a, const MetroLine& b) {
        return collator.compare(a.name, b.name) < 0;
    });

    for (const MetroLine& line : lines) {
        lineComboBox->addItem(line.name);
    }

    lineLayout->addWidget(lineComboBox);
    mainLayout->addLayout(lineLayout);

    /* 站点列表*/
    mainLayout->addWidget(new QLabel(QString::fromUtf8("选择站点:"), this));
    stationsListWidget = new QListWidget(this);
    mainLayout->addWidget(stationsListWidget);

    /* 按钮*/
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    okButton     = new QPushButton(QString::fromUtf8("确定"), this);
    cancelButton = new QPushButton(QString::fromUtf8("取消"), this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout  ->addLayout(buttonLayout);

    /* 连接信号*/
    connect(lineComboBox,       &QComboBox::currentTextChanged, this, &LineStationDialog::onLineSelected);
    connect(stationsListWidget, &QListWidget::itemClicked,      this, &LineStationDialog::onStationSelected);
    connect(okButton,           &QPushButton::clicked,          this, &LineStationDialog::onOkClicked);
    connect(cancelButton,       &QPushButton::clicked,          this, &LineStationDialog::onCancelClicked);

    // 初始化站点列表
    if (lineComboBox->count() > 0) {
        updateStationsList(lineComboBox->currentText());
    }
}

/***************************************************************************
  函数名称：LineStationDialog::onLineSelected
  功    能：线路被选择时更新站点列表
  输入参数：const QString& line - 选择的线路名称
  返 回 值：
  说    明：
***************************************************************************/
void LineStationDialog::onLineSelected(const QString& line) {
    updateStationsList(line);
}

/***************************************************************************
  函数名称：LineStationDialog::updateStationsList
  功    能：更新线路上的站点，并按拼音排序
  输入参数：const QString& line - 选择的线路名称
  返 回 值：
  说    明：
***************************************************************************/
void LineStationDialog::updateStationsList(const QString& line) {
    stationsListWidget->clear();
    selectedStation.clear();
    selectedLine = line;

    // 获取该线路上的所有站点
    QMap<QString, QVector<QString>> lineStations = metroGraph.getLineStations();
    QVector<QString>                stations     = lineStations[line];

    // 按拼音排序
    QCollator collator;
    collator.setNumericMode(false);
    std::sort(stations.begin(), stations.end(), [&collator](const QString& a, const QString& b) {
        return collator.compare(a, b) < 0;
    });

    // 添加到列表
    for (const QString& station : stations) {
        stationsListWidget->addItem(station);
    }
}

/***************************************************************************
  函数名称：LineStationDialog::onStationSelected
  功    能：站点被选择时记录选择的站点
  输入参数：QListWidgetItem* item - 被选择的站点项
  返 回 值：
  说    明：
  ***************************************************************************/
void LineStationDialog::onStationSelected(QListWidgetItem* item) {
    selectedStation = item->text();
}

/***************************************************************************
  函数名称：LineStationDialog::onOkClicked
  功    能：提示用户选择站点并关闭对话框
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void LineStationDialog::onOkClicked() {
    if (!selectedStation.isEmpty()) {
        accept();
    }
    else {
        // 提示用户选择站点
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请选择一个站点"));
    }
}

/***************************************************************************
  函数名称：LineStationDialog::onCancelClicked
  功    能：关闭窗口
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void LineStationDialog::onCancelClicked() {
    reject();
}

/***************************************************************************
  函数名称：LineStationDialog::getSelectedStation
  功    能：获取被选中的站点名称
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
QString LineStationDialog::getSelectedStation() const {
    return selectedStation;
}

/***************************************************************************
  函数名称：LineStationDialog::getSelectedLine
  功    能：获取被选中的线路名称
  输入参数：
  返 回 值：
  说    明：
  ***************************************************************************/
QString LineStationDialog::getSelectedLine() const {
    return selectedLine;
}

/*LineStationDialog.cpp*/