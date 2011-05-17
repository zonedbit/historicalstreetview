#ifndef INFOPOI_H
#define INFOPOI_H

#include "Poi.h"

#include <QWidget>


namespace Ui {
    class InfoPoi;
}


class InfoPoi : public QWidget
{
    Q_OBJECT

public:
    explicit InfoPoi(QWidget *parent = 0);
    explicit InfoPoi(const Poi* poi, QWidget *parent = 0);
    ~InfoPoi();

private:
    Ui::InfoPoi *ui;
};

#endif // INFOPOI_H
