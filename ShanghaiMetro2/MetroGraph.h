#ifndef METROGRAPH_H
#define METROGRAPH_H

#include <QString>
#include <QVector>
#include <QPoint>
#include <QColor>
#include <QMap>
#include <QJsonObject>
#include <QSet>

// 地铁线路信息
struct MetroLine {
    QString name;
    QColor color;
};

// 地铁站点连接信息
struct StationConnection {
    QString station1;
    QString station2;
    QString line;
    QVector<QPoint> viaPoints; // 连接线的拐点
};

// 地铁站点信息
struct Station {
    QString name;
    QString tag;
    QString type;
    QPoint graphPosition;
    QPointF realPosition;
    QVector<QString> connectedStations; // 连接的站点名称
};

// 地铁网络图
class MetroGraph {
public:
    MetroGraph();
    ~MetroGraph(); // 添加析构函数
    bool loadFromJson(const QString& filename);
    QVector<MetroLine> getLines() const;
    QVector<Station> getStations() const;
    Station getStation(const QString& name) const;
    QVector<QString> getStationNames() const;
    QVector<StationConnection> getConnections() const;
    StationConnection getConnection(const QString& station1, const QString& station2) const;
    bool hasStation(const QString& name) const; // 添加检查站点是否存在的方法
    // 在MetroGraph.h中添加以下方法声明
    bool addLine(const MetroLine& line);
    bool addStation(const Station& station);
    bool addConnection(const QString& station1, const QString& station2, const QString& line, const QVector<QPoint>& viaPoints = QVector<QPoint>());

private:
    QVector<MetroLine> lines;
    QVector<Station> stations;
    QVector<StationConnection> connections;
    QMap<QString, Station> stationMap; // 使用QMap而不是QVector来存储站点
    QMap<QPair<QString, QString>, StationConnection> connectionMap;

    void parseLines(const QJsonArray& linesArray);
    void parseStations(const QJsonArray& stationsArray);
    void parseConnections(const QJsonArray& stationsArray);
    void buildStationMap(); // 添加构建站点映射的方法
};

#endif // METROGRAPH_H