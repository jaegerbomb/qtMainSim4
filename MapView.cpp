#include "MapView.h"
#include "MainWindow.h"
#include "MapItem.h"
#include "Station.h"
#include <QTimer>
#include <iostream>
#include <QResizeEvent>

namespace QtMainSim4 {

MapView::MapView(QWidget* parent)
   : QGraphicsView(parent)
{
   QGraphicsScene* myScene = new QGraphicsScene(this);
   myScene->setItemIndexMethod(QGraphicsScene::NoIndex);
   myScene->setSceneRect(-(width()/2), -(height()/2), width(), height());
   setScene(myScene);

   // now add the map item
   map = new MapItem();
   map->setSize(width(), height());
   map->setNorthUp(false);
   scene()->addItem(map);



   stn = 0;

   // setup a timer to call background refresh
   QTimer* bgTimer = new QTimer(this);
   connect(bgTimer, SIGNAL(timeout()), this, SLOT(refreshView()));
   bgTimer->start(20);
}

MapView::~MapView()
{
   if (stn != 0) {
      stn->unref();
      stn = 0;
   }
}

void MapView::resizeEvent(QResizeEvent* event)
{
   if (map != 0) map->setSize(event->size().width(), event->size().height());
   QGraphicsView::resizeEvent(event);
}

Lee - almost there... just need to get the ref lat / lon stuff updated.  Go with
the qtGdalMapTest as a reference point, and get that drawing this way first,
before you start trying to add players and the such.

void MapView::refreshView()
{
   // find our station
   if (stn == 0) {
      // and get the station from it, so we can set some defaults up
      QWidget* wid = dynamic_cast<QWidget*>(parent());
      MainWindow* win = dynamic_cast<MainWindow*>(wid);
      // keep checking our parent(s) until we find the main window.
      while (win == 0 && wid != 0) {
         wid = dynamic_cast<QWidget*>(wid->parent());
         if (wid != 0) win = dynamic_cast<MainWindow*>(wid);
      }
      if (win != 0) {
         stn = win->getStation();
         if (stn != 0) {
            stn->ref();
         }
      }
   }
   else {
      // update map!
      if (map != 0) map->updateBG();
   }
}

}
