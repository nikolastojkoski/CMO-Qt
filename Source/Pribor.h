#ifndef PRIBOR_H
#define PRIBOR_H

#include "Request.h"

class Pribor
{
public:
  Pribor() = default;
  Pribor(int ID, double LAM);

  void setParam(int ID, double LAM);

  double TOSV() const;
  bool idle() const;
  double getLAM() const;
  double getKIsp() const;

  void obsluz(const Request &req);
  Request finalize();
  const Request& getRequest() const;

private:
  int ID_;
  double TOSV_, LAM_;
  Request req_ = Request(0, 0, 0, 0);
  bool idle_ = true;

  double TTObsl_;

};

#endif