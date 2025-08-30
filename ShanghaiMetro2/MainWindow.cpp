#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>
#include <QCompleter>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QInputDialog>
#include <QColorDialog>
#include "AddLineDialog.h"
#include "AddStationDialog.h"
#include <QGraphicsOpacityEffect>
#include<QPropertyAnimation>
#include <QCollator>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), pathFinder(&metroGraph), selectedStrategy(MIN_STATIONS) {
    setupUI();
    loadMetroData();
}
MainWindow::~MainWindow() {}

// MainWindow.cpp
void MainWindow::setupUI() {
    // 设置应用程序样式
    qApp->setStyle(QStyleFactory::create("Fusion"));

    // 创建深色调色板
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(10, 14, 20));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(21, 26, 33));
    darkPalette.setColor(QPalette::AlternateBase, QColor(10, 14, 20));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(0, 212, 255));
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(21, 26, 33));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(0, 212, 255));
    darkPalette.setColor(QPalette::Highlight, QColor(0, 212, 255));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(darkPalette);

    // 设置样式表
    QString styleSheet = R"(
        QMainWindow, QDialog {
            background-color: #0a0e14;
            color: #ffffff;
            font-family: "Microsoft YaHei", "Arial";
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #2a2f3b;
            border-radius: 8px;
            margin-top: 1ex;
            padding-top: 10px;
            background-color: #151a21;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #00d4ff;
        }
        QPushButton {
            background-color: #1e2229;
            border: 1px solid #2a2f3b;
            border-radius: 4px;
            padding: 8px 12px;
            color: #ffffff;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2a2f3b;
            border: 1px solid #00d4ff;
        }
        QPushButton:pressed {
            background-color: #00d4ff;
            color: #000000;
        }
        QPushButton:focus {
            outline: 2px solid #00d4ff;
            outline-offset: 2px;
        }
        QComboBox {
            background-color: #1e2229;
            border: 1px solid #2a2f3b;
            border-radius: 4px;
            padding: 5px;
            color: #ffffff;
            min-width: 6em;
        }
        QComboBox:editable {
            background: #1e2229;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 15px;
            border-left-width: 1px;
            border-left-color: #2a2f3b;
            border-left-style: solid;
            border-top-right-radius: 3px;
            border-bottom-right-radius: 3px;
        }
        QComboBox QAbstractItemView {
            background-color: #1e2229;
            color: #ffffff;
            selection-background-color: #00d4ff;
            selection-color: #000000;
            border: 1px solid #2a2f3b;
        }
        QTextEdit {
            background-color: #1e2229;
            border: 1px solid #2a2f3b;
            border-radius: 4px;
            color: #ffffff;
            padding: 5px;
        }
        QRadioButton {
            color: #ffffff;
            spacing: 5px;
            padding: 4px;
        }
        QRadioButton::indicator {
            width: 13px;
            height: 13px;
        }
        QRadioButton::indicator:unchecked {
            border: 2px solid #2a2f3b;
            border-radius: 7px;
            background-color: #1e2229;
        }
        QRadioButton::indicator:checked {
            border: 2px solid #00d4ff;
            border-radius: 7px;
            background-color: #00d4ff;
        }
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        QScrollArea > QWidget > QWidget {
            background-color: transparent;
        }
        QScrollBar:vertical {
            border: none;
            background: #151a21;
            width: 10px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #2a2f3b;
            min-height: 20px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical:hover {
            background: #00d4ff;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QLabel {
            color: #8b9bb4;
        }
        QSplitter::handle {
            background-color: #2a2f3b;
        }
        QSplitter::handle:hover {
            background-color: #00d4ff;
        }
    )";

    qApp->setStyleSheet(styleSheet);

    // 设置窗口标题和大小
    setWindowTitle(QString::fromUtf8("上海地铁换乘指南系统"));
    setMinimumSize(1200, 800);

    // 创建中央部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 主布局
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // 使用QSplitter实现可调整的分割布局
    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    mainSplitter->setChildrenCollapsible(false);
    mainSplitter->setHandleWidth(2);

    // 左侧地铁图
    stationWidget = new StationWidget(this);
    mainSplitter->addWidget(stationWidget);

    // 右侧控制面板
    controlScrollArea = new QScrollArea(this);
    controlScrollArea->setWidgetResizable(true);
    controlScrollArea->setMaximumWidth(350);
    controlScrollArea->setFrameShape(QFrame::NoFrame);
    controlScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    controlPanel = new QWidget(this);
    QVBoxLayout* controlLayout = new QVBoxLayout(controlPanel);
    controlLayout->setAlignment(Qt::AlignTop);
    controlLayout->setContentsMargins(10, 10, 10, 10);
    controlLayout->setSpacing(15);

    // 起点选择
    QGroupBox* fromGroup = new QGroupBox(QString::fromUtf8("起点站"), this);
    QVBoxLayout* fromLayout = new QVBoxLayout(fromGroup);

    QHBoxLayout* fromButtonLayout = new QHBoxLayout();
    fromComboBox = new QComboBox(this);
    selectStartByLineButton = new QPushButton(QString::fromUtf8("按线路选择"), this);
    fromButtonLayout->addWidget(fromComboBox);
    fromButtonLayout->addWidget(selectStartByLineButton);
    fromLayout->addLayout(fromButtonLayout);

    controlLayout->addWidget(fromGroup);

    // 终点选择
    QGroupBox* toGroup = new QGroupBox(QString::fromUtf8("终点站"), this);
    QVBoxLayout* toLayout = new QVBoxLayout(toGroup);

    QHBoxLayout* toButtonLayout = new QHBoxLayout();
    toComboBox = new QComboBox(this);
    selectEndByLineButton = new QPushButton(QString::fromUtf8("按线路选择"), this);
    toButtonLayout->addWidget(toComboBox);
    toButtonLayout->addWidget(selectEndByLineButton);
    toLayout->addLayout(toButtonLayout);

    controlLayout->addWidget(toGroup);

    // 策略选择
    QGroupBox* strategyGroup = new QGroupBox(QString::fromUtf8("搜索策略"), this);
    QVBoxLayout* strategyLayout = new QVBoxLayout(strategyGroup);

    QRadioButton* minTransferRadio = new QRadioButton(QString::fromUtf8("换乘最少"), this);
    QRadioButton* minStationsRadio = new QRadioButton(QString::fromUtf8("经过站点最少"), this);
    QRadioButton* minDistanceRadio = new QRadioButton(QString::fromUtf8("路径长度最短"), this);

    minStationsRadio->setChecked(true); // 默认选择

    strategyLayout->addWidget(minTransferRadio);
    strategyLayout->addWidget(minStationsRadio);
    strategyLayout->addWidget(minDistanceRadio);

    controlLayout->addWidget(strategyGroup);

    // 创建按钮组
    strategyButtonGroup = new QButtonGroup(this);
    strategyButtonGroup->addButton(minTransferRadio, MIN_TRANSFER);
    strategyButtonGroup->addButton(minStationsRadio, MIN_STATIONS);
    strategyButtonGroup->addButton(minDistanceRadio, MIN_DISTANCE);

    // 查找按钮和清除按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    findPathButton = new QPushButton(QString::fromUtf8("查找路径"), this);
    findPathButton->setMinimumHeight(40);
    findPathButton->setStyleSheet("QPushButton { background-color: #00d4ff; color: #000000; font-size: 14px; }");

    clearButton = new QPushButton(QString::fromUtf8("清除"), this);
    clearButton->setMinimumHeight(40);

    buttonLayout->addWidget(findPathButton);
    buttonLayout->addWidget(clearButton);
    controlLayout->addLayout(buttonLayout);

    // 添加线路和站点按钮
    QHBoxLayout* addButtonLayout = new QHBoxLayout();
    addLineButton = new QPushButton(QString::fromUtf8("添加线路"), this);
    addStationButton = new QPushButton(QString::fromUtf8("添加站点"), this);

    addButtonLayout->addWidget(addLineButton);
    addButtonLayout->addWidget(addStationButton);
    controlLayout->addLayout(addButtonLayout);

    // 换乘指南
    QGroupBox* guideGroup = new QGroupBox(QString::fromUtf8("换乘指南"), this);
    QVBoxLayout* guideLayout = new QVBoxLayout(guideGroup);
    pathGuideText = new QTextEdit(this);
    pathGuideText->setMinimumHeight(200);
    guideLayout->addWidget(pathGuideText);
    controlLayout->addWidget(guideGroup, 1);

    controlScrollArea->setWidget(controlPanel);
    mainSplitter->addWidget(controlScrollArea);

    // 设置分割比例
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 1);

    QList<int> sizes;
    sizes << width() * 0.7 << width() * 0.3;
    mainSplitter->setSizes(sizes);

    mainLayout->addWidget(mainSplitter);

    // 创建状态栏
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    // 添加状态信息
    stationCountLabel = new QLabel(this);
    lineCountLabel = new QLabel(this);

    statusBar->addPermanentWidget(stationCountLabel);
    statusBar->addPermanentWidget(lineCountLabel);

    // 初始化状态栏
    updateStatusBar();

    // 连接信号槽
    connect(fromComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onFromStationSelected);
    connect(toComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onToStationSelected);
    connect(findPathButton, &QPushButton::clicked, this, &MainWindow::onFindPathClicked);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(addLineButton, &QPushButton::clicked, this, &MainWindow::onAddLineClicked);
    connect(addStationButton, &QPushButton::clicked, this, &MainWindow::onAddStationClicked);
    connect(stationWidget, &StationWidget::stationSelected, this, &MainWindow::onStationClicked);
    connect(strategyButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
        this, &MainWindow::onStrategyChanged);
    connect(stationWidget, &StationWidget::positionSelected,
        this, &MainWindow::onAddStationAtPosition);
    connect(selectStartByLineButton, &QPushButton::clicked, this, &MainWindow::onSelectStartByLine);
    connect(selectEndByLineButton, &QPushButton::clicked, this, &MainWindow::onSelectEndByLine);

    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    // 添加状态信息
    stationCountLabel = new QLabel(this);
    lineCountLabel = new QLabel(this);
    mousePosLabel = new QLabel(this); // 新增：鼠标位置标签

    statusBar->addPermanentWidget(stationCountLabel);
    statusBar->addPermanentWidget(lineCountLabel);
    statusBar->addPermanentWidget(mousePosLabel); // 添加到状态栏
    QLabel* hintLabel = new QLabel(QString::fromUtf8("提示: 右键点击地图空白处添加站点"), this);
    statusBar->addWidget(hintLabel);

    // 初始化状态栏
    updateStatusBar();
    mousePosLabel->setText("X: 0, Y: 0"); // 初始文本

}

void MainWindow::updateStatusBar() {
    stationCountLabel->setText(QString::fromUtf8("站点数量: %1").arg(metroGraph.getStations().size()));
    lineCountLabel->setText(QString::fromUtf8("线路数量: %1").arg(metroGraph.getLines().size()));
}

void MainWindow::onStrategyChanged(QAbstractButton* button) {
    selectedStrategy = static_cast<SearchStrategy>(strategyButtonGroup->id(button));
}

void MainWindow::loadSortedStations() {
    // 读取拼音排序后的站点列表
    QString basePath = QCoreApplication::applicationDirPath();
    QString dataPath = basePath + "/data/";
    QFile file(dataPath+"sorted_stations.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开排序后的站点文件";
        // 使用原始顺序
        QVector<QString> stationNames = metroGraph.getStationNames();
        for (const QString& name : stationNames) {
            fromComboBox->addItem(name);
            toComboBox->addItem(name);
        }
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qWarning() << "无效的排序站点JSON文件";
        return;
    }

    QJsonArray sortedStations = doc.array();
    for (const QJsonValue& value : sortedStations) {
        QString stationName = value.toString();
        fromComboBox->addItem(stationName);
        toComboBox->addItem(stationName);
    }

    qDebug() << "已加载" << sortedStations.size() << "个排序后的站点";
}

// 修改loadMetroData方法，确保搜索框支持输入和自动补全
void MainWindow::loadMetroData() {
    QString basePath = QCoreApplication::applicationDirPath();
    QString dataPath = basePath + "/data/";
    if (metroGraph.loadFromJson(dataPath+"metroInfo.json")) {
        stationWidget->setMetroGraph(metroGraph);

        // 更新PathFinder中的图指针
        pathFinder.setGraph(&metroGraph);

        connect(stationWidget, &StationWidget::mousePositionChanged,
            this, &MainWindow::onMousePositionChanged);

        // 获取所有站点名称
        QVector<QString> stationNames = metroGraph.getStationNames();
        qDebug() << "站点总数:" << stationNames.size();

        // 清空下拉框
        fromComboBox->clear();
        toComboBox->clear();

        // 创建自动补全模型
        QStringList stationList;
        for (const QString& name : stationNames) {
            stationList << name;
            fromComboBox->addItem(name);
            toComboBox->addItem(name);
        }

        // 设置自动补全
        QCompleter* fromCompleter = new QCompleter(stationList, this);
        fromCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        fromCompleter->setFilterMode(Qt::MatchContains);
        fromComboBox->setCompleter(fromCompleter);
        fromComboBox->setEditable(true);

        QCompleter* toCompleter = new QCompleter(stationList, this);
        toCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        toCompleter->setFilterMode(Qt::MatchContains);
        toComboBox->setCompleter(toCompleter);
        toComboBox->setEditable(true);

        // 连接编辑完成的信号
        connect(fromComboBox->lineEdit(), &QLineEdit::editingFinished, this, [this]() {
            QString text = fromComboBox->currentText();
            if (!text.isEmpty()) {
                fromComboBox->setCurrentText(text);
                selectedFromStation = text;
            }
            });

        connect(toComboBox->lineEdit(), &QLineEdit::editingFinished, this, [this]() {
            QString text = toComboBox->currentText();
            if (!text.isEmpty()) {
                toComboBox->setCurrentText(text);
                selectedToStation = text;
            }
            });
        updateStatusBar();
    }
    else {
        QMessageBox::critical(this, QString::fromUtf8("错误"), QString::fromUtf8("无法加载地铁数据文件"));
    }
}

void MainWindow::onFromStationSelected(const QString& station) {
    selectedFromStation = station;
}

void MainWindow::onToStationSelected(const QString& station) {
    selectedToStation = station;
}

void MainWindow::onFindPathClicked() {
    if (selectedFromStation.isEmpty() || selectedToStation.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请选择起点和终点站"));
        return;
    }

    if (selectedFromStation == selectedToStation) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("起点和终点相同"));
        return;
    }

    // 检查站点是否存在
    if (!metroGraph.hasStation(selectedFromStation)) {
        QMessageBox::warning(this, QString::fromUtf8("错误"),
            QString::fromUtf8("起点站 '%1' 不存在").arg(selectedFromStation));
        return;
    }

    if (!metroGraph.hasStation(selectedToStation)) {
        QMessageBox::warning(this, QString::fromUtf8("错误"),
            QString::fromUtf8("终点站 '%1' 不存在").arg(selectedToStation));
        return;
    }

    MetroPath path = pathFinder.findPath(selectedFromStation, selectedToStation, selectedStrategy);
    stationWidget->setPath(path);
    updatePathGuide(path);
}

void MainWindow::onStationClicked(const QString& station) {
    // 交替设置起点和终点
    if (selectedFromStation.isEmpty() || selectedToStation == station) {
        selectedFromStation = station;
        fromComboBox->setCurrentText(station);
    }
    else {
        selectedToStation = station;
        toComboBox->setCurrentText(station);
    }
}

void MainWindow::updatePathGuide(const MetroPath& path) {
    if (path.segments.isEmpty()) {
        pathGuideText->setPlainText(QString::fromUtf8("无法找到路径"));
        return;
    }

    // 检查路径数据是否有效
    if (path.stationCount <= 0) {
        pathGuideText->setPlainText(QString::fromUtf8("路径数据无效"));
        return;
    }

    QString guide = QString::fromUtf8("从 %1 到 %2 的换乘指南:\n\n").arg(selectedFromStation).arg(selectedToStation);

    // 根据策略显示不同的信息
    switch (selectedStrategy) {
        case MIN_TRANSFER:
            guide += QString::fromUtf8("共经过 %1 站, 换乘 %2 次\n\n").arg(path.stationCount).arg(path.transferCount);
            break;
        case MIN_STATIONS:
            guide += QString::fromUtf8("共经过 %1 站, 换乘 %2 次\n\n").arg(path.stationCount).arg(path.transferCount);
            break;
        case MIN_DISTANCE:
            guide += QString::fromUtf8("共经过 %1 站, 换乘 %2 次, 总距离 %3 公里\n\n")
                .arg(path.stationCount)
                .arg(path.transferCount)
                .arg(QString::number(path.totalDistance, 'f', 2));
            break;
    }

    for (int i = 0; i < path.segments.size(); i++) {
        const PathSegment& segment = path.segments[i];

        if (i == 0) {
            guide += QString::fromUtf8("从 %1 站乘坐 %2 \n").arg(segment.from).arg(segment.line);
        }
        else {
            guide += QString::fromUtf8("在 %1 站换乘 %2 \n").arg(segment.from).arg(segment.line);
        }

        guide += QString::fromUtf8("经过: ");
        for (int j = 0; j < segment.stations.size(); j++) {
            if (j > 0) guide += QString::fromUtf8(" → ");
            guide += segment.stations[j];
        }
        guide += QString::fromUtf8("\n\n");
    }

    guide += QString::fromUtf8("到达终点站: %1").arg(selectedToStation);
    pathGuideText->setPlainText(guide);
}

void MainWindow::onClearClicked() {
    // 清除搜索框
    fromComboBox->setCurrentIndex(-1);
    toComboBox->setCurrentIndex(-1);

    // 清除路径指南
    pathGuideText->clear();

    // 清除地图上的高亮
    MetroPath emptyPath;
    stationWidget->setPath(emptyPath);

    // 重置选择的站点
    selectedFromStation = "";
    selectedToStation = "";
}

// MainWindow.cpp - 修改refreshUI方法
void MainWindow::refreshUI() {
    // 更新下拉框
    fromComboBox->clear();
    toComboBox->clear();

    // 获取所有站点名称并按拼音排序
    QVector<QString> stationNames = metroGraph.getStationNames();

    // 使用QCollator进行中文拼音排序
    QCollator collator;
    collator.setNumericMode(false);
    std::sort(stationNames.begin(), stationNames.end(), [&collator](const QString& a, const QString& b) {
        return collator.compare(a, b) < 0;
        });

    // 添加站点到下拉框
    for (const QString& name : stationNames) {
        fromComboBox->addItem(name);
        toComboBox->addItem(name);
    }

    // 更新自动补全
    QStringList stationList;
    for (const QString& name : stationNames) {
        stationList << name;
    }

    QCompleter* fromCompleter = new QCompleter(stationList, this);
    fromCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    fromCompleter->setFilterMode(Qt::MatchContains);
    fromComboBox->setCompleter(fromCompleter);

    QCompleter* toCompleter = new QCompleter(stationList, this);
    toCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    toCompleter->setFilterMode(Qt::MatchContains);
    toComboBox->setCompleter(toCompleter);

    // 更新地铁图
    stationWidget->setMetroGraph(metroGraph);

    // 更新路径查找器
    pathFinder.setGraph(&metroGraph);

    // 更新状态栏
    updateStatusBar();
}

void MainWindow::onAddLineClicked() {
    AddLineDialog dialog(metroGraph, this);
    if (dialog.exec() == QDialog::Accepted) {
        // 获取线路信息
        MetroLine line = dialog.getLine();

        // 获取站点和连接信息
        QVector<QPair<QString, QVector<QPoint>>> stationsAndConnections = dialog.getStationsAndConnections();

        // 添加线路
        if (metroGraph.addLine(line)) {
            // 添加连接
            QVector<QString> selectedStations;
            for (int i = 0; i < dialog.selectedStationsList->count(); i++) {
                selectedStations.append(dialog.selectedStationsList->item(i)->text());
            }

            for (int i = 0; i < selectedStations.size() - 1; i++) {
                QString from = selectedStations[i];
                QString to = selectedStations[i + 1];

                // 获取转折点信息
                QVector<QPoint> viaPoints;
                if (dialog.connectionTypeCombo->currentIndex() == 1) { // 折线连接
                    for (int j = 0; j < dialog.viaPointsList->count(); j++) {
                        QString viaText = dialog.viaPointsList->item(j)->text();
                        // 解析坐标，格式为 "(x, y)"
                        viaText = viaText.mid(1, viaText.length() - 2); // 去掉括号
                        QStringList coords = viaText.split(", ");
                        if (coords.size() == 2) {
                            int x = coords[0].toInt();
                            int y = coords[1].toInt();
                            viaPoints.append(QPoint(x, y));
                        }
                    }
                }

                if (!metroGraph.addConnection(from, to, line.name, viaPoints)) {
                    QMessageBox::warning(this, QString::fromUtf8("警告"),
                        QString::fromUtf8("无法添加连接: %1 -> %2").arg(from).arg(to));
                }
            }

            // 刷新UI
            refreshUI();

            QMessageBox::information(this, QString::fromUtf8("成功"),
                QString::fromUtf8("已添加线路: %1").arg(line.name));
        }
        else {
            QMessageBox::warning(this, QString::fromUtf8("警告"),
                QString::fromUtf8("无法添加线路: %1").arg(line.name));
        }
    }
}

// MainWindow.cpp - 修改onAddStationClicked方法
void MainWindow::onAddStationClicked() {
    AddStationDialog dialog(stationWidget, this);

    // 确保对话框居中显示
    dialog.move(this->geometry().center() - dialog.rect().center());

    if (dialog.exec() == QDialog::Accepted) {
        // 获取站点信息
        QString name = dialog.getStationName();
        QPoint position = dialog.getStationPosition();
        QString tag = dialog.getStationTag();
        QString type = dialog.getStationType();

        // 创建站点
        Station station;
        station.name = name;
        station.graphPosition = position;
        station.realPosition = QPointF(0, 0); // 暂时使用默认值
        station.tag = tag;
        station.type = type;

        // 添加站点
        if (metroGraph.addStation(station)) {
            // 刷新UI
            refreshUI();

            QMessageBox::information(this, QString::fromUtf8("成功"),
                QString::fromUtf8("已添加站点: %1").arg(name));
        }
        else {
            QMessageBox::warning(this, QString::fromUtf8("警告"),
                QString::fromUtf8("无法添加站点: %1").arg(name));
        }
    }
}

void MainWindow::onMousePositionChanged(const QPoint& pos) {
    mousePosLabel->setText(QString("X: %1, Y: %2").arg(pos.x()).arg(pos.y()));
}


void MainWindow::onAddStationAtPosition(const QPoint& position) {
    // 创建简化的添加站点对话框
    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("添加站点"));
    dialog.setMinimumWidth(300);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // 站点名称输入
    QHBoxLayout* nameLayout = new QHBoxLayout();
    QLabel* nameLabel = new QLabel(QString::fromUtf8("站点名称:"), &dialog);
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    layout->addLayout(nameLayout);

    // 显示坐标信息（只读）
    QHBoxLayout* posLayout = new QHBoxLayout();
    QLabel* posLabel = new QLabel(QString::fromUtf8("位置坐标:"), &dialog);
    QLabel* posValue = new QLabel(QString("X: %1, Y: %2").arg(position.x()).arg(position.y()), &dialog);
    posLayout->addWidget(posLabel);
    posLayout->addWidget(posValue);
    layout->addLayout(posLayout);

    // 按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* okButton = new QPushButton(QString::fromUtf8("确定"), &dialog);
    QPushButton* cancelButton = new QPushButton(QString::fromUtf8("取消"), &dialog);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    // 连接信号
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // 设置焦点到名称输入框
    nameEdit->setFocus();

    // 显示对话框
    if (dialog.exec() == QDialog::Accepted && !nameEdit->text().isEmpty()) {
        // 创建站点
        Station station;
        station.name = nameEdit->text();
        station.graphPosition = position;
        station.realPosition = QPointF(0, 0); // 暂时使用默认值
        station.tag = "right"; // 默认标签位置
        station.type = "normal"; // 默认类型

        // 添加站点
        if (metroGraph.addStation(station)) {
            // 刷新UI
            refreshUI();

            QMessageBox::information(this, QString::fromUtf8("成功"),
                QString::fromUtf8("已添加站点: %1").arg(station.name));
        }
        else {
            QMessageBox::warning(this, QString::fromUtf8("警告"),
                QString::fromUtf8("无法添加站点: %1").arg(station.name));
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape && stationWidget) {
        // ESC键退出选择模式
        stationWidget->setSelectionMode(false);
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::onSelectStartByLine() {
    LineStationDialog dialog(metroGraph, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString station = dialog.getSelectedStation();
        fromComboBox->setCurrentText(station);
        selectedFromStation = station;
    }
}

void MainWindow::onSelectEndByLine() {
    LineStationDialog dialog(metroGraph, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString station = dialog.getSelectedStation();
        toComboBox->setCurrentText(station);
        selectedToStation = station;
    }
}