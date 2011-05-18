#include "InfoPoi.h"
#include "ui_InfoPoi.h"

#include <QPicture>

InfoPoi::InfoPoi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoPoi)
{
    ui->setupUi(this);
}

InfoPoi::InfoPoi(const Poi* poi, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoPoi)
{
    // Must be the first call
    ui->setupUi(this);

    // Setup the window
    this->setWindowTitle(poi->getBriefDescription());

    // Text in the description field
    ui->description->setText(poi->getDescription());

    // Set the image to the ui
    ui->image->setPixmap(QPixmap(":/default.png"));

}

InfoPoi::~InfoPoi()
{
    delete ui;
}
