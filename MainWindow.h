#ifndef __QTMAINSIM4_MAINWINDOW_H__
#define __QTMAINSIM4_MAINWINDOW_H__

#include <QMainWindow>
#include "ui_MainWindow.h"

namespace QtMainSim4 {

class Station;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
   Q_OBJECT

public:
   MainWindow(Station* stn, QWidget* parent = 0);
   ~MainWindow();

   const Station* getStation() const;
   Station* getStation();

public slots:
   // update ourselves
   virtual void refreshWindow();

protected:
   virtual void closeEvent(QCloseEvent* event);

private:
   Station* stn;        // holds our station for quick reference
};

inline Station* MainWindow::getStation()     { return stn; }
inline const Station* MainWindow::getStation() const  { return stn; }

}

#endif
