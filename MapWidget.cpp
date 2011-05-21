#include "MapWidget.h"
#include "InfoPoi.h"

#include <QDebug>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QSvgRenderer>
#include <QMessageBox>

QTM_USE_NAMESPACE

class Poi;


MapWidget::MapWidget(QGeoMappingManager *manager, const QGeoCoordinate& center)
: QGraphicsGeoMap(manager),m_currentObj(0),m_areacircle(0),m_areaMonitor(0)
{
    m_panActive = false;
    m_dragging = false;
    setCenter(center);
}


MapWidget::~MapWidget() {
    delete m_areaMonitor;
}



void MapWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "void MapWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)";

    setFocus();
    if (event->button() == Qt::LeftButton) {
        qDebug() << "event->button() == Qt::LeftButton";

        // Wenn nicht kann die Karte nicht bewegt werden
        m_panActive = true;
        m_currentObj = NULL;

            bool match = false;
            QList<QGeoMapObject*> objList = mapObjectsAtScreenPosition(event->pos());
            qDebug() << "objList.count(): " << objList.count();
            for(int i=0; !match && i < objList.count(); i++){
                qDebug() << "if(QGeoMapObject::PixmapType == objList[i]->type()";
                qDebug() << "if(QGeoMapObject::PixmapType ==" << (QGeoMapObject::PixmapType == (objList[i]->type())) << ")";
                if(QGeoMapObject::PixmapType == objList[i]->type()){
                    m_currentObj = objList[i];
                    qDebug() << "onClick Marker" << m_currentObj;

//                    Poi* p = dynamic_cast<Poi*>(m_currentObj);
                    m_currentObj->setSelected(true);

                    match = true;
                }
            }
    }

    event->accept();
}


void MapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(!m_areaMonitor){
        if(m_currentObj){
            m_dragging = true;
            QGeoCoordinate coord = screenPositionToCoordinate(event->pos());

            if (m_currentObj == m_centerMark){
                m_centerMark->setCoordinate(coord);
                m_areacircle->setCenter(coord);
                emit areaChanged(m_areacircle->circle());
            }
//            else if(m_currentObj == m_areacircle){

//                resizeCircle(event);
//                emit areaChanged(m_areacircle->circle());
//            }
            else{
                int xDiff = (m_SelectPoint.x() - event->pos().x());
                int yDiff = (m_SelectPoint.y() - event->pos().y());

                if(xDiff > 7 || xDiff < -7 || yDiff > 7 || yDiff < -7){
                    m_currentObj = 0;
                }
            }
        }else if(m_panActive){
            pan(event->lastPos().x() - event->pos().x(), event->lastPos().y() - event->pos().y());
        }

        event->accept();
    }
}
