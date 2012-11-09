
#include <LCD.h>
#include <p18f4520.h>
#include <delays.h>

char _acLcdName[]="Pic Epeiche";
char _acLcdVer[]="V302";
/**************************************************************************/

/* 8-bit or 4-bit interface type
 * For 8-bit operation uncomment the #define BIT8
 */
#define BIT8 

/* When in 4-bit interface define if the data is in the upper or lower nibble.  
For lower nibble, comment the #define UPPER
 */
/* #define UPPER */

/* DATA_PORT defines the port which the LCD data lines are connected to */
#define DATA_PORT      PORTD
#define TRIS_DATA_PORT TRISD

/* CTRL_PORT defines the port where the control lines are connected.
 * These are just samples, change to match your application.
 */
#define RW_PIN   PORTAbits.RA2         /* PORT for RW */
#define TRIS_RW  DDRAbits.RA2          /* TRIS for RW */
#define RS_PIN   PORTAbits.RA3         /* PORT for RS */
#define TRIS_RS  DDRAbits.RA3          /* TRIS for RS */
#define E_PIN    PORTAbits.RA1         /* PORT for E  */
#define TRIS_E   DDRAbits.RA1          /* TRIS for E  */

/* Display ON/OFF Control defines */
#define DON         0b00001111        /* Display on     */
#define DOFF        0b00001011        /* Display off     */
#define CURSOR_ON   0b00001111     /* Cursor on      */
#define CURSOR_OFF  0b00001101     /* Cursor off      */
#define BLINK_ON    0b00001111       /* Cursor Blink    */
#define BLINK_OFF   0b00001110       /* Cursor No Blink */

/* Cursor or Display Shift defines */
#define SHIFT_CUR_LEFT    0b00010011  /* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT   0b00010111  /* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT   0b00011011  /* Display shifts to the left  */
#define SHIFT_DISP_RIGHT  0b00011111  /* Display shifts to the right */

/* Function Set defines */
#define FOUR_BIT   0b00101111  /* 4-bit Interface               */
#define EIGHT_BIT  0b00111111  /* 8-bit Interface               */
#define LINE_5X7   0b00110011  /* 5x7 characters, single line   */
#define LINE_5X10  0b00110111  /* 5x10 characters            */
#define LINES_5X7  0b00111011  /* 5x7 characters, multiple line */



#define MEM_MODEL far  /* Change this to near for small memory model */

/* OpenXLCD
 * Configures I/O pins for external LCD
 */
void OpenXLCD(PARAM_SCLASS unsigned char);

/* SetCGRamAddr
 * Sets the character generator address
 */
void SetCGRamAddr(PARAM_SCLASS unsigned char);

/* SetDDRamAddr
 * Sets the display data address
 */
void SetDDRamAddr(PARAM_SCLASS unsigned char);

/* BusyXLCD
 * Returns the busy status of the LCD
 */
unsigned char BusyXLCD(void);

/* ReadAddrXLCD
 * Reads the current address
 */
unsigned char ReadAddrXLCD(void);

/* ReadDataXLCD
 * Reads a byte of data
 */
char ReadDataXLCD(void);

/* WriteCmdXLCD
 * Writes a command to the LCD
 */
void WriteCmdXLCD(PARAM_SCLASS unsigned char);

/* WriteDataXLCD
 * Writes a data byte to the LCD
 */
void WriteDataXLCD(PARAM_SCLASS char);

/* putcXLCD
 * A putc is a write
 */
#define putcXLCD WriteDataXLCD



/* putrsXLCD
 * Writes a string of characters in ROM to the LCD
 */
void putrsXLCD(PARAM_SCLASS const MEM_MODEL rom char *);

void LCD_initCGRAM(void);


/* User defines these routines according to the oscillator frequency */
extern void DelayFor18TCY(void);
extern void DelayPORXLCD(void);
extern void DelayXLCD(void);


/********************************************************************
 *       Function Name:  BusyXLCD                             *
 *       Return Value:   char: busy status of LCD controller         *
 *       Parameters:     void                                   *
 *       Description:    This routine reads the busy status of the     *
 *                       Hitachi HD44780 LCD controller.          *
 ********************************************************************/
unsigned char BusyXLCD(void)
{
	RW_PIN = 1;                     // Set the control bits for read
	RS_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                      // Clock in the command
	DelayFor18TCY();
#ifdef BIT8                             // 8-bit interface
	if(DATA_PORT&0x80)                      // Read bit 7 (busy bit)
	{                               // If high
		E_PIN = 0;              // Reset clock line
		RW_PIN = 0;             // Reset control line
		return 1;               // Return TRUE
	}
	else                            // Bit 7 low
	{
		E_PIN = 0;              // Reset clock line
		RW_PIN = 0;             // Reset control line
		return 0;               // Return FALSE
	}
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
	if(DATA_PORT&0x80)
#else                                   // Lower nibble interface
		if(DATA_PORT&0x08)
#endif
		{
			E_PIN = 0;              // Reset clock line
			DelayFor18TCY();
			E_PIN = 1;              // Clock out other nibble
			DelayFor18TCY();
			E_PIN = 0;
			RW_PIN = 0;             // Reset control line
			return 1;               // Return TRUE
		}
		else                            // Busy bit is low
		{
			E_PIN = 0;              // Reset clock line
			DelayFor18TCY();
			E_PIN = 1;              // Clock out other nibble
			DelayFor18TCY();
			E_PIN = 0;
			RW_PIN = 0;             // Reset control line
			return 0;               // Return FALSE
		}
#endif
}

/********************************************************************
 *       Function Name:  OpenXLCD                                    *
 *       Return Value:   void                                        *
 *       Parameters:     lcdtype: sets the type of LCD (lines)       *
 *       Description:    This routine configures the LCD. Based on   *
 *                       the Hitachi HD44780 LCD controller. The     *
 *                       routine will configure the I/O pins of the  *
 *                       microcontroller, setup the LCD for 4-bit or    *
 *                       8-bit mode and clear screen. The user  *
 *                       must provide three delay routines:          *
 *                       DelayFor18TCY() provides a 18 Tcy delay     *
 *                       DelayPORXLCD() provides at least 15ms delay *
 *                       DelayXLCD() provides at least 5ms delay     *
 ********************************************************************/
void OpenXLCD(unsigned char lcdtype)
{
	// The data bits must be either a 8-bit port or the upper or
	// lower 4-bits of a port. These pins are made into inputs
#ifdef BIT8                             // 8-bit mode, use whole port
	DATA_PORT &= 0;
	TRIS_DATA_PORT |= 0xff;
#else                                   // 4-bit mode
#ifdef UPPER                            // Upper 4-bits of the port
	DATA_PORT &= 0x0f;
	TRIS_DATA_PORT |= 0xf0;
#else                                   // Lower 4-bits of the port
	DATA_PORT &= 0xf0;
	TRIS_DATA_PORT |= 0x0f;
#endif
#endif
	TRIS_RW = 0;                    // All control signals made outputs
	TRIS_RS = 0;
	TRIS_E = 0;
	RW_PIN = 0;                     // R/W pin made low
	RS_PIN = 0;                     // Register select pin made low
	E_PIN = 0;                      // Clock pin made low

	// Delay for 15ms to allow for LCD Power on reset
	DelayPORXLCD();

	// Setup interface to LCD
#ifdef BIT8                             // 8-bit mode interface
	TRIS_DATA_PORT &= 0;             // Data port output
	DATA_PORT &= 0;
	DATA_PORT |= 0b00110000;         // Function set cmd(8-bit interface)
#else                                   // 4-bit mode interface
#ifdef UPPER                            // Upper nibble interface
	TRIS_DATA_PORT &= 0x0f;
	DATA_PORT &= 0x0f;
	DATA_PORT |= 0b00100000;        // Function set cmd(4-bit interface)
#else                                   // Lower nibble interface
	TRIS_DATA_PORT &= 0xf0;
	DATA_PORT &= 0xf0;
	DATA_PORT |= 0b00000010;        // Function set cmd(4-bit interface)
#endif
#endif
	E_PIN = 1;                      // Clock the cmd in
	DelayFor18TCY();
	E_PIN = 0;

	// Delay for at least 4.1ms
	DelayXLCD();

	// Setup interface to LCD
#ifdef BIT8                             // 8-bit interface
	DATA_PORT &= 0;
	DATA_PORT |= 0b00110000;         // Function set cmd(8-bit interface)
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
	DATA_PORT &= 0x0f;              // Function set cmd(4-bit interface)
	DATA_PORT |= 0b00100000;
#else                                   // Lower nibble interface
	DATA_PORT &= 0xf0;              // Function set cmd(4-bit interface)
	DATA_PORT |= 0b00000010;
#endif
#endif
	E_PIN = 1;                      // Clock the cmd in
	DelayFor18TCY();
	E_PIN = 0;

	// Delay for at least 100us
	DelayXLCD();

	// Setup interface to LCD
#ifdef BIT8                             // 8-bit interface
	DATA_PORT &= 0;
	DATA_PORT |= 0b00110000;         // Function set cmd(8-bit interface)
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
	DATA_PORT &= 0x0f;              // Function set cmd(4-bit interface)
	DATA_PORT |= 0b00100000;
#else                                   // Lower nibble interface
	DATA_PORT &= 0xf0;              // Function set cmd(4-bit interface)
	DATA_PORT |= 0b00000010;
#endif
#endif
	E_PIN = 1;                      // Clock cmd in
	DelayFor18TCY();
	E_PIN = 0;

#ifdef BIT8                             // 8-bit interface
	TRIS_DATA_PORT |= 0xff;          // Make data port input
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
	TRIS_DATA_PORT |= 0xf0;         // Make data nibble input
#else                                   // Lower nibble interface
	TRIS_DATA_PORT |= 0x0f;         // Make data nibble input
#endif
#endif

	// Set data interface width, # lines, font
	while(BusyXLCD());              // Wait if LCD busy
	WriteCmdXLCD(lcdtype);          // Function set cmd

	// Turn the display on then off
	while(BusyXLCD());              // Wait if LCD busy
	WriteCmdXLCD(DOFF);        // Display OFF/Blink OFF
	while(BusyXLCD());              // Wait if LCD busy
	WriteCmdXLCD(DON);           // Display ON/Blink ON
	while(BusyXLCD());              // Wait if LCD busy
	WriteCmdXLCD(BLINK_OFF&CURSOR_OFF);     // Display ON/Blink OFF

	// Clear display
	while(BusyXLCD());              // Wait if LCD busy
	WriteCmdXLCD(0x01);             // Clear display

	// Set entry mode inc, no shift
	while(BusyXLCD());              // Wait if LCD busy
	WriteCmdXLCD(SHIFT_CUR_LEFT);   // Entry Mode

	// Set DD Ram address to 0
	while(BusyXLCD());              // Wait if LCD busy
	SetDDRamAddr(FIRST_LINE_ADDRESS);                // Set Display data ram address to 0

	return;
}



/********************************************************************
 *       Function Name:  putrsXLCD
 *       Return Value:   void
 *       Parameters:     buffer: pointer to string
 *       Description:    This routine writes a string of bytes to the
 *                       Hitachi HD44780 LCD controller. The user
 *                       must check to see if the LCD controller is
 *                       busy before calling this routine. The data
 *                       is written to the character generator RAM or
 *                       the display data RAM depending on what the
 *                       previous SetxxRamAddr routine was called.
 ********************************************************************/
/*
void putrsXLCD(const rom char *buffer)
{
        while(*buffer)                  // Write data to LCD up to null
        {
                while(BusyXLCD());      // Wait while LCD is busy
                WriteDataXLCD(*buffer); // Write character to LCD
                buffer++;               // Increment buffer
        }
        return;
}

 */
/********************************************************************
 *       Function Name:  putsXLCD
 *       Return Value:   void
 *       Parameters:     buffer: pointer to string
 *       Description:    This routine writes a string of bytes to the
 *                       Hitachi HD44780 LCD controller. The user
 *                       must check to see if the LCD controller is
 *                       busy before calling this routine. The data
 *                       is written to the character generator RAM or
 *                       the display data RAM depending on what the
 *                       previous SetxxRamAddr routine was called.
 ********************************************************************/
void putsXLCD(char *buffer)
{
	char i=0;
	for(i=0;i<16&&buffer[i]!='\0';i++)
	{
		while(BusyXLCD());      // Wait while LCD is busy
		WriteDataXLCD(buffer[i]); // Write character to LCD
	}
	return;
}


/*********************************************************************
 *       Function Name:  ReadAddrXLCD                                 *
 *       Return Value:   char: address from LCD controller            *
 *       Parameters:     void                                         *
 *       Description:    This routine reads an address byte from the  *
 *                       Hitachi HD44780 LCD controller. The user     *
 *                       must check to see if the LCD controller is   *
 *                       busy before calling this routine. The address*
 *                       is read from the character generator RAM or  *
 *                       the display data RAM depending on what the   *
 *                       previous SetxxRamAddr routine was called.    *
 *********************************************************************/
unsigned char ReadAddrXLCD(void)
{
	char data;                      // Holds the data retrieved from the LCD

#ifdef BIT8                             // 8-bit interface
	RW_PIN = 1;                     // Set control bits for the read
	RS_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                      // Clock data out of the LCD controller
	DelayFor18TCY();
	data = DATA_PORT;               // Save the data in the register
	E_PIN = 0;
	RW_PIN = 0;                     // Reset the control bits
#else                                   // 4-bit interface
	RW_PIN = 1;                     // Set control bits for the read
	RS_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                      // Clock data out of the LCD controller
	DelayFor18TCY();
#ifdef UPPER                            // Upper nibble interface
	data = DATA_PORT&0xf0;          // Read the nibble into the upper nibble of data
#else                                   // Lower nibble interface
	data = (DATA_PORT<<4)&0xf0;     // Read the nibble into the upper nibble of data
#endif
	E_PIN = 0;                      // Reset the clock
	DelayFor18TCY();
	E_PIN = 1;                      // Clock out the lower nibble
	DelayFor18TCY();
#ifdef UPPER                            // Upper nibble interface
	data |= (DATA_PORT>>4)&0x0f;    // Read the nibble into the lower nibble of data
#else                                   // Lower nibble interface
	data |= DATA_PORT&0x0f;         // Read the nibble into the lower nibble of data
#endif
	E_PIN = 0;
	RW_PIN = 0;                     // Reset the control lines
#endif
	return (data&0x7f);             // Return the address, Mask off the busy bit
}


/********************************************************************
 *       Function Name:  ReadDataXLCD                                *
 *       Return Value:   char: data byte from LCD controller         *
 *       Parameters:     void                                        *
 *       Description:    This routine reads a data byte from the     *
 *                       Hitachi HD44780 LCD controller. The user    *
 *                       must check to see if the LCD controller is  *
 *                       busy before calling this routine. The data  *
 *                       is read from the character generator RAM or *
 *                       the display data RAM depending on what the  *
 *                       previous SetxxRamAddr routine was called.   *
 ********************************************************************/
char ReadDataXLCD(void)
{
	char data;

#ifdef BIT8                             // 8-bit interface
	RS_PIN = 1;                     // Set the control bits
	RW_PIN = 1;
	DelayFor18TCY();
	E_PIN = 1;                      // Clock the data out of the LCD
	DelayFor18TCY();
	data = DATA_PORT;               // Read the data
	E_PIN = 0;
	RS_PIN = 0;                     // Reset the control bits
	RW_PIN = 0;
#else                                   // 4-bit interface
	RW_PIN = 1;
	RS_PIN = 1;
	DelayFor18TCY();
	E_PIN = 1;                      // Clock the data out of the LCD
	DelayFor18TCY();
#ifdef UPPER                            // Upper nibble interface
	data = DATA_PORT&0xf0;          // Read the upper nibble of data
#else                                   // Lower nibble interface
	data = (DATA_PORT<<4)&0xf0;     // read the upper nibble of data
#endif
	E_PIN = 0;                      // Reset the clock line
	DelayFor18TCY();
	E_PIN = 1;                      // Clock the next nibble out of the LCD
	DelayFor18TCY();
#ifdef UPPER                            // Upper nibble interface
	data |= (DATA_PORT>>4)&0x0f;    // Read the lower nibble of data
#else                                   // Lower nibble interface
	data |= DATA_PORT&0x0f;         // Read the lower nibble of data
#endif
	E_PIN = 0;
	RS_PIN = 0;                     // Reset the control bits
	RW_PIN = 0;
#endif
	return(data);                   // Return the data byte
}


/********************************************************************
 *       Function Name:  SetCGRamAddr                                *
 *       Return Value:   void                                        *
 *       Parameters:     CGaddr: character generator ram address     *
 *       Description:    This routine sets the character generator   *
 *                       address of the Hitachi HD44780 LCD          *
 *                       controller. The user must check to see if   *
 *                       the LCD controller is busy before calling   *
 *                       this routine.                               *
 ********************************************************************/
void SetCGRamAddr(unsigned char CGaddr)
{
#ifdef BIT8                                     // 8-bit interface
	TRIS_DATA_PORT = 0;                     // Make data port ouput
	DATA_PORT = CGaddr | 0b01000000;        // Write cmd and address to port
	RW_PIN = 0;                             // Set control signals
	RS_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                              // Clock cmd and address in
	DelayFor18TCY();
	E_PIN = 0;
	DelayFor18TCY();
	TRIS_DATA_PORT = 0xff;                  // Make data port inputs
#else                                           // 4-bit interface
#ifdef UPPER                                    // Upper nibble interface
	TRIS_DATA_PORT &= 0x0f;                 // Make nibble input
	DATA_PORT &= 0x0f;                      // and write upper nibble
	DATA_PORT |= ((CGaddr | 0b01000000) & 0xf0);
#else                                           // Lower nibble interface
	TRIS_DATA_PORT &= 0xf0;                 // Make nibble input
	DATA_PORT &= 0xf0;                      // and write upper nibble
	DATA_PORT |= (((CGaddr |0b01000000)>>4) & 0x0f);
#endif
	RW_PIN = 0;                             // Set control signals
	RS_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                              // Clock cmd and address in
	DelayFor18TCY();
	E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
	DATA_PORT &= 0x0f;                      // Write lower nibble
	DATA_PORT |= ((CGaddr<<4)&0xf0);
#else                                           // Lower nibble interface
	DATA_PORT &= 0xf0;                      // Write lower nibble
	DATA_PORT |= (CGaddr&0x0f);
#endif
	DelayFor18TCY();
	E_PIN = 1;                              // Clock cmd and address in
	DelayFor18TCY();
	E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
	TRIS_DATA_PORT |= 0xf0;                 // Make inputs
#else                                           // Lower nibble interface
	TRIS_DATA_PORT |= 0x0f;                 // Make inputs
#endif
#endif
	return;
}


/********************************************************************
 *       Function Name:  SetDDRamAddr                                *
 *       Return Value:   void                                        *
 *       Parameters:     CGaddr: display data address                *
 *       Description:    This routine sets the display data address  *
 *                       of the Hitachi HD44780 LCD controller. The  *
 *                       user must check to see if the LCD controller*
 *                       is busy before calling this routine.        *
 ********************************************************************/
void SetDDRamAddr(unsigned char DDaddr)
{
#ifdef BIT8                                     // 8-bit interface
	TRIS_DATA_PORT = 0;                     // Make port output
	DATA_PORT = DDaddr | 0b10000000;        // Write cmd and address to port
	RW_PIN = 0;                             // Set the control bits
	RS_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                              // Clock the cmd and address in
	DelayFor18TCY();
	E_PIN = 0;
	DelayFor18TCY();
	TRIS_DATA_PORT = 0xff;                  // Make port input
#else                                           // 4-bit interface
#ifdef UPPER                                    // Upper nibble  interface
	TRIS_DATA_PORT &= 0x0f;                 // Make port output
	DATA_PORT &= 0x0f;                      // and write upper nibble
	DATA_PORT |= ((DDaddr | 0b10000000) & 0xf0);
#else                                           // Lower nibble interface
	TRIS_DATA_PORT &= 0xf0;                 // Make port output
	DATA_PORT &= 0xf0;                      // and write upper nibble
	DATA_PORT |= (((DDaddr | 0b10000000)>>4) & 0x0f);
#endif
	RW_PIN = 0;                             // Set control bits
	RS_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                              // Clock the cmd and address in
	DelayFor18TCY();
	E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
	DATA_PORT &= 0x0f;                      // Write lower nibble
	DATA_PORT |= ((DDaddr<<4)&0xf0);
#else                                           // Lower nibble interface
	DATA_PORT &= 0xf0;                    // Write lower nibble
	DATA_PORT |= (DDaddr&0x0f);
#endif
	DelayFor18TCY();
	E_PIN = 1;                              // Clock the cmd and address in
	DelayFor18TCY();
	E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
	TRIS_DATA_PORT |= 0xf0;                 // Make port input
#else                                           // Lower nibble interface
	TRIS_DATA_PORT |= 0x0f;                 // Make port input
#endif
#endif
	return;
}


/********************************************************************
 *       Function Name:  WriteCmdXLCD                                *
 *       Return Value:   void                                        *
 *       Parameters:     cmd: command to send to LCD                 *
 *       Description:    This routine writes a command to the Hitachi*
 *                       HD44780 LCD controller. The user must check *
 *                       to see if the LCD controller is busy before *
 *                       calling this routine.                       *
 ********************************************************************/
void WriteCmdXLCD(unsigned char cmd)
{
#ifdef BIT8                             // 8-bit interface
	TRIS_DATA_PORT &= 0;         // Data port output
	DATA_PORT &= 0;
	DATA_PORT |= cmd;              // Write command to data port
	RW_PIN = 0;                     // Set the control signals
	RS_PIN = 0;                     // for sending a command
	DelayFor18TCY();
	E_PIN = 1;                      // Clock the command in
	DelayFor18TCY();
	E_PIN = 0;
	DelayFor18TCY();
	TRIS_DATA_PORT |= 0xff;        // Data port input
#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
	TRIS_DATA_PORT &= 0x0f;
	DATA_PORT &= 0x0f;
	DATA_PORT |= cmd&0xf0;
#else                                   // Lower nibble interface
	TRIS_DATA_PORT &= 0xf0;
	DATA_PORT &= 0xf0;
	DATA_PORT |= (cmd>>4)&0x0f;
#endif
	RW_PIN = 0;                     // Set control signals for command
	RS_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                      // Clock command in
	DelayFor18TCY();
	E_PIN = 0;
#ifdef UPPER                            // Upper nibble interface
	DATA_PORT &= 0x0f;
	DATA_PORT |= (cmd<<4)&0xf0;
#else                                   // Lower nibble interface
	DATA_PORT &= 0xf0;
	DATA_PORT |= cmd&0x0f;
#endif
	DelayFor18TCY();
	E_PIN = 1;                      // Clock command in
	DelayFor18TCY();
	E_PIN = 0;
#ifdef UPPER                            // Make data nibble input
	TRIS_DATA_PORT |= 0xf0;
#else
	TRIS_DATA_PORT |= 0x0f;
#endif
#endif
	return;
}


/********************************************************************
 *       Function Name:  WriteDataXLCD                               *
 *       Return Value:   void                                        *
 *       Parameters:     data: data byte to be written to LCD        *
 *       Description:    This routine writes a data byte to the      *
 *                       Hitachi HD44780 LCD controller. The user    *
 *                       must check to see if the LCD controller is  *
 *                       busy before calling this routine. The data  *
 *                       is written to the character generator RAM or*
 *                       the display data RAM depending on what the  *
 *                       previous SetxxRamAddr routine was called.   *
 ********************************************************************/
void WriteDataXLCD(char data)
{
#ifdef BIT8                             // 8-bit interface
	TRIS_DATA_PORT = 0;         // Make port output
	DATA_PORT = data;            // Write data to port
	RS_PIN = 1;                    // Set control bits
	RW_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                     // Clock data into LCD
	DelayFor18TCY();
	E_PIN = 0;
	RS_PIN = 0;                    // Reset control bits
	TRIS_DATA_PORT = 0xff;          // Make port input
#else                                  // 4-bit interface
#ifdef UPPER                           // Upper nibble interface
	TRIS_DATA_PORT &= 0x0f;
	DATA_PORT &= 0x0f;
	DATA_PORT |= data&0xf0;
#else                                   // Lower nibble interface
	TRIS_DATA_PORT &= 0xf0;
	DATA_PORT &= 0xf0;
	DATA_PORT |= ((data>>4)&0x0f);
#endif
	RS_PIN = 1;                     // Set control bits
	RW_PIN = 0;
	DelayFor18TCY();
	E_PIN = 1;                      // Clock nibble into LCD
	DelayFor18TCY();
	E_PIN = 0;
#ifdef UPPER                            // Upper nibble interface
	DATA_PORT &= 0x0f;
	DATA_PORT |= ((data<<4)&0xf0);
#else                                   // Lower nibble interface
	DATA_PORT &= 0xf0;
	DATA_PORT |= (data&0x0f);
#endif
	DelayFor18TCY();
	E_PIN = 1;                      // Clock nibble into LCD
	DelayFor18TCY();
	E_PIN = 0;
#ifdef UPPER                            // Upper nibble interface
	TRIS_DATA_PORT |= 0xf0;
#else                                   // Lower nibble interface
	TRIS_DATA_PORT |= 0x0f;
#endif
#endif
	return;
}

void DelayFor18TCY( void )
{
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
}
void DelayPORXLCD (void)
{
	Delay1KTCYx(75); // Delay of 15ms
	// Cycles = (TimeDelay * Fosc) / 4
	// Cycles = (15ms * 20MHz) / 4
	// Cycles = 75,000
	return;
}
void DelayXLCD (void)
{
	Delay1KTCYx(25); // Delay of 5ms
	// Cycles = (TimeDelay * Fosc) / 4
	// Cycles = (5ms * 20MHz) / 4
	// Cycles = 25,000
	return;
}
void vDelayInit(void)
{
	Delay10KTCYx(200);
}
void LCD_vInit(void)
{		
	int i;
int j;
	OpenXLCD( EIGHT_BIT&LINES_5X7 );// Configure External LCD
	LCD_initCGRAM();

	while(BusyXLCD());             	// Wait if LCD is busy
	SetDDRamAddr(FIRST_LINE_ADDRESS);	

	while(BusyXLCD());              	// Wait if LCD is busy
	putsXLCD(_acLcdName);			// write to LCD

	while(BusyXLCD());             	// Wait if LCD is busy
	SetDDRamAddr(SECOND_LINE_ADDRESS);				// Set Display data RAM address to 0x40
	putsXLCD(_acLcdVer);				// write to LCD

	vDelayInit();

}
void LCD_vSetBarre(unsigned char ucX,unsigned char ucValue)
{
	if(ucValue>NB_BARRE_LCD)
		ucValue=NB_BARRE_LCD;
	while(BusyXLCD());    
	SetDDRamAddr(FIRST_LINE_ADDRESS+ucX);
	if(ucValue<9)
	{
		while(BusyXLCD());
		WriteDataXLCD(0X10);
	}
	else
	{
		while(BusyXLCD());
		WriteDataXLCD(ucValue-9);
	}
	while(BusyXLCD());
	SetDDRamAddr(SECOND_LINE_ADDRESS+ucX);
	if(ucValue>7)
	{
		while(BusyXLCD());
		WriteDataXLCD(7);
	}
	else
	{
		if(ucValue)
		{
			while(BusyXLCD());
			WriteDataXLCD(ucValue-1);
		}
		else
		{
			while(BusyXLCD());
			WriteDataXLCD(0x10);
		}
	}


}		

void LCD_initCGRAM(void)
{
	int i,j;


	SetDDRamAddr(0);
	SetCGRamAddr(0);
	while(BusyXLCD()); 
	for(j=0;j<8;j++)
		for(i=0;i<8;i++)
		{
			if(i<(7-j))
			{
				while(BusyXLCD());      // Wait while LCD is busy
				WriteDataXLCD(0x00);
			}
			else
			{
				while(BusyXLCD());      // Wait while LCD is busy
				WriteDataXLCD(0x1F);
			}
		}
	while(BusyXLCD()); 

}
void LCD_vDisplay(char *i)
{
	// Set DD Ram address to 0
	while(BusyXLCD());              	// Wait if LCD is busy
	SetDDRamAddr(SECOND_LINE_ADDRESS);             // Set Display data RAM address

	// write to LCD
	while(BusyXLCD());              	// Wait if LCD is busy
	putsXLCD(i);

}
//affiche une histo avec les valeurs du tableau, l'index de debut peut ne pas etre à 0
//buffer tournant
void LCD_vHisto(unsigned int auiValHisto[NB_COL_LCD],unsigned char ucIndex)
{
	char i;
	char acBuf[20];
	unsigned int uiMax,uiMin,uiQuantum,uiEqui,uiValue;


	//Calcul des max /min
	uiMax=auiValHisto[0];
	uiMin=auiValHisto[0];
	for(i=0;i<NB_COL_LCD;i++)
	{

		if(auiValHisto[i]>uiMax)
			uiMax=auiValHisto[i];

		if(auiValHisto[i]<uiMin)
			uiMin=auiValHisto[i];
#ifdef DEBUG
		printf("(%d %d %d %d)\n\r",i,auiValHisto[i],uiMax,uiMin);
#endif
	}

	//affichage de l'histo
	uiQuantum=((uiMax-uiMin)/NB_BARRE_LCD)+1;
	uiEqui=((uiMax-uiMin)/2)/uiQuantum;
#ifdef DEBUG
	printf("\n\rmax=%d min=%d Q=%d E=%d\n\r",uiMax,uiMin,uiQuantum,uiEqui);
#endif
	for(i=0;i<NB_BARRE_LCD;i++,ucIndex++)
	{
		//construction des barres lignes par lignes
		uiValue=((auiValHisto[ucIndex%NB_BARRE_LCD]-uiMin)/uiQuantum)+(NB_BARRE_LCD/2)-uiEqui;
#ifdef DEBUG
		printf("%d ",uiValue);
#endif
		LCD_vSetBarre(i,uiValue);
	}
#ifdef DEBUG
	printf("%d \n\r");
#endif
}

void LCD_vWrite(char cLine,char cCol,char acBuf[NB_COL_LCD])
{
	while(BusyXLCD());              // Wait if LCD is busy
	SetDDRamAddr(cLine*SECOND_LINE_ADDRESS+cCol);      // Set Display data RAM address
	putsXLCD(acBuf);
}
