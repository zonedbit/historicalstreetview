#include "InfoPoi.h"
#include "ui_InfoPoi.h"

InfoPoi::InfoPoi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoPoi)
{
    ui->setupUi(this);
}

InfoPoi::~InfoPoi()
{
    delete ui;
}
