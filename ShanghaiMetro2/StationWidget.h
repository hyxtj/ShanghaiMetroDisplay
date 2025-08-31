/***************************************************************************
  文件名称：StationWidget.h
  功    能：地铁站点显示部件的头文件
  说    明：定义地铁线路和站点的可视化显示部件类和交互接口
***************************************************************************/

#ifndef STATIONWIDGET_H
#define STATIONWIDGET_H

#include <QWidget>
#include <QPainter>
#include "MetroGraph.h"
#include "PathFinder.h"

class StationWidget : public QWidget {
    Q_OBJECT
public:
	explicit StationWidget(QWidget* parent = nullptr); // 构造函数
	void     setMetroGraph(const MetroGraph& graph);   // 设置地铁图
	void     setPath(const MetroPath& path);           // 设置当前路径

protected:
    /*重写鼠标事件*/
	void paintEvent(QPaintEvent* event)        override; // 绘制事件
	void mousePressEvent(QMouseEvent* event)   override; // 鼠标按下事件
	void mouseMoveEvent(QMouseEvent* event)    override; // 鼠标移动事件
	void mouseReleaseEvent(QMouseEvent* event) override; // 鼠标释放事件
	void wheelEvent(QWheelEvent* event)        override; // 鼠标滚轮事件

signals:
	void stationSelected(const QString& stationName);  // 站点被选中信号
	void positionSelected(const QPoint& position);     // 位置被选中信号
    void mousePositionChanged(const QPoint& graphPos); // 鼠标位置变化信号

public slots:
    void setSelectionMode(bool enabled); //设置选择模式

private:
	const MetroGraph*     metroGraph;		      // 地铁线路图指针
    MetroPath             currentPath;            // 当前路径
	QMap<QString, QPoint> stationPositions;		  // 站点位置映射
	double                scale;				  // 缩放比例
	QPoint                offset;				  // 偏移量
	QPoint                lastDragPos;			  // 上次拖拽位置
	bool                  isDragging;			  // 是否正在拖拽
	bool                  selectionMode;          // 是否处于选择模式
	QPoint                lastMousePos;           // 最后鼠标位置
	QPoint                rightClickPos;          // 记录右键点击的位置
	bool                  showRightClickFeedback; // 是否显示右键点击反馈
	QTimer*               feedbackTimer;          // 反馈显示定时器

	/*绘制方法*/
    void drawStation(QPainter& painter, const Station& station, bool isHighlighted = false);           //绘制站点
    void drawConnection(QPainter& painter, const StationConnection& conn, bool isHighlighted = false); //绘制连接线
	void drawPath(QPainter& painter);                                                                  //绘制路径
	void drawLegend(QPainter& painter);                                                                //绘制图例

	/*辅助方法*/
    QPoint                     getStationPosition(const Station& station) const; //获取站点位置
    QPoint					   toViewport(const QPoint& graphPoint)       const; //换算窗口坐标
    QPoint					   toGraph(const QPoint& viewportPoint)       const; //换算图上实际坐标
	QVector<StationConnection> getPathConnections()                       const; //获取路径连接线
	QColor getStationLineColor(const QString& stationName)				  const; //获取站点线路颜色

};

#endif // STATIONWIDGET_H