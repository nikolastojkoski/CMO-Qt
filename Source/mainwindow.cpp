#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QTableWidget>
#include <QMessageBox>
#include <QEventLoop>
#include <QTabWidget>
#include <vector>
#include "Istocnik.h"
#include "Buffer.h"
#include "Pribor.h"
#include "Modulator.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  setWindowTitle("CMO");

  istResultTable = findChild<QTableWidget*>("tableWidget");
  istResultTable->resizeColumnToContents(0);
  istResultTable->resizeColumnToContents(1);
  istResultTable->verticalHeader()->setVisible(false);
  istResultTable->setHorizontalHeaderItem(0, new QTableWidgetItem("№ \n источника"));
  istResultTable->setHorizontalHeaderItem(1, new QTableWidgetItem("количество \n заявок"));

  pribResultTable = findChild<QTableWidget*>("tableWidget_2");
  pribResultTable->resizeColumnsToContents();
  pribResultTable->verticalHeader()->setVisible(false);

  QPushButton * modulateButton = findChild<QPushButton*>("modulateButton");
  connect(modulateButton, SIGNAL(released()), this, SLOT(modulate()));

  QPushButton * nextStepButton = findChild<QPushButton*>("nextStepButton");
  connect(nextStepButton, SIGNAL(released()), this, SLOT(nextStep()));

  n_ist_edit = findChild<QLineEdit*>("n_ist_edit");
  n_prib_edit = findChild<QLineEdit*>("n_prib_edit");
  n_buf_edit = findChild<QLineEdit*>("n_buf_edit");
  kmin_edit = findChild<QLineEdit*>("kmin_edit");
  tay1_spin = findChild<QDoubleSpinBox*>("tay1_spin");
  tay2_spin = findChild<QDoubleSpinBox*>("tay2_spin");
  lambda_spin = findChild<QDoubleSpinBox*>("lambda_spin");

  eventTable = findChild<QTableWidget*>("eventTable");

  priborTable = findChild<QTableWidget*>("priborTable");
  priborTable->verticalHeader()->setVisible(false);
  priborTable->setHorizontalHeaderItem(0, new QTableWidgetItem("№ \n прибора"));
  priborTable->setHorizontalHeaderItem(1, new QTableWidgetItem("время \n освобождения"));
  priborTable->resizeColumnsToContents();

  bufferTable = findChild<QTableWidget*>("bufferTable");
  bufferTable->verticalHeader()->setVisible(false);
  bufferTable->setHorizontalHeaderItem(1, new QTableWidgetItem("время \n поступления"));
  bufferTable->resizeColumnsToContents();

  QTabWidget *tabWidget = findChild<QTabWidget*>("tabWidget");
  tabWidget->setCurrentIndex(0);

}

MainWindow::~MainWindow()
{
  delete ui;
}
void MainWindow::modulate()
{

  step_by_step_reset();

  modulation_parameters_t params = getModulationParameters();

  if(!params.valid)
    return;

  std::vector<Istocnik> ist;
  std::vector<Pribor> prib;
  setupIstPrib(ist, prib, params);
  Buffer buf(params.n_buf);

  Modulator modulator(ist, buf, prib) ;
  modulator.modulate(params.kmin);
  updateFinalResults(ist, prib);

}

void MainWindow::nextStep()
{
  step_number++;
  if(step_number == 1)
  {
    step_by_step_init();
  }
  eventLoop->quit();
}

MainWindow::modulation_parameters_t MainWindow::getModulationParameters()
{
  modulation_parameters_t p;
  p.n_ist = n_ist_edit->text().toInt();
  p.n_buf = n_buf_edit->text().toInt();
  p.n_prib = n_prib_edit->text().toInt();
  p.kmin = kmin_edit->text().toInt();
  p.tay1 = tay1_spin->value();
  p.tay2 = tay2_spin->value();
  p.lambda = lambda_spin->value();
  p.valid = true;

  if((p.n_ist == 0) || (p.n_buf == 0) || (p.n_prib == 0) || (p.kmin == 0) ||
     (p.tay1 == 0) || (p.tay2 == 0) || (p.lambda == 0) || (p.tay2 < p.tay1))
  {
    p.valid = false;
    QMessageBox messageBox;
    messageBox.setWindowTitle("Error");
    messageBox.setText("<p align='center'>Invalid parameters!</p>");
    messageBox.setStyleSheet("QLabel{min-width: 200px;}");
    messageBox.exec();
  }
  return p;
}

void MainWindow::setupIstPrib(std::vector<Istocnik> &ist, std::vector<Pribor> &prib,
                              const MainWindow::modulation_parameters_t &params)
{
  ist.resize(params.n_ist);
  for(int i = 0; i < params.n_ist; i++)
    ist[i].setParam(i, i, params.tay1, params.tay2);

  prib.resize(params.n_prib);
  for(int i = 0; i < params.n_prib; i++)
    prib[i].setParam(i, params.lambda);
}

void MainWindow::step_by_step_init()
{
  modulation_parameters_t params = getModulationParameters();

  if(!params.valid)
    return;

  setupIstPrib(ist, prib, params);

  buf = new Buffer(params.n_buf);
  modulator = new Modulator(ist, *buf, prib);
  eventLoop = new QEventLoop();

  auto process_event = [&](const EVENT_TYPE &type, const Request &req, double ctime)
  {
    updateEventTable(type, req, ctime);
    updateBufferTable();
    updatePriborTable();
    eventLoop->exec();
  };
  modulator->setSlot(Modulator::SLOT_TYPE::POST, [&](const Request &req, double ctime)
  {
    process_event(EVENT_TYPE::POST, req, ctime);
  });
  modulator->setSlot(Modulator::SLOT_TYPE::OTK, [&](const Request &req, double ctime)
  {
    process_event(EVENT_TYPE::OTK, req, ctime);
  });
  modulator->setSlot(Modulator::SLOT_TYPE::OBR, [&](const Request &req, double ctime)
  {
    process_event(EVENT_TYPE::OBR, req, ctime);
  });

  modulator->modulate(params.kmin, true);
}
void MainWindow::step_by_step_reset()
{
  eventTable->setRowCount(0);
  priborTable->setRowCount(0);
  bufferTable->setRowCount(0);
  ist.clear();
  prib.clear();
  step_number = 0;
}

void MainWindow::updateEventTable(const EVENT_TYPE &type, const Request &req, double ctime)
{
  eventTable->insertRow(eventTable->rowCount());
  eventTable->scrollToBottom();
  int row = eventTable->rowCount() - 1;

  QString eventDescription;
  switch(type)
  {
    case EVENT_TYPE::POST:
      eventDescription = "Ист. " + QString::number(req.ID());
      addToTable(eventTable, row, 3, ist[req.ID()].getN_ID() - 1);
      addToTable(eventTable, row, 4, ist[req.ID()].getOTK());
      break;
    case EVENT_TYPE::OBR:
      eventDescription = "Приб. " + QString::number(req.getPribId());
      break;
    case EVENT_TYPE::OTK:
      eventDescription = "Отк. " + QString::number(req.ID());
      break;
  }

  QString id = QString::number(req.ID()) + "." + QString::number(req.getR_ID());
  addToTable(eventTable, row, 0, eventDescription);
  addToTable(eventTable, row, 1, id);
  addToTable(eventTable, row, 2, ctime, 4);

}

void MainWindow::updateBufferTable()
{
  bufferTable->setRowCount(0);
  bufferTable->setRowCount(buf->size());
  int i = 0;
  for(auto it = buf->begin(); it != buf->end(); it++)
  {
    bufferTable->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
    bufferTable->setItem(i, 1, new QTableWidgetItem(QString::number(it->TPOST(), 'g', 4)));
    bufferTable->setItem(i, 2, new QTableWidgetItem(QString::number(it->ID())));
    bufferTable->setItem(i, 3, new QTableWidgetItem(QString::number(it->getR_ID())));
    i++;
  }
}
void MainWindow::updatePriborTable()
{
  priborTable->setRowCount(0);
  priborTable->setRowCount(prib.size());
  for(int i = 0; i < prib.size(); i++)
  {
    addToTable(priborTable, i, 0, i);
    if (prib[i].idle())
    {
      addToTable(priborTable, i, 1, "idle");
    }
    else
    {
      addToTable(priborTable, i, 1, prib[i].TOSV(), 4);
      addToTable(priborTable, i, 2, prib[i].getRequest().ID());
      addToTable(priborTable, i, 3, prib[i].getRequest().getR_ID());
    }
  }
}
void MainWindow::updateFinalResults(const std::vector<Istocnik> &ist,
                                    const std::vector<Pribor> &prib)
{
  istResultTable->setRowCount(ist.size());
  for(int i = 0; i < ist.size(); i++)
  {
    addToTable(istResultTable, i, 0, i);
    addToTable(istResultTable, i, 1, ist[i].getKOL());
    addToTable(istResultTable, i, 2, ist[i].getBOTK(), 3);
    addToTable(istResultTable, i, 3, ist[i].getMTOG(), 3);
    addToTable(istResultTable, i, 4, ist[i].getTBuf(), 3);
    addToTable(istResultTable, i, 5, ist[i].getTObsluz(), 3);
    addToTable(istResultTable, i, 6, ist[i].getDispTbuf(), 3);
    addToTable(istResultTable, i, 7, ist[i].getDispTOblsuz(), 3);
  }

  pribResultTable->setRowCount(prib.size());
  for(int i = 0; i < prib.size(); i++)
  {
    addToTable(pribResultTable, i, 0, i);
    addToTable(pribResultTable, i, 1, prib[i].getKIsp(), 3);
  }
}

void MainWindow::addToTable(QTableWidget *table, int row, int col, const QString &content)
{
  QTableWidgetItem* item = new QTableWidgetItem(content);
  item->setTextAlignment(Qt::AlignCenter);
  table->setItem(row, col, item);
}

void MainWindow::addToTable(QTableWidget *table, int row, int col, int value)
{
  addToTable(table, row, col, QString::number(value));
}

void MainWindow::addToTable(QTableWidget *table, int row, int col, double value, int precision)
{
  addToTable(table, row, col, QString::number(value, 'g', precision));
}
