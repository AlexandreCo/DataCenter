#include <utils.h>
#include <stdio.h>
unsigned int _uiDiSeconde=0;
unsigned int _uiHeure=0;
unsigned int _uiMinute=0;
unsigned int _uiSeconde=0;

//realise une action toutes les n secondes (max 3600)
char cron_bActionSec(unsigned int uiSec)
{
	static char _bActionFaite=FALSE;
	char bRet=FALSE;
#ifdef DEBUG
	printf("time %02d:%02d:%02d %d/%d\n\r",_uiHeure,_uiMinute,_uiSeconde,((_uiSeconde+(_uiMinute*60)) % uiSec),uiSec);
#endif

	if(((_uiSeconde + (_uiMinute *60)) % uiSec)==0)
	{
		if(_bActionFaite==FALSE)
		{
			bRet=TRUE;
			_bActionFaite=TRUE;
		}
	}
	else
	{
		_bActionFaite=FALSE;
	}
	return bRet;
}

//realise une action toutes les n minute (max 1440)
char cron_bActionMin(unsigned int uiMin)
{
	static char _bActionFaite=FALSE;
	char bRet=FALSE;
#ifdef DEBUG
	printf("time %02d:%02d:%02d %d/%d\n\r",_uiHeure,_uiMinute,_uiSeconde,((_uiMinute+(_uiHeure*60)) %uiMin),uiMin);
#endif
	if(((_uiMinute+(_uiHeure*60)) %uiMin)==0)
	{
		if(_bActionFaite==FALSE)
		{
			bRet=TRUE;
			_bActionFaite=TRUE;
		}
	}
	else
	{
		_bActionFaite=FALSE;
	}
	return bRet;
}


void cron_vSetMin(char uiMinute)
{
	_uiMinute=uiMinute;
}

void cron_vSetHeure(char uiHeure)
{
	_uiHeure=uiHeure;
}
void cron_vSetSec(char uiSeconde)
{
	_uiSeconde=uiSeconde;
}

void cron_vGetTime(char * buf)
{
	sprintf(buf,"%02d:%02d:%02d\r",_uiHeure,_uiMinute,_uiSeconde);
}

unsigned int cron_uiHeure(void)
{
	return _uiHeure;
}
unsigned int cron_uiMinute(void)
{
	return _uiMinute;
}
unsigned int cron_uiSeconde(void)
{
	return _uiSeconde;
}


//Cette fonction doit etre appelé toutes les dixiemes de secondes
void cron_addDixieme(void)
{
	_uiDiSeconde++;
	if(_uiDiSeconde>10)
	{
		_uiDiSeconde=0;
		_uiSeconde++;
		if(_uiSeconde>59)
		{
			_uiSeconde=0;
			_uiMinute++;
			if(_uiMinute>59)
			{
				_uiMinute=0;
				_uiHeure++;
				if(_uiHeure>23)
				{
					_uiHeure=0;
				}
			}
		}
	}
}
