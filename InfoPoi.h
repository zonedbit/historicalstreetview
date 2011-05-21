#ifndef INFOPOI_H
#define INFOPOI_H

#include "Poi.h"

#include <QDialog>


namespace Ui {
    class InfoPoi;
}


class InfoPoi : public QDialog
{
    Q_OBJECT

public:
    explicit InfoPoi(QDialog *parent = 0);
    explicit InfoPoi(const Poi* poi, QDialog *parent = 0);
    ~InfoPoi();

private:
    Ui::InfoPoi *ui;
};

#endif // INFOPOI_H
