#include "Request.h"

Request::Request(int ID, int R_ID, double TPOST, int PRIO)
{
  ID_ = ID;
  R_ID_ = R_ID;
  TPOST_ = TPOST;
  PRIO_ = PRIO;
  TOG_ = 0;
}
int Request::ID() const
{
  return ID_;
}
int Request::PRIO() const
{
  return PRIO_;
}
double Request::TPOST() const
{
  return TPOST_;
}
void Request::setTBuf(double TBuf)
{
  TBuf_ = TBuf;
  TOG_ += TBuf_;
}
void Request::setTObsl(double TObsl)
{
  TObsl_ = TObsl;
  TOG_ += TObsl_;
}
void Request::setTExit(double TExit)
{
  TExit_ = TExit;
}
void Request::setPribId(int pribId)
{
  PribId_ = pribId;
}
double Request::getTBuf() const
{
  return TBuf_;
}
double Request::getTObsl() const
{
  return TObsl_;
}

double Request::getTOG() const
{
  return TOG_;
}

double Request::getTExit() const
{
  return TExit_;
}

int Request::getPribId() const
{
  return PribId_;
}

int Request::getR_ID() const
{
  return R_ID_;
}
