#include "Modulator.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <set>

Modulator::Modulator(std::vector<Istocnik>& ist_, Buffer & buf_, std::vector<Pribor>& prib_):
  ist(ist_), buf(buf_), prib(prib_)
{
  n_ist = ist_.size();
  n_buf = buf_.size();
  n_prib = prib_.size();
}

void Modulator::setIstocnik(std::vector<Istocnik> &ist_)
{
  ist = ist_;
  n_ist = ist_.size();

}
void Modulator::setBuffer(Buffer &buf_)
{
  buf_ = buf_;
  n_buf = buf_.size();
}
void Modulator::setPribor(std::vector<Pribor> &prib_)
{
  prib = prib_;
  n_prib = prib_.size();
}

void Modulator::setSlot(const SLOT_TYPE &type, std::function<void(const Request &req, double)> func)
{
  switch (type)
  {
  case POST:
    sl_post = func;
    break;
  case OTK:
    sl_otk = func;
    break;
  case OBR:
    sl_obr = func;
    break;
  }
}

void Modulator::modulate(int KMIN, bool step_by_step)
{

  for (int i = 0; i < n_ist; i++)
    ist[i].makeNextPost();

  std::multiset<int> kol_ist;
  std::vector<decltype(kol_ist)::iterator> it_kol(n_ist);

  for (int i = 0; i < n_ist; i++)
  {
    it_kol[i] = kol_ist.insert(ist[i].getKOL());
  }

  std::vector<double> prevBOTK(n_ist);

  int N = KMIN;
  bool first_iteration = true;

  int TKOL = 0;

  while (true)
  {

    //if (*kol_ist.begin() >= N)
    if(TKOL >= N)
    {
      const double ta2 = std::pow(1.643, 2);
      const double d2 = std::pow(0.1, 2);

      for (int i = 0; i < n_ist; i++)
        N = std::max(N, (int)((ta2 * (1.0 - ist[i].getBOTK())) / (ist[i].getBOTK() * d2)));

      if (first_iteration)
      {
        first_iteration = false;
        
        if (N <= KMIN)
          return;

        for (int i = 0; i < n_ist; i++)
          prevBOTK[i] = ist[i].getBOTK();
      }
      else
      {
        bool diffLessThan_10p = true;
        for (int i = 0; i < n_ist; i++)
        {
          if (std::abs(prevBOTK[i] - ist[i].getBOTK()) >= 0.1 * prevBOTK[i])
          {
            diffLessThan_10p = false;
            break;
          }
        }

        if (diffLessThan_10p)
          return;

        for (int i = 0; i < n_ist; i++)
          prevBOTK[i] = ist[i].getBOTK();
      }
    }

    int minID_ist = get_minID_ist();
    int minID_prib = get_minID_prib();
    int id_idle_prib = get_id_idle_prib();


    if(minID_prib != -1 && prib[minID_prib].TOSV() < ist[minID_ist].getTPOST() && buf.isEmpty())
    {
      Request obr = prib[minID_prib].finalize();

      TKOL++;

      ist[obr.ID()].updateStats(obr, true);
      kol_ist.erase(it_kol[obr.ID()]);
      it_kol[obr.ID()] = kol_ist.insert(ist[obr.ID()].getKOL());

      if(!buf.isEmpty())
        prib[minID_prib].obsluz(buf.popBestRequest());

      if (step_by_step)
        sl_obr(obr, obr.getTExit());
    }
    else if (buf.isEmpty())
    {
      //generate new request and add to buffer
      Request req(minID_ist, ist[minID_ist].getN_ID(), ist[minID_ist].getTPOST(), minID_ist);
      buf.addRequest(req);
      ist[minID_ist].makeNextPost();

      if (step_by_step) 
        sl_post(req, req.TPOST());
    }
    else if (id_idle_prib != -1)
    {
      //take request from buffer
      Request req = buf.popBestRequest();
      prib[id_idle_prib].obsluz(req);
    }
    else if (ist[minID_ist].getTPOST() < prib[minID_prib].TOSV())
    {
      //generate new request and add to buffer
      Request req(minID_ist, ist[minID_ist].getN_ID(), ist[minID_ist].getTPOST(), minID_ist);
      ist[minID_ist].makeNextPost();

      if (buf.isFull())
      {
        Request rotk = buf.popWorstRequest();

        TKOL++;

        ist[rotk.ID()].updateStats(rotk, false);
        kol_ist.erase(it_kol[rotk.ID()]);
        it_kol[rotk.ID()] = kol_ist.insert(ist[rotk.ID()].getKOL());

        buf.addRequest(req);

        if (step_by_step)
        {
          sl_otk(rotk, req.TPOST());
          sl_post(req, req.TPOST());
        }

      }
      else
      {
        buf.addRequest(req);

        if (step_by_step)
          sl_post(req, req.TPOST());
      }

    }
    else
    {
      Request obr = prib[minID_prib].finalize();

      TKOL++;

      ist[obr.ID()].updateStats(obr, true);
      kol_ist.erase(it_kol[obr.ID()]);
      it_kol[obr.ID()] = kol_ist.insert(ist[obr.ID()].getKOL());

      prib[minID_prib].obsluz(buf.popBestRequest());

      if (step_by_step)
        sl_obr(obr, obr.getTExit());

    }

  }
}

int Modulator::get_minID_ist()
{
  int minID_ist = 0;
  for (int i = 1; i < n_ist; i++)
    if (ist[i].getTPOST() < ist[minID_ist].getTPOST())
      minID_ist = i;
  
  return minID_ist;
}

int Modulator::get_minID_prib()
{
  int minID_prib = -1;

  for (int i = 0; i < n_prib; i++)
    if (!prib[i].idle())
    {
      minID_prib = i;
      break;
    }

  if (minID_prib == -1)
    return minID_prib;

  for (int i = minID_prib; i < n_prib; i++)
    if ((!prib[i].idle()) && (prib[i].TOSV() < prib[minID_prib].TOSV()))
      minID_prib = i;

  return minID_prib;
}

int Modulator::get_id_idle_prib()
{
  int id_idle_prib = -1;
  bool found_idle_prib = false;
  for (int i = id_last_idle_prib; i < n_prib; i++)
  {
    if (prib[i].idle())
    {
      id_idle_prib = i;
      id_last_idle_prib = i;
      found_idle_prib = true;
      break;
    }
  }
  if (!found_idle_prib)
  {
    for (int i = 0; i < id_last_idle_prib; i++)
    {
      if (prib[i].idle())
      {
        id_idle_prib = i;
        id_last_idle_prib = i;
        found_idle_prib = true;
        break;
      }
    }
  }
  return id_idle_prib;
}
