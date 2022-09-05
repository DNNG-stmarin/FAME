#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "readFiss.h"

#include <TBrowser.h>

#include <QFileDialog>

#include <sstream>

using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    f = new readFiss();
    // cout << "constructed " << f << endl;
}

MainWindow::~MainWindow()
{
    delete ui;
    // delete f;
}

void MainWindow::on_Run_clicked()
{
    cout << "\nRunning READ_FAME...\n" << endl;

    ui->centralwidget->setDisabled(true);
    ui->progressBar->setValue(0);
    ui->centralwidget->repaint();


    f->SetInfo(this);
    f->Run();

    f->runNum = 0;
    ui->progressBar->setValue(100);
    ui->centralwidget->setDisabled(false);
}


void MainWindow::on_Save_clicked()
{
    cout << "\nSaving...\n" << endl;
    ui->centralwidget->setDisabled(true);

    QString fileName = QFileDialog::getSaveFileName(this, "Save READ_FAME Input", "", "All Files (*)");
    nameInput = fileName.toStdString();
    if(nameInput == "")
    {
      cout << "fileName empty (user cancelled saving)" << endl;
      ui->centralwidget->setDisabled(false);
      return;
    }

    f->Save(this);

    ui->centralwidget->setDisabled(false);
}


void MainWindow::on_Load_clicked()
{
    cout << "\nLoading...\n" << endl;
    ui->centralwidget->setDisabled(true);

    QString fileName = QFileDialog::getOpenFileName(this, "Load READ_FAME Input", "", "All Files (*)");
    nameInput = fileName.toStdString();
    if(nameInput == "")
    {
      cout << "fileName empty (user cancelled loading)" << endl;
      ui->centralwidget->setDisabled(false);
      return;
    }
    cout << "loading from: " << endl;
    f->Load(this);

    ui->centralwidget->setDisabled(false);
}

void MainWindow::on_writeFinder_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save READ_FAME output", "", "ROOT File (*.root);;All Files (*)");
    fileName.remove(".root");
    ui->nameWrite->setText(fileName);
}

void MainWindow::on_expFinder_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Find Experiment File", "", "ROOT File (*.root);;All Files (*)");
    fileName.remove(".root");
    ui->nameExp->setText(fileName);
}


void MainWindow::on_coordFinder_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Find Coordinate File", "", "All Files (*)");
    ui->nameCoords->setText(fileName);
}


void MainWindow::on_simFinder_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Find Simulation File", "", "ROOT File (*.root);;All Files (*)");
    fileName.remove(".root");
    ui->nameSim->setText(fileName);
}


void MainWindow::on_beamFinder_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Find Beam File", "", "ROOT File (*.root);;All Files (*)");
    ui->nameBeam->setText(fileName);
}

void MainWindow::on_openBrowser_clicked()
{
    gROOT->SetBatch(kFALSE);
    cout << gROOT->IsBatch() << endl;
    TBrowser Browser;
}


// getters/setters
// progress bar, setter only
void MainWindow::setProgress(int value)
{
  ui->progressBar->setValue(value);
}

// inputFile name, getter only
string MainWindow::GetNameInput()
{
  return nameInput;
}

// modes
int MainWindow::mode(int set)
{
  if(set == -1)
  {
    if(ui->simMode->isChecked()) return 1;
    else if(ui->beamMode->isChecked()) return 2;
    else if(ui->fragMode->isChecked()) return 3;
    else return 0;
  }
  else
  {
    if(set == 1) ui->simMode->setChecked(true);
    else if(set == 2) ui->beamMode->setChecked(true);
    else if(set == 3) ui->fragMode->setChecked(true);
    else
    {
      ui->simMode->setChecked(false);
      ui->beamMode->setChecked(false);
      ui->fragMode->setChecked(false);
    }
    return -1;
  }
}
bool MainWindow::CovEM_in(int set)
{
  if(set == -1)
  {
    return ui->CovEM_in->isChecked();
  }
  else
  {
    ui->CovEM_in->setChecked((bool)set);
    return -1;
  }
}

// paths
string MainWindow::nameExp(string set)
{
  if(set == "")
  {
    return ui->nameExp->displayText().toStdString();
  }
  else
  {
    ui->nameExp->setText(QString::fromStdString(set));
    return "";
  }
}
string MainWindow::nameSim(string set)
{
  if(set == "")
  {
    return ui->nameSim->displayText().toStdString();
  }
  else
  {
    ui->nameSim->setText(QString::fromStdString(set));
    return "";
  }
}
string MainWindow::nameBeam(string set)
{
  if(set == "")
  {
    return ui->nameBeam->displayText().toStdString();
  }
  else
  {
    ui->nameBeam->setText(QString::fromStdString(set));
    return "";
  }
}
string MainWindow::nameCoords(string set)
{
  if(set == "")
  {
    return ui->nameCoords->displayText().toStdString();
  }
  else
  {
    ui->nameCoords->setText(QString::fromStdString(set));
    return "";
  }
}
string MainWindow::nameWrite(string set)
{
  if(set == "")
  {
    return ui->nameWrite->displayText().toStdString();
  }
  else
  {
    ui->nameWrite->setText(QString::fromStdString(set));
    return "";
  }
}

int MainWindow::numExpFiles(int set)
{
  if(set == -1)
  {
    return ui->numExpFiles->displayText().toInt();
  }
  else
  {
    ui->numExpFiles->setText(QString::number(set));
    return -1;
  }
}
int MainWindow::numSimFiles(int set)
{
  if(set == -1)
  {
    return ui->numSimFiles->displayText().toInt();
  }
  else
  {
    ui->numSimFiles->setText(QString::number(set));
    return -1;
  }
}

// detectors
int MainWindow::NUM_DETECTORS(int set)
{
  if(set == -1)
  {
    return ui->NUM_DETECTORS->displayText().toInt();
  }
  else
  {
    ui->NUM_DETECTORS->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::THRESHOLD(double set)
{
  if(set == -1)
  {
    return ui->THRESHOLD->displayText().toDouble();
  }
  else
  {
    ui->THRESHOLD->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::CLIPPING(double set)
{
  if(set == -1)
  {
    return ui->CLIPPING->displayText().toDouble();
  }
  else
  {
    ui->CLIPPING->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MAX_TIME_N(double set)
{
  if(set == -1)
  {
    return ui->MAX_TIME_N->displayText().toDouble();
  }
  else
  {
    ui->MAX_TIME_N->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MIN_TIME_N(double set)
{
  if(set == -1)
  {
    return ui->MIN_TIME_N->displayText().toDouble();
  }
  else
  {
    ui->MIN_TIME_N->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MAX_TIME_P(double set)
{
  if(set == -1)
  {
    return ui->MAX_TIME_P->displayText().toDouble();
  }
  else
  {
    ui->MAX_TIME_P->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MIN_TIME_P(double set)
{
  if(set == -1)
  {
    return ui->MIN_TIME_P->displayText().toDouble();
  }
  else
  {
    ui->MIN_TIME_P->setText(QString::number(set));
    return -1;
  }
}
// triggers
int MainWindow::NUM_TRIGGERS(int set)
{
  if(set == -1)
  {
    return ui->NUM_TRIGGERS->displayText().toInt();
  }
  else
  {
    ui->NUM_TRIGGERS->setText(QString::number(set));
    return -1;
  }
}
int* MainWindow::TRIGGERS(int* set)
{
  if(set == nullptr)
  {
    int* TRIGGERS = new int[NUM_TRIGGERS()];
    stringstream tIn(ui->TRIGGERS->displayText().toStdString());
    for(int i = 0; i < NUM_TRIGGERS(); ++i)
    {
        tIn >> TRIGGERS[i];
    }
    return TRIGGERS;
  }
  else
  {
    QString tLoad = "";
    for(int i = 0; i < NUM_TRIGGERS(); ++i)
    {
        tLoad += (QString::number(set[i]) + " ");
    }
    ui->TRIGGERS->setText(tLoad);
    return nullptr;
  }
}
double MainWindow::THRESHOLD_DEP(double set)
{
  if(set == -1)
  {
    return ui->THRESHOLD_DEP->displayText().toDouble();
  }
  else
  {
    ui->THRESHOLD_DEP->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::CLIPPING_DEP(double set)
{
  if(set == -1)
  {
    return ui->CLIPPING_DEP->displayText().toDouble();
  }
  else
  {
    ui->CLIPPING_DEP->setText(QString::number(set));
    return -1;
  }
}

// other
double MainWindow::BACKGROUND_DELAY(double set)
{
  if(set == -1)
  {
    return ui->BACKGROUND_DELAY->displayText().toDouble();
  }
  else
  {
    ui->BACKGROUND_DELAY->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::FISS_PILEUP_TIME(double set)
{
  if(set == -1)
  {
    return ui->FISS_PILEUP_TIME->displayText().toDouble();
  }
  else
  {
    ui->FISS_PILEUP_TIME->setText(QString::number(set));
    return -1;
  }
}

// beam settings
double MainWindow::BEAM_ERG_MIN(double set)
{
  if(set == -1)
  {
    return ui->BEAM_ERG_MIN->displayText().toDouble();
  }
  else
  {
    ui->BEAM_ERG_MIN->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::BEAM_ERG_MAX(double set)
{
  if(set == -1)
  {
    return ui->BEAM_ERG_MAX->displayText().toDouble();
  }
  else
  {
    ui->BEAM_ERG_MAX->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::BEAM_ERG_BINNUM(double set)
{
  if(set == -1)
  {
    return ui->BEAM_ERG_BINNUM->displayText().toDouble();
  }
  else
  {
    ui->BEAM_ERG_BINNUM->setText(QString::number(set));
    return -1;
  }
}

// fragment settings
double MainWindow::MIN_ANGLE(double set)
{
  if(set == -1)
  {
    return ui->MIN_ANGLE->displayText().toDouble();
  }
  else
  {
    ui->MIN_ANGLE->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MAX_ANGLE(double set)
{
  if(set == -1)
  {
    return ui->MAX_ANGLE->displayText().toDouble();
  }
  else
  {
    ui->MAX_ANGLE->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MIN_MASS(double set)
{
  if(set == -1)
  {
    return ui->MIN_MASS->displayText().toDouble();
  }
  else
  {
    ui->MIN_MASS->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MAX_MASS(double set)
{
  if(set == -1)
  {
    return ui->MAX_MASS->displayText().toDouble();
  }
  else
  {
    ui->MAX_MASS->setText(QString::number(set));
    return -1;
  }
}
int MainWindow::MASS_BINNUM(double set)
{
  if(set == -1)
  {
    return ui->MASS_BINNUM->displayText().toInt();
  }
  else
  {
    ui->MASS_BINNUM->setText(QString::number(set));
    return -1;
  }
}

// CovEM settings
double MainWindow::MIN_N_ERG(double set)
{
  if(set == -1)
  {
    return ui->MIN_N_ERG->displayText().toDouble();
  }
  else
  {
    ui->MIN_N_ERG->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MAX_N_ERG(double set)
{
  if(set == -1)
  {
    return ui->MAX_N_ERG->displayText().toDouble();
  }
  else
  {
    ui->MAX_N_ERG->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MIN_P_ERG(double set)
{
  if(set == -1)
  {
    return ui->MIN_P_ERG->displayText().toDouble();
  }
  else
  {
    ui->MIN_P_ERG->setText(QString::number(set));
    return -1;
  }
}
double MainWindow::MAX_P_ERG(double set)
{
  if(set == -1)
  {
    return ui->MAX_P_ERG->displayText().toDouble();
  }
  else
  {
    ui->MAX_P_ERG->setText(QString::number(set));
    return -1;
  }
}
int MainWindow::BN(int set)
{
  if(set == -1)
  {
    return ui->BN->displayText().toInt();
  }
  else
  {
    ui->BN->setText(QString::number(set));
    return -1;
  }
}
int MainWindow::BP(int set)
{
  if(set == -1)
  {
    return ui->BP->displayText().toInt();
  }
  else
  {
    ui->BP->setText(QString::number(set));
    return -1;
  }
}

int MainWindow::BA(int set)
{
  if(set == -1)
  {
    return ui->BA->displayText().toInt();
  }
  else
  {
    ui->BA->setText(QString::number(set));
    return -1;
  }
}
