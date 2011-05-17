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

    void setDescription( const QString& description);
    void setBriefDescription( const QString& briefDescription);

    const QString& getDescription(){return m_description;};
    const QString& getBriefDescription(){return m_brief_description;};

public slots:
    virtual void mySlot();


private:
    QString m_description;
    QString m_brief_description;


};

#endif // POI_H
