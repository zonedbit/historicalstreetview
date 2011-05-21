#include "Poi.h"
#include "InfoPoi.h"

#include <qdebug.h>
#include <QSvgRenderer>
#include <QPainter>


int Poi::m_cnt = 0;

/*!
 * \brief Default Constructor 
 * 
 *  Detailed description starts here.
 *
 * \param coordinate The coordinates of the POI
 * \param offset     The offset of the POI
 * \param pixmap     The image to display the poi on the map
 * \exception Exceptionclasse_condition_for_Exception
 * \return
 * \see Class::Method(Attribut)
 */
Poi::Poi(const QGeoCoordinate & coordinate, const QPoint & offset, 
				 const QPixmap & pixmap)
        : QGeoMapPixmapObject(coordinate, offset, pixmap)
{
    qDebug() << "Poi::Poi(const QGeoCoordinate & coordinate, const QPoint & offset, const QPixmap & pixmap)";

    m_ptr_poi_ui = 0;

    // Set a default image
    setPoiImage( QPixmap(":/default.png"));

    // TODO Warum geht es nicht und wird die Oberklasse ausgewaehlt
    connect(this, SIGNAL(selectedChanged(bool)), this, SLOT(onSelection()));
}

/*!
 * \brief Default Constructor
 *
 *  Detailed description starts here.
 *
 * \param coordinate The coordinates of the POI
 * \param offset     The offset of the POI
 * \param pixmap     The image to display the poi on the map
 * \exception Exceptionclasse_condition_for_Exception
 * \return
 * \see Class::Method(Attribut)
 */
Poi::Poi(const QGeoCoordinate & coordinate)
{
    qDebug() << "Poi::Poi(const QGeoCoordinate & coordinate)";

    m_ptr_poi_ui = 0;

    this->setCoordinate(coordinate);

    QPair<QPixmap, QPoint> markerPair = makeStandardMarker();

    this->setOffset(markerPair.second);
    this->setPixmap(markerPair.first);


    // Set a default image
    setPoiImage( QPixmap(":/default.png"));

    // TODO Warum geht es nicht und wird die Oberklasse ausgewaehlt
    connect(this, SIGNAL(selectedChanged(bool)), this, SLOT(onSelection()));
}


/*!
 * \brief Destructor
 * 
 *  Has no job
 */
Poi::~Poi(){
    if(m_ptr_poi_ui){
        delete m_ptr_poi_ui;
    }
}

void Poi::onSelection(){
    qDebug() << "void Poi::onSelection()" << this->isSelected();

    if(this->isSelected()){

        if(m_ptr_poi_ui == 0){
            qDebug() << "m_ptr_poi_ui = new InfoPoi(this);";
            m_ptr_poi_ui = new InfoPoi(this);
        }

        if(m_ptr_poi_ui != 0){
            qDebug() << "m_ptr_poi_ui->show();";
            m_ptr_poi_ui->show();
            m_ptr_poi_ui->connect(m_ptr_poi_ui,SIGNAL(finished(int)),this,SLOT(onDestroyed()));
        }
    }

}

void Poi::onDestroyed(){
    qDebug() << "void Poi::onDestroyed()";
    this->setSelected(false);
//    if(m_ptr_poi_ui != 0){
//        delete m_ptr_poi_ui;
//    }
}

/*!
 * \brief simple setter
 *
 * \param description    reference to the string that contains
 * 			 the description
 */
void Poi::setDescription(const QString& description){
    m_description = description;
}



/*!
 * \brief simple setter
 *
 * \param briefDescription reference to the string that contains
 * 			   the brief description
 */
void Poi::setBriefDescription(const QString& briefDescription){
    m_brief_description = briefDescription;
}

/*!
 * \brief set the image to a Poi object
 * 
 *  This method set the image to a Poi object. Moreover,
 *  the image is scaled to a proper size, by default
 *  350x350.
 *
 * \param QPixmap& reference to the image object itself
 * \param width	the width of the image; default 350
 * \param height the height of the image; default 35o
 * \exception Exceptionclasse_condition_for_Exception
 * \return
 * \see Class::Method(Attribut)
 */
void Poi::setPoiImage(const QPixmap& poiImage, int width, int height){
    static QSize size(width,height);
    m_poi_image = poiImage.scaled(size);
}

QPair<QPixmap, QPoint> Poi::makeStandardMarker(const QPen & pen,
                                               const QPen & textPen,
                                               const QBrush & brush)
{


    ++m_cnt;
    QString text;
    text.setNum(m_cnt,10);

    QString   svgTemplate = "<svg><path style=\"fill:%brush%;fill-opacity:%brushOpacity%\" d=\"M 16.00663,.0000019037492 C 7.1692812,.0000019037492 0,7.1692841 0,16.006632 0,24.843981 7.1692812,32 16.00663,32 24.84398,32 32,24.843981 32,16.006632 32,7.1692841 24.84398,.0000019037492 16.00663,.0000019037492 z M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><path style=\"fill:%pen%;fill-opacity:%penOpacity%\" d=\"M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><text x=\"16\" y=\"20\" font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" fill=\"%textPen%\" textContent = \"%textContent%\">%text%</text></svg>";
    static const QPoint    offset (-16, -16);

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
