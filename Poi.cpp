#include "Poi.h"

#include <qdebug.h>

Poi::Poi(const QGeoCoordinate & coordinate, const QPoint & offset, const QPixmap & pixmap)
        : QGeoMapPixmapObject(coordinate, offset, pixmap)
{
    qDebug() << "Poi::Poi(const QGeoCoordinate & coordinate, const QPoint & offset, const QPixmap & pixmap)";

    // TODO Warum geht es nicht und wird die Oberklasse ausgewaehlt
    connect(this, SIGNAL(selectedChanged(bool)), this, SLOT(mySlot()));
}

Poi::~Poi(){

}

void Poi::mySlot(){
    qDebug() << "void Poi::setVisible()" ;
    this->setVisible(true);
}

void Poi::setDescription(const QString& description){
    this->m_description = description;
}
