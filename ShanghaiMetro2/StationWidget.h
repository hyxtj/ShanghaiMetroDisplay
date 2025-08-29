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
};

#endif // STATIONWIDGET_H