#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "MetroGraph.h"
#include <QVector>
#include <QString>
#include <QMap>
#include <queue>
#include <functional>
#include <QSet>

// 搜索策略枚举
enum SearchStrategy {
    MIN_TRANSFER,    // 换乘最少
    MIN_STATIONS,    // 经过站点最少
    MIN_DISTANCE     // 路径长度最短
};

// 路径段信息
struct PathSegment {
    QString line;
    QString from;
    QString to;
    QVector<QString> stations;
};

// 完整路径信息
struct MetroPath {
    QVector<PathSegment> segments;
    int transferCount;
    int stationCount;
    double totalDistance;
};

// 路径查找器
class PathFinder {
public:
    PathFinder(const MetroGraph* graph);
    MetroPath findPath(const QString& from, const QString& to, SearchStrategy strategy);
    void setGraph(const MetroGraph* graph);

private:
    const MetroGraph* graph;

    // 三种搜索策略的具体实现
    MetroPath findMinTransferPath(const QString& from, const QString& to);
    MetroPath findMinStationsPath(const QString& from, const QString& to);
    MetroPath findMinDistancePath(const QString& from, const QString& to);

    // 辅助函数
    QVector<QString> bfsShortestPath(const QString& from, const QString& to);
    QVector<QString> dijkstraShortestPath(const QString& from, const QString& to);
    MetroPath buildPath(const QVector<QString>& stationNames);
    QString getLineBetweenStations(const QString& station1, const QString& station2) const;
    double calculateDistance(const QString& station1, const QString& station2);

    // 最少换乘算法的辅助函数
    QMap<QString, QVector<QString>> getStationLines() const;
    QMap<QString, QVector<QString>> getLineStations() const;

    // 新增方法
    QVector<QString> convertLinePathToStationPath(const QVector<QString>& linePath,
        const QString& from,
        const QString& to);
    QVector<QString> findPathOnLine(const QString& line, const QString& from, const QString& to);
    QVector<QString> findAlternativePathOnLine(const QString& line, const QString& from,
        const QString& to, const QString& branchPoint);

};

#endif // PATHFINDER_H