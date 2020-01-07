#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QEventLoop>
#include <vector>
#include "Modulator.h"
#include "Istocnik.h"
#include "Pribor.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void modulate();
  void nextStep();

private:
  Ui::MainWindow *ui;
  QLineEdit* n_ist_edit;
  QLineEdit* n_buf_edit;
  QLineEdit* n_prib_edit;
  QLineEdit* kmin_edit;
  QDoubleSpinBox* tay1_spin;
  QDoubleSpinBox* tay2_spin;
  QDoubleSpinBox* lambda_spin;
  QTableWidget* istResultTable;
  QTableWidget* pribResultTable;
  QTableWidget* eventTable;
  QTableWidget* priborTable;
  QTableWidget* bufferTable;

  QEventLoop* eventLoop = nullptr;
  Modulator* modulator = nullptr;
  Buffer* buf = nullptr;
  std::vector<Istocnik> ist;
  std::vector<Pribor> prib;
  int step_number = 0;

  struct modulation_parameters_t
  {
    int n_ist, n_buf, n_prib, kmin;
    double tay1, tay2, lambda;
    bool valid;
  };

  enum EVENT_TYPE{POST, OBR, OTK};

  modulation_parameters_t getModulationParameters();
  void setupIstPrib(std::vector<Istocnik> &ist, std::vector<Pribor> &prib,
                    const modulation_parameters_t &params);
  void step_by_step_init();
  void step_by_step_reset();

  void updateEventTable(const EVENT_TYPE &type, const Request &req, double ctime);
  void updateBufferTable();
  void updatePriborTable();
  void updateFinalResults(const std::vector<Istocnik> &ist, const std::vector<Pribor> &prib);

  void addToTable(QTableWidget* table, int row, int col, const QString &content);
  void addToTable(QTableWidget* table, int row, int col, int value);
  void addToTable(QTableWidget* table, int row, int col, double value, int precision);


};

#endif // MAINWINDOW_H
