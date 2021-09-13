#include <stdio.h>
#include "serlib.h"

int sp_initialize(char *serial_port, char *speed) {

    int serial_fd;

    serial_fd = idt_SerialOpen ("/dev/ttymxc2", "115200", 1);

    return serial_fd;
}   

int sp_transceive(int serial_fd, unsigned short command, unsigned char *data, unsigned int data_len) {
    unsigned char send_buffer[8] = {0};
    int i = 0;
    send_buffer[0] = 0x02; //STX
    send_buffer[1] = (command >> 8) & 0xff;
    send_buffer[2] = (command >> 0) & 0xff;
    send_buffer[3] = (data_len >> 8) & 0xff;
    send_buffer[4] = (data_len >> 0) & 0xff;
    send_buffer[5] = 0x03; //ETX 
    idt_SerialPurge(serial_fd);
    for(i = 0; i < 8; i++) {
        idt_SerialSendChar(serial_fd, send_buffer[i]);
    }
    return 0;
}


