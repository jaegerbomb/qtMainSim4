#include <QApplication>
#include "MainWindow.h"
#include "ui_MainWindow.h"

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   // default flags for now
   MainWindow* win = new MainWindow(0);

   Ui::MainWindow* myWindow = new Ui::MainWindow();
   myWindow->setupUi(win);
   // In order to cleanly delete and shutdown all graphics, make sure this attribute it set when shutting down
   win->setAttribute(Qt::WA_DeleteOnClose, true);
   win->show();

   return a.exec();
}
