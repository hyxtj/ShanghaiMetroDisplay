/***************************************************************************
  文件名称：PathFinder.h
  功    能：路径查找算法的头文件
  说    明：定义路径搜索策略枚举和路径查找器类接口
***************************************************************************/

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "MetroGraph.h"
#include <QVector>
#include <QString>
#include <QMap>
#include <queue>
#include <functional>
#include <QSet>

/*搜索策略枚举*/
enum SearchStrategy {
    MIN_TRANSFER,    // 换乘最少
    MIN_STATIONS,    // 经过站点最少
    MIN_DISTANCE     // 路径长度最短
};

/*路径段信息*/
struct PathSegment {
	QString          line;    //线路名称
    QString          from;    //起点站名称
	QString          to;      //终点站名称
	QVector<QString> stations;//站点列表
};

/*完整路径信息*/
struct MetroPath {
	QVector<PathSegment> segments;      //路径段列表
	int                  transferCount; //换乘次数 
	int                  stationCount;  //经过站点数
	double               totalDistance; //总距离
};

/*路径查找器*/
class PathFinder {
public:
    PathFinder(const MetroGraph* graph);                                                //构造函数
    MetroPath findPath(const QString& from, const QString& to, SearchStrategy strategy);//查找路径
	void      setGraph(const MetroGraph* graph);                                        //设置地铁图

private:
	const MetroGraph* graph; //地铁线路图指针

    /* 三种搜索策略的具体实现*/
	MetroPath findMinTransferPath(const QString& from, const QString& to); // 最少换乘
	MetroPath findMinStationsPath(const QString& from, const QString& to); // 经过站点最少
	MetroPath findMinDistancePath(const QString& from, const QString& to); // 路径长度最短

    /* 辅助函数*/
	QVector<QString> bfsShortestPath(const QString& from, const QString& to);                       // 广度优先搜索
	QVector<QString> dijkstraShortestPath(const QString& from, const QString& to);                  // Dijkstra算法
	MetroPath        buildPath(const QVector<QString>& stationNames);                               // 构建路径信息
	QString          getLineBetweenStations(const QString& station1, const QString& station2) const;// 获取两站间的线路
	double           calculateDistance(const QString& station1, const QString& station2);           // 计算两站间距离

    /* 最少换乘算法的辅助函数*/
	QMap<QString, QVector<QString>> getStationLines() const; // 获取每个站点的线路信息
	QMap<QString, QVector<QString>> getLineStations() const; // 获取每条线路的站点信息
    QVector<QString> convertLinePathToStationPath(
		const QVector<QString>& linePath,const QString& from, 
		const QString& to);                                  // 将线路路径转换为站点路径
    QVector<QString> findPathOnLine(const QString& line,
		const QString& from, const QString& to);             // 在路线上查路径
    QVector<QString> findAlternativePathOnLine(const QString& line, const QString& from,
		const QString& to, const QString& branchPoint);      // 在支线上查路径

};

#endif // PATHFINDER_H