#ifndef STATIONWIDGET_H
#define STATIONWIDGET_H

#include <QWidget>
#include <QPainter>
#include "MetroGraph.h"
#include "PathFinder.h"

class StationWidget : public QWidget {
    Q_OBJECT
public:
    explicit StationWidget(QWidget* parent = nullptr);
    void setMetroGraph(const MetroGraph& graph);
    void setPath(const MetroPath& path);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

signals:
    void stationSelected(const QString& stationName);
    void positionSelected(const QPoint& position); // 确保这个信号存在
    void mousePositionChanged(const QPoint& graphPos); // 新增：鼠标位置变化信号
public slots:
    void setSelectionMode(bool enabled); // 新增：设置选择模式

private:
    const MetroGraph* metroGraph;
    MetroPath currentPath;
    QMap<QString, QPoint> stationPositions;
    double scale;
    QPoint offset;
    QPoint lastDragPos;
    bool isDragging;

    void drawStation(QPainter& painter, const Station& station, bool isHighlighted = false);
    void drawConnection(QPainter& painter, const StationConnection& conn, bool isHighlighted = false);
    void drawPath(QPainter& painter);
    void drawLegend(QPainter& painter);
    QPoint getStationPosition(const Station& station) const;
    QPoint toViewport(const QPoint& graphPoint) const;
    QPoint toGraph(const QPoint& viewportPoint) const;

    // 获取路径中的连接
    QVector<StationConnection> getPathConnections() const;

    QColor getStationLineColor(const QString& stationName) const;

    bool selectionMode; // 新增：是否处于选择模式
    QPoint lastMousePos; // 新增：最后鼠标位置

    QPoint rightClickPos; // 记录右键点击的位置
    bool showRightClickFeedback; // 是否显示右键点击反馈
    QTimer* feedbackTimer; // 反馈显示定时器
};

#endif // STATIONWIDGET_H