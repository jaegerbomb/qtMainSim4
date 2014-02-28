#include "MapItem.h"
#include "openeaagles/basic/units/Angles.h"

#include <QPainter>
#include <sstream>

namespace QtMainSim4 {

MapItem::MapItem(MapView* v, QGraphicsItem *parent)
     : QGraphicsItem(parent)
{

   view = v;
   bRect.setLeft(-5000);
   bRect.setWidth(10000);
   bRect.setTop(-5000);
   bRect.setHeight(10000);

   refLat = 0;
   refLon = 0;
   range = 500;
   pixNSRes = 1;
   pixWERes = 1;
   gridVis = false;

   // non-equitorial map math
   cosineLatReference = 1.0;
   northUp = true;
   setHeading(0);
}

QRectF MapItem::boundingRect() const
{
   return bRect;
}

MapItem::~MapItem()
{
}

// ---
// setSize() - called by the parent view (or whatever, this will set our bounding rectangle size
// ---
void MapItem::setSize(const int w, const int h)
{
   bRect.setRect(-(w/2), -(h/2), w, h);
}

// ---
// updateBG() - background call
// ---
void MapItem::updateBG()
{
   // width / height ratios and pixels
   double vpRatio = 1;

   vpRatio = double(bRect.width()) / double(bRect.height());
   pixNSRes = range / double(bRect.height());
   pixWERes = (range / double(bRect.width())) * vpRatio;

   update();
}

// ---
// moveMap() - does what it says
// ---
void MapItem::moveMap(const double deltaN, const double deltaE)
{
   double lat = 0;
   double lon = 0;
   pixelsToLL(deltaE, deltaN, lon, lat);

   // update our lat/lon
   if (lon > 180) lon -= 360;
   else if (lon < -180) lon += 360;
   // set our new reference lat/lon
   setRefLat(lat);
   setRefLon(lon);
}

// ---
// setRefLat()
// ---
bool MapItem::setRefLat(const double lat)
{
   bool ok = false;
   // nothing more than 70 degrees north or south will suffice
   if (lat <= 90 && lat >= -90) {
      refLat = lat;
      cosineLatReference = cos(Eaagles::Basic::Angle::D2RCC * refLat);
      ok = true;
   }
   return ok;
}

// ---
// setRefLon()
// ---
bool MapItem::setRefLon(const double lon)
{
   bool ok = false;
   if (lon <= 180 && lon >= -180) {
      refLon = lon;
      ok = true;
   }

   return ok;
}

// ---
// setRange()
// ---
bool MapItem::setRange(const double x)
{
   bool ok = (x > 0);
   if (ok) range = x;
   return ok;
}

// ---
// paint() - paints!
// ---
void MapItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
   // the only thing this map will provide is drawing the grid... if  you don't wish
   // to draw it, then override paint in your derived class and draw when necessary (if at all).
   drawGrid(painter);
}

// ---
// drawGrid() - draws the overlay grid lines
// ---
void MapItem::drawGrid(QPainter* painter)
{
   // smaller font for easy viewing
   QFont font(painter->font());
   font.setPointSize(10);
   painter->setFont(font);

   QFontMetrics fm = painter->fontMetrics();
   // setup our pen
   // the color of our pen will be the complement of the strength of our intensity
   QPen pen(Qt::white, 2.0f);
   painter->setPen(pen);

   if (heading > 0 && !northUp) setRotation(heading);

   // get the nearest starting lat and lon lines to draw
   int centerLat = Eaagles::nint(refLat);
   int centerLon = Eaagles::nint(refLon);

   // calculate the number of latitude lines
   int numLinesLat = (range / 60.0) + 1;

   // calculate EW range
   double rangeEW = 1;
   if (northUp) rangeEW = (bRect.width() * pixWERes);
   else rangeEW = (bRect.width() * pixWERes / cosineLatReference);
   int numLinesLon = (rangeEW / 60.0) + 1;

   int startLat = centerLat - (numLinesLat / 2);
   int endLat = centerLat + (numLinesLat / 2);
   // top side (or bottom, depending on hemisphere)
   double py = 0, px = 0;
   int wid = bRect.width();
   int hei = bRect.height();
   if (endLat >= 90) endLat = 90;
   if (startLat <= -90) startLat = -90;

   QVector<QLine> lines;

   for (int i = startLat; i <= endLat; i++) {
      llToPixels(i, 0, py, px);
      QLine line(-wid/2, py, wid, py);
      //painter->drawLine(-wid/2, py, wid, py);
      lines << line;
   }

   painter->drawLines(lines);
   lines.clear();

   int startLon = centerLon - (numLinesLon / 2);
   int endLon = centerLon + (numLinesLon / 2);
   // top side (or bottom, depending on hemisphere)
   for (int i = startLon; i <= endLon; i++) {
      llToPixels(0, i, py, px);
      QLine line(px, -hei / 2, px, hei / 2);
      //painter->drawLine(px, -hei / 2, px, hei / 2);
      lines << line;
   }

   painter->drawLines(lines);
   lines.clear();

   // now for the text
   QString string;
   double adjustHeight = fm.height();
   std::ostringstream stream;
   for (int i = startLat; i <= endLat; i++) {
      string.clear();
      stream.str("");
      int aI = abs(i);
      llToPixels(i, 0, py, px);
      if (i > 0) stream << "N" << aI;
      else if (i < 0) stream << "S" << aI;
      else stream << aI;
      string = stream.str().c_str();
      // Lee - have to use the unicode code for a degree symbol
      string.append(0x00B0);
      double adjustWidth = fm.width(string);
      painter->drawText(-(wid/2) + 5, py - adjustHeight, adjustWidth, adjustHeight, 0, string);
   }

   for (int i = startLon; i <= endLon; i++) {
      string.clear();
      stream.str("");
      llToPixels(0, i, py, px);
      int aI = abs(i);
      if (i > 180) aI = (180 - i) + 180;
      else if (i < -180) aI = (180 + i) + 180;
      if (i != 0 && i != 180 && i != -180) {
         if (i > 0 || i < -180) stream << "E" << aI;
         else if (i < 0 || i > 180) stream << "W" << aI;
         else stream << aI;
      }
      string = stream.str().c_str();
      // Lee - have to use the unicode code for a degree symbol
      string.append(0x00B0);
      double adjustWidth = fm.width(string);
      painter->drawText(px + 5, -hei/2, adjustWidth, adjustHeight, 0, string);
   }

}

// ---
// MAP MATH SECTIONS
// ---

// ---
// setHeading()
// ---
void MapItem::setHeading(const double x)
{
   double hdgRad = Eaagles::Basic::Angle::D2RCC * x;
   heading = x;
   headingSin = sin(hdgRad);
   headingCos = cos(hdgRad);
}

// ---
// pixelsToLL - converts pixels (x, y) to lat, lon coordinates
// ---
void MapItem::pixelsToLL(const double px, const double py, double &lon, double &lat) const
{
   // pixels to aircraft
   double acX = px * pixWERes;
   double acY = -py * pixNSRes;

   double east = 0.0f, north = 0.0f;
   if (northUp) {
      east = acX;
      north = acY;
   }
   else {
      east = (acX * headingCos) - (acY * headingSin);
      north = (acX * headingSin) + (acY * headingCos);
   }

   // ne to aircraft (if needed)
   lat = (north / 60.0f) + refLat;
   if (northUp) lon = (east / 60.0f) + refLon;
   else lon = (east / 60.0f / cosineLatReference) + refLon;
}

// ---
// llToPixels() - converts lat/lon position to screen x, y
// ---
void MapItem::llToPixels(const double lat, const double lon, double &py, double &px) const
{
   // first go from lat lon to NE
   double north = 0, east = 0;
   north = (lat - refLat) * 60.0f;
   if (!northUp) east = (lon - refLon) * 60.0f * cosineLatReference;
   else east = (lon - refLon) * 60.0f;

   double acX = 0;
   double acY = 0;
   if (northUp) {
      acX = east;
      acY = north;
   }
   else {
      acX = (east * headingCos) + (north * headingSin);
      acY = -(east * headingSin) + (north * headingCos);
   }

   px = acX / pixWERes;
   py = -(acY / pixNSRes);
}

}
