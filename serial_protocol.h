#ifndef _SERIAL_PROTOCOL_H_
#define _SERIAL_PROTOCOL_H_
int sp_initialize(char *serial_port, char *speed);
int sp_transceive(int serial_fd, unsigned short command, unsigned char *data, unsigned int data_len);

#endif
