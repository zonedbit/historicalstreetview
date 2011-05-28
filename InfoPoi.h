#ifndef INFOPOI_H
#define INFOPOI_H

#include "Poi.h"

#include <QDialog>
#include <QBuffer>
#include <QByteArray>
#include <QHttp>
#include <QDebug>

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
    QBuffer *buffer;
    QByteArray bytes;
    QHttp *http;
    int request;
    Ui::InfoPoi *ui;

private slots:
    void httpRequestFinished(int code, bool error);
};

#endif // INFOPOI_H
