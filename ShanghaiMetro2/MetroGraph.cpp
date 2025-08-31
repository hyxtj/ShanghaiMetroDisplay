/***************************************************************************
  文件名称：MetroGraph.cpp
  功    能：地铁图数据模型的实现文件
  说    明：实现地铁线路、站点和连接的数据管理和文件操作功能
***************************************************************************/

#include "MetroGraph.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QPair>
/***************************************************************************
  函数名称：MetroGraph::MetroGraph
  功    能：构造函数，初始化地铁图数据
  输入参数：
  返 回 值：
  说    明：初始化空的地铁图数据结构
***************************************************************************/
MetroGraph::MetroGraph() {
}

/***************************************************************************
  函数名称：MetroGraph::~MetroGraph
  功    能：析构函数
  输入参数：
  返 回 值：
  说    明：清除各个数据结构，释放资源
***************************************************************************/
MetroGraph::~MetroGraph() {
    /* 确保在析构时清理资源*/
    lines.clear();
    stations.clear();
    connections.clear();
    stationMap.clear();
    connectionMap.clear();
}

/***************************************************************************
  函数名称：MetroGraph::loadFromJson
  功    能：从Json文件加载地铁图数据
  输入参数：const QString& filename - 地铁数据文件名
  返 回 值：bool 是否正确加载
  说    明：
***************************************************************************/
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
        parseStations   (root[QString::fromUtf8("stations")].toArray());
        parseConnections(root[QString::fromUtf8("stations")].toArray());
        buildStationMap(); // 构建站点映射
    }

    qDebug() << "加载完成: " << stations.size() << "个站点, " << connections.size() << "个连接";
    return true;
}

/***************************************************************************
  函数名称：MetroGraph::parseLines
  功    能：构造函数，初始化地铁图数据
  输入参数：const QJsonArray& linesArray - 线路数据数组
  返 回 值：
  说    明：
***************************************************************************/
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

/***************************************************************************
  函数名称：MetroGraph::parseStation
  功    能：解析站点信息
  输入参数：const QJsonArray& stationsArray - 站点信息Json数组
  返 回 值：
  说    明：
***************************************************************************/
void MetroGraph::parseStations(const QJsonArray& stationsArray) {
    stations.clear();

    for (const QJsonValue& value : stationsArray) {
        QJsonObject obj = value.toObject();
        Station station;
        station.name = obj[QString::fromUtf8("name")].toString();
        station.tag  = obj[QString::fromUtf8("tag") ].toString();
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

/***************************************************************************
  函数名称：MetroGraph::buildStationMap
  功    能：构建站点名称到站点对象的映射
  输入参数：
  返 回 值：
  说    明：用于快速查找站点信息
***************************************************************************/
void MetroGraph::buildStationMap() {
    stationMap.clear();
    for (const Station& station : stations) {
        stationMap[station.name] = station;
    }
    qDebug() << "构建站点映射: " << stationMap.size() << "个站点";
}

/***************************************************************************
  函数名称：MetroGraph::parseConnections
  功    能：解析站点连接状态
  输入参数：const QJsonArray& stationsArray - 站点数据Json数组
  返 回 值：
  说    明：
***************************************************************************/
void MetroGraph::parseConnections(const QJsonArray& stationsArray) {
    connections.clear();
    connectionMap.clear();
    QSet<QPair<QString, QString>> addedConnections;

    qDebug() << "开始解析连接信息";

    /* 首先构建完整的站点映射*/
    buildStationMap();

    for (const QJsonValue& value : stationsArray) {
        QJsonObject obj = value.toObject();
        QString fromStation = obj[QString::fromUtf8("name")].toString();

        /* 检查站点是否存在*/
        if (!stationMap.contains(fromStation)) {
            qDebug() << "警告: 站点" << fromStation << "不存在于站点映射中";
            continue;
        }

        /* 解析连接信息*/
        if (obj.contains(QString::fromUtf8("edges")) && obj[QString::fromUtf8("edges")].isArray()) {
            QJsonArray edgesArray = obj[QString::fromUtf8("edges")].toArray();

            for (const QJsonValue& edgeValue : edgesArray) {
                QJsonObject edgeObj   = edgeValue.toObject();
                QString     toStation = edgeObj[QString::fromUtf8("to")  ].toString();
                QString     line      = edgeObj[QString::fromUtf8("line")].toString();

                /* 检查目标站点是否存在*/
                if (!stationMap.contains(toStation)) {
                    qDebug() << "警告: 目标站点" << toStation << "不存在于站点映射中";
                    continue;
                }

                /* 创建连接的键（按字母顺序排序，确保双向连接使用相同的键）*/
                QPair<QString, QString> connectionKey;
                if (fromStation < toStation) {
                    connectionKey = qMakePair(fromStation, toStation);
                }
                else {
                    connectionKey = qMakePair(toStation, fromStation);
                }

                /* 如果这个连接还没有被添加，或者需要更新转折点信息*/
                if (!addedConnections.contains(connectionKey)) {
                    StationConnection connection;
                    connection.station1 = connectionKey.first;
                    connection.station2 = connectionKey.second;
                    connection.line     = line;

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

                    /* 更新站点的连接信息*/
                    if (stationMap.contains(fromStation)) {
                        stationMap[fromStation].connectedStations.append(toStation);
                    }
                    if (stationMap.contains(toStation)) {
                        stationMap[toStation].connectedStations.append(fromStation);
                    }
                }
                else {
                    /* 如果连接已存在，检查是否需要更新转折点信息*/
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

                        /* 如果新的转折点信息更详细，则更新*/
                        if (viaPoints.size() > existingConnection.viaPoints.size()) {
                            existingConnection.viaPoints = viaPoints;
                        }
                    }
                }
            }
        }
    }

    /* 更新stations向量中的连接信息*/
    for (Station& station : stations) {
        if (stationMap.contains(station.name)) {
            station.connectedStations = stationMap[station.name].connectedStations;
        }
    }

    qDebug() << "解析完成，共添加" << connections.size() << "个连接";
}

/***************************************************************************
  函数名称：MetroGraph::getLines
  功    能：向外提供线路数据
  输入参数：
  返 回 值：QVector<MetroLine> 线路列表
  说    明：
***************************************************************************/
QVector<MetroLine> MetroGraph::getLines() const {
    return lines;
}

/***************************************************************************
  函数名称：MetroGraph::getStations
  功    能：向外提供站点数据
  输入参数：
  返 回 值：QVector<Station> 站点列表
  说    明：
***************************************************************************/
QVector<Station> MetroGraph::getStations() const {
    return stations;
}

/***************************************************************************
  函数名称：MetroGraph::getStation
  功    能：根据输入的站点名称返回站点信息
  输入参数：const QString &name - 站点名称
  返 回 值：Station 相应的站点信息
  说    明：查无此站点则返回空
***************************************************************************/
Station MetroGraph::getStation(const QString& name) const {
    if (stationMap.contains(name)) {
        return stationMap.value(name);
    }
    return Station(); // 返回空站点
}

/***************************************************************************
  函数名称：MetroGraph::hasStation
  功    能：根据输入站点名称返回线路图中是否有此站点
  输入参数：const QString &name - 站点名称
  返 回 值：bool 是否含有此站点
  说    明：
***************************************************************************/
bool MetroGraph::hasStation(const QString& name) const {
    return stationMap.contains(name);
}

/***************************************************************************
  函数名称：MetroGraph::getStationNames
  功    能：获取所有站点的名称
  输入参数：
  返 回 值：QVector<QString> 站点名称列表
  说    明：
***************************************************************************/
QVector<QString> MetroGraph::getStationNames() const {
    QVector<QString> names;
    for (const Station& station : stations) {
        names.append(station.name);
    }
    return names;
}

/***************************************************************************
  函数名称：MetroGraph::getConnections
  功    能：获取所有连接信息
  输入参数：
  返 回 值：QVector<StationConnection> 连接列表
  说    明：
***************************************************************************/
QVector<StationConnection> MetroGraph::getConnections() const {
    return connections;
}

/***************************************************************************
  函数名称：MetroGraph::getConnection
  功    能：获取两个站点之间的连接信息
  输入参数：const QString& station1 - 站点1 
            const QString& station2 - 站点2
  返 回 值：StationConnection 连接信息
  说    明：
***************************************************************************/
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

/***************************************************************************
  函数名称：MetroGraph::addLine
  功    能：添加新的线路
  输入参数：const MetroLine& line - 要添加的线路信息
  返 回 值：bool - 是否添加成功
  说    明：
***************************************************************************/
bool MetroGraph::addLine(const MetroLine& line) {
    /* 检查线路是否已存在*/
    for (const MetroLine& existingLine : lines) {
        if (existingLine.name == line.name) {
            qWarning() << "线路已存在:" << line.name;
            return false;
        }
    }

    /* 添加新线路*/
    lines.append(line);
    qDebug() << "成功添加线路:" << line.name;
    return true;
}

/***************************************************************************
  函数名称：MetroGraph::addStation
  功    能：添加新的站点
  输入参数：const Station& station - 要添加的站点信息
  返 回 值：bool - 是否添加成功
  说    明：
***************************************************************************/
bool MetroGraph::addStation(const Station& station) {
    /* 检查站点是否已存在*/
    if (stationMap.contains(station.name)) {
        qWarning() << "站点已存在:" << station.name;
        return false;
    }

    /* 添加新站点*/
    stations.append(station);
    stationMap[station.name] = station;
    qDebug() << "成功添加站点:" << station.name;
    return true;
}

/***************************************************************************
  函数名称：MetroGraph::addConnection
  功    能：添加连接
  输入参数：const QString& station1          - 站点1名称
            const QString& station2          - 站点2名称
            const QString& line              - 所在线路名称
			const QVector<QPoint>& viaPoints - 连接的转折点列表
  返 回 值：bool - 是否添加成功
  说    明：
***************************************************************************/
bool MetroGraph::addConnection(const QString& station1, const QString& station2, const QString& line, const QVector<QPoint>& viaPoints) {
    /* 检查站点是否存在*/
    if (!stationMap.contains(station1) || !stationMap.contains(station2)) {
        qWarning() << "站点不存在:" << station1 << "或" << station2;
        return false;
    }

    /* 检查线路是否存在*/
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

    /* 创建连接的键（按字母顺序排序，确保双向连接使用相同的键）*/
    QPair<QString, QString> connectionKey;
    if (station1 < station2) {
        connectionKey = qMakePair(station1, station2);
    }
    else {
        connectionKey = qMakePair(station2, station1);
    }

    /* 检查连接是否已存在*/
    if (connectionMap.contains(connectionKey)) {
        qWarning() << "连接已存在:" << station1 << "<->" << station2;
        return false;
    }

    /* 创建新连接*/
    StationConnection connection;
    connection.station1  = connectionKey.first;
    connection.station2  = connectionKey.second;
    connection.line      = line;
    connection.viaPoints = viaPoints;

    /* 添加到连接列表和映射*/
    connections.append(connection);
    connectionMap[connectionKey] = connection;

    // 更新站点的连接信息
    stationMap[station1].connectedStations.append(station2);
    stationMap[station2].connectedStations.append(station1);

    qDebug() << "成功添加连接:" << station1 << "<->" << station2 << "线路:" << line;
    return true;
}

/***************************************************************************
  函数名称：MetroGraph::getStationLines
  功    能：获取每个站点所属的线路
  输入参数：
  返 回 值：QMap<QString, QVector<QString>> 站点到线路的映射
  说    明：
  ***************************************************************************/
QMap<QString, QVector<QString>> MetroGraph::getLineStations() const {
    QMap<QString, QVector<QString>> lineStations;

    // 遍历所有连接，构建线路到站点的映射
    for (const StationConnection& conn : connections) {
        if (!lineStations[conn.line].contains(conn.station1)) {
            lineStations[conn.line].append(conn.station1);
        }
        if (!lineStations[conn.line].contains(conn.station2)) {
            lineStations[conn.line].append(conn.station2);
        }
    }

    return lineStations;
}
/*MetroGraph.cpp*/