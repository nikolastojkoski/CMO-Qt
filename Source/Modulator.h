#ifndef MODULATOR_H
#define MODULATOR_H

#include <QObject>
#include <vector>
#include <functional>
#include "Istocnik.h"
#include "Buffer.h"
#include "Pribor.h"

class Modulator: public QObject
{
  Q_OBJECT
public:

  Modulator(std::vector<Istocnik> &ist_, Buffer &buf_, std::vector<Pribor> &prib_);

  void setIstocnik(std::vector<Istocnik> &ist_);
  void setBuffer(Buffer &buf_);
  void setPribor(std::vector<Pribor> &prib_);

  enum SLOT_TYPE{ POST, OTK, OBR};
  void setSlot(const SLOT_TYPE &type, std::function<void(const Request &req, double ctime)> func);
  
  void modulate(int KMIN, bool step_by_step = false);

  void cancelModulation();
  bool isModulationFinished();

private:
  std::vector<Istocnik> &ist;
  std::vector<Pribor> &prib;
  Buffer &buf;
  int n_ist, n_buf, n_prib;
  int id_last_idle_prib = 0;

  std::function<void(const Request &req, double ctime)> sl_post, sl_otk, sl_obr;

  int get_minID_ist();
  int get_minID_prib();
  int get_id_idle_prib();

};

#endif
