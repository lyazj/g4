#include "LiseHelper.hh"
#include "LiseDetector.hh"
#include "LiseGenerator.hh"
#include <iostream>
#include <sys/stat.h>

using namespace std;

int main()
{
  string runpath = BASEDIR "/run";
  mkdir(runpath.c_str(), 0755);

  vector<string> items = LiseHelper::ListItem();
  for(const string &item : items) {
    LiseHelper *lise = new LiseHelper(item);
    cout << lise->GetPath() << "\t"
         << lise->GetBeam() << "\t"
         << lise->GetTarget() << endl;

    LiseGenerator generator((runpath + "/" + lise->GetBeam() + "_" + lise->GetTarget() + ".root").c_str(), 0, 300);
    generator.AddDetector(new LiseDetector(lise,  100));
    generator.AddDetector(new LiseDetector(lise,  300));
    generator.AddDetector(new LiseDetector(lise, 2000));
    generator.GenerateEvents(100000);

    delete lise;
  }

  return 0;
}
