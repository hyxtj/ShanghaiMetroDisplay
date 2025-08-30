#include "PathFinder.h"
#include <queue>
#include <QSet>
#include <QMap>
#include <cmath>
#include <limits>
#include <QDebug>
#include <algorithm>

// 用于优先队列的比较函数
struct ComparePair {
    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
        if (a.first != b.first) return a.first > b.first;
        return a.second > b.second;
    }
};

PathFinder::PathFinder(const MetroGraph* graph) : graph(graph) {}

void PathFinder::setGraph(const MetroGraph* graph) {
    this->graph = graph;
}

MetroPath PathFinder::findPath(const QString& from, const QString& to, SearchStrategy strategy) {
    qDebug() << "开始搜索从" << from << "到" << to << "的策略:" << strategy;

    // 检查graph指针是否有效
    if (graph == nullptr) {
        qDebug() << "错误: graph指针为空";
        return MetroPath();
    }

    // 检查起点和终点是否存在
    if (!graph->hasStation(from)) {
        qDebug() << "错误: 起点" << from << "不存在";
        return MetroPath();
    }
    if (!graph->hasStation(to)) {
        qDebug() << "错误: 终点" << to << "不存在";
        return MetroPath();
    }

    switch (strategy) {
        case MIN_TRANSFER:
            return findMinTransferPath(from, to);
        case MIN_STATIONS:
            return findMinStationsPath(from, to);
        case MIN_DISTANCE:
            return findMinDistancePath(from, to);
        default:
            return findMinStationsPath(from, to);
    }
}

//MetroPath PathFinder::findMinTransferPath(const QString& from, const QString& to) {
//    qDebug() << "开始搜索最少换乘路径从" << from << "到" << to;
//
//    // 检查graph指针是否有效
//    if (graph == nullptr) {
//        qDebug() << "错误: graph指针为空";
//        return MetroPath();
//    }
//
//    // 检查起点和终点是否存在
//    if (!graph->hasStation(from)) {
//        qDebug() << "错误: 起点" << from << "不存在";
//        return MetroPath();
//    }
//    if (!graph->hasStation(to)) {
//        qDebug() << "错误: 终点" << to << "不存在";
//        return MetroPath();
//    }
//
//    // 获取所有线路和换乘站信息
//    QMap<QString, QVector<QString>> lineStations = getLineStations();
//    QMap<QString, QVector<QString>> transferStations; // 每条线路的换乘站
//
//    // 找出每条线路的换乘站
//    for (const auto& line : lineStations.keys()) {
//        for (const QString& station : lineStations[line]) {
//            Station stationInfo = graph->getStation(station);
//            if (stationInfo.connectedStations.size() > 2) {
//                // 如果一个站连接了超过2个站，可能是换乘站
//                transferStations[line].append(station);
//            }
//        }
//    }
//
//    // 构建线路图（将每条线路视为一个节点）
//    QMap<QString, QVector<QPair<QString, int>>> lineGraph; // 线路图：线路 -> [(相邻线路, 权重)]
//
//    // 找出所有换乘站，并构建线路之间的连接
//    for (const auto& line1 : lineStations.keys()) {
//        for (const auto& line2 : lineStations.keys()) {
//            if (line1 == line2) continue;
//
//            // 检查两条线路是否有共同的换乘站
//            for (const QString& station : transferStations[line1]) {
//                if (transferStations[line2].contains(station)) {
//                    // 找到换乘站，在两条线路之间添加边
//                    lineGraph[line1].append(qMakePair(line2, 1));
//                    lineGraph[line2].append(qMakePair(line1, 1));
//                    break;
//                }
//            }
//        }
//    }
//
//    // 找出起点和终点所在的线路
//    QVector<QString> startLines = getStationLines()[from];
//    QVector<QString> endLines = getStationLines()[to];
//
//    if (startLines.isEmpty() || endLines.isEmpty()) {
//        qDebug() << "错误: 无法确定起点或终点所在的线路";
//        return MetroPath();
//    }
//
//    // 使用Dijkstra算法找到最少换乘路径
//    QMap<QString, int> dist;
//    QMap<QString, QString> prev;
//    QSet<QString> visited;
//
//    // 初始化距离
//    for (const auto& line : lineGraph.keys()) {
//        dist[line] = std::numeric_limits<int>::max();
//    }
//
//    // 将起点所在的所有线路加入队列
//    std::priority_queue<std::pair<int, QString>,
//        std::vector<std::pair<int, QString>>,
//        std::greater<std::pair<int, QString>>> pq;
//
//    for (const QString& line : startLines) {
//        dist[line] = 0;
//        pq.push({ 0, line });
//    }
//
//    // Dijkstra算法
//    while (!pq.empty()) {
//        auto [currentDist, currentLine] = pq.top();
//        pq.pop();
//
//        if (visited.contains(currentLine)) continue;
//        visited.insert(currentLine);
//
//        // 如果当前线路是终点所在的线路，找到路径
//        if (endLines.contains(currentLine)) {
//            break;
//        }
//
//        // 遍历所有相邻线路
//        for (const auto& [neighbor, weight] : lineGraph[currentLine]) {
//            if (!visited.contains(neighbor)) {
//                int newDist = currentDist + weight;
//                if (newDist < dist[neighbor]) {
//                    dist[neighbor] = newDist;
//                    prev[neighbor] = currentLine;
//                    pq.push({ newDist, neighbor });
//                }
//            }
//        }
//    }
//
//    // 找到最少换乘的线路路径
//    QString bestEndLine;
//    int minTransfers = std::numeric_limits<int>::max();
//
//    for (const QString& line : endLines) {
//        if (dist[line] < minTransfers) {
//            minTransfers = dist[line];
//            bestEndLine = line;
//        }
//    }
//
//    if (minTransfers == std::numeric_limits<int>::max()) {
//        qDebug() << "最少换乘算法未找到有效路径";
//        return MetroPath();
//    }
//
//    // 重构线路路径
//    QVector<QString> linePath;
//    QString currentLine = bestEndLine;
//
//    while (!currentLine.isEmpty()) {
//        linePath.prepend(currentLine);
//        currentLine = prev.value(currentLine, "");
//    }
//
//    qDebug() << "线路路径:" << linePath;
//
//    // 将线路路径转换为站点路径
//    QVector<QString> stationNames = convertLinePathToStationPath(linePath, from, to);
//    if (stationNames.isEmpty()) {
//        qDebug() << "错误: 无法将线路路径转换为站点路径";
//        return MetroPath();
//    }
//
//    MetroPath result = buildPath(stationNames);
//    result.transferCount = minTransfers;
//
//    qDebug() << "最少换乘路径找到，换乘次数:" << result.transferCount;
//
//    return result;
//}

MetroPath PathFinder::findMinTransferPath(const QString& from, const QString& to) {
    qDebug() << "开始搜索最少换乘路径从" << from << "到" << to;

    // 检查graph指针是否有效
    if (graph == nullptr) {
        qDebug() << "错误: graph指针为空";
        return MetroPath();
    }

    // 检查起点和终点是否存在
    if (!graph->hasStation(from)) {
        qDebug() << "错误: 起点" << from << "不存在";
        return MetroPath();
    }
    if (!graph->hasStation(to)) {
        qDebug() << "错误: 终点" << to << "不存在";
        return MetroPath();
    }

    // 获取所有线路和站点信息
    QMap<QString, QVector<QString>> lineStations = getLineStations();
    QMap<QString, QVector<QString>> stationLines = getStationLines();

    // 找出起点和终点所在的线路
    QVector<QString> startLines = stationLines[from];
    QVector<QString> endLines = stationLines[to];

    if (startLines.isEmpty() || endLines.isEmpty()) {
        qDebug() << "错误: 无法确定起点或终点所在的线路";
        return MetroPath();
    }

    qDebug() << "起点" << from << "所在线路:" << startLines;
    qDebug() << "终点" << to << "所在线路:" << endLines;

    // 如果起点和终点在同一条线路上，直接返回这条线路上的路径
    for (const QString& line : startLines) {
        if (endLines.contains(line)) {
            qDebug() << "起点和终点在同一线路" << line << "上";
            QVector<QString> stationNames = findPathOnLine(line, from, to);
            if (!stationNames.isEmpty()) {
                MetroPath result = buildPath(stationNames);
                result.transferCount = 0;
                return result;
            }
        }
    }

    // 构建线路图（将每条线路视为一个节点）
    QMap<QString, QVector<QPair<QString, int>>> lineGraph; // 线路图：线路 -> [(相邻线路, 权重)]

    // 找出所有换乘站，并构建线路之间的连接
    for (const auto& line1 : lineStations.keys()) {
        for (const auto& line2 : lineStations.keys()) {
            if (line1 == line2) continue;

            // 检查两条线路是否有共同的站点（换乘站）
            for (const QString& station : lineStations[line1]) {
                if (lineStations[line2].contains(station)) {
                    // 找到换乘站，在两条线路之间添加边
                    // 检查是否已存在连接
                    bool exists = false;
                    for (const auto& pair : lineGraph[line1]) {
                        if (pair.first == line2) {
                            exists = true;
                            break;
                        }
                    }

                    if (!exists) {
                        lineGraph[line1].append(qMakePair(line2, 1));
                        lineGraph[line2].append(qMakePair(line1, 1));
                    }
                    break;
                }
            }
        }
    }

    // 使用BFS找到最少换乘路径
    QMap<QString, int> dist;
    QMap<QString, QString> prev;
    QSet<QString> visited;

    // 初始化距离
    for (const auto& line : lineGraph.keys()) {
        dist[line] = std::numeric_limits<int>::max();
    }

    // 将起点所在的所有线路加入队列
    std::queue<QString> q;

    for (const QString& line : startLines) {
        dist[line] = 0;
        q.push(line);
        visited.insert(line);
    }

    // BFS算法
    while (!q.empty()) {
        QString currentLine = q.front();
        q.pop();

        // 如果当前线路是终点所在的线路，找到路径
        if (endLines.contains(currentLine)) {
            break;
        }

        // 遍历所有相邻线路
        for (const auto& [neighbor, weight] : lineGraph[currentLine]) {
            if (!visited.contains(neighbor)) {
                dist[neighbor] = dist[currentLine] + weight;
                prev[neighbor] = currentLine;
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }

    // 找到最少换乘的线路路径
    QString bestEndLine;
    int minTransfers = std::numeric_limits<int>::max();

    for (const QString& line : endLines) {
        if (dist[line] < minTransfers) {
            minTransfers = dist[line];
            bestEndLine = line;
        }
    }

    if (minTransfers == std::numeric_limits<int>::max()) {
        qDebug() << "最少换乘算法未找到有效路径";
        // 回退到最少站点算法
        qDebug() << "回退到最少站点算法";
        return findMinStationsPath(from, to);
    }

    // 重构线路路径
    QVector<QString> linePath;
    QString currentLine = bestEndLine;

    while (!currentLine.isEmpty()) {
        linePath.prepend(currentLine);
        currentLine = prev.value(currentLine, "");
    }

    qDebug() << "线路路径:" << linePath;

    // 将线路路径转换为站点路径
    QVector<QString> stationNames = convertLinePathToStationPath(linePath, from, to);
    if (stationNames.isEmpty()) {
        qDebug() << "错误: 无法将线路路径转换为站点路径";
        // 回退到最少站点算法
        qDebug() << "回退到最少站点算法";
        return findMinStationsPath(from, to);
    }

    MetroPath result = buildPath(stationNames);
    result.transferCount = minTransfers;

    qDebug() << "最少换乘路径找到，换乘次数:" << result.transferCount;

    return result;
}

QMap<QString, QVector<QString>> PathFinder::getStationLines() const {
    QMap<QString, QVector<QString>> stationLines;

    // 遍历所有连接，构建站点到线路的映射
    for (const StationConnection& conn : graph->getConnections()) {
        if (!stationLines[conn.station1].contains(conn.line)) {
            stationLines[conn.station1].append(conn.line);
        }
        if (!stationLines[conn.station2].contains(conn.line)) {
            stationLines[conn.station2].append(conn.line);
        }
    }

    return stationLines;
}
MetroPath PathFinder::findMinStationsPath(const QString& from, const QString& to) {
    qDebug() << "开始搜索最少站点路径从" << from << "到" << to;

    // 使用BFS找到最短路径（站点数最少）
    QVector<QString> stationNames = bfsShortestPath(from, to);
    MetroPath path = buildPath(stationNames);

    qDebug() << "最少站点路径找到，站点数:" << path.stationCount;
    return path;
}

MetroPath PathFinder::findMinDistancePath(const QString& from, const QString& to) {
    qDebug() << "开始搜索最短距离路径从" << from << "到" << to;

    // 使用Dijkstra算法找到最短路径（距离最短）
    QVector<QString> stationNames = dijkstraShortestPath(from, to);
    MetroPath path = buildPath(stationNames);

    qDebug() << "最短距离路径找到，总距离:" << path.totalDistance;
    return path;
}

// 修改Dijkstra算法，移除weights参数
QVector<QString> PathFinder::dijkstraShortestPath(const QString& from, const QString& to) {
    qDebug() << "开始Dijkstra搜索从" << from << "到" << to;

    QMap<QString, double> dist;
    QMap<QString, QString> prev;
    QSet<QString> unvisited;

    // 初始化
    QVector<QString> allStations = graph->getStationNames();
    for (const QString& station : allStations) {
        dist[station] = std::numeric_limits<double>::max();
        prev[station] = "";
        unvisited.insert(station);
    }
    dist[from] = 0;

    while (!unvisited.isEmpty()) {
        // 找到未访问节点中距离最小的
        QString current;
        double minDist = std::numeric_limits<double>::max();
        for (const QString& station : unvisited) {
            if (dist[station] < minDist) {
                minDist = dist[station];
                current = station;
            }
        }

        if (current.isEmpty()) {
            break; // 没有可达节点
        }

        if (current == to) {
            break; // 找到目标节点
        }

        unvisited.remove(current);

        // 更新邻居节点的距离
        Station station = graph->getStation(current);
        for (const QString& neighbor : station.connectedStations) {
            if (unvisited.contains(neighbor)) {
                double alt = dist[current] + calculateDistance(current, neighbor);
                if (alt < dist[neighbor]) {
                    dist[neighbor] = alt;
                    prev[neighbor] = current;
                }
            }
        }
    }

    // 重构路径
    QVector<QString> path;
    QString current = to;
    while (!current.isEmpty()) {
        path.prepend(current);
        current = prev.value(current, "");
    }

    // 检查是否找到了有效路径
    if (path.size() < 2 || path.first() != from) {
        qDebug() << "Dijkstra未找到有效路径";
        return QVector<QString>();
    }

    qDebug() << "Dijkstra找到路径:" << path;
    return path;
}

QVector<QString> PathFinder::bfsShortestPath(const QString& from, const QString& to) {
    qDebug() << "开始BFS搜索从" << from << "到" << to;

    // 检查起点和终点是否存在
    if (!graph->hasStation(from)) {
        qDebug() << "错误: 起点" << from << "不存在";
        return QVector<QString>();
    }
    if (!graph->hasStation(to)) {
        qDebug() << "错误: 终点" << to << "不存在";
        return QVector<QString>();
    }

    // 使用BFS找到最短路径
    QMap<QString, QString> cameFrom;
    QSet<QString> visited;
    std::queue<QString> queue;

    queue.push(from);
    visited.insert(from);
    cameFrom[from] = "";

    while (!queue.empty()) {
        QString current = queue.front();
        queue.pop();

        if (current == to) {
            // 重构路径
            QVector<QString> path;
            QString node = current;
            while (!node.isEmpty()) {
                path.prepend(node);
                node = cameFrom.value(node, "");
            }
            qDebug() << "找到路径:" << path;
            return path;
        }

        Station station = graph->getStation(current);

        // 按照某种顺序处理邻居，例如按名称排序以确保一致性
        QVector<QString> neighbors = station.connectedStations;
        std::sort(neighbors.begin(), neighbors.end());

        for (const QString& neighbor : neighbors) {
            if (!visited.contains(neighbor)) {
                visited.insert(neighbor);
                cameFrom[neighbor] = current;
                queue.push(neighbor);
            }
        }
    }

    qDebug() << "未找到路径";
    return QVector<QString>();
}


// 修改路径构建函数
MetroPath PathFinder::buildPath(const QVector<QString>& stationNames) {
    MetroPath path;
    path.transferCount = 0;
    path.stationCount = 0;
    path.totalDistance = 0;

    if (stationNames.size() < 2) {
        qDebug() << "路径构建失败: 站点数量不足";
        return path;
    }

    path.stationCount = stationNames.size();

    // 构建路径段
    PathSegment currentSegment;
    currentSegment.from = stationNames.first();
    currentSegment.stations.append(stationNames.first());

    for (int i = 1; i < stationNames.size(); i++) {
        QString currentStation = stationNames[i];
        QString prevStation = stationNames[i - 1];

        // 计算距离
        path.totalDistance += calculateDistance(prevStation, currentStation);

        QString line = getLineBetweenStations(prevStation, currentStation);

        if (line.isEmpty()) {
            qDebug() << "警告: 站点" << prevStation << "和" << currentStation << "之间没有线路信息";
            // 尝试使用前一段的线路
            if (!currentSegment.line.isEmpty()) {
                line = currentSegment.line;
                qDebug() << "使用前一段的线路:" << line;
            }
            else {
                line = QString::fromUtf8("未知");
            }
        }

        if (currentSegment.line.isEmpty()) {
            currentSegment.line = line;
            currentSegment.to = currentStation;
            currentSegment.stations.append(currentStation);
        }
        else if (currentSegment.line == line) {
            currentSegment.to = currentStation;
            currentSegment.stations.append(currentStation);
        }
        else {
            // 换乘点
            path.segments.append(currentSegment);
            path.transferCount++;

            currentSegment = PathSegment();
            currentSegment.line = line;
            currentSegment.from = prevStation; // 换乘站
            currentSegment.to = currentStation;
            currentSegment.stations.append(prevStation);
            currentSegment.stations.append(currentStation);
        }
    }

    if (!currentSegment.stations.isEmpty()) {
        path.segments.append(currentSegment);
    }

    qDebug() << "构建路径完成，段数:" << path.segments.size() << "换乘次数:" << path.transferCount;

    return path;
}
// 修改距离计算函数
double PathFinder::calculateDistance(const QString& station1, const QString& station2) {
    Station s1 = graph->getStation(station1);
    Station s2 = graph->getStation(station2);

    // 检查站点是否存在
    if (s1.name.isEmpty() || s2.name.isEmpty()) {
        qDebug() << "警告: 计算距离时站点不存在";
        return 1.0; // 返回默认距离
    }

    // 使用真实坐标计算距离（简单的欧几里得距离）
    double dx = s1.realPosition.x() - s2.realPosition.x();
    double dy = s1.realPosition.y() - s2.realPosition.y();
    double distance = std::sqrt(dx * dx + dy * dy);

    // 将经纬度距离转换为近似公里数
    // 1度纬度约111公里，1度经度约111*cos(纬度)公里
    // 上海纬度约31度，所以1度经度约111*cos(31°) ≈ 95公里
    double latToKm = 111.0;
    double lonToKm = 111.0 * std::cos(31.0 * M_PI / 180.0);

    double distanceKm = std::sqrt(std::pow(dx * lonToKm, 2) + std::pow(dy * latToKm, 2));

    qDebug() << "计算距离:" << station1 << "->" << station2 << "=" << distanceKm << "公里";

    return distanceKm;
}

QString PathFinder::getLineBetweenStations(const QString& station1, const QString& station2)const {
    StationConnection conn = graph->getConnection(station1, station2);
    if (!conn.line.isEmpty()) {
        return conn.line;
    }

    // 如果直接连接不存在，尝试反向查找
    conn = graph->getConnection(station2, station1);
    if (!conn.line.isEmpty()) {
        return conn.line;
    }

    // 如果还是没有找到，尝试查找两个站点之间的间接连接
    // 这可能需要更复杂的算法，但为了简单起见，我们返回空字符串
    qDebug() << "警告: 无法找到站点" << station1 << "和" << station2 << "之间的线路";
    return "";
}

// 将线路路径转换为站点路径
//QVector<QString> PathFinder::convertLinePathToStationPath(const QVector<QString>& linePath,
//    const QString& from,
//    const QString& to) {
//    qDebug() << "将线路路径转换为站点路径:" << linePath;
//
//    if (linePath.isEmpty()) {
//        qDebug() << "错误: 线路路径为空";
//        return QVector<QString>();
//    }
//
//    // 获取线路到站点的映射
//    QMap<QString, QVector<QString>> lineStations = getLineStations();
//
//    // 找出每条线路上的换乘站
//    QMap<QString, QVector<QString>> transferStations;
//    for (const auto& line : lineStations.keys()) {
//        for (const QString& station : lineStations[line]) {
//            Station stationInfo = graph->getStation(station);
//            if (stationInfo.connectedStations.size() > 2) {
//                // 如果一个站连接了超过2个站，可能是换乘站
//                transferStations[line].append(station);
//            }
//        }
//    }
//
//    // 找出起点和终点所在的线路
//    QVector<QString> startLines = getStationLines()[from];
//    QVector<QString> endLines = getStationLines()[to];
//
//    // 构建站点路径
//    QVector<QString> stationPath;
//    stationPath.append(from);
//
//    // 对于每条线路，找到最佳的换乘站
//    for (int i = 0; i < linePath.size() - 1; i++) {
//        QString currentLine = linePath[i];
//        QString nextLine = linePath[i + 1];
//
//        // 找出两条线路的共同换乘站
//        QVector<QString> commonTransferStations;
//        for (const QString& station : transferStations[currentLine]) {
//            if (transferStations[nextLine].contains(station)) {
//                commonTransferStations.append(station);
//            }
//        }
//
//        if (commonTransferStations.isEmpty()) {
//            qDebug() << "错误: 找不到线路" << currentLine << "和" << nextLine << "的共同换乘站";
//            return QVector<QString>();
//        }
//
//        // 选择最佳的换乘站（选择距离当前站点最近的）
//        QString bestTransferStation = commonTransferStations.first();
//        int minDistance = std::numeric_limits<int>::max();
//
//        for (const QString& station : commonTransferStations) {
//            // 计算从当前站点到换乘站的距离
//            QVector<QString> pathToStation = findPathOnLine(currentLine, stationPath.last(), station);
//            if (!pathToStation.isEmpty() && pathToStation.size() < minDistance) {
//                minDistance = pathToStation.size();
//                bestTransferStation = station;
//            }
//        }
//
//        // 在当前线路上找到从当前站点到换乘站的路径
//        QVector<QString> pathOnCurrentLine = findPathOnLine(currentLine, stationPath.last(), bestTransferStation);
//        if (pathOnCurrentLine.isEmpty()) {
//            qDebug() << "错误: 无法在线路" << currentLine << "上找到从" << stationPath.last() << "到" << bestTransferStation << "的路径";
//            return QVector<QString>();
//        }
//
//        // 添加到站点路径（去掉第一个站点，因为它已经在路径中）
//        for (int j = 1; j < pathOnCurrentLine.size(); j++) {
//            stationPath.append(pathOnCurrentLine[j]);
//        }
//    }
//
//    // 在最后一条线路上找到从当前站点到终点的路径
//    QString lastLine = linePath.last();
//    QVector<QString> pathOnLastLine = findPathOnLine(lastLine, stationPath.last(), to);
//    if (pathOnLastLine.isEmpty()) {
//        qDebug() << "错误: 无法在线路" << lastLine << "上找到从" << stationPath.last() << "到" << to << "的路径";
//        return QVector<QString>();
//    }
//
//    // 添加到站点路径（去掉第一个站点，因为它已经在路径中）
//    for (int j = 1; j < pathOnLastLine.size(); j++) {
//        stationPath.append(pathOnLastLine[j]);
//    }
//
//    qDebug() << "转换后的站点路径:" << stationPath;
//    return stationPath;
//}
//

// PathFinder.cpp - 修改convertLinePathToStationPath方法
QVector<QString> PathFinder::convertLinePathToStationPath(const QVector<QString>& linePath,
    const QString& from,
    const QString& to) {
    qDebug() << "将线路路径转换为站点路径:" << linePath;

    if (linePath.isEmpty()) {
        qDebug() << "错误: 线路路径为空";
        return QVector<QString>();
    }

    // 获取线路到站点的映射
    QMap<QString, QVector<QString>> lineStations = getLineStations();
    QMap<QString, QVector<QString>> stationLines = getStationLines();

    // 构建站点路径
    QVector<QString> stationPath;
    stationPath.append(from);

    // 对于每条线路，找到换乘站
    for (int i = 0; i < linePath.size() - 1; i++) {
        QString currentLine = linePath[i];
        QString nextLine = linePath[i + 1];

        // 找出两条线路的共同站点（换乘站）
        QVector<QString> commonStations;
        for (const QString& station : lineStations[currentLine]) {
            if (lineStations[nextLine].contains(station)) {
                commonStations.append(station);
            }
        }

        if (commonStations.isEmpty()) {
            qDebug() << "错误: 找不到线路" << currentLine << "和" << nextLine << "的共同站点";
            return QVector<QString>();
        }

        // 选择距离当前站点最近的换乘站
        QString bestTransferStation = commonStations.first();
        int minDistance = std::numeric_limits<int>::max();

        for (const QString& station : commonStations) {
            // 计算从当前站点到换乘站的距离（站点数）
            QVector<QString> pathToStation = findPathOnLine(currentLine, stationPath.last(), station);
            if (!pathToStation.isEmpty() && pathToStation.size() < minDistance) {
                minDistance = pathToStation.size();
                bestTransferStation = station;
            }
        }

        // 在当前线路上找到从当前站点到换乘站的路径
        QVector<QString> pathOnCurrentLine = findPathOnLine(currentLine, stationPath.last(), bestTransferStation);
        if (pathOnCurrentLine.isEmpty()) {
            qDebug() << "错误: 无法在线路" << currentLine << "上找到从" << stationPath.last() << "到" << bestTransferStation << "的路径";
            return QVector<QString>();
        }

        // 添加到站点路径（去掉第一个站点，因为它已经在路径中）
        for (int j = 1; j < pathOnCurrentLine.size(); j++) {
            stationPath.append(pathOnCurrentLine[j]);
        }
    }

    // 在最后一条线路上找到从当前站点到终点的路径
    QString lastLine = linePath.last();
    QVector<QString> pathOnLastLine = findPathOnLine(lastLine, stationPath.last(), to);
    if (pathOnLastLine.isEmpty()) {
        qDebug() << "错误: 无法在线路" << lastLine << "上找到从" << stationPath.last() << "到" << to << "的路径";
        return QVector<QString>();
    }

    // 添加到站点路径（去掉第一个站点，因为它已经在路径中）
    for (int j = 1; j < pathOnLastLine.size(); j++) {
        stationPath.append(pathOnLastLine[j]);
    }

    qDebug() << "转换后的站点路径:" << stationPath;
    return stationPath;
}

// 在线路上找到两个站点之间的路径，考虑分支情况
QVector<QString> PathFinder::findPathOnLine(const QString& line, const QString& from, const QString& to) {
    // 获取线路上的所有站点
    QMap<QString, QVector<QString>> lineStations = getLineStations();
    QVector<QString> stationsOnLine = lineStations[line];

    // 找出起点和终点在线路上的位置
    int fromIndex = stationsOnLine.indexOf(from);
    int toIndex = stationsOnLine.indexOf(to);

    if (fromIndex == -1 || toIndex == -1) {
        qDebug() << "错误: 站点" << from << "或" << to << "不在线路" << line << "上";
        return QVector<QString>();
    }

    // 确定方向（正向或反向）
    QVector<QString> path;
    if (fromIndex <= toIndex) {
        // 正向
        for (int i = fromIndex; i <= toIndex; i++) {
            path.append(stationsOnLine[i]);
        }
    }
    else {
        // 反向
        for (int i = fromIndex; i >= toIndex; i--) {
            path.append(stationsOnLine[i]);
        }
    }

    // 检查路径中是否有分支点
    // 如果有分支点，确保选择正确的分支方向
    for (int i = 0; i < path.size(); i++) {
        QString station = path[i];
        Station stationInfo = graph->getStation(station);

        // 如果站点有多个连接，可能是分支点
        if (stationInfo.connectedStations.size() > 2) {
            // 检查当前路径是否选择了正确的分支
            if (i + 1 < path.size()) {
                QString nextStation = path[i + 1];

                // 检查下一个站点是否在当前站点的连接中
                if (!stationInfo.connectedStations.contains(nextStation)) {
                    qDebug() << "警告: 路径中可能存在错误的分支选择"
                        << station << "->" << nextStation;

                    // 尝试找到正确的路径
                    QVector<QString> alternativePath = findAlternativePathOnLine(line, from, to, station);
                    if (!alternativePath.isEmpty()) {
                        return alternativePath;
                    }
                }
            }
        }
    }

    return path;
}


// 在线路上找到替代路径，处理分支情况
QVector<QString> PathFinder::findAlternativePathOnLine(const QString& line, const QString& from,
    const QString& to, const QString& branchPoint) {
    qDebug() << "尝试找到替代路径，处理分支:" << line << from << "->" << to << "分支点:" << branchPoint;

    // 使用BFS在线路上找到从起点到终点的路径
    QMap<QString, QString> cameFrom;
    QSet<QString> visited;
    std::queue<QString> queue;

    queue.push(from);
    visited.insert(from);
    cameFrom[from] = "";

    while (!queue.empty()) {
        QString current = queue.front();
        queue.pop();

        if (current == to) {
            // 重构路径
            QVector<QString> path;
            QString node = current;
            while (!node.isEmpty()) {
                path.prepend(node);
                node = cameFrom.value(node, "");
            }
            return path;
        }

        Station stationInfo = graph->getStation(current);

        // 只考虑在同一线路上的邻居
        for (const QString& neighbor : stationInfo.connectedStations) {
            QString neighborLine = getLineBetweenStations(current, neighbor);
            if (neighborLine == line && !visited.contains(neighbor)) {
                visited.insert(neighbor);
                cameFrom[neighbor] = current;
                queue.push(neighbor);
            }
        }
    }

    qDebug() << "错误: 无法在线路" << line << "上找到从" << from << "到" << to << "的替代路径";
    return QVector<QString>();
}

QMap<QString, QVector<QString>> PathFinder::getLineStations() const {
    QMap<QString, QVector<QString>> lineStations;

    // 遍历所有连接，构建线路到站点的映射
    for (const StationConnection& conn : graph->getConnections()) {
        if (!lineStations[conn.line].contains(conn.station1)) {
            lineStations[conn.line].append(conn.station1);
        }
        if (!lineStations[conn.line].contains(conn.station2)) {
            lineStations[conn.line].append(conn.station2);
        }
    }

    // 对每条线路的站点进行排序（按照线路顺序）
    for (auto& line : lineStations.keys()) {
        // 找出线路的端点（连接数最少的站点）
        QVector<QString> endStations;

        for (const QString& station : lineStations[line]) {
            Station stationInfo = graph->getStation(station);
            int connectionCount = stationInfo.connectedStations.size();

            // 如果站点只有一个连接，或者是分支点，可能是端点
            if (connectionCount == 1 || connectionCount > 2) {
                endStations.append(station);
            }
        }

        // 如果找不到端点，选择任意两个站点作为端点
        if (endStations.size() < 2) {
            for (const QString& station : lineStations[line]) {
                if (endStations.size() >= 2) break;
                if (!endStations.contains(station)) {
                    endStations.append(station);
                }
            }
        }

        // 使用BFS从端点开始构建线路顺序
        QVector<QVector<QString>> possiblePaths;

        // 尝试从每个端点开始构建路径
        for (const QString& endStation : endStations) {
            QVector<QString> path;
            QSet<QString> visited;
            std::queue<QString> queue;

            queue.push(endStation);
            visited.insert(endStation);

            while (!queue.empty()) {
                QString current = queue.front();
                queue.pop();

                path.append(current);

                Station stationInfo = graph->getStation(current);
                for (const QString& neighbor : stationInfo.connectedStations) {
                    if (!visited.contains(neighbor) && lineStations[line].contains(neighbor)) {
                        visited.insert(neighbor);
                        queue.push(neighbor);
                    }
                }
            }

            possiblePaths.append(path);
        }

        // 选择最长的路径作为线路顺序
        QVector<QString> longestPath;
        for (const QVector<QString>& path : possiblePaths) {
            if (path.size() > longestPath.size()) {
                longestPath = path;
            }
        }

        // 更新线路的站点顺序
        lineStations[line] = longestPath;
    }

    return lineStations;
}