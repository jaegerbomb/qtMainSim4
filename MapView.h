#ifndef __QTMAINSIM4_MAPVIEW_H__
#define __QTMAINSIM4_MAPVIEW_H__

#include <QGraphicsView>

namespace QtMainSim4 {
class Station;
class MapItem;

class MapView : public QGraphicsView
{
   Q_OBJECT

public:
   MapView(QWidget* parent = 0);
   ~MapView();


public slots:
   // refresh our view graphics
   virtual void refreshView();

protected:
   virtual void resizeEvent(QResizeEvent *event);


private:
   Station* stn;     // holds a station for reference
   MapItem* map;     // our map
};

}

#endif
