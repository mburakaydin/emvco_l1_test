
/**
 * @file serlib.c
 * Serial communication related defines and function prototypes.
 *
 * @par Company
 * Kentkart Ege Elektronik
 *
 * @par VSS Details:
 * $Archive: /Yazilim/xkb/valapp_2/serlib.c $
 * $Author$
 * $Revision$
 * $Date$
 */

//#include "bk4410.h"
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define OK 0
#define ERR -1

#include "serlib.h"

/**
 * Open Serial Port
 * @param ComStr [in] communication port
 * @param BaudStr [in]
 * @return Returns handle, if an error occurs ERR is returned.
 * @ingroup serial
 */
int idt_SerialOpen(char *ComStr, char *BaudStr, int timeout)
{
	int _Handle, n;
	struct termios newtio;
	struct serial_struct serial;

	_Handle = open(ComStr, O_RDWR | O_NOCTTY | O_NDELAY);
	if (_Handle >= 0) {
		n = fcntl(_Handle, F_GETFL, 0);
		fcntl(_Handle, F_SETFL, n & ~O_NDELAY);
	}

	if (_Handle < 0) {
		//   logprint ("File open error...");
		return _Handle;
	}

	bzero(&newtio, sizeof(newtio));

	if (strcmp(BaudStr, "2400") == 0) {
		newtio.c_cflag = B2400 | CS8 | CLOCAL | CREAD;
	}

	if (strcmp(BaudStr, "4800") == 0) {
		newtio.c_cflag = B4800 | CS8 | CLOCAL | CREAD;
	}

	if (strcmp(BaudStr, "9600") == 0) {
		newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	}

	if (strcmp(BaudStr, "19200") == 0) {
		newtio.c_cflag = B19200 | CS8 | CLOCAL | CREAD;
	}

	if (strcmp(BaudStr, "38400") == 0) {
		newtio.c_cflag = B38400 | CS8 | CLOCAL | CREAD;
	}

	if (strcmp(BaudStr, "57600") == 0) {
		newtio.c_cflag = B57600 | CS8 | CLOCAL | CREAD;
	}

	if (strcmp(BaudStr, "115200") == 0) {
		newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD /*| CSTOPB*/;
	}

	if (strcmp(BaudStr, "230400") == 0) {
		newtio.c_cflag = B230400 | CS8 | CLOCAL | CREAD /*| CSTOPB*/;
	}

	if (strcmp(BaudStr, "460800") == 0) {
		newtio.c_cflag = B460800 | CS8 | CLOCAL | CREAD /*| CSTOPB*/;
	}

	if (strcmp(BaudStr, "500000") == 0) {
		newtio.c_cflag = B500000 | CS8 | CLOCAL | CREAD /*| CSTOPB*/;
	}

	if (strcmp(BaudStr, "576000") == 0) {
		newtio.c_cflag = B576000 | CS8 | CLOCAL | CREAD /*| CSTOPB*/;
	}
	if (strcmp(BaudStr, "921600") == 0) {
		newtio.c_cflag = B921600 | CS8 | CLOCAL | CREAD /*| CSTOPB*/;
	}

	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	newtio.c_cc[VTIME] = timeout;
	newtio.c_cc[VMIN] = 0;

	tcflush(_Handle, TCIOFLUSH);
	tcsetattr(_Handle, TCSANOW, &newtio);

	//serial low latency - only effective when kernel supports it
	ioctl(_Handle, TIOCGSERIAL, &serial);	//get current configuration
	serial.flags |= ASYNC_LOW_LATENCY;	//set low latency flag
	ioctl(_Handle, TIOCSSERIAL, &serial);	//set new configuration
	//*****

	return ((int) _Handle);
}


int idt_SerialChangeTimeout(int _Handle, int timeout) {

	struct termios newtio;
	if(_Handle <= 0) {
		printf("%s() serial port not opened handle -> %d!\n", __FUNCTION__, _Handle);
		return -1;
	}

	tcgetattr(_Handle, &newtio);

	newtio.c_cc[VTIME] = timeout;
	newtio.c_cc[VMIN] = 0;

	tcsetattr(_Handle, TCSANOW, &newtio);

	printf("%s() changing timeout to -> [%d]\n", __FUNCTION__, timeout);

	return 0;
}

int idt_SerialChangeSpeed(int _Handle, char *BaudStr) {
	struct termios newtio;

	unsigned int baud = B2400;

	if(_Handle <= 0) {
		printf("%s() serial port not opened handle -> %d!\n", __FUNCTION__, _Handle);
		return -1;
	}

	tcgetattr(_Handle, &newtio);

	if (strcmp(BaudStr, "2400") == 0) {
		baud = B2400;
	} else if (strcmp(BaudStr, "4800") == 0) {
		baud = B4800;
	} else if (strcmp(BaudStr, "9600") == 0) {
		baud = B9600;
	} else if (strcmp(BaudStr, "19200") == 0) {
		baud = B19200;
	} else if (strcmp(BaudStr, "38400") == 0) {
		baud = B38400;
	} else if (strcmp(BaudStr, "57600") == 0) {
		baud = B57600;
	} else if (strcmp(BaudStr, "115200") == 0) {
		baud = B115200;
	} else if (strcmp(BaudStr, "230400") == 0) {
		baud = B230400;
	} else if (strcmp(BaudStr, "460800") == 0) {
		baud = B460800;
	} else if (strcmp(BaudStr, "500000") == 0) {
		baud = B500000;
	} else if (strcmp(BaudStr, "576000") == 0) {
		baud = B576000;
	} else if (strcmp(BaudStr, "921600") == 0) {
		baud = B921600;
	} else {
		printf("%s() undefined baud rate %s\n", __FUNCTION__, BaudStr);
		return -1;
	}

	printf("%s() changing speed to -> [%s]\n", __FUNCTION__, BaudStr);

	cfsetspeed(&newtio, baud);
	tcsetattr(_Handle, TCSANOW, &newtio);

	return 0;
}

int idt_SerialOpenwithOptions(char *ComStr, char *BaudStr, int timeout, char *options) {
	int _Handle, n;
	unsigned int i;
	struct termios newtio;
	struct serial_struct serial;

	_Handle = open(ComStr, O_RDWR | O_NOCTTY | O_NDELAY);
	if (_Handle >= 0) {
		n = fcntl(_Handle, F_GETFL, 0);
		fcntl(_Handle, F_SETFL, n & ~O_NDELAY);
	}

	if (_Handle < 0) {
		//   logprint ("File open error...");
		return _Handle;
	}

	bzero(&newtio, sizeof(newtio));

	newtio.c_cflag = CREAD; // receiver enabled
	newtio.c_iflag = 0;
	newtio.c_oflag = 0;

	// setting baud rate
	if (strcmp(BaudStr, "1200") == 0) {
		newtio.c_cflag |= B1200;
	} else if (strcmp(BaudStr, "2400") == 0) {
		newtio.c_cflag |= B2400;
	} else if (strcmp(BaudStr, "4800") == 0) {
		newtio.c_cflag |= B4800;
	} else if (strcmp(BaudStr, "9600") == 0) {
		newtio.c_cflag |= B9600;
	} else if (strcmp(BaudStr, "19200") == 0) {
		newtio.c_cflag |= B19200;
	} else if (strcmp(BaudStr, "38400") == 0) {
		newtio.c_cflag |= B38400;
	} else if (strcmp(BaudStr, "57600") == 0) {
		newtio.c_cflag |= B57600;
	} else if (strcmp(BaudStr, "115200") == 0) {
		newtio.c_cflag |= B115200;
	} else if (strcmp(BaudStr, "230400") == 0) {
		newtio.c_cflag |= B230400;
	} else if (strcmp(BaudStr, "460800") == 0) {
		newtio.c_cflag |= B460800;
	} else if (strcmp(BaudStr, "500000") == 0) {
		newtio.c_cflag |= B500000;
	} else if (strcmp(BaudStr, "576000") == 0) {
		newtio.c_cflag |= B576000;
	} else if (strcmp(BaudStr, "921600") == 0) {
		newtio.c_cflag |= B921600;
	}else {
		printf("Open Failed: %s not supported!\n", BaudStr);
		return -1;
	}

	printf("%s() Port->%s %s\n", __FUNCTION__, ComStr, BaudStr);

	if(options != NULL) {
		printf("%s() options->%s\n", __FUNCTION__, options);

		for(i = 0; i < strlen(options); i++) {
			switch(i) {
			case 0: // 5,6,7,8 bits
				switch(options[i]) {
				case '5':
					printf("%s() 5 bits selected\n", __FUNCTION__);
					newtio.c_cflag |= CS5;
					break;
				case '6':
					printf("%s() 6 bits selected\n", __FUNCTION__);
					newtio.c_cflag |= CS6;
					break;
				case '7':
					printf("%s() 7 bits selected\n", __FUNCTION__);
					newtio.c_cflag |= CS7;
					break;
				default:
				case '8':
					printf("%s() 8 bits selected\n", __FUNCTION__);
					newtio.c_cflag |= CS8;
					break;
				}
				break;
				case 1: // parity, even, odd, none
					switch(options[i]) {
					case 'e':
					case 'E':
						newtio.c_cflag |= PARENB;
						printf("%s() Even Parity\n", __FUNCTION__);
						break;
					case 'o':
					case 'O':
						newtio.c_cflag |= PARENB | PARODD;
						printf("%s() Odd Parity\n", __FUNCTION__);
						break;
					default:
					case 'N':
						printf("%s() No Parity\n", __FUNCTION__);
						break;
					}
					break;
					case 2: // stop bits, 1, 2
						switch(options[i]) {
						case '2':
							newtio.c_cflag |= CSTOPB;
							printf("%s() 2 stop bits\n", __FUNCTION__);
							break;
						default:
						case '1':
							printf("%s() 1 stop bit\n", __FUNCTION__);
							break;
						}
						break;
						case 3: // hardware/software flow control, ON /OFF
							switch(options[i]) {
							case 'h':
							case 'H':
								newtio.c_cflag |= CRTSCTS;
								printf("%s() Hardware Flow Control\n", __FUNCTION__);
								break;
							default:
							case 's':
							case 'S':
								newtio.c_cflag |= CLOCAL;
								newtio.c_iflag |= (IXON | IXOFF);
								newtio.c_oflag = 0;
								printf("%s() No Flow Control\n", __FUNCTION__);
								break;
							}

							break;
			}

		}
	} else {
		newtio.c_cflag |= CLOCAL | CS8;
		printf("%s() No options setting default 8n1 \n", __FUNCTION__);
	}


	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	newtio.c_cc[VTIME] = timeout;
	newtio.c_cc[VMIN] = 0;

	tcflush(_Handle, TCIOFLUSH);
	tcsetattr(_Handle, TCSANOW, &newtio);

	//serial low latency - only effective when kernel supports it
	ioctl(_Handle, TIOCGSERIAL, &serial);	//get current configuration
	serial.flags |= ASYNC_LOW_LATENCY;	//set low latency flag
	ioctl(_Handle, TIOCSSERIAL, &serial);	//set new configuration
	//*****

	return ((int) _Handle);
}


/**
 * Close Serial Port
 * @param _Handle [in] Serial handle
 * @ingroup serial
 */
void idt_SerialClose(int _Handle)
{
	int i, ret;

	for (i = 0; i < 3; i++) {
		ret = close(_Handle);
		if (ret < 0) {
			//                      logprint("idt_SerialSendChar error !");
			//                      system_log_errno(errno);
			if (errno == EINTR)
				continue;

			return;
		}
		break;
	}

	return;
}

/**
 * idt_Serial Send Character
 * @param _Handle [in] Serial handle
 * @param c [in] Character to send
 * @retval ERR if an error occurs
 * @retval OK if successful
 * @ingroup serial
 */
int idt_SerialSendChar(int _Handle, unsigned char c)
{
	int i, ret;
	//static char prev_c = 0;

	for (i = 0; i < 3; i++) {
		ret = write(_Handle, &c, 1);
		if (ret < 0) {

			if (errno == EINTR)
				continue;
			return (ERR);
		}
		break;
	}
	return (OK);

}

int idt_SerialSendBuf(int _Handle, unsigned char *c, int len)
{
	int i, ret;
	//static char prev_c = 0;

	for (i = 0; i < 3; i++) {
		ret = write(_Handle, c, len);
		if (ret < 0) {

			if (errno == EINTR)
				continue;
			return (ERR);
		}
		break;
	}
	/*
  if ((prev_c == c) && (c == 0x02)) {
//          printf("Found two:%02x\n", c);
    tcdrain(_Handle);
  }
	 */
	//prev_c = c;

	tcdrain(_Handle);
	return (OK);
}

/*
 * Required for izmir sam
 */
int idt_SerialSendChar_tcdrain(int _Handle, unsigned char c)
{
	int i, ret;
	static char prev_c = 0;

	for (i = 0; i < 3; i++) {
		ret = write(_Handle, &c, 1);
		if (ret < 0) {

			if (errno == EINTR)
				continue;
			return (ERR);
		}
		break;
	}

	if ((prev_c == c) && (c == 0x02)) {
		//          printf("Found two:%02x\n", c);
		tcdrain(_Handle);
	}
	prev_c = c;
	return (OK);
}



int idt_SerialGetReceiveBufferByteCount(int _Handle, unsigned int *bytes) {
	return ioctl(_Handle, FIONREAD, &bytes);
}

int idt_SerialCheckAvailableData(int _Handle, unsigned int timeout) {
	int ready = 0;
	struct timeval selTimeout;
	fd_set fdSet;
	int secs;
	int msecs;

	/* Wait firstCharTimeout to see if we'll get data at all */
	//	if(!firstCharTimeout)
	//		firstCharTimeout = 2000;//FIRST_BYTE_TIMEOUT;  // Default timeout value

	/* We may have more than 1 second timeout, handle it */
	secs  = timeout / 1000;
	msecs = timeout % 1000;

	//	printf("secs: %d, msecs: %d\n", secs, msecs);

	selTimeout.tv_sec = secs;
	selTimeout.tv_usec = msecs * 1000;  // structure takes microseconds and we give millisecond

	FD_ZERO(&fdSet);
	FD_SET(_Handle, &fdSet); /* We only have one source to test */

	/* We use select() here to act as a timeout before the first byte is read (if available). If select returns without a
	 * timeout, then it's safe to read from our descriptor until it doesn't return anymore data.
	 */
	ready = select(_Handle + 1, &fdSet, NULL, NULL, &selTimeout);
	if(!ready)
		return ERR; // Timeout error

	return OK;
}

int idt_SerialReceiveCharWithTimeout(int _Handle, unsigned char *c, unsigned int firstCharTimeout){
	int ready = 0;
	int dataLen = 0;
	struct timeval selTimeout;
	fd_set fdSet;
	int secs;
	int msecs;

	/* Wait firstCharTimeout to see if we'll get data at all */
	//	if(!firstCharTimeout)
	//		firstCharTimeout = 2000;//FIRST_BYTE_TIMEOUT;  // Default timeout value

	/* We may have more than 1 second timeout, handle it */
	secs  = firstCharTimeout / 1000;
	msecs = firstCharTimeout % 1000;

	//	printf("secs: %d, msecs: %d\n", secs, msecs);

	selTimeout.tv_sec = secs;
	selTimeout.tv_usec = msecs * 1000;  // structure takes microseconds and we give millisecond

	FD_ZERO(&fdSet);
	FD_SET(_Handle, &fdSet); /* We only have one source to test */

	/* We use select() here to act as a timeout before the first byte is read (if available). If select returns without a
	 * timeout, then it's safe to read from our descriptor until it doesn't return anymore data.
	 */
	ready = select(_Handle + 1, &fdSet, NULL, NULL, &selTimeout);
	if(!ready)
		return ERR; // Timeout error


	dataLen = read(_Handle, c, 1);

	return OK;
}

/**
 * idt_Serial Receive Character
 * @param _Handle [in] Serial handle
 * @param c [in] Character pointer to receive
 * @param timeout [in] Timeout value, dummy not working
 * @retval ERR if an error occurs
 * @retval OK if successful
 * @ingroup serial
 */
int idt_SerialReceiveChar(int _Handle, unsigned char *c)
{
	int ret, i;

	for (i = 0; i < 3; i++) {
		ret = read(_Handle, c, 1);
		if (ret == -1) {
			if (errno == EINTR)
				continue;
			printf("%s() here %d...\n", __FUNCTION__, __LINE__);
			return (ERR);
		}

		if (ret == 0) {
			printf("%s() here %d...\n", __FUNCTION__, __LINE__);
			return (ERR);
		}

		break;
	}
	return (OK);
}

/**
 * Purge Serial Port
 * @param _Handle [in] Serial handle
 * @ingroup serial
 */
void idt_SerialPurge(int _Handle)
{
	tcflush(_Handle, TCIOFLUSH);
}

