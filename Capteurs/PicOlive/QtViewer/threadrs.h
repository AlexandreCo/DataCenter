#ifndef THREADRS_H
#define THREADRS_H

#include <QThread>
#include <qextserialport.h>

#define TIME  0
#define READ  1
#define HISTO 2

class ThreadRs : public QThread
{
    Q_OBJECT

public:
    ThreadRs();
    virtual void run();
    void getData(void);
    void setTime(QString);
    void setPeriode(QString);
    void sendErase(void);
    void sendRaz();

private:

    void readArchive(void);
    void readHisto(void);
    void getTime(void);
    QString portName;
    char cGetNewData;
    QextSerialPort *port;
    bool bWrite;

signals:
    void finEmission();
    void newTime(char* msg);
};

#endif // THREADRS_H
