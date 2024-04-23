#include "LiseHelper.hh"
#include <iostream>
#include <fstream>
#include <TGraph.h>
#include <limits.h>

using namespace std;

namespace {

void ReadLiseMeta(istream &is, std::string &beam, std::string &target)
{
  string s;
  is >> s /* ! */ >> beam >> s /* range */ >> s /* in */ >> target;
  getline(is, s);
  getline(is, s);
  getline(is, s);
}

void ReadLiseData(istream &is, TGraph *&E2x, TGraph *&x2E)
{
  E2x = new TGraph;
  x2E = new TGraph;

  string s;
  double E, x;
  for(;;) {
    is
      >> s >> s  // 0 - [He-base] F.Hubert et al, AD&ND Tables 46(1990)1
      >> s >> s  // 1 - [H -base] J.F.Ziegler et al,Pergamon Press,NY(low energy)
      >> s >> s  // 2 - ATIMA 1.2 LS-theory (recommended for high energy)
      >> s >> s  // 3 - ATIMA 1.2 without LS-correction
      >> E >> x  // 4 - ATIMA 1.4 H.Weick, improved mean charge formula for HI
    ;
    if(!getline(is, s)) break;

    E2x->AddPoint(E, x);
    x2E->AddPoint(x, E);
  }

  if(E2x->GetN() == 0) {  // Refuse an empty table.
    delete E2x;
    delete x2E;
    E2x = x2E = nullptr;
  }
}

}  // namespace

LiseHelper::LiseHelper(const std::string &path)
{
  path_ = path;
  ifstream is(path_);
  ReadLiseMeta(is, beam_, target_);
  ReadLiseData(is, E2x_, x2E_);
}

LiseHelper::~LiseHelper()
{
  delete E2x_;
  delete x2E_;
}

LiseHelper::LiseHelper(LiseHelper &&lise)
{
  std::swap(path_, lise.path_);
  std::swap(beam_, lise.beam_);
  std::swap(target_, lise.target_);
  E2x_ = lise.E2x_;
  x2E_ = lise.x2E_;
  lise.E2x_ = lise.x2E_ = nullptr;
}

vector<std::string> LiseHelper::ListItem()
{
  FILE *pipe = popen("ls '" BASEDIR "'/data/*.txt | sort -V", "r");
  if(!pipe) return { };

  vector<string> list;
  char buf[PATH_MAX + 1];
  while(fgets(buf, sizeof(buf), pipe)) {
    string s(buf);
    s.erase(s.find_last_not_of(" \n\r\t") + 1);
    list.push_back(s);
  }

  pclose(pipe);
  return list;
}
