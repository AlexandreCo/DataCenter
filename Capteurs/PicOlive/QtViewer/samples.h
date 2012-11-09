#ifndef SAMPLES_H
#define SAMPLES_H

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotCurve;
class QwtPlotItem;
class QwtPlot;

#define NB_SAMPLE_JOUR  2048
#define NB_JOUR_MAX 10
class Samples
{
public:
    Samples();
    void addNew(double time,double tLm,double tSht,double hSht,double pMPX);
    void clear(void);
    void attach( QwtPlot  * plot);
    void detach();

    void showCurveTime(void);
    void showCurveX(void);

    void showLm(bool);
    void showShtT(bool);
    void showShtH(bool);
    void showMPX(bool);


    void save(const QString &fileName);
    int read(const QString &fileName);

    void getFisrtDay(Samples * smpl);
    bool getNextDay(Samples * smpl);
    bool getPrevDay(Samples * smpl);
    int indexDay[NB_JOUR_MAX];

private:

    QwtPlotCurve *pltCrvLm75;
    QwtPlotCurve *pltCrvShtH;
    QwtPlotCurve *pltCrvShtT;
    QwtPlotCurve *pltCrvMPX;
    double * adTime;//[NB_SAMPLE_JOUR];
    double * adTempLM75;//[NB_SAMPLE_JOUR];
    double * adTempSHT11;//[NB_SAMPLE_JOUR];
    double * adHumiSHT11;//[NB_SAMPLE_JOUR];
    double * adPressMPX;//[NB_SAMPLE_JOUR];
    double * adX;//[NB_SAMPLE_JOUR];

    int day;
    int prevDay;
    int nbSample;
};

#endif // SAMPLES_H
