#include "InfoPoi.h"
#include "ui_InfoPoi.h"

InfoPoi::InfoPoi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoPoi)
{
    ui->setupUi(this);
}

InfoPoi::InfoPoi(Poi* poi, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoPoi)
{

    // Must be the first call
    ui->setupUi(this);

    // Setup the window
    this->setWindowTitle(poi->getBriefDescription());

    // Text in the description field
    ui->textBrowser->setText(poi->getDescription());
}

InfoPoi::~InfoPoi()
{
    delete ui;
}
