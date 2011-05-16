#include "MapEvents.h"
#include "InfoPoi.h"

#include <QDebug>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QSvgRenderer>
#include <QMessageBox>

QTM_USE_NAMESPACE



CircleAreaSelectWidget::CircleAreaSelectWidget(QGeoMappingManager *manager, const QGeoCoordinate& center)
: QGraphicsGeoMap(manager),m_currentObj(0),m_areacircle(0),m_areaMonitor(0)
{
    m_panActive = false;
    m_dragging = false;
    setCenter(center);
}


CircleAreaSelectWidget::~CircleAreaSelectWidget() {
    delete m_areaMonitor;
}



void CircleAreaSelectWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "void CircleAreaSelectWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)";

    setFocus();
    if (event->button() == Qt::LeftButton) {

        // Wenn nicht kann die Karte nicht bewegt werden
        m_panActive = true;
        m_currentObj = NULL;

            bool match = false;
            QList<QGeoMapObject*> objList = mapObjectsAtScreenPosition(event->pos());
            for(int i=0; !match && i < objList.count(); i++){
                if(QGeoMapObject::PixmapType == objList[i]->type()){
                    m_currentObj = objList[i];
                    qDebug() << "onClick Marker" << m_currentObj;

                    InfoPoi *ip = new InfoPoi();                    
                    if(ip){
                        ip->show();
                    }

                    match = true;
                }
            }
    }

    event->accept();
}


void CircleAreaSelectWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
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


QPair<QPixmap, QPoint> CircleAreaSelectWidget::makeStandardMarker(int shapeType, const QString & text, const QPen & pen, const QPen & textPen, const QBrush & brush)
{
    QString svgTemplate;
    QPoint offset;
    switch (shapeType) {
        case 0:
            svgTemplate = "<svg>"
                          "<path "
                          "style=\"fill:#000000;fill-opacity:.4\" "
                          "d=\"m 18.948,33.432051 c 0,1.41694 -2.238,2.567949 -5,2.567949 -2.762,0 -5,-1.151009 -5,-2.567949 0,-1.416939 2.238,-2.567948 5,-2.567948 2.762,-0.002 5,1.149994 5,2.567948\" "
                          "/>"
                          "<path "
                          "style=\"fill:#666\" "
                          "d=\"M 14 0 C 6.28 0 0 6.3348796 0 14.125 C 0 16.209809 0.4425 18.209911 1.3125 20.09375 L 1.84375 21.125 C 2.96275 23.084964 12.2565 32.190555 13.3125 33.21875 L 14 33.875 L 14.6875 33.21875 C 15.7435 32.188525 25.026 23.109909 26.125 21.1875 L 26.65625 20.15625 C 27.54825 18.225721 28 16.204734 28 14.125 C 28.001 6.3348796 21.72 0 14 0 z M 14 0.90625 C 21.175 0.90625 27 6.7640687 27 14 C 27 15.982294 26.55825 17.873541 25.78125 19.5625 L 25.28125 20.53125 C 24.22125 22.386669 14 32.375 14 32.375 C 14 32.375 3.77875 22.387684 2.71875 20.53125 L 2.21875 19.5625 C 1.43975 17.875571 1 15.982294 1 14 C 1 6.7640687 6.823 0.90625 14 0.90625 z \" "
                          "/>"
                          "<path "
                          "style=\"fill:#fff\" "
                          "d=\"M 14 0.90625 C 6.823 0.90625 1 6.7640687 1 14 C 1 15.982294 1.43975 17.875571 2.21875 19.5625 L 2.71875 20.53125 C 3.77875 22.387684 14 32.375 14 32.375 C 14 32.375 24.22125 22.386669 25.28125 20.53125 L 25.78125 19.5625 C 26.55825 17.873541 27 15.982294 27 14 C 27 6.7640687 21.175 0.90625 14 0.90625 z M 14 1.90625 C 20.616 1.90625 26 7.3294234 26 14 C 25.910217 16.115917 25.491329 18.196844 24.40625 20.03125 C 23.776565 21.248455 17.937 27.075427 14 30.9375 C 10.063 27.075427 4.206579 21.245609 3.59375 20.0625 C 2.614452 18.171896 1.90079 16.186259 2 14 C 2 7.3294234 7.383 1.90625 14 1.90625 z \" "
                          "/>"
                          "<path "
                          "style=\"fill:%brush%;fill-opacity:%brushOpacity%\" "
                          "d=\"m 13.996268,1.9105251 c -6.617,0 -12.001,5.4261873 -12.001,12.0967639 -0.09921,2.186259 0.631702,4.174018 1.611,6.064622 0.612829,1.183109 6.453,7.017707 10.39,10.87978 3.937,-3.862073 9.777315,-9.70013 10.407,-10.917335 1.085079,-1.834406 1.503217,-3.91115 1.593,-6.027067 0,-6.6705766 -5.384,-12.0967639 -12,-12.0967639 l 0,0 z\" "
                          "/>"
                          "<path style=\"fill:#ffffff;fill-opacity:0.7\" "
                          "d=\"M 20.968528,6.089997 C 17.785074,3.5240085 13.725682,2.5379787 11.919627,3.8924751 11.436514,4.2548035 11.151528,4.750748 11.073071,5.3665525 c 4.1092,0.6284223 7.111478,2.6511761 8.521564,4.4853881 2.388133,3.1064364 1.967904,3.9515754 1.967904,3.9515754 0.774274,-0.0387 1.422004,-0.242256 1.904023,-0.603757 1.806055,-1.354497 0.684944,-4.5441591 -2.498034,-7.109762 l 0,0 z\" "
                          "/>"
                          "<text "
                          "x=\"14\" y=\"19\" "
                          "font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" "
                          "fill=\"%textPen%\" "
                          "textContent = \"%textContent%\""
                          ">"
                          "%text%"
                          "</text>"
                          "</svg>";
            offset = QPoint(-14, -34);
            break;

        case 1:
            svgTemplate = "<svg><path style=\"fill:%brush%;fill-opacity:%brushOpacity%\" d=\"M 16.00663,.0000019037492 C 7.1692812,.0000019037492 0,7.1692841 0,16.006632 0,24.843981 7.1692812,32 16.00663,32 24.84398,32 32,24.843981 32,16.006632 32,7.1692841 24.84398,.0000019037492 16.00663,.0000019037492 z M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><path style=\"fill:%pen%;fill-opacity:%penOpacity%\" d=\"M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><text x=\"16\" y=\"20\" font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" fill=\"%textPen%\" textContent = \"%textContent%\">%text%</text></svg>";
            offset = QPoint(-16, -16);
            break;
    }

    svgTemplate.replace("%text%", text, Qt::CaseSensitive);
    svgTemplate.replace("%pen%", pen.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%penOpacity%", QString::number(pen.color().alpha() / 255.0), Qt::CaseSensitive);
    svgTemplate.replace("%textPen%", textPen.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%brush%", brush.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%brushOpacity%", QString::number(brush.color().alpha() / 255.0), Qt::CaseSensitive);
    svgTemplate.replace("%textContent%", "", Qt::CaseSensitive);

    QSvgRenderer renderer(svgTemplate.toAscii());
    QPixmap pixmap(renderer.defaultSize());
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&pixmap);
    renderer.render(&painter);

    return qMakePair(pixmap, offset);
}
