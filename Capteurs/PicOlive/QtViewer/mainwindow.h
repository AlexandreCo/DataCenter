#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "threadrs.h"
#include "samples.h"

namespace Ui {
    class MainWindow;
    class QAction;
    class QMenu;
}
//class QLabel;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotCurve;
class QwtPlotItem;
class Samples;


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    double getValueTab(int y);

protected:
    void changeEvent(QEvent *e);

private:

    //attributs
    // int i ;
    Ui::MainWindow *ui;


#define NB_BLOCK 256
#define BLOCK_SIZE 8
#define NB_SAMPLE_MAX (NB_BLOCK*BLOCK_SIZE)


    //double adX[NB_SAMPLE_MAX];

    Samples smplJour;
    Samples smplHisto;

    QwtPlotZoomer *pltZoomHisto[2];
    QwtPlotZoomer *pltZoomJour[2];
    QwtPlotPicker *d_picker;
    QwtPlotPanner *d_panner;


    QToolBar *tBarCurve;

    QAction *m_aboutAction;
    QAction *m_aboutQtAction;
    QAction *m_cacher;
    QAction *m_quit;
    QAction *m_open;
    QAction *m_save;
    QAction *m_read;

    QAction *m_raz;
    QAction *m_erase;

    QAction *m_heure;

    ThreadRs rs;

    //fonctions
    void ReadData(const QString &fileName);
    void computeJour(void);
    void ShowData();


    //fonctions d'init
    void Init();
    void InitTabHisto(void);
    void InitTabJour(void);
    void InitTabParam(void);
    void InitPlotHisto(void);
    void InitPlotJour(void);
    void InitThreadRs(void);

    void createMenu(void);
    void createToolBar(void);
    void createCurvesToolBar(void);
    void createActions(void);

signals:
    void toto(QString);

private slots:
    void on_Periode_valueChanged(double value);
    void on_SendPer_clicked();
    void on_clBtnPvDay_clicked();
    void on_clBtnNxDay_clicked();
    void on_chBxSHT11H_clicked(bool checked);
    void on_chBxLM75_clicked(bool checked);
    void on_chBxMPX_clicked(bool checked);
    void on_chBxSHT11T_clicked(bool checked);
    void openFile(void);
    void save(void);
    void FinThread(void);
    void lire(void);
    void setTime();
    void getTime(char * buff);
    void razHisto(void);
    void eraseStation(void);
    void testToto(QString);
    void about();
    void showCurve(QwtPlotItem *, bool on);

};

#endif // MAINWINDOW_H
