#include "MapView.h"
#include "MainWindow.h"
#include "MapItem.h"
#include "Station.h"
#include "PlayerItem.h"

#include "openeaagles/simulation/Simulation.h"
#include "openeaagles/simulation/Player.h"
#include "openeaagles/basic/List.h"
#include "openeaagles/basic/PairStream.h"
#include "openeaagles/basic/Pair.h"

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
   map = new MapItem(this);
   map->setSize(width(), height());
   map->setNorthUp(false);
   scene()->addItem(map);

   stn = 0;
   players.clear();

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

void MapView::clearPlayers()
{
   while (!players.isEmpty()) {
      PlayerItem* p = players.last();
      scene()->removeItem(players.last());
      players.removeLast();
      delete p;
   }
}

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
            // init the map
            if (map != 0 && !map->isInit()) {
               // grab our simulation, if we have one
               const Eaagles::Simulation::Simulation* sim = stn->getSimulation();
               if (sim != 0) {
                  map->initialize(sim->getRefLatitude(), sim->getRefLongitude(), 100);
               }
            }
         }
      }
   }
   else {
      // update the map and players
      if (map != 0) map->updateBG();
      const Eaagles::Simulation::Simulation* sim = stn->getSimulation();
      if (sim != 0) {
         const Eaagles::Basic::PairStream* stream = sim->getPlayers();
         if (stream != 0) {
            // go through the eaagles players and add new or update existing players
            const Eaagles::Basic::List::Item* item = stream->getFirstItem();
            while (item != 0) {
               const Eaagles::Basic::Pair* pair = (const Eaagles::Basic::Pair*)item->getValue();
               if (pair != 0) {
                  const Eaagles::Simulation::Player* ply = dynamic_cast<const Eaagles::Simulation::Player*>(pair->object());
                  if (ply != 0) {
                     // hold onto this player, just to be safe
                     ply->ref();
                     // see if we have a graphical representation of this player
                     bool found = false;
                     unsigned short id = ply->getID();
                     for (int i = 0; i < players.size() && !found; i++) {
                        found = (id == players[i]->getPlayerId());
                        // if we find it, update it
                        if (found) players[i]->refreshPlayer(ply);
                     }

                     // create a new player
                     if (!found) {
                        PlayerItem* p = new PlayerItem(this, map);
                        // default it to the f16
                        p->useDefaultImage(":f16.png", QSize(32, 32));
                        p->refreshPlayer(ply);
                        scene()->addItem(p);
                        players << p;
                     }

                     // all done with player
                     ply->unref();
                  }
               }
               item = item->getNext();
            }
            // now we have checked the sim against our players... let's check
            // our players against the sim, removing any old players
            for (int i = 0; i < players.size(); i++) {
               unsigned short id = players[i]->getPlayerId();
               const Eaagles::Simulation::Player* ply = sim->findPlayer(id);
               if (ply == 0) {
                  // player is gone, remove
                  PlayerItem* p = players[i];
                  scene()->removeItem(p);
                  players.removeAt(i);
                  delete p;
                  i--;
               }
            }
         }
      }
      // no sim, clear players
      else {
         clearPlayers();
      }
   }
}

}
