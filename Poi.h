#ifndef POI_H
#define POI_H


#include <QGeoMapPixmapObject>
#include <QString>
#include <QPair>
#include <QPen>
#include <QBrush>

class InfoPoi;

QTM_USE_NAMESPACE

class Poi : public QGeoMapPixmapObject
{
    Q_OBJECT
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
    virtual void onSelection();

private slots:
    virtual void onDestroyed();


public:
    static int m_cnt;

private:
    InfoPoi *m_ptr_poi_ui;
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
};

#endif // POI_H
