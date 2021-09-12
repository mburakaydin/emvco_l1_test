#include <stdio.h>
#include "serlib.h"
#include "serial_protocol.h"

#if 0

static int serial_fd = -1;

int EMVCO_setFileDescriptor(int fd) {
    serial_fd = fd;
}

int EMVCO_loopback_application() {
    if(serial_fd < 0) {
        printf("Error serial port not initialized!\n");
        return -1;
    }
    return sp_transceive(serial_fd, EMVCO_LOOPBACK_APP, NULL, 0); 
}

int EMVCO_transsend_typeA() {
    if(serial_fd < 0) {
        printf("Error serial port not initialized!\n");
        return -1;
    }
    return sp_transceive(serial_fd, EMVCO_LOOPBACK_APP, NULL, 0); 
}
#endif