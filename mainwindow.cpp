/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "MapWidget.h"
#include "Poi.h"

#include <QApplication>
#include <QMessageBox>
#include <QGeoCoordinate>
#include <QProcessEnvironment>
#include <QUrl>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QTimer>
#include <QAction>
#include <QMenuBar>
#include <qnetworkconfigmanager.h>
#include <QSvgRenderer>
#include <QPainter>
#include <QHBoxLayout>


QTM_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        m_serviceProvider(0),
        m_mapWidget(0),m_myPositionMarker(0),locationDataSource(0)
{
    setWindowTitle(tr("Historical Street View"));

    QGraphicsScene *sc = new QGraphicsScene;

    m_qgv = new QGraphicsView(sc, this);
    m_qgv->setVisible(true);
    m_qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_slider = new QSlider(Qt::Vertical, this);
    m_slider->setTickInterval(1);
    m_slider->setTickPosition(QSlider::TicksBothSides);
    m_slider->setVisible(false);

    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));

     QHBoxLayout* layout = new QHBoxLayout();

    layout->addWidget(m_slider);
    layout->addWidget(m_qgv);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    setCentralWidget(widget);

    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);

    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        QMessageBox::information(this, tr("Map Viewer"), tr(
                                     "Available Access Points not found."));
        return;
    }

    //set callbacks and start internet connection
    m_session = new QNetworkSession(cfg, this);
    connect(m_session, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    connect(m_session,SIGNAL(error(QNetworkSession::SessionError)),this,SLOT(error(QNetworkSession::SessionError)));
    m_session->open();

}



MainWindow::~MainWindow()
{
    delete m_serviceProvider;
    delete locationDataSource;
}

void MainWindow::resizeEvent(QResizeEvent* /*event*/)
{
    m_qgv->setSceneRect(QRectF(QPointF(0.0, 0.0), m_qgv->size()));
    if (m_mapWidget)
        m_mapWidget->resize(m_qgv->size());
}

void MainWindow::showEvent(QShowEvent* /*event*/)
{
    resizeEvent(0);
}

void MainWindow::networkSessionOpened()
{
    QString urlEnv = QProcessEnvironment::systemEnvironment().value("http_proxy");
    if (!urlEnv.isEmpty()) {
        QUrl url = QUrl(urlEnv, QUrl::TolerantMode);
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(url.host());
        proxy.setPort(url.port(8080));
        QNetworkProxy::setApplicationProxy(proxy);
    } else
        QNetworkProxyFactory::setUseSystemConfiguration(true);

    //get provider, we are hardcoding it to nokia
    setProvider("nokia");
    //set up the map widget
    setupMap();
}

void MainWindow::error(QNetworkSession::SessionError error)
{
    if (error == QNetworkSession::UnknownSessionError) {
        if (m_session->state() == QNetworkSession::Connecting) {
            QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
            msgBox.setText("This application requires network access to function.");
            msgBox.setInformativeText("Press Cancel to quit the application.");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Retry);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Retry) {
                QTimer::singleShot(0, m_session, SLOT(open()));
            } else if (ret == QMessageBox::Cancel) {
                close();
            }
        }
    } else if (error == QNetworkSession::SessionAbortedError) {
        QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
        msgBox.setText("Out of range of network.");
        msgBox.setInformativeText("Move back into range and press Retry, or press Cancel to quit the application.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Retry);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Retry) {
            QTimer::singleShot(0, m_session, SLOT(open()));
        } else if (ret == QMessageBox::Cancel) {
            close();
        }
    }
}

void MainWindow::setProvider(QString providerId)
{
    if (m_serviceProvider)
        delete m_serviceProvider;
    m_serviceProvider = new QGeoServiceProvider(providerId);
    if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("MapViewer Example"), tr(
                                     "Unable to find the %1 geoservices plugin.").arg(providerId));
        qApp->quit();
        return;
    }

    m_mapManager = m_serviceProvider->mappingManager();
}

void MainWindow::setupMap()
{
    m_mapWidget = new MapWidget(m_mapManager,QGeoCoordinate(51.5,7.5));
    m_qgv->scene()->addItem(m_mapWidget);
    m_mapWidget->setZoomLevel(10);

    m_slider->setMaximum(m_mapManager->maximumZoomLevel());
    m_slider->setMinimum(m_mapManager->minimumZoomLevel());
    m_slider->setSliderPosition(m_mapWidget->zoomLevel());

    QObject::connect(m_mapWidget, SIGNAL(zoomLevelChanged(qreal)), this, SLOT(mapZoomLevelChanged(qreal)));

    setupMenu();
    resizeEvent(0);
}

void MainWindow::setupMenu()
{

    QAction* doPosAct = new QAction("Start/Stop position  monitor", this);
    connect(doPosAct, SIGNAL(triggered(bool)), this, SLOT(startStopPositionMonitor()));
    menuBar()->addAction(doPosAct);

    QAction* zomAction = new QAction(tr("Show/Hide zoom"), this);
    QObject::connect(zomAction, SIGNAL(triggered(bool)), this, SLOT(hideShowZoom()));
    menuBar()->addAction(zomAction);

    QAction* addPOIAction = new QAction(tr("Add POI"), this);
    QObject::connect(addPOIAction, SIGNAL(triggered(bool)), this, SLOT(addPOI()));
    menuBar()->addAction(addPOIAction);
}


void MainWindow::startStopPositionMonitor()
{
    // Obtain the location data source if it is not obtained already
    if (!locationDataSource)
    {
        locationDataSource = QGeoPositionInfoSource::createDefaultSource(this);
        // Whenever the location data source signals that the current
        // position is updated, the positionUpdated function is called
        connect(locationDataSource, SIGNAL(positionUpdated(QGeoPositionInfo)),this, SLOT(positionUpdated(QGeoPositionInfo)));

        // Start listening for position updates
        locationDataSource->startUpdates();
    }else{
        locationDataSource->stopUpdates();
        disconnect(locationDataSource, SIGNAL(positionUpdated(QGeoPositionInfo)),this, SLOT(positionUpdated(QGeoPositionInfo)));
        delete locationDataSource;
        locationDataSource = 0;

        if(m_myPositionMarker){
            m_mapWidget->removeMapObject(m_myPositionMarker);
            m_myPositionMarker = 0;
        }
    }
}

void MainWindow::hideShowZoom()
{
    if(m_slider){
        m_slider->setVisible(m_slider->isHidden());
    }
}

void MainWindow::sliderValueChanged(int zoomLevel)
{
    if(m_mapWidget){
       m_mapWidget->setZoomLevel(zoomLevel);
    }
}

void MainWindow::mapZoomLevelChanged(qreal zoomLevel)
{
    if(m_slider){
        m_slider->setSliderPosition(qRound(zoomLevel));
    }
}


void MainWindow::positionUpdated(QGeoPositionInfo geoPositionInfo)
{
    if (geoPositionInfo.isValid())
    {
        // Save the position information into a member variable
        myPositionInfo = geoPositionInfo;

        // Get the current location as latitude and longitude
        QGeoCoordinate geoCoordinate = geoPositionInfo.coordinate();
        m_mapWidget->setCenter(geoCoordinate);

        if(m_myPositionMarker){
            m_myPositionMarker->setCoordinate(geoCoordinate);
        }else{
            m_myPositionMarker = new QGeoMapPixmapObject(geoCoordinate, QPoint(-20,-20), QPixmap(":/position.png"));
            m_mapWidget->addMapObject(m_myPositionMarker);
        }
    }
}

// Methode um die POI auf der Karte anzuzeigen
void MainWindow::addPOI()
{

    QUrl qurl;
    qurl.setUrl("http://193.25.22.137:4713/HistMapServerREST/resources/imagebases/");
    fetchJSON(qurl);


    qDebug()  << "void MainWindow::addPOI()" ;

    /*
    static int cnt = 0;
    ++cnt;
    QString strCnt;
    strCnt.setNum(cnt,10);

    // Koordinate fuer den Marker
    QGeoCoordinate coor(m_mapWidget->center());


    Poi *pix = new Poi(coor);
    pix->setBriefDescription("The Poi " + strCnt);
    pix->setDescription("This is a test text");
    pix->setPoiImage(QPixmap(":/default.png"));

    // Marker zur Karte Setzen
    m_mapWidget->addMapObject(pix);
    */


    return ;
}

void MainWindow::fetchJSON(QUrl url)
{
    if ( url.isValid() )
    {
       QNetworkAccessManager *_accessManager = new QNetworkAccessManager(this);
       connect(_accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
       _accessManager->get(QNetworkRequest(url));
    }
    else
    {
        qDebug() << "URL is not valid";
    }

    return ;
}


void MainWindow::replyFinished( QNetworkReply* reply )
{
    if ( reply->error() != QNetworkReply::NoError )
    {
        qDebug() << "Request failed, " << reply->errorString();
        emit finished(false);
        return;
    }

    qDebug() << "Request succeeded";
    bool ok = processJSON( reply );
    emit finished( ok );
}

bool MainWindow::processJSON( QNetworkReply* reply )
{
    QByteArray result = reply->readAll();

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + QString(result) + ")");

    if (sc.property("imagebase").isArray())
    {
        QScriptValueIterator it(sc.property("imagebase"));
        while (it.hasNext())
        {
            it.next();

            QString lons(it.value().property("longitude").toString().toStdString().c_str());
            QString lats(it.value().property("latitude").toString().toStdString().c_str());

            double lat = lats.toDouble();
            double lon = lons.toDouble();

            qDebug() << lat;
            qDebug() << lon;

            QGeoCoordinate coor(lat,
                                lon
                                );

            Poi *pix = new Poi(coor);

            qDebug() << it.value().property("id").toString().toStdString().c_str();
            qDebug() << it.value().property("description").toString().toStdString().c_str();

            pix->setBriefDescription(it.value().property("id").toString().toStdString().c_str());
            pix->setDescription(it.value().property("description").toString().toStdString().c_str());

            pix->setPoiImage(QPixmap(":/default.png"));

            pix->setImageUrl(it.value().property("imageUrl").toString().toStdString().c_str());

            // Marker zur Karte Setzen
            m_mapWidget->addMapObject(pix);


        }
    }

    return true;
}
