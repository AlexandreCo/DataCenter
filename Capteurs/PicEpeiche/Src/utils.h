#ifndef __UTILS_H
#define __UTILS_H

#define TRUE 	1
#define FALSE 	0

#define OLD_VERSION
#define RF_ID	1
#ifdef OLD_VERSION
#define FOSC	12000000
#define CODE_RF	0
#else
#define FOSC	20000000
#define CODE_RF	1
#endif

#endif