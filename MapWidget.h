#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <qgraphicsgeomap.h>
#include <QGeoMapPixmapObject>
#include <QGeoMapCircleObject>
#include <QGeoBoundingCircle>
#include <QGeoAreaMonitor>
#include <QGeoPositionInfo>

QTM_USE_NAMESPACE

class MapWidget : public QGraphicsGeoMap
{
    Q_OBJECT
public:
    MapWidget(QGeoMappingManager *manager, const QGeoCoordinate&  center);

    ~MapWidget();
public:
    void setCircle(const QGeoCoordinate& circleCenter,const qreal& radius);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);


signals:
     void areaChanged(const QGeoBoundingCircle& box);
     void markerSelected(QGeoMapPixmapObject* pixmap);

private:
    bool m_panActive;
    bool m_dragging;

    QGeoMapObject*          m_currentObj;
    QGeoMapPixmapObject*    m_centerMark;
    QGeoMapCircleObject*    m_areacircle;
    QPointF                 m_SelectPoint;

    QGeoAreaMonitor*        m_areaMonitor;
};

#endif // MAPWIDGET_H
