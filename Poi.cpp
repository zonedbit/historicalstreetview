#include "Poi.h"

#include <qdebug.h>


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

    // Set a default image
    setPoiImage( QPixmap(":/default.png"));

    // TODO Warum geht es nicht und wird die Oberklasse ausgewaehlt
    connect(this, SIGNAL(selectedChanged(bool)), this, SLOT(mySlot()));
}


/*!
 * \brief Destructor
 * 
 *  Has no job
 */
Poi::~Poi(){

}

void Poi::mySlot(){
    qDebug() << "void Poi::setVisible()" ;
    this->setVisible(true);
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
