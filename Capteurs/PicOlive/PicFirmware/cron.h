#ifndef __CRON_H
#define __CRON_H
void cron_addDixieme(void);
char cron_bActionMin(unsigned int uiMin);
char cron_bActionSec(unsigned int uiSec);

void cron_vSetMin(char uiMinute);
void cron_vSetSec(char uiSeconde);
void cron_vSetHeure(char uiHeure);

unsigned int cron_uiHeure(void);
unsigned int cron_uiMinute(void);
unsigned int cron_uiSeconde(void);
#endif


