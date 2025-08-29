#include "MetroGraph.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QPair>

MetroGraph::MetroGraph() {}

MetroGraph::~MetroGraph() {
    // 确保在析构时清理资源
    lines.clear();
    stations.clear();
    connections.clear();
    stationMap.clear();
    connectionMap.clear();
}

bool MetroGraph::loadFromJson(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << QString::fromUtf8("无法打开文件:") << filename;
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qWarning() << QString::fromUtf8("无效的JSON文件");
        return false;
    }

    QJsonObject root = doc.object();
    if (root.contains(QString::fromUtf8("lines")) && root[QString::fromUtf8("lines")].isArray()) {
        parseLines(root[QString::fromUtf8("lines")].toArray());
    }

    if (root.contains(QString::fromUtf8("stations")) && root[QString::fromUtf8("stations")].isArray()) {
        parseStations(root[QString::fromUtf8("stations")].toArray());
        parseConnections(root[QString::fromUtf8("stations")].toArray());
        buildStationMap(); // 构建站点映射
    }

    qDebug() << "加载完成: " << stations.size() << "个站点, " << connections.size() << "个连接";
    return true;
}

void MetroGraph::parseLines(const QJsonArray& linesArray) {
    lines.clear();
    for (const QJsonValue& value : linesArray) {
        QJsonObject obj = value.toObject();
        MetroLine line;
        line.name = obj[QString::fromUtf8("name")].toString();

        QJsonArray colorArray = obj[QString::fromUtf8("color")].toArray();
        if (colorArray.size() == 3) {
            line.color = QColor(colorArray[0].toInt(),
                colorArray[1].toInt(),
                colorArray[2].toInt());
        }
        else {
            line.color = Qt::black;
        }

        lines.append(line);
    }
}

void MetroGraph::parseStations(const QJsonArray& stationsArray) {
    stations.clear();

    for (const QJsonValue& value : stationsArray) {
        QJsonObject obj = value.toObject();
        Station station;
        station.name = obj[QString::fromUtf8("name")].toString();
        station.tag = obj[QString::fromUtf8("tag")].toString();
        station.type = obj[QString::fromUtf8("type")].toString();

        QJsonArray posArray = obj[QString::fromUtf8("graph-position")].toArray();
        if (posArray.size() == 2) {
            station.graphPosition = QPoint(posArray[0].toInt(), posArray[1].toInt());
        }

        QJsonArray realPosArray = obj[QString::fromUtf8("real-position")].toArray();
        if (realPosArray.size() == 2) {
            station.realPosition = QPointF(realPosArray[0].toDouble(), realPosArray[1].toDouble());
        }

        stations.append(station);
    }
}

void MetroGraph::buildStationMap() {
    stationMap.clear();
    for (const Station& station : stations) {
        stationMap[station.name] = station;
    }
    qDebug() << "构建站点映射: " << stationMap.size() << "个站点";
}

void MetroGraph::parseConnections(const QJsonArray& stationsArray) {
    connections.clear();
    connectionMap.clear();
    QSet<QPair<QString, QString>> addedConnections;

    qDebug() << "开始解析连接信息";

    // 首先构建完整的站点映射
    buildStationMap();

    for (const QJsonValue& value : stationsArray) {
        QJsonObject obj = value.toObject();
        QString fromStation = obj[QString::fromUtf8("name")].toString();

        // 检查站点是否存在
        if (!stationMap.contains(fromStation)) {
            qDebug() << "警告: 站点" << fromStation << "不存在于站点映射中";
            continue;
        }

        // 解析连接信息
        if (obj.contains(QString::fromUtf8("edges")) && obj[QString::fromUtf8("edges")].isArray()) {
            QJsonArray edgesArray = obj[QString::fromUtf8("edges")].toArray();

            for (const QJsonValue& edgeValue : edgesArray) {
                QJsonObject edgeObj = edgeValue.toObject();
                QString toStation = edgeObj[QString::fromUtf8("to")].toString();
                QString line = edgeObj[QString::fromUtf8("line")].toString();

                // 检查目标站点是否存在
                if (!stationMap.contains(toStation)) {
                    qDebug() << "警告: 目标站点" << toStation << "不存在于站点映射中";
                    continue;
                }

                // 创建连接的键（按字母顺序排序，确保双向连接使用相同的键）
                QPair<QString, QString> connectionKey;
                if (fromStation < toStation) {
                    connectionKey = qMakePair(fromStation, toStation);
                }
                else {
                    connectionKey = qMakePair(toStation, fromStation);
                }

                // 如果这个连接还没有被添加，或者需要更新转折点信息
                if (!addedConnections.contains(connectionKey)) {
                    StationConnection connection;
                    connection.station1 = connectionKey.first;
                    connection.station2 = connectionKey.second;
                    connection.line = line;

                    if (edgeObj.contains(QString::fromUtf8("via")) && edgeObj[QString::fromUtf8("via")].isArray()) {
                        QJsonArray viaArray = edgeObj[QString::fromUtf8("via")].toArray();
                        for (const QJsonValue& viaValue : viaArray) {
                            if (viaValue.isArray()) {
                                QJsonArray pointArray = viaValue.toArray();
                                if (pointArray.size() == 2) {
                                    connection.viaPoints.append(QPoint(pointArray[0].toInt(), pointArray[1].toInt()));
                                }
                            }
                        }
                    }

                    connections.append(connection);
                    connectionMap[connectionKey] = connection;
                    addedConnections.insert(connectionKey);

                    // 更新站点的连接信息
                    if (stationMap.contains(fromStation)) {
                        stationMap[fromStation].connectedStations.append(toStation);
                    }
                    if (stationMap.contains(toStation)) {
                        stationMap[toStation].connectedStations.append(fromStation);
                    }
                }
                else {
                    // 如果连接已存在，检查是否需要更新转折点信息
                    StationConnection& existingConnection = connectionMap[connectionKey];
                    if (edgeObj.contains(QString::fromUtf8("via")) && edgeObj[QString::fromUtf8("via")].isArray()) {
                        QJsonArray viaArray = edgeObj[QString::fromUtf8("via")].toArray();
                        QVector<QPoint> viaPoints;

                        for (const QJsonValue& viaValue : viaArray) {
                            if (viaValue.isArray()) {
                                QJsonArray pointArray = viaValue.toArray();
                                if (pointArray.size() == 2) {
                                    viaPoints.append(QPoint(pointArray[0].toInt(), pointArray[1].toInt()));
                                }
                            }
                        }

                        // 如果新的转折点信息更详细，则更新
                        if (viaPoints.size() > existingConnection.viaPoints.size()) {
                            existingConnection.viaPoints = viaPoints;
                        }
                    }
                }
            }
        }
    }

    // 更新stations向量中的连接信息
    for (Station& station : stations) {
        if (stationMap.contains(station.name)) {
            station.connectedStations = stationMap[station.name].connectedStations;
        }
    }

    qDebug() << "解析完成，共添加" << connections.size() << "个连接";
}

QVector<MetroLine> MetroGraph::getLines() const {
    return lines;
}

QVector<Station> MetroGraph::getStations() const {
    return stations;
}

Station MetroGraph::getStation(const QString& name) const {
    if (stationMap.contains(name)) {
        return stationMap.value(name);
    }
    return Station(); // 返回空站点
}

bool MetroGraph::hasStation(const QString& name) const {
    return stationMap.contains(name);
}

QVector<QString> MetroGraph::getStationNames() const {
    QVector<QString> names;
    for (const Station& station : stations) {
        names.append(station.name);
    }
    return names;
}

QVector<StationConnection> MetroGraph::getConnections() const {
    return connections;
}

StationConnection MetroGraph::getConnection(const QString& station1, const QString& station2) const {
    QPair<QString, QString> key;
    if (station1 < station2) {
        key = qMakePair(station1, station2);
    }
    else {
        key = qMakePair(station2, station1);
    }
    return connectionMap.value(key);
}

bool MetroGraph::addLine(const MetroLine& line) {
    // 检查线路是否已存在
    for (const MetroLine& existingLine : lines) {
        if (existingLine.name == line.name) {
            qWarning() << "线路已存在:" << line.name;
            return false;
        }
    }

    // 添加新线路
    lines.append(line);
    qDebug() << "成功添加线路:" << line.name;
    return true;
}

bool MetroGraph::addStation(const Station& station) {
    // 检查站点是否已存在
    if (stationMap.contains(station.name)) {
        qWarning() << "站点已存在:" << station.name;
        return false;
    }

    // 添加新站点
    stations.append(station);
    stationMap[station.name] = station;
    qDebug() << "成功添加站点:" << station.name;
    return true;
}

bool MetroGraph::addConnection(const QString& station1, const QString& station2, const QString& line, const QVector<QPoint>& viaPoints) {
    // 检查站点是否存在
    if (!stationMap.contains(station1) || !stationMap.contains(station2)) {
        qWarning() << "站点不存在:" << station1 << "或" << station2;
        return false;
    }

    // 检查线路是否存在
    bool lineExists = false;
    for (const MetroLine& metroLine : lines) {
        if (metroLine.name == line) {
            lineExists = true;
            break;
        }
    }

    if (!lineExists) {
        qWarning() << "线路不存在:" << line;
        return false;
    }

    // 创建连接的键（按字母顺序排序，确保双向连接使用相同的键）
    QPair<QString, QString> connectionKey;
    if (station1 < station2) {
        connectionKey = qMakePair(station1, station2);
    }
    else {
        connectionKey = qMakePair(station2, station1);
    }

    // 检查连接是否已存在
    if (connectionMap.contains(connectionKey)) {
        qWarning() << "连接已存在:" << station1 << "<->" << station2;
        return false;
    }

    // 创建新连接
    StationConnection connection;
    connection.station1 = connectionKey.first;
    connection.station2 = connectionKey.second;
    connection.line = line;
    connection.viaPoints = viaPoints;

    // 添加到连接列表和映射
    connections.append(connection);
    connectionMap[connectionKey] = connection;

    // 更新站点的连接信息
    stationMap[station1].connectedStations.append(station2);
    stationMap[station2].connectedStations.append(station1);

    qDebug() << "成功添加连接:" << station1 << "<->" << station2 << "线路:" << line;
    return true;
}