#include "threadrs.h"

#include "qdebug.h"
#include <qextserialport.h>

ThreadRs::ThreadRs()
{

    port = new QextSerialPort();
    // L’ordre est important
    portName="/dev/ttyUSB0";
    port->setPortName(portName);
    port->open( QIODevice::ReadWrite );
    port->setFlowControl(FLOW_OFF);
    port->setParity(PAR_NONE);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    port->setBaudRate(BAUD9600);
    cGetNewData=TIME;
    bWrite=false;
}



void ThreadRs::run()
{

    while(1)
    {
        if(bWrite==false)
        {
            if(  port->isOpen( ) ) /* reception de donnees */
            {
                switch(cGetNewData)
                {
                case READ :
                    readArchive();
                    cGetNewData=TIME;
                    break;

                case HISTO :
                    readHisto();
                    cGetNewData=TIME;
                    break;
                case TIME :
                    getTime();
                    break;
                }

            }
            else
            {
                //port pas open todo
                qDebug()<<"port pas open\n";
                sleep(5);
            }
        }
    }
    port->close(); /*fermeture du port */
}

void ThreadRs::getTime(void)
{
    int taille;
    int numBytes;
    char buff[100];
    char index=0;
    sleep(1);

    port->write("gettime",7);
    msleep(100);
    port->putChar(0x0d);
    msleep(100);
    do
    {
        numBytes = port->bytesAvailable();
        if(numBytes>=0)
        {
            taille=port->readLine(&buff[index],100);
            index+=taille;
        }
    }while(numBytes>0);
    index=0;
    emit this->newTime(buff);
}

void ThreadRs::readArchive(void)
{
    QFile *file = new QFile("temp.csv");

    file->remove();
    file->open( QFile::ReadWrite );
    port->flush();
    msleep(100);
    port->putChar(0x0d);
    msleep(100);
    port->write("read",4);
    msleep(100);
    port->putChar(0x0d);
    bool isNotEnd=true;
    do
    {
        char buff[100];
        qint64 lineLength = port->read(buff, sizeof(buff));
        file->write(buff,lineLength);

        for(int i=0;i<lineLength;i++)
        {
            if(buff[i]=='f')
            {
                isNotEnd=false;
            }

        }
        //qDebug()<<lineLength<<buff;
    }
    while(isNotEnd);
    file->close();
    emit this->finEmission();
}

void ThreadRs::readHisto(void)
{
    QFile *file = new QFile("temp.csv");

    file->remove();
    file->open( QFile::ReadWrite );
    port->flush();
    msleep(100);
    port->putChar(0x0d);
    msleep(100);
    port->write("histo",5);
    msleep(100);
    port->putChar(0x0d);
    bool isNotEnd=true;
    do
    {
        char buff[100];
        qint64 lineLength = port->read(buff, sizeof(buff));
        file->write(buff,lineLength);

        for(int i=0;i<lineLength;i++)
        {
            if(buff[i]=='f')
            {
                isNotEnd=false;
            }

        }
        //qDebug()<<lineLength<<buff;
    }
    while(isNotEnd);
    file->close();
    emit this->finEmission();
}


void ThreadRs::getData(void)
{
    cGetNewData=READ;
}

void ThreadRs::sendRaz()
{
    bWrite=true;
    sleep(1);
    port->flush();
    msleep(100);
    port->putChar(0x0d);
    msleep(100);
    port->write("raz");
    msleep(100);
    port->putChar(0x0d);
    sleep(1);
    port->flush();
    bWrite=false;


}


void ThreadRs::sendErase()
{
    bWrite=true;
    sleep(1);
    port->flush();
    msleep(100);
    port->putChar(0x0d);
    msleep(100);
    port->write("erase");
    msleep(100);
    port->putChar(0x0d);
    sleep(1);
    port->flush();
    bWrite=false;

}



void ThreadRs::setTime(QString str)
{
    bWrite=true;
    sleep(1);
    port->putChar(0x0d);
    msleep(100);
    for(int i=0;i<str.length();i++)
    {
        if(str[i]==':')
        {
            str[i]=0x0d;
        }
    }
    qDebug()<<str;
    port->write("settime");
    port->putChar(0x0d);
    msleep(100);
    for(int i=0;i<str.length();i++)
    {
        port->putChar(str[i].toAscii());
        msleep(100);
    }
    port->putChar(0x0d);
    sleep(1);
    port->flush();
    bWrite=false;

}

void ThreadRs::setPeriode(QString str)
 {
     bWrite=true;
     sleep(1);
     port->putChar(0x0d);
     msleep(100);
     port->write("periode");
     port->putChar(0x0d);
     msleep(100);
     for(int i=0;i<str.length();i++)
     {
         port->putChar(str[i].toAscii());
         msleep(100);
     }
     port->putChar(0x0d);
     sleep(1);
     port->flush();
     bWrite=false;
 }
