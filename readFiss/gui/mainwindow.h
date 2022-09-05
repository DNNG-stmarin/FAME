#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEventLoop>
#include <QMessageBox>

#include <iostream>
#include <string>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class readFiss;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // error messages
    void noWriteFile()
    {
      std::cout << "Couldn't open output file" << std::endl;
      QMessageBox messageBox;
      messageBox.critical(0,"Error","Couldn't open output file");
      messageBox.setFixedSize(500,200);
      close();
    }

    void noExpFile()
    {
      std::cout << "Couldn't open experiment file" << std::endl;
      QMessageBox messageBox;
      messageBox.critical(0,"Error","Couldn't open experiment file");
      messageBox.setFixedSize(500,200);
    }

    void noSimFile()
    {
      std::cout << "Couldn't open simulation file" << std::endl;
      QMessageBox messageBox;
      messageBox.critical(0,"Error","Couldn't open simulation file");
      messageBox.setFixedSize(500,200);
    }

    void noBeamFile()
    {
      std::cout << "Couldn't open beam file" << std::endl;
      QMessageBox messageBox;
      messageBox.critical(0,"Error","Couldn't open beam file");
      messageBox.setFixedSize(500,200);
    }

    void noCoordsFile()
    {
      std::cout << "Couldn't open coordinate file" << std::endl;
      QMessageBox messageBox;
      messageBox.critical(0,"Error","Couldn't open coordinate file");
      messageBox.setFixedSize(500,200);
    }

    // getters/setters
    // progress bar setter
    void setProgress(int value);

    // inputFile name, for saving AND loading
    std::string GetNameInput();

    // modes
    int mode(int set = -1);
    bool CovEM_in(int set = -1);

    // paths
    std::string nameExp(std::string set = "");
    std::string nameSim(std::string set = "");
    std::string nameBeam(std::string set = "");
    std::string nameCoords(std::string set = "");
    std::string nameWrite(std::string set = "");

    int numExpFiles(int set = -1);
    int numSimFiles(int set = -1);

    // detectors
    int NUM_DETECTORS(int set = -1);
    double THRESHOLD(double set = -1);
    double CLIPPING(double set = -1);


    double MIN_TIME_N(double set = -1);
    double MAX_TIME_N(double set = -1);
    double MIN_TIME_P(double set = -1);
    double MAX_TIME_P(double set = -1);

    // triggers
    int NUM_TRIGGERS(int set = -1);
    int* TRIGGERS(int* set = nullptr);
    double THRESHOLD_DEP(double set = -1);
    double CLIPPING_DEP(double set = -1);

    // other
    double BACKGROUND_DELAY(double set = -1);
    double FISS_PILEUP_TIME(double set = -1);

    // beam settings
    double BEAM_ERG_MIN(double set = -1);
    double BEAM_ERG_MAX(double set = -1);
    double BEAM_ERG_BINNUM(double set = -1);

    // fragment settings
    double MIN_ANGLE(double set = -1);
    double MAX_ANGLE(double set = -1);
    double MIN_MASS(double set = -1);
    double MAX_MASS(double set = -1);
    int    MASS_BINNUM(double set=-1);

    // CovEM settings
    double MIN_N_ERG(double set = -1);
    double MAX_N_ERG(double set = -1);
    double MIN_P_ERG(double set = -1);
    double MAX_P_ERG(double set = -1);

    int BN(int set = -1);
    int BP(int set = -1);
    int BA(int set = -1);

private slots:

    void on_Run_clicked();

    void on_Save_clicked();

    void on_Load_clicked();

    void on_writeFinder_clicked();

    void on_expFinder_clicked();

    void on_coordFinder_clicked();

    void on_simFinder_clicked();

    void on_beamFinder_clicked();

    void on_openBrowser_clicked();


private:
    Ui::MainWindow *ui;
    std::string nameInput; // inputFile name, for saving AND loading
public:
    readFiss* f;
};
#endif // MAINWINDOW_H
