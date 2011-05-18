#ifndef POI_H
#define POI_H

#include <QGeoMapPixmapObject>
#include <QString>

QTM_USE_NAMESPACE

class Poi : public QGeoMapPixmapObject
{
public:
    explicit Poi(const QGeoCoordinate & coordinate, const QPoint & offset = QPoint( 0, 0 ), const QPixmap & pixmap = QPixmap());

    ~Poi();

		
   /*-----------------------------------------------------------------------------
    *  Setter
    *-----------------------------------------------------------------------------*/
    void setDescription      ( const QString& description);
    void setBriefDescription ( const QString& briefDescription);
    void setPoiImage         ( const QPixmap& poiImagent, int width=350, int height=350);

    /*-----------------------------------------------------------------------------
     *  Simple getter
     *-----------------------------------------------------------------------------*/
    const QString& getDescription()      const {return m_description;};
    const QString& getBriefDescription() const {return m_brief_description;};
    const QPixmap& getPoiImage()         const {return m_poi_image;};

public slots:
    virtual void mySlot();


private:
    QString m_description;        /*!< \brief long description of the POI */
    QString m_brief_description;  /*!< \brief brief description of the POI
                                              is used as the title for the window */
																							used as the title for the window */

    /*!
     * \brief The photo of the poi
     *
     * \attention To scale the image to a proper size,
     *            read access is necessary
     */
    QPixmap m_poi_image;


};

#endif // POI_H
