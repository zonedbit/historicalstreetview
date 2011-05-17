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
#include "MapEvents.h"
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

#include <stdio.h>
#include <stdlib.h>


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
    m_mapWidget = new CircleAreaSelectWidget(m_mapManager,QGeoCoordinate(51.5,7.5));
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
    qDebug() << "map zoomlevel changed: " + QString::number(zoomLevel);

    if(m_slider){
        m_slider->setSliderPosition(qRound(zoomLevel));
    }
}



QPair<QPixmap, QPoint> MainWindow::makeStandardMarker(int shapeType, const QString & text, const QPen & pen, const QPen & textPen, const QBrush & brush)
{
    qDebug() << "makeStandardMarker: " + QString::number(shapeType);

    QString svgTemplate;
    QPoint offset;
    switch (shapeType) {
        case 0:
            svgTemplate = "<svg>"
                          "<path "
                          "style=\"fill:#000000;fill-opacity:.4\" "
                          "d=\"m 18.948,33.432051 c 0,1.41694 -2.238,2.567949 -5,2.567949 -2.762,0 -5,-1.151009 -5,-2.567949 0,-1.416939 2.238,-2.567948 5,-2.567948 2.762,-0.002 5,1.149994 5,2.567948\" "
                          "/>"
                          "<path "
                          "style=\"fill:#666\" "
                          "d=\"M 14 0 C 6.28 0 0 6.3348796 0 14.125 C 0 16.209809 0.4425 18.209911 1.3125 20.09375 L 1.84375 21.125 C 2.96275 23.084964 12.2565 32.190555 13.3125 33.21875 L 14 33.875 L 14.6875 33.21875 C 15.7435 32.188525 25.026 23.109909 26.125 21.1875 L 26.65625 20.15625 C 27.54825 18.225721 28 16.204734 28 14.125 C 28.001 6.3348796 21.72 0 14 0 z M 14 0.90625 C 21.175 0.90625 27 6.7640687 27 14 C 27 15.982294 26.55825 17.873541 25.78125 19.5625 L 25.28125 20.53125 C 24.22125 22.386669 14 32.375 14 32.375 C 14 32.375 3.77875 22.387684 2.71875 20.53125 L 2.21875 19.5625 C 1.43975 17.875571 1 15.982294 1 14 C 1 6.7640687 6.823 0.90625 14 0.90625 z \" "
                          "/>"
                          "<path "
                          "style=\"fill:#fff\" "
                          "d=\"M 14 0.90625 C 6.823 0.90625 1 6.7640687 1 14 C 1 15.982294 1.43975 17.875571 2.21875 19.5625 L 2.71875 20.53125 C 3.77875 22.387684 14 32.375 14 32.375 C 14 32.375 24.22125 22.386669 25.28125 20.53125 L 25.78125 19.5625 C 26.55825 17.873541 27 15.982294 27 14 C 27 6.7640687 21.175 0.90625 14 0.90625 z M 14 1.90625 C 20.616 1.90625 26 7.3294234 26 14 C 25.910217 16.115917 25.491329 18.196844 24.40625 20.03125 C 23.776565 21.248455 17.937 27.075427 14 30.9375 C 10.063 27.075427 4.206579 21.245609 3.59375 20.0625 C 2.614452 18.171896 1.90079 16.186259 2 14 C 2 7.3294234 7.383 1.90625 14 1.90625 z \" "
                          "/>"
                          "<path "
                          "style=\"fill:%brush%;fill-opacity:%brushOpacity%\" "
                          "d=\"m 13.996268,1.9105251 c -6.617,0 -12.001,5.4261873 -12.001,12.0967639 -0.09921,2.186259 0.631702,4.174018 1.611,6.064622 0.612829,1.183109 6.453,7.017707 10.39,10.87978 3.937,-3.862073 9.777315,-9.70013 10.407,-10.917335 1.085079,-1.834406 1.503217,-3.91115 1.593,-6.027067 0,-6.6705766 -5.384,-12.0967639 -12,-12.0967639 l 0,0 z\" "
                          "/>"
                          "<path style=\"fill:#ffffff;fill-opacity:0.7\" "
                          "d=\"M 20.968528,6.089997 C 17.785074,3.5240085 13.725682,2.5379787 11.919627,3.8924751 11.436514,4.2548035 11.151528,4.750748 11.073071,5.3665525 c 4.1092,0.6284223 7.111478,2.6511761 8.521564,4.4853881 2.388133,3.1064364 1.967904,3.9515754 1.967904,3.9515754 0.774274,-0.0387 1.422004,-0.242256 1.904023,-0.603757 1.806055,-1.354497 0.684944,-4.5441591 -2.498034,-7.109762 l 0,0 z\" "
                          "/>"
                          "<text "
                          "x=\"14\" y=\"19\" "
                          "font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" "
                          "fill=\"%textPen%\" "
                          "textContent = \"%textContent%\""
                          ">"
                          "%text%"
                          "</text>"
                          "</svg>";
            offset = QPoint(-14, -34);
            break;

        case 1:
            svgTemplate = "<svg><path style=\"fill:%brush%;fill-opacity:%brushOpacity%\" d=\"M 16.00663,.0000019037492 C 7.1692812,.0000019037492 0,7.1692841 0,16.006632 0,24.843981 7.1692812,32 16.00663,32 24.84398,32 32,24.843981 32,16.006632 32,7.1692841 24.84398,.0000019037492 16.00663,.0000019037492 z M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><path style=\"fill:%pen%;fill-opacity:%penOpacity%\" d=\"M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><text x=\"16\" y=\"20\" font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" fill=\"%textPen%\" textContent = \"%textContent%\">%text%</text></svg>";
            offset = QPoint(-16, -16);
            break;
    }

    svgTemplate.replace("%text%", text, Qt::CaseSensitive);
    svgTemplate.replace("%pen%", pen.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%penOpacity%", QString::number(pen.color().alpha() / 255.0), Qt::CaseSensitive);
    svgTemplate.replace("%textPen%", textPen.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%brush%", brush.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%brushOpacity%", QString::number(brush.color().alpha() / 255.0), Qt::CaseSensitive);
    svgTemplate.replace("%textContent%", "", Qt::CaseSensitive);

    QSvgRenderer renderer(svgTemplate.toAscii());
    QPixmap pixmap(renderer.defaultSize());
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&pixmap);
    renderer.render(&painter);

    return qMakePair(pixmap, offset);
}



void MainWindow::positionUpdated(QGeoPositionInfo geoPositionInfo)
{
    if (geoPositionInfo.isValid())
    {
        // We've got the position. No need to continue the listening.
        locationDataSource->stopUpdates();

        // Save the position information into a member variable
        myPositionInfo = geoPositionInfo;

        // Get the current location as latitude and longitude
        QGeoCoordinate geoCoordinate = geoPositionInfo.coordinate();
        qreal latitude = geoCoordinate.latitude();
        qreal longitude = geoCoordinate.longitude();
        qDebug() << "coordinate changed: " + QString("Latitude: %1 Longitude: %2").arg(latitude).arg(longitude);

        m_mapWidget->setCenter(geoCoordinate);

        if(m_myPositionMarker){
            m_myPositionMarker->setCoordinate(geoCoordinate);
        }else{
            QPair<QPixmap, QPoint> markerPair = makeStandardMarker(0,"Me", QPen(), QPen(QColor(Qt::white)), QBrush(QColor(Qt::darkCyan)));
            m_myPositionMarker = new QGeoMapPixmapObject(geoCoordinate,markerPair.second,markerPair.first);
            m_mapWidget->addMapObject(m_myPositionMarker);
        }
    }
}

// Methode um die POI auf der Karte anzuzeigen
void MainWindow::addPOI(){
    qDebug()  << "void MainWindow::addPOI()" ;

    static int cnt = 0;
    ++cnt;

    qDebug() << "cnt: " << cnt;

    QString markerTxt;
    markerTxt.setNum(cnt,10);

    // Koordinate fuer den Marker
    QGeoCoordinate coor(m_mapWidget->center());

    // Erzuegen des Markers
    QPair<QPixmap, QPoint> markerPair = makeStandardMarker(1, markerTxt, QPen(), QPen(QColor(Qt::white)), QBrush(QColor(Qt::darkCyan)));

    Poi *pix = new Poi(coor,markerPair.second,markerPair.first);
    pix->setBriefDescription("The Poi name");
    pix->setDescription("This is a test text");

    // TODO Warum geht das nicht?
//    connect(pix, SIGNAL(selectedChanged(bool)), pix, SLOT(mySlot(bool)));

    // Marker zur Karte Setzen
    m_mapWidget->addMapObject(pix);


    return ;
}
