#include "LiseHelper.hh"
#include <iostream>

using namespace std;

int main()
{
  vector<string> items = LiseHelper::ListItem();
  for(const string &item : items) {
    LiseHelper lise(item);
    cout << lise.GetPath() << "\t"
         << lise.GetBeam() << "\t"
         << lise.GetTarget() << endl;
  }

  return 0;
}
