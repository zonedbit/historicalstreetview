#ifndef POI_H
#define POI_H

#define SVG_TAMPLATE "<svg><path style=\"fill:%brush%;fill-opacity:%brushOpacity%\" d=\"M 16.00663,.0000019037492 C 7.1692812,.0000019037492 0,7.1692841 0,16.006632 0,24.843981 7.1692812,32 16.00663,32 24.84398,32 32,24.843981 32,16.006632 32,7.1692841 24.84398,.0000019037492 16.00663,.0000019037492 z M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><path style=\"fill:%pen%;fill-opacity:%penOpacity%\" d=\"M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><text x=\"16\" y=\"20\" font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" fill=\"%textPen%\" textContent = \"%textContent%\">%text%</text></svg>"

#include <QGeoMapPixmapObject>
#include <QString>
#include <QPair>
#include <QPen>
#include <QBrush>

QTM_USE_NAMESPACE

class Poi : public QGeoMapPixmapObject
{
public:
    explicit Poi(const QGeoCoordinate & coordinate, const QPoint & offset, const QPixmap & pixmap);
    explicit Poi(const QGeoCoordinate & coordinate );

    ~Poi();

    QPair<QPixmap, QPoint> makeStandardMarker(const QPen    & pen      = QPen(),
                                              const QPen    & textPen  = QPen(QColor(Qt::white)),
                                              const QBrush  & brush    = QBrush(QColor(Qt::darkCyan)));
		
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


public:
    static int m_cnt;

private:
    QString m_description;        /*!< \brief long description of the POI */
    QString m_brief_description;  /*!< \brief brief description of the POI
                                              is used as the title for the window */


    /*!
     * \brief The photo of the poi
     *
     * \attention To scale the image to a proper size,
     *            read access is necessary
     */
    QPixmap m_poi_image;


    const QString m_svgTemplate;
    const QPoint  m_offset;

    //offset = QPoint(-16, -16);
    //= "<svg><path style=\"fill:%brush%;fill-opacity:%brushOpacity%\" d=\"M 16.00663,.0000019037492 C 7.1692812,.0000019037492 0,7.1692841 0,16.006632 0,24.843981 7.1692812,32 16.00663,32 24.84398,32 32,24.843981 32,16.006632 32,7.1692841 24.84398,.0000019037492 16.00663,.0000019037492 z M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><path style=\"fill:%pen%;fill-opacity:%penOpacity%\" d=\"M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><text x=\"16\" y=\"20\" font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" fill=\"%textPen%\" textContent = \"%textContent%\">%text%</text></svg>";
};

#endif // POI_H
