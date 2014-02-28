#include <QApplication>
#include "MainWindow.h"
#include "Station.h"

#include "openeaagles/basic/Parser.h"
#include "openeaagles/basic/Factory.h"
#include "openeaagles/basic/Pair.h"
#include "openeaagles/simulation/Factory.h"

namespace QtMainSim4 {

// The main station
Station* station = 0;

// our class factory
static Eaagles::Basic::Object* factory(const char* name)
{
    Eaagles::Basic::Object* obj = 0;

    // This test ...
    if ( strcmp(name, Station::getFactoryName()) == 0 ) {
        obj = new Station;
    }

    else {
        if (obj == 0) obj = Eaagles::Simulation::Factory::createObj(name);
        if (obj == 0) obj = Eaagles::Basic::Factory::createObj(name);
    }
    return obj;
}


// build a display
static void builder(const char* const testFileName)
{
    // Read the description file
    int errors = 0;
    Eaagles::Basic::Object* q1 = Eaagles::Basic::lcParser(testFileName, factory, &errors);
    if (errors > 0) {
        std::cerr << "Errors in reading file: " << errors << std::endl;
        exit(1);
    }

    // Set 'station' to our basic description object.
    station = 0;
    if (q1 != 0) {

        // When we were given a Pair, get the pointer to its object.
        Eaagles::Basic::Pair* pp = dynamic_cast<Eaagles::Basic::Pair*>(q1);
        if (pp != 0) {
           std::cout << "Form: " << *pp->slot() << std::endl;
            q1 = pp->object();
        }

        // What we should have here is the description object and
        // it should be of type 'Station'.
        station = dynamic_cast<Station*>(q1);
    }

    // Make sure we did get a valid object (we must have one!)
    if (station == 0) {
        std::cout << "Invalid description file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    //station->serialize(std::cout);
}

int main(int argc, char *argv[])
{
   const char* fileName = "test.edl";

   for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i],"-f") == 0) {
         fileName = argv[++i];
      }
   }

   char* funcCall = 0;
   char fileNames[80];

#if WIN32
   funcCall = "mcpp.exe ";
#else
   funcCall = "cpp ";
#endif
   strcpy(fileNames, funcCall);
   strcat(fileNames, "inputs/");
   if (strlen(fileName) < 60){
      strcat(fileNames, fileName);
      strcpy(fileNames + strlen(fileNames)-2, "pp");
   }
   strcat(fileNames, ">");
   strcat(fileNames, fileName);
   funcCall = fileNames;
   std::cout << "Precompiling: " << fileName << std::endl;
   std::cout << funcCall << std::endl;
   system(funcCall);

   builder(fileName);

   // prime the station
   station->event(Eaagles::Basic::Component::RESET_EVENT);

   // create the time critical process
   station->createTimeCriticalProcess();
   station->createWindow(argc, argv);

   return 0;}

}

int main(int argc, char* argv[]) {
   return QtMainSim4::main(argc, argv);
}
