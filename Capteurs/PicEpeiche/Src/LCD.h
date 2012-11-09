#ifndef __LCD_H
#define __LCD_H

#define PARAM_SCLASS auto
#define NB_COL_LCD 		16  // taille de l'affichage 16 colonnes
#define NB_BARRE_LCD 	16  // taille de l'affichage 16 barres (pour les fonctions d'histo)
#define NB_LINE_LCD 	2  // taille de l'affichage 2 lignes
#define FIRST_LINE_ADDRESS	0x00
#define SECOND_LINE_ADDRESS	0x40


//unsigned char BusyXLCD(void);
void LCD_vInit(void);
void LCD_vDisplay(char *i);
//void SetDDRamAddr(PARAM_SCLASS unsigned char);
/* putsXLCD
 * Writes a string of characters to the LCD
 */
//void putsXLCD(PARAM_SCLASS char *);
void LCD_vSetBarre(unsigned char ucX,unsigned char ucValue);
void LCD_vHisto(unsigned int auiValHisto[NB_COL_LCD],unsigned char ucIndex);
void LCD_vWrite(char cLine,char cCol,char acBuf[NB_COL_LCD]);
#endif
