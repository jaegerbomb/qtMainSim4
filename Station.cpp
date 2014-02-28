#include "Station.h"
#include "MainWindow.h"
#include <QTimer>

namespace QtMainSim4 {

IMPLEMENT_EMPTY_SLOTTABLE_SUBCLASS(Station, "QtStation")
EMPTY_SERIALIZER(Station)

EMPTY_CONSTRUCTOR(Station)
EMPTY_COPYDATA(Station)
EMPTY_DELETEDATA(Station)

// ---
// Class - StnTimerObject
// ---
StnTimerObject::StnTimerObject(Station* station, QObject* parent) : QObject(parent)
{
   stn = station;
   if (stn != 0) stn->ref();
   // create a timer for our background thread
   bgTimer = new QTimer(this);
   connect(bgTimer, SIGNAL(timeout()), this, SLOT(updateStation()));
   // 20 milliseconds
   bgTimer->start(20);
}

StnTimerObject::~StnTimerObject()
{
   if (stn != 0) {
      stn->unref();
      stn = 0;
   }
}

void StnTimerObject::updateStation()
{
   if (stn != 0) {
      stn->updateData(20.0f/1000.0f);
   }
}


// create our window and startup any timers needed
int Station::createWindow(int argc, char* argv[])
{
   QApplication a(argc, argv);

   // start our background timer, which will call our update data
   timerObj = new StnTimerObject(this);


   // default flags for now
   MainWindow* win = new MainWindow(this);
   win->show();

   return a.exec();
}

}
