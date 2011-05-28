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

    http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port() == -1 ? 0 : url.port());
    connect(http, SIGNAL(requestFinished(int, bool)),this, SLOT(httpRequestFinished(int, bool)));
    http->get(poi->getImageURL());

}

void InfoPoi::httpRequestFinished(int code, bool error)
{
    qDebug() << "Image loaded";
    qDebug() << code;
    qDebug() << request;
    qDebug() << error;

    qDebug() << "code == request";

    QByteArray picture = http->readAll();

    if(!error)
    {
        if (!picture.isNull() && picture.size() > 0)
        {
            QImage image;

            if (image.loadFromData(picture))
            {
                QSize thumbnailSize = QSize(300, 300);
                ui->image->setPixmap(QPixmap::fromImage(image.scaled(thumbnailSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
                ui->image->update();
            }
        }
        else
        {
            qDebug() << "Picture is null or has no size";
        }
    }
    else
    {
        qDebug() << "An error has occurend during the download of the image";
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
