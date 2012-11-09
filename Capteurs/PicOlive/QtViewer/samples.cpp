#include <stdio.h>
#include <qdebug.h>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <csv.h>
#include <QFileDialog>
#include "samples.h"

Samples::Samples()
{
    this->nbSample=0;
    //init curve
    this->pltCrvShtH= new QwtPlotCurve("Sht Humi");
    this->pltCrvShtH->setPen(QPen(Qt::black));
    this->pltCrvShtH->setYAxis(QwtPlot::yLeft);

    this->pltCrvShtT= new QwtPlotCurve("Sht Temp");
    this->pltCrvShtT->setPen(QPen(Qt::red));
    this->pltCrvShtT->setYAxis(QwtPlot::yLeft);

    this->pltCrvLm75= new QwtPlotCurve("LM Temp");
    this->pltCrvLm75->setPen(QPen(Qt::blue));
    this->pltCrvLm75->setYAxis(QwtPlot::yLeft);

    this->pltCrvMPX= new QwtPlotCurve("MPX Press");
    this->pltCrvMPX->setPen(QPen(Qt::magenta));
    this->pltCrvMPX->setYAxis(QwtPlot::yRight);

    //qDebug()<<"init sample";
    adTime=new double[NB_SAMPLE_JOUR];
    if(adTime==NULL)
    {
        qDebug()<<"error alloc adTime";
    }
    adTempLM75=new double[NB_SAMPLE_JOUR];
    if(adTempLM75==NULL)
    {
        qDebug()<<"error alloc adTempLM75";
    }
    adTempSHT11=new double[NB_SAMPLE_JOUR];
    if(adTempSHT11==NULL)
    {
        qDebug()<<"error alloc adTempSHT11";
    }
    adHumiSHT11=new double[NB_SAMPLE_JOUR];
    if(adHumiSHT11==NULL)
    {
        qDebug()<<"error alloc adHumiSHT11";
    }
    adPressMPX=new double[NB_SAMPLE_JOUR];
    if(adPressMPX==NULL)
    {
        qDebug()<<"error alloc adPressMPX";
    }
    adX=new double[NB_SAMPLE_JOUR];
    if(adX==NULL)
    {
        qDebug()<<"error alloc adX";
    }

    this->day=0;

}

void Samples::addNew(double time, double tLm, double tSht, double hSht, double pMPX)
{
    //qDebug()<<"add new" <<nbSample;
    if( this->nbSample<NB_SAMPLE_JOUR)
    {

        //qDebug()<<"add"<<nbSample<<adTime[nbSample]<<adTempLM75[nbSample];
        this->adTime[nbSample]=time;
        this->adTempLM75[nbSample]=tLm;
        this->adTempSHT11[nbSample]=tSht;
        this->adHumiSHT11[nbSample]=hSht;
        this->adPressMPX[nbSample]=pMPX;


        this->nbSample++;
    }
    else
    {
        qDebug()<<"plus de place";
    }

}

void Samples::clear()
{
    this->nbSample=0;
}





void Samples::attach(QwtPlot * plot)
{
    //qDebug()<<"attach";

    this->pltCrvLm75->attach(plot);
    this->pltCrvShtT->attach(plot);
    this->pltCrvShtH->attach(plot);
    this->pltCrvMPX->attach(plot);
}



void Samples::detach()
{
    //qDebug()<<"detach";

    this->pltCrvLm75->detach();
    this->pltCrvShtT->detach();
    this->pltCrvShtH->detach();
    this->pltCrvMPX->detach();
}


void Samples::showCurveTime()
{

    double adX[NB_SAMPLE_JOUR];
    for(int i=0;i<nbSample;i++)
    {
        int temp;
        temp=((int)adTime[i]/100)*60+(int)adTime[i]%100;
        adX[i]=temp;
        //qDebug()<<adX[i]<<adTime[i];
    }

    this->pltCrvLm75->setData(adX,adTempLM75,nbSample);
    this->pltCrvShtT->setData(adX,adTempSHT11,nbSample);
    this->pltCrvShtH->setData(adX,adHumiSHT11,nbSample);
    this->pltCrvMPX->setData(adX,adPressMPX,nbSample);
}

void Samples::showCurveX()
{
    double adX[NB_SAMPLE_JOUR];
    for(int i=0;i<nbSample;i++)
    {
        adX[i]=i;
    }

    this->pltCrvLm75->setData(adX,adTempLM75,nbSample);
    this->pltCrvShtT->setData(adX,adTempSHT11,nbSample);
    this->pltCrvShtH->setData(adX,adHumiSHT11,nbSample);
    this->pltCrvMPX->setData(adX,adPressMPX,nbSample);
    //Todo les autres courbes
}



void Samples::showLm(bool show)
{
    if(show)
        this->pltCrvLm75->show();
    else
        this->pltCrvLm75->hide();
}

void Samples::showShtT(bool show)
{
    if(show)
        this->pltCrvShtT->show();
    else
        this->pltCrvShtT->hide();
}

void Samples::showShtH(bool show)
{
    if(show)
        this->pltCrvShtH->show();
    else
        this->pltCrvShtH->hide();
}


void Samples::showMPX(bool show)
{
    if(show)
        this->pltCrvMPX->show();
    else
        this->pltCrvMPX->hide();
}


void Samples::getFisrtDay(Samples * smpl)
{
    int i;
    double oldtime=0;
    indexDay[0]=0;
    day=0;
    for(i=0;i<nbSample;i++)
    {
        if(oldtime>this->adTime[i])
        {
            indexDay[++day]=i;
            break;
        }
        smpl->addNew(adTime[i],adTempLM75[i],adTempSHT11[i],adHumiSHT11[i],adPressMPX[i]);
        oldtime=this->adTime[i];
        //qDebug()<<"add"<<adTime[i];
    }
    //qDebug()<<i;
}

bool Samples::getNextDay(Samples * smpl)
{
    int i;
    double oldtime=0;
    bool ret=false;
    smpl->clear();
    indexDay[day+1]=0;
    for(i=indexDay[day];i<nbSample;i++)
    {
        if(oldtime>this->adTime[i])
        {
            indexDay[++day]=i;
            ret=true;
            break;
        }
        smpl->addNew(adTime[i],adTempLM75[i],adTempSHT11[i],adHumiSHT11[i],adPressMPX[i]);
        oldtime=this->adTime[i];
        //qDebug()<<"add"<<adTime[i];
    }

    return ret;
}

bool Samples::getPrevDay(Samples * smpl)
{
    int i;
    double oldtime=0;
    bool ret=false;
    smpl->clear();
    if(day>0)
    {
        for(i=indexDay[day-2];i<nbSample;i++)
        {
            if(oldtime>this->adTime[i])
            {
                day--;
                if(day>1)
                {
                    ret= true;
                }
                break;
            }
            smpl->addNew(adTime[i],adTempLM75[i],adTempSHT11[i],adHumiSHT11[i],adPressMPX[i]);
            oldtime=this->adTime[i];
            //qDebug()<<"add"<<adTime[i];
        }

    }
    return ret;

}

void Samples::save(const QString &fileName)
{

    FILE * pFile;
    int i=0;

    if((pFile=fopen(fileName.toStdString().c_str(),"w"))==NULL)
    {
        qDebug() << "error ouverture fichier "<<fileName;
    }
    else
    {
        for ( i=0 ;i< nbSample; i++)
        {

            double dLM75=adTempLM75[i]*2;
            double dSHT11=adTempSHT11[i]*10;
            fprintf(pFile,"%d;%04d;%d;%d;%d;%d\n",i,(int)adTime[i],(int)adPressMPX[i],(int)(dLM75),(int)(dSHT11),(int)adHumiSHT11[i]);
        }
        fclose(pFile);
    }
}



int Samples::read(const QString &fileName)
{
    int line=0;
    char buff[100];


    QStringList list;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<< "cannot read file!";
    }
    else
    {
        CSV csv(&file);
        do
        {
            if(file.readLine(buff,100)!=-1)
            {
                list = csv.parseLine(buff);
                if(list.count()==6)
                {

                    QString str=list.takeFirst();
                    str=list.takeFirst();
                    double adtime=str.toDouble();

                    str=list.takeFirst();
                    double adMPX=str.toDouble();

                    str=list.takeFirst();
                    double adLm=str.toDouble()/2;

                    str=list.takeFirst();
                    double adShtT=str.toDouble()/10;

                    str=list.takeFirst();
                    double adShtH=str.toDouble();

                    this->addNew(adtime,adLm,adShtT,adShtH,adMPX);
                    line++;
                }
            }
            else
            {
                break;
            }
        }while(1);


        file.close();
    }
    return line;

}
