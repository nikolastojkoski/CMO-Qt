#ifndef REQUEST_H
#define REQUEST_H

class Request
{
public:
  Request(int ID, int R_ID, double TPOST, int PRIO);
  
  void setTBuf(double TBuf);
  void setTObsl(double TObsl);
  void setTExit(double TExit);
  void setPribId(int pribId);

  double getTBuf() const;
  double getTObsl() const;
  double getTOG() const;
  double getTExit() const;
  int getPribId() const;
  int getR_ID() const;

  int ID() const;
  int PRIO() const;
  double TPOST() const;

private:
  int ID_, R_ID_, PRIO_, TOG_;
  double TPOST_;
  double TBuf_, TObsl_, TExit_;
  int PribId_;

};


#endif