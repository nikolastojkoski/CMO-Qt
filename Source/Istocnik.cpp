#include "Istocnik.h"
#include <random>
#include <math.h>

Istocnik::Istocnik()
{
  reset();
}
Istocnik::Istocnik(int ID, int PRIO, double TAY1, double TAY2)
{
  setParam(ID, PRIO, TAY1, TAY2);
}
void Istocnik::setParam(int ID, int PRIO, double TAY1, double TAY2)
{
  n_id = 0;
  ID_ = ID;
  PRIO_ = PRIO;
  TAY1_ = TAY1;
  TAY2_ = TAY2;
  TPOST_ = 0.0;
  reset();
}
void Istocnik::makeNextPost()
{
  static std::mt19937 rng(std::random_device{}());
  static std::uniform_real_distribution<double> dist(0, 1);

  double TAY = TAY1_ + (TAY2_ - TAY1_) * dist(rng);
  TPOST_ = TPOST_ + TAY;
  n_id++;
}
void Istocnik::updateStats(const Request &req, bool isObrabotano)
{
  KOL_++;
  if (isObrabotano)
  {
    KOBR_++;
    TOG_ += req.getTBuf() + req.getTObsl();
    TTObsluz_ += req.getTObsl();
    TTBuf_ += req.getTBuf();
    req_stats_.push_back({ req.getTObsl(), req.getTBuf() });
  }
  else
  {
    OTK_++;
  }
}
double Istocnik::getTPOST() const
{
  return TPOST_;
}
int Istocnik::getPRIO() const
{
  return PRIO_;
}

int Istocnik::getKOL() const
{
  return KOL_;
}
int Istocnik::getOTK() const
{
  return OTK_;
}
int Istocnik::getKOBR() const
{
  return KOBR_;
}
int Istocnik::getN_ID() const
{
  return n_id;
}
double Istocnik::getBOTK() const
{
  return (double)OTK_ / (double)KOL_;
}

double Istocnik::getMTOG() const
{
  return TOG_ / (double)KOBR_;
}

double Istocnik::getTObsluz() const
{
  return TTObsluz_ / (double)KOBR_;
}

double Istocnik::getTBuf() const
{
  return TTBuf_ / (double)KOBR_;
}

double Istocnik::getDispTOblsuz() const
{
  double mean = getTObsluz();
  double sum2 = 0;
  for (int i = 0; i < req_stats_.size(); i++)
    sum2 += std::pow(req_stats_[i].TObsluz - mean, 2);
  double disp = sum2 / (req_stats_.size() - 1);
  return disp;
}

double Istocnik::getDispTbuf() const
{
  double mean = getTBuf();
  double sum2 = 0;
  for (int i = 0; i < req_stats_.size(); i++)
    sum2 += std::pow(req_stats_[i].TBuf - mean, 2);
  double disp = sum2 / (req_stats_.size() - 1);
  return disp;
}

double Istocnik::getTAY1() const
{
  return TAY1_;
}

double Istocnik::getTAY2() const
{
  return TAY2_;
}

void Istocnik::reset()
{
  TPOST_ = 0;
  KOL_ = 0;
  OTK_ = 0;
  KOBR_ = 0;
  TOG_ = 0;
  TTObsluz_ = 0;
  TTBuf_ = 0;
  req_stats_.clear();
}
