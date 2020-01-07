#include "Pribor.h"

#include <iostream>
#include <algorithm>
#include <random>
#include <math.h>

Pribor::Pribor(int ID, double LAM)
{
  setParam(ID, LAM);
}

void Pribor::setParam(int ID, double LAM)
{
  ID_ = ID;
  LAM_ = LAM;
  TOSV_ = 0;
  TTObsl_ = 0;
}

double Pribor::TOSV() const
{
  return TOSV_;
}
bool Pribor::idle() const
{
  return idle_;
}

double Pribor::getLAM() const
{
  return LAM_;
}

double Pribor::getKIsp() const
{
  return TTObsl_ / (std::max(TOSV_, req_.TPOST()) + req_.getTOG());
}

void Pribor::obsluz(const Request &req)
{
  req_ = req;

  static std::mt19937 rng(std::random_device{}());
  static std::uniform_real_distribution<double> dist(0, 1);

  double  CTIME = std::max(TOSV_, req_.TPOST());
  double TAYOB = -1 / LAM_ * std::log(dist(rng));
  TOSV_ = CTIME + TAYOB;
  idle_ = false;
  TTObsl_ += TAYOB;

  req_.setTBuf(CTIME - req_.TPOST());
  req_.setTObsl(TAYOB);
  req_.setTExit(TOSV_);
  req_.setPribId(ID_);
}

Request Pribor::finalize()
{
  idle_ = true;
  return req_;
}

const Request & Pribor::getRequest() const
{
  return req_;
}
