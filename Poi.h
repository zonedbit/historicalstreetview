#ifndef POI_H
#define POI_H

#include <QGeoMapPixmapObject>

QTM_USE_NAMESPACE

class Poi : public QGeoMapPixmapObject
{
public:
    explicit Poi(const QGeoCoordinate & coordinate, const QPoint & offset = QPoint( 0, 0 ), const QPixmap & pixmap = QPixmap());

    ~Poi();

public slots:
    virtual void mySlot();

};

#endif // POI_H
