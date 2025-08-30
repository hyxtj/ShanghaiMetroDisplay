#include "StationWidget.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>
#include <QPainterpath>
#include <Qtimer>
// StationWidget.cpp - 修改构造函数
StationWidget::StationWidget(QWidget* parent)
    : QWidget(parent), metroGraph(nullptr), scale(1.0), offset(0, 0),
    isDragging(false), selectionMode(false), showRightClickFeedback(false) {
    setMouseTracking(true);

    // 创建定时器
    feedbackTimer = new QTimer(this);
    feedbackTimer->setSingleShot(true);
    connect(feedbackTimer, &QTimer::timeout, this, [this]() {
        showRightClickFeedback = false;
        update();
        });
}

void StationWidget::setSelectionMode(bool enabled) {
    selectionMode = enabled;
    if (enabled) {
        setCursor(Qt::CrossCursor); // 设置为十字准星光标
    }
    else {
        setCursor(Qt::ArrowCursor); // 恢复默认光标
    }
    update();
}


void StationWidget::setMetroGraph(const MetroGraph& graph) {
    metroGraph = &graph;
    stationPositions.clear();

    for (const Station& station : graph.getStations()) {
        stationPositions[station.name] = station.graphPosition;
    }

    update(); // 强制重绘
}

void StationWidget::setPath(const MetroPath& path) {
    currentPath = path;
    update();
}

void StationWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), QColor(240, 240, 240));

    // 检查metroGraph指针是否有效
    if (metroGraph == nullptr) {
        painter.drawText(rect(), Qt::AlignCenter, QString::fromUtf8("未加载地铁数据"));
        return;
    }

    // 应用缩放和平移
    painter.save();
    painter.translate(offset);
    painter.scale(scale, scale);

    // 首先绘制所有连接
    for (const StationConnection& conn : metroGraph->getConnections()) {
        drawConnection(painter, conn, false);
    }

    // 然后绘制路径（高亮显示）
    drawPath(painter);

    // 最后绘制所有站点
    for (const Station& station : metroGraph->getStations()) {
        bool isInPath = false;
        for (const PathSegment& segment : currentPath.segments) {
            if (segment.stations.contains(station.name)) {
                isInPath = true;
                break;
            }
        }
        drawStation(painter, station, isInPath);
    }

    painter.restore();

    // 绘制图例（在右下角）
    drawLegend(painter);

    if (showRightClickFeedback) {
        painter.save();
        QPoint viewportPos = toViewport(rightClickPos);

        // 绘制十字准星
        painter.setPen(QPen(Qt::red, 2));
        painter.drawLine(viewportPos.x() - 10, viewportPos.y(), viewportPos.x() + 10, viewportPos.y());
        painter.drawLine(viewportPos.x(), viewportPos.y() - 10, viewportPos.x(), viewportPos.y() + 10);

        // 绘制坐标文本
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 8));
        painter.drawText(viewportPos + QPoint(15, -5),
            QString("X: %1, Y: %2").arg(rightClickPos.x()).arg(rightClickPos.y()));

        painter.restore();
    }
}

// 绘制图例
void StationWidget::drawLegend(QPainter& painter) {
    if (metroGraph == nullptr) return;
    
	int maxNameLength = 0;
	for (const MetroLine& line : metroGraph->getLines()) {
		maxNameLength = qMax(maxNameLength, line.name.length());
	}

    // 设置图例位置和大小
    int legendWidth = maxNameLength*12 + 30;
    int legendHeight = metroGraph->getLines().size() * 15;
    int legendX = width() - legendWidth-5;
    int legendY = height() - legendHeight -10;

    // 绘制图例背景
    painter.setPen(Qt::black);
    painter.setBrush(QColor(0, 0,0, 50)); // 半透明黑色
    painter.drawRect(legendX, legendY, legendWidth, legendHeight);

    // 绘制每条线路的图例项
    int yOffset = 15;
    for (const MetroLine& line : metroGraph->getLines()) {

        /*绘制线路颜色*/
        QPen pen;
		pen.setWidth(3);
        pen.setColor(line.color);
        painter.setPen(pen);
		painter.drawLine(legendX + 10, legendY + yOffset + 1, legendX + 36, legendY + yOffset + 1);

        // 绘制线路名称
		pen.setColor(Qt::black);
        pen.setWidth(3);
        painter.setPen(pen);
        painter.setFont(QFont("Arial", 8));
        painter.drawText(legendX +40, legendY + yOffset+5 , line.name);

        yOffset += 14;
    }
}

// StationWidget.cpp - 修改换乘站识别方法
void StationWidget::drawStation(QPainter& painter, const Station& station, bool isHighlighted) {
    QPoint pos = getStationPosition(station);

    // 识别换乘站：检查站点是否连接了多条线路
    bool isTransferStation = false;
    QSet<QString> connectedLines; // 使用集合存储连接的线路

    // 获取站点连接的所有线路
    for (const StationConnection& conn : metroGraph->getConnections()) {
        if (conn.station1 == station.name || conn.station2 == station.name) {
            connectedLines.insert(conn.line);
        }
    }

    // 如果连接了多条线路，则是换乘站
    if (connectedLines.size() > 1) {
        isTransferStation = true;
    }

    // 绘制站点
    if (isTransferStation) {
        // 换乘站 - 先绘制白色背景圆
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(240, 240, 240)); // 使用与背景相同的颜色
        painter.drawEllipse(pos, 7, 7);

        // 再绘制灰色圆圈
        painter.setPen(QPen(Qt::gray, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(pos, 6, 6);

        // 绘制换乘标志（两个旋转箭头）
        painter.setPen(QPen(Qt::black, 1));
        QFont font = painter.font();
        font.setPointSize(6);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(QRect(pos.x() - 4, pos.y() - 4, 8, 8), Qt::AlignCenter, "⇄");
    }
    else {
        // 普通站 - 先绘制白色背景圆
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(240, 240, 240)); // 使用与背景相同的颜色
        painter.drawEllipse(pos, 6, 6);

        // 再使用线路颜色的空心圆
        QColor lineColor = getStationLineColor(station.name);
        painter.setPen(QPen(lineColor, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(pos, 5, 5);
    }

    // 绘制站点名称 - 使用黑色字体
    QFont font("Microsoft YaHei", 8);
    painter.setFont(font);
    painter.setPen(Qt::black);

    // 根据tag确定文本位置
    if (station.tag == QString::fromUtf8("left")) {
        painter.drawText(pos + QPoint(-60, 0), station.name);
    }
    else if (station.tag == QString::fromUtf8("right")) {
        painter.drawText(pos + QPoint(10, 0), station.name);
    }
    else if (station.tag == QString::fromUtf8("top")) {
        painter.drawText(pos + QPoint(-30, -10), station.name);
    }
    else if (station.tag == QString::fromUtf8("bottom")) {
        painter.drawText(pos + QPoint(-30, 20), station.name);
    }
    else if (station.tag == QString::fromUtf8("topleft")) {
        painter.drawText(pos + QPoint(-45, -10), station.name);
    }
    else {
        // 默认在右侧显示
        painter.drawText(pos + QPoint(10, 0), station.name);
    }
}

// StationWidget.cpp - 修改drawConnection方法
void StationWidget::drawConnection(QPainter& painter, const StationConnection& conn, bool isHighlighted) {
    QPoint fromPos = getStationPosition(metroGraph->getStation(conn.station1));
    QPoint toPos = getStationPosition(metroGraph->getStation(conn.station2));

    // 找到线路颜色
    QColor lineColor = QColor(100, 100, 100, 150); // 默认灰色
    for (const MetroLine& metroLine : metroGraph->getLines()) {
        if (metroLine.name == conn.line) {
            lineColor = metroLine.color;
            break;
        }
    }

    // 如果是高亮显示，使用更亮的颜色并加粗
    if (isHighlighted) {
        // 绘制阴影效果
        painter.setPen(QPen(QColor(0, 0, 0, 100), 7, Qt::SolidLine, Qt::RoundCap));
        if (conn.viaPoints.isEmpty()) {
            painter.drawLine(fromPos, toPos);
        }
        else {
            QPainterPath path;
            path.moveTo(fromPos);
            for (const QPoint& via : conn.viaPoints) {
                path.lineTo(via);
            }
            path.lineTo(toPos);
            painter.drawPath(path);
        }

        // 绘制高亮线路
        lineColor = QColor(255, 204, 0); // 使用金色高亮
        painter.setPen(QPen(lineColor, 5, Qt::SolidLine, Qt::RoundCap));
    }
    else {
        // 非高亮连接使用半透明
        lineColor.setAlpha(150);
        painter.setPen(QPen(lineColor, 3, Qt::SolidLine, Qt::RoundCap));
    }

    if (conn.viaPoints.isEmpty()) {
        // 直接连接
        painter.drawLine(fromPos, toPos);
    }
    else {
        // 有拐点的连接 - 绘制折线
        QVector<QPoint> sortedViaPoints = conn.viaPoints;

        // 计算每个转折点到起点的距离，用于排序
        auto distanceToStart = [fromPos](const QPoint& p) {
            return std::sqrt(std::pow(p.x() - fromPos.x(), 2) + std::pow(p.y() - fromPos.y(), 2));
            };

        // 按照距离起点的远近排序
        std::sort(sortedViaPoints.begin(), sortedViaPoints.end(),
            [&](const QPoint& a, const QPoint& b) {
                return distanceToStart(a) < distanceToStart(b);
            });

        // 绘制折线
        QPainterPath path;
        path.moveTo(fromPos);

        for (const QPoint& via : sortedViaPoints) {
            path.lineTo(via);
        }

        path.lineTo(toPos);
        painter.drawPath(path);
    }
}

// StationWidget.cpp - 修改drawPath方法
void StationWidget::drawPath(QPainter& painter) {
    if (currentPath.segments.isEmpty()) return;

    // 设置高亮颜色 - 使用鲜艳的红色
    QColor highlightColor(255, 50, 50);
    QColor glowColor(255, 100, 100, 200); // 半透明的红色用于发光效果

    // 获取路径中的所有连接
    QVector<StationConnection> pathConnections = getPathConnections();

    // 首先绘制发光效果（阴影）
    painter.setPen(QPen(glowColor, 7, Qt::SolidLine, Qt::RoundCap));
    for (const StationConnection& conn : pathConnections) {
        QPoint fromPos = getStationPosition(metroGraph->getStation(conn.station1));
        QPoint toPos = getStationPosition(metroGraph->getStation(conn.station2));

        if (conn.viaPoints.isEmpty()) {
            painter.drawLine(fromPos, toPos);
        }
        else {
            QVector<QPoint> sortedViaPoints = conn.viaPoints;

            // 计算每个转折点到起点的距离，用于排序
            auto distanceToStart = [fromPos](const QPoint& p) {
                return std::sqrt(std::pow(p.x() - fromPos.x(), 2) + std::pow(p.y() - fromPos.y(), 2));
                };

            // 按照距离起点的远近排序
            std::sort(sortedViaPoints.begin(), sortedViaPoints.end(),
                [&](const QPoint& a, const QPoint& b) {
                    return distanceToStart(a) < distanceToStart(b);
                });

            // 绘制折线
            QPainterPath path;
            path.moveTo(fromPos);

            for (const QPoint& via : sortedViaPoints) {
                path.lineTo(via);
            }

            path.lineTo(toPos);
            painter.drawPath(path);
        }
    }

    // 然后绘制高亮连接线
    painter.setPen(QPen(highlightColor, 3, Qt::SolidLine, Qt::RoundCap));
    for (const StationConnection& conn : pathConnections) {
        QPoint fromPos = getStationPosition(metroGraph->getStation(conn.station1));
        QPoint toPos = getStationPosition(metroGraph->getStation(conn.station2));

        if (conn.viaPoints.isEmpty()) {
            painter.drawLine(fromPos, toPos);
        }
        else {
            QVector<QPoint> sortedViaPoints = conn.viaPoints;

            // 计算每个转折点到起点的距离，用于排序
            auto distanceToStart = [fromPos](const QPoint& p) {
                return std::sqrt(std::pow(p.x() - fromPos.x(), 2) + std::pow(p.y() - fromPos.y(), 2));
                };

            // 按照距离起点的远近排序
            std::sort(sortedViaPoints.begin(), sortedViaPoints.end(),
                [&](const QPoint& a, const QPoint& b) {
                    return distanceToStart(a) < distanceToStart(b);
                });

            // 绘制折线
            QPainterPath path;
            path.moveTo(fromPos);

            for (const QPoint& via : sortedViaPoints) {
                path.lineTo(via);
            }

            path.lineTo(toPos);
            painter.drawPath(path);
        }
    }

    // 高亮显示路径上的站点
    for (const PathSegment& segment : currentPath.segments) {
        for (const QString& stationName : segment.stations) {
            if (metroGraph->hasStation(stationName)) {
                Station station = metroGraph->getStation(stationName);

                // 绘制高亮效果
                QPoint pos = getStationPosition(station);

                // 绘制发光效果
                QRadialGradient gradient(pos, 15);
                gradient.setColorAt(0, QColor(255, 100, 100, 200));
                gradient.setColorAt(1, QColor(255, 100, 100, 0));

                painter.setPen(Qt::NoPen);
                painter.setBrush(gradient);
                painter.drawEllipse(pos, 20, 20);

                // 先绘制白色背景圆
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(240, 240, 240)); // 使用与背景相同的颜色
                painter.drawEllipse(pos, 6, 6);

                // 绘制红色圆圈
                painter.setPen(QPen(highlightColor, 2));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(pos, 5, 5);

                // 如果是换乘站，添加换乘标志
                bool isTransferStation = false;
                if (station.type == "transfer" || station.type == "换乘站") {
                    isTransferStation = true;
                }
                else if (station.connectedStations.size() >= 3) {
                    isTransferStation = true;
                }
                else if (station.name.contains("换乘") || station.name.contains("Transfer")) {
                    isTransferStation = true;
                }

                if (isTransferStation) {
                    // 绘制换乘标志
                    painter.setPen(QPen(Qt::black, 1));
                    QFont font = painter.font();
                    font.setPointSize(6);
                    font.setBold(true);
                    painter.setFont(font);
                    painter.drawText(QRect(pos.x() - 4, pos.y() - 4, 8, 8), Qt::AlignCenter, "⇄");
                }

                // 绘制站点名称
                QFont font("Microsoft YaHei", 8);
                painter.setFont(font);
                painter.setPen(Qt::black);

                // 根据tag确定文本位置
                if (station.tag == QString::fromUtf8("left")) {
                    painter.drawText(pos + QPoint(-60, 0), station.name);
                }
                else if (station.tag == QString::fromUtf8("right")) {
                    painter.drawText(pos + QPoint(10, 0), station.name);
                }
                else if (station.tag == QString::fromUtf8("top")) {
                    painter.drawText(pos + QPoint(-30, -10), station.name);
                }
                else if (station.tag == QString::fromUtf8("bottom")) {
                    painter.drawText(pos + QPoint(-30, 20), station.name);
                }
                else if (station.tag == QString::fromUtf8("topleft")) {
                    painter.drawText(pos + QPoint(-45, -10), station.name);
                }
                else {
                    painter.drawText(pos + QPoint(10, 0), station.name);
                }
            }
        }
    }
}

QPoint StationWidget::getStationPosition(const Station& station) const {
    return station.graphPosition;
}

void StationWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 开始拖拽
        isDragging = true;
        lastDragPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    else if (event->button() == Qt::RightButton) {
        // 将鼠标位置转换为图上的坐标
        QPoint scenePos = toGraph(event->pos());

        // 查找最近的站点
        double minDist = 20.0 / scale;
        QString selectedStation;

        for (const Station& station : metroGraph->getStations()) {
            QPoint stationPos = getStationPosition(station);
            double dist = std::sqrt(std::pow(stationPos.x() - scenePos.x(), 2) +
                std::pow(stationPos.y() - scenePos.y(), 2));

            if (dist < minDist) {
                minDist = dist;
                selectedStation = station.name;
            }
        }

        if (!selectedStation.isEmpty()) {
            emit stationSelected(selectedStation);
        }
        else {
            // 如果没有选中站点，发送位置选择信号
            emit positionSelected(scenePos);
        }
    }

    QWidget::mousePressEvent(event);
}

void StationWidget::mouseMoveEvent(QMouseEvent* event) {
    // 记录鼠标位置并发送信号
    lastMousePos = toGraph(event->pos());
    emit mousePositionChanged(lastMousePos);

    if (isDragging) {
        QPoint delta = event->pos() - lastDragPos;
        offset += delta;
        lastDragPos = event->pos();
        update();
    }

    QWidget::mouseMoveEvent(event);
}


void StationWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && isDragging) {
        isDragging = false;
        setCursor(Qt::ArrowCursor);
    }

    QWidget::mouseReleaseEvent(event);
}

void StationWidget::wheelEvent(QWheelEvent* event) {
    // 获取鼠标位置
    QPoint mousePos = event->position().toPoint();

    // 计算缩放前的图坐标
    QPoint graphPosBefore = toGraph(mousePos);

    // 计算缩放因子
    double zoomFactor = 1.1;
    if (event->angleDelta().y() < 0) {
        zoomFactor = 1.0 / zoomFactor;
    }

    // 应用缩放
    scale *= zoomFactor;

    // 限制缩放范围
    scale = qMax(0.1, qMin(scale, 5.0));

    // 计算缩放后的图坐标
    QPoint graphPosAfter = toGraph(mousePos);

    // 调整偏移量以保持鼠标位置不变
    offset += (graphPosAfter - graphPosBefore) * scale;

    update();

    event->accept();
}

QPoint StationWidget::toViewport(const QPoint& graphPoint) const {
    return graphPoint * scale + offset;
}

QPoint StationWidget::toGraph(const QPoint& viewportPoint) const {
    return (viewportPoint - offset) / scale;
}

QVector<StationConnection> StationWidget::getPathConnections() const {
    QVector<StationConnection> pathConnections;

    if (currentPath.segments.isEmpty()) return pathConnections;

    // 收集路径中所有相邻站点对
    QVector<QPair<QString, QString>> stationPairs;
    for (const PathSegment& segment : currentPath.segments) {
        for (int i = 1; i < segment.stations.size(); i++) {
            stationPairs.append(qMakePair(segment.stations[i - 1], segment.stations[i]));
        }
    }

    // 为每对站点找到对应的连接
    for (const auto& pair : stationPairs) {
        StationConnection conn = metroGraph->getConnection(pair.first, pair.second);
        if (!conn.station1.isEmpty()) {
            pathConnections.append(conn);
        }
    }

    return pathConnections;
}

QColor StationWidget::getStationLineColor(const QString& stationName) const {
    if (!metroGraph) return Qt::black;

    // 获取站点所在的第一个线路的颜色
    for (const StationConnection& conn : metroGraph->getConnections()) {
        if (conn.station1 == stationName || conn.station2 == stationName) {
            for (const MetroLine& metroLine : metroGraph->getLines()) {
                if (metroLine.name == conn.line) {
                    return metroLine.color;
                }
            }
            break;
        }
    }

    return Qt::black;
}
