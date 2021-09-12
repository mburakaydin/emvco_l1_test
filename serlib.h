/**
 * @file serlib.h
 * main operation related defines and function prototypes.
 *
 * @par Company
 * Kentkart Ege Elektronik
 * @author Sezai Cam
 * @version 1.01
 * @warning .
 *
 * @par VSS Details:
 * $Archive: /Yazilim/xkb/valapp_2/serlib.h $
 * $Author: Aydinb $
 * $Revision: 1 $
 * $Date: 8.04.08 20:33 $
 */

#ifndef _SERLIB_H_
#define _SERLIB_H_


//#define POSIX_SOURCE 1
//#define FALSE     0
//#define TRUE      1

//#define SERIAL_TIMEOUT    10      // 10 corresponds to 1 sec.

#define SERIALLIB_VERSION "V1.0.1.2"


extern int idt_SerialOpen(char *ComStr, char *BaudStr, int timeout);
extern int idt_SerialOpenwithOptions(char *ComStr, char *BaudStr, int timeout, char *options);
extern void idt_SerialClose(int _Handle);
extern int idt_SerialSendChar(int _Handle, unsigned char c);
extern int idt_SerialSendChar_tcdrain(int _Handle, unsigned char c);
extern int idt_SerialReceiveChar(int _Handle, unsigned char *c);
extern int idt_SerialReceiveCharWithTimeout(int _Handle, unsigned char *c, unsigned int firstCharTimeout);
extern void idt_SerialPurge(int _Handle);
extern int idt_SerialSendBuf(int _Handle, unsigned char *c,int len);
extern int idt_SerialGetReceiveBufferByteCount(int _Handle, unsigned int *bytes);
extern int idt_SerialChangeSpeed(int _Handle, char *BaudStr);
#endif
