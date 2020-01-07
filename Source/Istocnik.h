#ifndef ISTOCNIK_H
#define ISTOCNIK_H

#include <vector>
#include "Request.h"

class Istocnik
{
public:
  Istocnik();
  Istocnik(int ID, int PRIO, double TAY1, double TAY2);

  void setParam(int ID, int PRIO, double TAY1, double TAY2);

  void makeNextPost();
  void updateStats(const Request &req, bool isObrabotano);
  void reset();

  double getTPOST() const;
  int getPRIO() const;
  int getKOL() const;
  int getOTK() const;
  int getKOBR() const;
  int getN_ID() const;
  double getBOTK() const;
  double getMTOG() const;
  double getTObsluz() const;
  double getTBuf() const;
  double getDispTOblsuz() const;
  double getDispTbuf() const;
  
  double getTAY1() const;
  double getTAY2() const;

private:
  double TAY1_, TAY2_;
  double TPOST_;
  int ID_, PRIO_;
  int n_id;

  int KOL_, OTK_, KOBR_;
  double TOG_, TTObsluz_, TTBuf_;

  struct RequestStats
  {
    double TObsluz, TBuf;
  };
  std::vector<RequestStats> req_stats_;

};


#endif
