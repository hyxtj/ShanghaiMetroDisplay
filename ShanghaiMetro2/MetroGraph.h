/***************************************************************************
  文件名称：MetroGraph.h
  功    能：地铁图数据模型的头文件
  说    明：定义地铁线路、站点和连接的数据结构和操作接口
***************************************************************************/

#ifndef METROGRAPH_H
#define METROGRAPH_H

#include <QString>
#include <QVector>
#include <QPoint>
#include <QColor>
#include <QMap>
#include <QJsonObject>
#include <QSet>

/*地铁线路信息*/
struct MetroLine {
    QString name;  //站点名称
    QColor  color; //站点颜色
};

/*地铁站点连接信息*/
struct StationConnection {
    QString         station1;  //站点名称1
    QString         station2;  //站点名称2
	QString         line;      //连接线路
    QVector<QPoint> viaPoints; //连接线的拐点
};

/*地铁站点信息*/
struct Station {
    QString          name;              //站点名称
    QString          tag;               //地铁站标签位置
    QString          type;              //地铁站类型 
    QPoint           graphPosition;     //在地图上的位置
    QPointF          realPosition;      //实际地理位置
    QVector<QString> connectedStations; //连接的站点名称
};

/*地铁网络图*/
class MetroGraph {
public:
    MetroGraph();  //构造函数
    ~MetroGraph(); //析构函数

    bool loadFromJson(const QString& filename); //加载数据

	/*向外提供的接口*/
    QVector<MetroLine>              getLines()                                                      const;//获取全部路线
	QVector<Station>                getStations()                                                   const;//获取全部站点
	Station                         getStation(const QString& name)                                 const;//获取指定站点
	QVector<QString>                getStationNames()                                               const;//获取站点名称列表
    QVector<StationConnection>      getConnections()                                                const;//获取全部连接
	StationConnection               getConnection(const QString& station1, const QString& station2) const;//获取指定连接
    bool                            hasStation(const QString& name)                                 const;//检查站点是否存在
    QMap<QString, QVector<QString>> getLineStations()                                               const;//获取每条线路的站点列表

    /*添加方法*/
    bool addLine(const MetroLine& line);                                           //添加线路
    bool addStation(const Station& station);                                       //添加站点
    bool addConnection(const QString& station1, const QString& station2, 
        const QString& line, const QVector<QPoint>& viaPoints = QVector<QPoint>());//添加连接

private:
    /*内部存储数据结构*/
    QVector<MetroLine>                               lines;         //路线信息
    QVector<Station>                                 stations;      //站点信息
    QVector<StationConnection>                       connections;   //连接信息
    QMap<QString, Station>                           stationMap;    //站点与名称映射表
    QMap<QPair<QString, QString>, StationConnection> connectionMap; //连接映射表

    /*根据数组解析信息及构建映射方法*/
	void parseLines(const QJsonArray& linesArray);         // 解析线路信息
    void parseStations(const QJsonArray& stationsArray);   // 解析站点信息
	void parseConnections(const QJsonArray& stationsArray);// 解析连接信息
    void buildStationMap();                                // 构建站点映射
};

#endif // METROGRAPH_H