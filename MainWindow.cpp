#include "MainWindow.h"
#include "Station.h"

#include <QTimer>
#include <iostream>

namespace QtMainSim4 {

MainWindow::MainWindow(Station* station, QWidget *parent) :
QMainWindow(parent)
{
   stn = station;
   if (stn != 0) stn->ref();
   setupUi(this);
   // Only use this if you want to refresh the top level window... which
   // usually doesn't draw dynamically.  If you do use this, make it not
   // update that quickly to avoid performance hits with your viewer.
//   QTimer* bgTimer = new QTimer(this);
//   connect(bgTimer, SIGNAL(timeout()), this, SLOT(refreshWindow()));
//   // not very of
//   bgTimer->start(2);
}

MainWindow::~MainWindow()
{
   if (stn != 0) {
      stn->unref();
      stn = 0;
   }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
   // tell our station to shutdown
   if (stn != 0 && !stn->isShutdown()) stn->event(Eaagles::Basic::Component::SHUTDOWN_EVENT);
   QMainWindow::closeEvent(event);
}

void MainWindow::refreshWindow()
{
}

}
