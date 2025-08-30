// LineStationDialog.cpp
#include "LineStationDialog.h"
#include <QCollator>

LineStationDialog::LineStationDialog(const MetroGraph& metroGraph, QWidget* parent)
    : QDialog(parent), metroGraph(metroGraph) {
    setupUI();
}

void LineStationDialog::setupUI() {
    setWindowTitle(QString::fromUtf8("选择线路和站点"));
    setMinimumSize(400, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 线路选择
    QHBoxLayout* lineLayout = new QHBoxLayout();
    lineLayout->addWidget(new QLabel(QString::fromUtf8("选择线路:"), this));

    lineComboBox = new QComboBox(this);

    // 获取所有线路并按名称排序
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

    // 站点列表
    mainLayout->addWidget(new QLabel(QString::fromUtf8("选择站点:"), this));
    stationsListWidget = new QListWidget(this);
    mainLayout->addWidget(stationsListWidget);

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    okButton = new QPushButton(QString::fromUtf8("确定"), this);
    cancelButton = new QPushButton(QString::fromUtf8("取消"), this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // 连接信号
    connect(lineComboBox, &QComboBox::currentTextChanged, this, &LineStationDialog::onLineSelected);
    connect(stationsListWidget, &QListWidget::itemClicked, this, &LineStationDialog::onStationSelected);
    connect(okButton, &QPushButton::clicked, this, &LineStationDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &LineStationDialog::onCancelClicked);

    // 初始化站点列表
    if (lineComboBox->count() > 0) {
        updateStationsList(lineComboBox->currentText());
    }
}

void LineStationDialog::onLineSelected(const QString& line) {
    updateStationsList(line);
}

void LineStationDialog::updateStationsList(const QString& line) {
    stationsListWidget->clear();
    selectedStation.clear();
    selectedLine = line;

    // 获取该线路上的所有站点
    QMap<QString, QVector<QString>> lineStations = metroGraph.getLineStations();
    QVector<QString> stations = lineStations[line];

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

void LineStationDialog::onStationSelected(QListWidgetItem* item) {
    selectedStation = item->text();
}

void LineStationDialog::onOkClicked() {
    if (!selectedStation.isEmpty()) {
        accept();
    }
    else {
        // 提示用户选择站点
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请选择一个站点"));
    }
}

void LineStationDialog::onCancelClicked() {
    reject();
}

QString LineStationDialog::getSelectedStation() const {
    return selectedStation;
}

QString LineStationDialog::getSelectedLine() const {
    return selectedLine;
}