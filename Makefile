UI=gtk3
ifeq ($(UI), gtk3) 
	CFLAGS=-g `pkg-config --cflags gtk+-3.0`
	LIBS=`pkg-config --libs gtk+-3.0`
else
	CFLAGS=-g `pkg-config --cflags gtk+-2.0`
	LIBS=`pkg-config --libs gtk+-2.0`
endif


TARGET=emvco_l1_test
OBJ= serlib.o emvco_l1_test.o serial_protocol.o
HEADER	= test.h

all: $(TARGET)

clean:
	rm -f *.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<


$(TARGET): $(OBJ)
	$(CC) -O2 $(OBJ) -o $(TARGET) $(LIBS) 

	