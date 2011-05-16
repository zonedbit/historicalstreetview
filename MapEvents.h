#ifndef CIRCLEAREASELECTWIDGET_H
#define CIRCLEAREASELECTWIDGET_H

#include <qgraphicsgeomap.h>
#include <QGeoMapPixmapObject>
#include <QGeoMapCircleObject>
#include <QGeoBoundingCircle>
#include <QGeoAreaMonitor>
#include <QGeoPositionInfo>

QTM_USE_NAMESPACE

class CircleAreaSelectWidget : public QGraphicsGeoMap
{
    Q_OBJECT
public:
    CircleAreaSelectWidget(QGeoMappingManager *manager, const QGeoCoordinate&  center);

    ~CircleAreaSelectWidget();
public:
    void setCircle(const QGeoCoordinate& circleCenter,const qreal& radius);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

private:
    QPair<QPixmap, QPoint> makeStandardMarker(int shapeType, const QString & text, const QPen & pen, const QPen & textPen, const QBrush & brush);

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

#endif // CIRCLEAREASELECTWIDGET_H
