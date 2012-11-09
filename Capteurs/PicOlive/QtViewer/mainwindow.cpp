#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdebug.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_interval_data.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QThread>




class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas):
            QwtPlotZoomer(xAxis, yAxis, canvas)
    {
        setSelectionFlags(QwtPicker::DragSelection | QwtPicker::CornerToCorner);
        setTrackerMode(QwtPicker::AlwaysOff);
        setRubberBand(QwtPicker::NoRubberBand);
        
        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size
        
        setMousePattern(QwtEventPattern::MouseSelect2,
                        Qt::RightButton, Qt::ControlModifier);
        
        setMousePattern(QwtEventPattern::MouseSelect3,
                        Qt::RightButton);
    }
    
    virtual QwtText trackerText(const QwtDoublePoint &pos) const
    {
        QColor bg(Qt::white);
        bg.setAlpha(200);
        
        QwtText text = QwtPlotZoomer::trackerText(pos);
        text.setBackgroundBrush( QBrush( bg ));
        return text;
    }
};


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->Init();
    this->createToolBar();
    this->setWindowIcon(QIcon(":/img/meteo.png"));
    this->setWindowTitle("Weather Station 0.9");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::InitTabHisto(void)
{
    this->InitPlotHisto();
}

void MainWindow::InitTabJour(void)
{
    this->InitPlotJour();
    ui->clBtnNxDay->setEnabled(false);
    ui->clBtnPvDay->setEnabled(false);
}

void MainWindow::InitTabParam(void)
{
    ui->Periode->setRange(1,90,1,10);
}

void MainWindow::InitPlotHisto(void)
{
    //init background
    ui->plotHisto->setCanvasBackground(QColor(Qt::white));

    //init legend
    QwtLegend *legend = new QwtLegend;
    legend->setItemMode(QwtLegend::CheckableItem);
    ui->plotHisto->insertLegend(legend, QwtPlot::RightLegend);

    //Init grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
    grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(ui->plotHisto);

    //init axis
    ui->plotHisto->enableAxis(QwtPlot::yRight);
    ui->plotHisto->setAxisTitle(QwtPlot::yRight, "Pression [hPa]");
    ui->plotHisto->setAxisTitle(QwtPlot::xBottom, "Time/seconds");
    ui->plotHisto->setAxisAutoScale(QwtPlot::xBottom);
    ui->plotHisto->setAxisTitle(QwtPlot::yLeft, "Values");
    ui->plotHisto->setAxisAutoScale(QwtPlot::yLeft);
    //init Histo

    this->smplHisto.attach(ui->plotHisto);
    this->smplHisto.showLm(true);
    this->smplHisto.showMPX(true);
    this->smplHisto.showShtH(true);
    this->smplHisto.showShtT(true);

    pltZoomHisto[0] = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft, ui->plotHisto->canvas());
    pltZoomHisto[0]->setRubberBand(QwtPicker::RectRubberBand);
    pltZoomHisto[0]->setRubberBandPen(QColor(Qt::darkCyan));
    pltZoomHisto[0]->setTrackerMode(QwtPicker::AlwaysOn);

    connect(ui->plotHisto, SIGNAL(legendChecked(QwtPlotItem *, bool)), SLOT(showCurve(QwtPlotItem *, bool)));

}

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw(const QTime &base):
            baseTime(base)
    {
    }
    virtual QwtText label(double v) const
    {
        QTime upTime = baseTime.addSecs((int)v*60);
        return upTime.toString();
    }
private:
    QTime baseTime;
};



void MainWindow::InitPlotJour(void)
{
    //init background
    ui->plotHisto->setCanvasBackground(QColor(Qt::white));

    //init legend
    QwtLegend *legend = new QwtLegend;
    legend->setItemMode(QwtLegend::CheckableItem);
    ui->plotHisto->insertLegend(legend, QwtPlot::RightLegend);

    //Init grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
    grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(ui->plotJour);

    //init axis
    ui->plotJour->enableAxis(QwtPlot::yRight);
    ui->plotJour->setAxisTitle(QwtPlot::yRight, "Pression [hPa]");
    ui->plotJour->setAxisTitle(QwtPlot::xBottom, " [h:m:s]");
    ui->plotJour->setAxisAutoScale(QwtPlot::xBottom);
    ui->plotJour->setAxisTitle(QwtPlot::yLeft, "Values");
    ui->plotJour->setAxisAutoScale(QwtPlot::yLeft);
    ui->plotJour->setAxisScaleDraw(QwtPlot::xBottom,new TimeScaleDraw(QTime::fromString("00")));
    ui->plotJour->setAxisLabelRotation(QwtPlot::xBottom, -45.0);

    pltZoomJour[0] = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft, ui->plotJour->canvas());
    pltZoomJour[0]->setRubberBand(QwtPicker::RectRubberBand);
    pltZoomJour[0]->setRubberBandPen(QColor(Qt::darkCyan));
    pltZoomJour[0]->setTrackerMode(QwtPicker::AlwaysOn);

    connect(ui->plotJour, SIGNAL(legendChecked(QwtPlotItem *, bool)), SLOT(showCurve(QwtPlotItem *, bool)));


    this->smplJour.attach(ui->plotJour);

}


void MainWindow::InitThreadRs(void)
{
    connect(&rs, SIGNAL(finEmission()), this, SLOT(FinThread()));
    connect(&rs, SIGNAL(newTime(char*)), this, SLOT(getTime(char*)));
    rs.start();
}

void MainWindow::Init(void)
{
    this->InitTabHisto();
    this->InitTabJour();
    this->InitTabParam();
    this->InitThreadRs();

    this->ShowData();

}

void MainWindow::testToto(QString titi)        
{
    qDebug() << titi;
}

void MainWindow::ShowData()
{
    ui->plotHisto->setAxisAutoScale(QwtPlot::xBottom);
    ui->plotHisto->setAxisAutoScale(QwtPlot::yLeft);
    pltZoomHisto[0]->setZoomBase(true);

    ui->plotJour->setAxisAutoScale(QwtPlot::xBottom);
    ui->plotJour->setAxisAutoScale(QwtPlot::yLeft);
    pltZoomJour[0]->setZoomBase(true);
    
    ui->plotHisto->replot();
    ui->plotJour->replot();
}

void MainWindow::ReadData(const QString &fileName)
{
    int nb;
    this->smplHisto.clear();
    nb=this->smplHisto.read(fileName);
    ui->Memoire->setValue(nb);
    this->smplHisto.showCurveX();
    this->smplJour.clear();
    this->computeJour();
}

void MainWindow::computeJour(void)
{
    smplHisto.getFisrtDay(&smplJour);
    ui->clBtnNxDay->setEnabled(true);
    smplJour.showCurveTime();
    ui->plotJour->show();
}


void MainWindow::createMenu()
{

    ui->menuAide->addAction(m_aboutAction);
    ui->menuAide->addAction(m_aboutQtAction);

    ui->menuFichier->addAction(m_open);
    ui->menuFichier->addSeparator();
    ui->menuFichier->addAction(m_save);
    ui->menuFichier->addSeparator();
    ui->menuFichier->addAction(m_quit);


    ui->menuOutils->addAction(m_read);
    ui->menuOutils->addSeparator();
    ui->menuOutils->addAction(m_heure);
    ui->menuOutils->addSeparator();
    ui->menuOutils->addAction(m_erase);
    ui->menuOutils->addAction(m_raz);

}

void MainWindow::createCurvesToolBar()
{
    tBarCurve = this->addToolBar(tr("Composants"));
    tBarCurve->addWidget(ui->chBxLM75);
    tBarCurve->addWidget(ui->chBxMPX);
    tBarCurve->addWidget(ui->chBxSHT11H);
    tBarCurve->addWidget(ui->chBxSHT11T);
}

void MainWindow::createActions()
{

    m_aboutAction = new QAction(tr("&About"), this);
    m_aboutAction->setStatusTip(tr("Show the application's About box"));
    m_aboutAction->setIcon(QIcon(":/img/meteo.png"));
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    m_quit = new QAction(tr("&Quitter"), this);
    m_quit->setStatusTip(tr("Quitter"));
    m_quit->setIcon(QIcon(":/img/cross.png"));
    connect(m_quit, SIGNAL(triggered()), qApp, SLOT(quit()));

    m_open = new QAction(tr("&Ouvrir un fichier"), this);
    m_open->setStatusTip(tr("Open an existing file"));
    m_open->setIcon(QIcon(":/img/open.png"));
    connect(m_open, SIGNAL(triggered()), this, SLOT(openFile()));

    m_save = new QAction(tr("&Enregistrer"), this);
    m_save->setStatusTip(tr("Enregistrer"));
    m_save->setIcon(QIcon(":/img/save.png"));
    connect(m_save, SIGNAL(triggered()), this, SLOT(save()));

    m_read = new QAction(tr("&Lire la station"), this);
    m_read->setCheckable(true);
    m_read->setStatusTip(tr("Lire"));
    m_read->setIcon(QIcon(":/img/read.png"));
    connect(m_read, SIGNAL(triggered()), this, SLOT(lire()));

    m_heure = new QAction(tr("&Fixer l'heure"), this);
    m_heure->setStatusTip(tr("Heure"));
    m_heure->setIcon(QIcon(":/img/time.png"));
    connect(m_heure, SIGNAL(triggered()), this, SLOT(setTime()));

    m_raz = new QAction(tr("&Raz current"), this);
    m_raz->setStatusTip(tr("Raz"));
    m_raz->setIcon(QIcon(":/img/raz.jpg"));
    connect(m_raz, SIGNAL(triggered()), this, SLOT(razHisto()));


    m_erase = new QAction(tr("&Vide la station"), this);
    m_erase->setStatusTip(tr("Erase"));
    m_erase->setIcon(QIcon(":/img/erase.jpg"));
    connect(m_erase, SIGNAL(triggered()), this, SLOT(eraseStation()));


    m_heure = new QAction(tr("&Fixer l'heure"), this);
    m_heure->setStatusTip(tr("Heure"));
    m_heure->setIcon(QIcon(":/img/time.png"));
    connect(m_heure, SIGNAL(triggered()), this, SLOT(setTime()));

    m_aboutQtAction = new QAction(tr("About &Qt"), this);
    m_aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    m_aboutQtAction->setIcon(QIcon(":/img/qt.png"));
    connect(m_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

void MainWindow::createToolBar()
{

    this->createActions();
    this->createCurvesToolBar();
    QList <QAction*> qListAction ;
    qListAction<<m_open<<m_read<<m_read<<m_heure;
    
    this->createMenu();
    ui->tBarMain->addActions(qListAction);

}

void MainWindow:: lire(void)
{
    if(m_read->isChecked())
    {
        rs.getData();
    }
}

void MainWindow:: getTime(char * buff)
{
    int heure,minute,seconde,periode,ret;
    QString qstring("Periode en cours : ");
    //ui->Time->setCurrentTime();
    
    ret=sscanf(buff,"gettime\ngettime\n%d:%d:%d (%d min)",&heure,&minute,&seconde,&periode);
    if(ret==4)
    {

        ui->Time->setValue(heure*3600+minute*60+seconde);
        qstring.setNum(periode);
        qstring.prepend("periode en cours : ");
        qstring.append(" minute");
        if(periode>1)
        {
            qstring.append("s");
        }
        ui->labelPer->setText(qstring);
    }
}
void MainWindow:: setTime()
{
    QTime date = QTime::currentTime();
    qDebug()<<date.toString();
    rs.setTime(date.toString());
}


void MainWindow:: razHisto()
{
    rs.sendRaz();
}

void MainWindow:: eraseStation()
{
    rs.sendErase();
}

void MainWindow:: openFile(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Weather Station File"), ".",
                                                    tr("Weather Station files (*.wsf *.csv)"));
    
    if (!fileName.isEmpty())
    {
        this->ReadData(fileName);
        this->ShowData();
    }
}

void MainWindow:: save(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open Weather Station File"), ".",
                                                    tr("Weather Station files (*.wsf *.csv)"));
    this->smplHisto.save(fileName);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Weather Station"),
                       tr("<h2>Weather Station 0.9</h2>"
                          "<p>Copyright &copy; 2010 Software Inc."
                          "<p>Weather Station SARL : 152000 euros"));
}


void MainWindow::FinThread()
{
    
    this->m_read->setChecked(false);
    this->m_read->setEnabled(true);
    this->ReadData("temp.csv");
    this->ShowData();
}

void MainWindow::showCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
    QWidget *wHisto = ui->plotHisto->legend()->find(item);
    if ( wHisto && wHisto->inherits("QwtLegendItem") )
        ((QwtLegendItem *)wHisto)->setChecked(on);
    ui->plotHisto->replot();
    ui->plotJour->replot();
}


void MainWindow::on_chBxSHT11T_clicked(bool checked)
{

    this->smplHisto.showShtT(checked);
    this->smplJour.showShtT(checked);
    this->ShowData();
}

void MainWindow::on_chBxMPX_clicked(bool checked)
{

    this->smplHisto.showMPX(checked);
    this->smplJour.showMPX(checked);
    this->ShowData();
}

void MainWindow::on_chBxLM75_clicked(bool checked)
{

    this->smplHisto.showLm(checked);
    this->smplJour.showLm(checked);
    this->ShowData();
}

void MainWindow::on_chBxSHT11H_clicked(bool checked)
{
    this->smplHisto.showShtH(checked);
    this->smplJour.showShtH(checked);
    this->ShowData();
}


void MainWindow::on_clBtnNxDay_clicked()
{

    ui->clBtnNxDay->setEnabled(smplHisto.getNextDay(&smplJour));
    ui->clBtnPvDay->setEnabled(true);
    smplJour.showCurveTime();
    ui->plotJour->show();
    ui->plotJour->replot();

}

void MainWindow::on_clBtnPvDay_clicked()
{
    ui->clBtnPvDay->setEnabled(smplHisto.getPrevDay(&smplJour));
    ui->clBtnNxDay->setEnabled(true);
    smplJour.showCurveTime();
    ui->plotJour->show();
    ui->plotJour->replot();
}

void MainWindow::on_SendPer_clicked()
{
    QString str;
    str.setNum( ui->Periode->value()) ;
    qDebug()<<str;
    rs.setPeriode(str);
}

void MainWindow::on_Periode_valueChanged(double value)
{
    QString str;

    str.setNum(value);
    str.prepend("periode : ");
    str.append(" minute");
    if(value>1)
    {
        str.append("s");
    }
    ui->labelPerSend->setText(str);
}
