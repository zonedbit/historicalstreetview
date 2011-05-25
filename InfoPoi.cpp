#include "InfoPoi.h"
#include "ui_InfoPoi.h"

#include <QDebug>
#include <QPicture>

InfoPoi::InfoPoi(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::InfoPoi)
{
    ui->setupUi(this);
}

InfoPoi::InfoPoi(const Poi* poi, QDialog *parent) :
    QDialog(parent),
    ui(new Ui::InfoPoi)
{
    // Must be the first call
    ui->setupUi(this);

    // Setup the window
    this->setWindowTitle(poi->getBriefDescription());

    // Text in the description field
    ui->description->setText(poi->getDescription());



    QUrl url(poi->getImageURL());

    qDebug() << poi->getImageURL();

    http = new QHttp(this);

    qDebug() << "connect";

    connect(http, SIGNAL(requestFinished(int, bool)),this, SLOT(finished(int, bool)));
    buffer = new QBuffer(&bytes);
    buffer->open(QIODevice::WriteOnly);
    http->setHost(url.host());
    request=http->get (url.path(),buffer);

}

void InfoPoi::finished(int code, bool error)
{
    qDebug() << "Image loaded";

    qDebug() << code;
    qDebug() << error;

    if(code == request)
    {
        qDebug() << "code == request";

        QImage img;

        qDebug() << "isNull" + bytes.isNull();
        qDebug() << "isEmpty" + bytes.isEmpty();
        qDebug() << bytes;

        img.loadFromData(bytes);
        ui->image->setPixmap(QPixmap::fromImage(img));
    }
}

InfoPoi::~InfoPoi()
{
    qDebug()  << "InfoPoi::~InfoPoi()";

    if(ui != 0)
    {
        delete ui;
    }
}
