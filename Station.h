#ifndef __QTMAINSIM4_STATION_H__
#define __QTMAINSIM4_STATION_H__

#include "openeaagles/simulation/Station.h"
#include <QObject>
class QTimer;

namespace QtMainSim4 {

class Station;

// helper object for background station tasks
class StnTimerObject : public QObject
{
   Q_OBJECT

public:
   StnTimerObject(Station* station, QObject* parent = 0);
   ~StnTimerObject();

public slots:
   virtual void updateStation();

private:
   Station* stn;     // station pointer
   QTimer* bgTimer;  // timer
};

class Station : public Eaagles::Simulation::Station
{
   DECLARE_SUBCLASS(Station, Eaagles::Simulation::Station)
public:
   Station();

   // create our top level windows
   virtual int createWindow(int argc, char *argv[]);

private:
   StnTimerObject* timerObj;
};

}

#endif
