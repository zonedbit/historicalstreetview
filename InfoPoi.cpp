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

    // Set the image to the ui
    ui->image->setPixmap(poi->getPoiImage());

}

InfoPoi::~InfoPoi()
{
    qDebug()  << "InfoPoi::~InfoPoi()";

    if(ui != 0){
        delete ui;
    }
}
