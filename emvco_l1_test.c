#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <gtk/gtk.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "emvco_functions.h"
#include "serial_protocol.h"

#define VERSION "1.0.0"
#define OK 0
#define ERR 1

int reader_port = -1, pc_port = -1;

#include "serlib.h"
static char *baud_rate = "115200";



gboolean oti_message(GIOChannel *source, GIOCondition condition, gpointer data) {
	unsigned char c;

	if (condition & (G_IO_HUP)) {
		printf("GIOChannel condition error, exiting!!\n");
		return FALSE;
	}

	if ((reader_port <= 0) || (pc_port <= 0)){
		printf("ports are closed!!");
		return FALSE;
	}
	if (source == NULL){
		printf("source is null!!");
		return FALSE;
	}

//	if(state == STANDALONE) {
//		printf("%s() mode is standalone exiting...\n", __FUNCTION__);
//		return FALSE;
//	}


	switch(condition) {
	case G_IO_IN:
		if(idt_SerialReceiveChar(reader_port, &c) == OK){
			//debugl(2,"**oti get char %02x",c);
			idt_SerialSendChar(pc_port, c);
		}
		break;
	case G_IO_HUP:
		printf("hup\n");
		break;
	case G_IO_ERR:
		printf("error\n");
		break;
	default:
		printf("other\n");
		break;
	}
	return TRUE;
}

gboolean pc_message(GIOChannel *source, GIOCondition condition, gpointer data) {

	unsigned char c;

	if (condition & (G_IO_HUP)) {
		printf("GIOChannel condition error, exiting!!\n");
		return FALSE;
	}

	if ((reader_port <= 0) || (pc_port <= 0)){
		printf("ports are closed!!\n");
		return FALSE;
	}
	if (source == NULL){
		printf("source is null!!\n");
		return FALSE;
	}

	switch(condition) {
	case G_IO_IN:
//		if(state == STANDALONE_MODE) {
//			debug("Setting state to PC");
//			//SerialPurge(oti_port);
//			g_io_add_watch(OtiSocket, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL | G_IO_PRI, oti_message, NULL);
//			state = PC_MODE;
//		}
//
//		last_pc_command_time = time(NULL);

#if 0		//FULL TRANSPARENT MODE
		if (operation_mode_timer_handler_id != 0){
			debug("TRANSPARENT MODE active..");
			gtk_label_set_label(labelOperationMode, "~TRANSPARENT MODE~");
			g_source_remove(operation_mode_timer_handler_id);
			operation_mode_timer_handler_id = 0;
		}
#endif

		if(idt_SerialReceiveChar(pc_port, &c) == OK){
			//debugl(2, "**PC get char %02x",c);

			idt_SerialSendChar(reader_port, c);
		}
		break;
	case G_IO_HUP:
		printf("hup\n");
		break;
	case G_IO_ERR:
		printf("error\n");
		break;
	default:
		printf("other\n");
		break;
	}
	return TRUE;
}


int init_io_channel_connection(){

	static GIOChannel* ServerSocket = NULL;
	static GIOChannel* OtiSocket = NULL;

	if (ServerSocket != NULL){
		g_io_channel_shutdown(ServerSocket, TRUE, NULL);
		g_io_channel_unref(ServerSocket);
		ServerSocket = NULL;
	}

	if (OtiSocket != NULL){
		g_io_channel_shutdown(OtiSocket, TRUE, NULL);
		g_io_channel_unref(OtiSocket);
		OtiSocket = NULL;
	}

	if (reader_port > 0)
		idt_SerialClose(reader_port);

	if (pc_port > 0)
		idt_SerialClose(pc_port);

	pc_port = idt_SerialOpen ("/dev/ttymxc1", baud_rate, 1);
	reader_port = idt_SerialOpen ("/dev/ttymxc2", baud_rate, 1);

	printf("pc port: %d\n\r", pc_port);
	printf("emv_reader port: %d\n\r", reader_port);

	if(pc_port < 0 || reader_port < 0) {
		printf("error initializing ports!\n");
		return -1;	
	}

	ServerSocket = g_io_channel_unix_new((int) pc_port);
	OtiSocket = g_io_channel_unix_new((int) reader_port);

	printf("\n\r**starting main loop**\n\r");

	g_io_add_watch(ServerSocket, G_IO_IN | G_IO_ERR | G_IO_HUP, pc_message, NULL);
	g_io_add_watch(OtiSocket, G_IO_IN | G_IO_ERR | G_IO_HUP, oti_message, NULL);
}



static int check_args(int argc, char **argv)
{
	int opt;
	if (argc > 1) {
		while ((opt = getopt(argc, argv, "hvb:")) != -1) {

			switch (opt) {

			case 'b':
				baud_rate = strdup(optarg);
				printf("**selected baud rate->%s**\n", baud_rate);
				return 0;
			case 'v':
				printf("Version is %s\n\r", "1.0.0.1");
				return 1;
			default:
			case 'h':
				printf("\n\nUSAGE: transparent_serial -d <baud_rate>\n\n");		
				return 1;
			}
		}
	}

	return 0;
}

GtkWindow *appWindow		= NULL;
GtkFixed *appView			= NULL;
GtkLabel *lblReadWrite 	= NULL;
GtkWidget *buttonBuzzer, *buttonSound, *buttonTransparent;
GtkLabel *lblSetIP, *lblInfo;
	
#define BUTTON_SIZE_W			100
#define BUTTON_SIZE_H			60

#define WINDOW_SIZE_800_W		800
#define WINDOW_SIZE_640_W		640
#define WINDOW_SIZE_480_H		480
#define WINDOW_SIZE_240_H		240

#define WINDOW_SIZE_480_W		480
#define WINDOW_SIZE_272_H		272

#define FRAME_SIZE_W			640
#define FRAME_SIZE_H			385



void buzzer_cb() {
	static int buzzer_state = 0;
	
	if(buzzer_state) {
		system("echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable");
		gtk_label_set_text(lblInfo, "Buzzer Stopped...");
	} else {
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable");
		gtk_label_set_text(lblInfo, "Buzzer Started...");
	}
	
	buzzer_state = !buzzer_state;
	
}

void sound_cb() {
	
	system("aplay /usr/valapp/sounds/expected_trip.wav");
	gtk_label_set_text(lblInfo, "Sound Played...");
	
}

void transparent_cb() {
	
	gtk_label_set_text(lblInfo, "Transparent Channel\n Started...");
	init_io_channel_connection();
	
	gtk_widget_set_sensitive(buttonTransparent, FALSE);
}

#if 0
int init_windows() {
	appWindow = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_window_set_position(appWindow, GTK_WIN_POS_CENTER);
	gtk_window_set_title(appWindow, "appWindow");

	appView = GTK_FIXED(gtk_fixed_new());
	gtk_widget_set_size_request((appView ),WINDOW_SIZE_800_W,WINDOW_SIZE_480_H);
	
	gtk_container_add((appWindow),(appView));

	buttonBuzzer = gtk_button_new_with_label("Buzzer");
	
	g_signal_connect(buttonBuzzer, "clicked", (buzzer_cb), NULL);
	gtk_widget_set_size_request(buttonBuzzer, BUTTON_SIZE_W, BUTTON_SIZE_H);
	gtk_fixed_put(appView,(buttonBuzzer), 40, 40);

	buttonSound = gtk_button_new_with_label("Sound");
	g_signal_connect(buttonSound, "clicked", (sound_cb), NULL);
	gtk_widget_set_size_request(buttonSound, BUTTON_SIZE_W, BUTTON_SIZE_H);
	gtk_fixed_put(appView,(buttonSound), 40, 120);
	
	buttonTransparent = gtk_button_new_with_label("Transparent\nCommunication");
	g_signal_connect((buttonTransparent), "clicked", (transparent_cb), NULL);
	gtk_widget_set_size_request(buttonTransparent, BUTTON_SIZE_W, BUTTON_SIZE_H);
	gtk_fixed_put(appView,(buttonTransparent), 40, 200);
	
	
	lblInfo = (GtkLabel *) gtk_label_new("Hello World");
	gtk_widget_set_size_request(GTK_WIDGET(lblInfo),  300, 120);
	gtk_misc_set_alignment(GTK_MISC(lblInfo), 0.5, 0.5);
	gtk_label_set_justify(GTK_LABEL(lblInfo),GTK_JUSTIFY_CENTER);

	gtk_fixed_put(appView , GTK_WIDGET(lblInfo), 80 , 80);
	

	gtk_widget_show_all(GTK_WIDGET(appWindow));
}
#endif
GtkBuilder *builder;
GtkWidget *mainWindow;
GtkWidget *btnRfOn, *btnRfOff, *btnPoll, *btnRfReset,
	*btnTransacA, *btnTransacB, *btnLoopBack, *btnInterop,
	*btnWUPA, *btnWUPB, *btnRATS, *btnAttrib;

GtkWidget *lblApplication;


void reset_reader_and_wait_ms(int wait_ms) {
	system("echo -en 1 > /sys/class/gpio/gpio328/value");
	usleep(10000);
	system("echo -en 0 > /sys/class/gpio/gpio328/value");
	usleep(wait_ms * 1000);
}

void emvco_buttons_cb(GtkWidget* widget, gpointer data) {
	unsigned short command = (unsigned short)data;

	/*here reset the reader before sending the command*/
	reset_reader_and_wait_ms(1000);
	printf("sending command:%02x\n", command);
	sp_transceive(reader_port, command, NULL, 0); 
}

int init_glade_windows() {
	printf("initializing glade!\n");
	builder = gtk_builder_new();

	if(gtk_builder_add_from_file(builder, "/home/root/emvl1.glade", NULL) == NULL) {
		printf("builder error!\n");
		return -1;
	}

	
	mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));

	btnRfOn = GTK_WIDGET(gtk_builder_get_object(builder, "btnRfOn"));
	btnRfOff = GTK_WIDGET(gtk_builder_get_object(builder, "btnRfOff"));
	btnPoll = GTK_WIDGET(gtk_builder_get_object(builder, "btnPoll"));
	btnRfReset = GTK_WIDGET(gtk_builder_get_object(builder, "btnRfReset"));

	btnTransacA = GTK_WIDGET(gtk_builder_get_object(builder, "btnTransacA"));
	btnTransacB = GTK_WIDGET(gtk_builder_get_object(builder, "btnTransacB"));
	btnLoopBack = GTK_WIDGET(gtk_builder_get_object(builder, "btnLoopBack"));
	btnInterop = GTK_WIDGET(gtk_builder_get_object(builder, "btnInterop"));

	btnWUPA = GTK_WIDGET(gtk_builder_get_object(builder, "btnWUPA"));
	btnWUPB = GTK_WIDGET(gtk_builder_get_object(builder, "btnWUPB"));
	btnRATS = GTK_WIDGET(gtk_builder_get_object(builder, "btnRATS"));
	btnAttrib = GTK_WIDGET(gtk_builder_get_object(builder, "btnAttrib"));

	lblApplication = GTK_WIDGET(gtk_builder_get_object(builder, "lblApplication"));


	g_signal_connect(G_OBJECT(btnRfOn), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_FIELD_ON);
	g_signal_connect(G_OBJECT(btnRfOff), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_FIELD_OFF);
	g_signal_connect(G_OBJECT(btnPoll), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_POLLING);
	g_signal_connect(G_OBJECT(btnRfReset), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_RESET);

	g_signal_connect(G_OBJECT(btnTransacA), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_TRANSSEND_TYPEA);
	g_signal_connect(G_OBJECT(btnTransacB), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_TRANSSEND_TYPEB);
	g_signal_connect(G_OBJECT(btnLoopBack), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_LOOPBACK_APP);
	g_signal_connect(G_OBJECT(btnInterop), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_INTEROPERABILITY);

	g_signal_connect(G_OBJECT(btnWUPA), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_WUPA);
	g_signal_connect(G_OBJECT(btnWUPB), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_WUPB);	
	g_signal_connect(G_OBJECT(btnRATS), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_RATS);	
	g_signal_connect(G_OBJECT(btnAttrib), "clicked", G_CALLBACK(emvco_buttons_cb), (gpointer)EMVCO_ATTRIB);	

	gtk_widget_show_all(GTK_WIDGET(mainWindow));

	return 0;
} 



int main(int argc, char *argv[]){

	int ret;

	setpriority(PRIO_PROCESS, 0, -20);

	gtk_init(NULL, NULL);




	/*Required for buzzer*/
	//system("echo 0 > /sys/class/pwm/pwmchip0/export");
	//system("echo 2000000 > /sys/class/pwm/pwmchip0/pwm0/period");
	//system("echo 2000000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");

	/*Required for sound*/
	//system("amixer sset Master 100% on");

	/*Initialize Reset Pin*/
	system("echo -en 328 > /sys/class/gpio/export");
	system("echo -en out > /sys/class/gpio/gpio328/direction");

	if (check_args(argc, argv)) {
		return 1;
	}

	ret = init_glade_windows();
	if(ret != 0) {
		printf("Glade file problem, please make sure that the glade file is in the right location!\n");
		return -1;
	}
	init_io_channel_connection();

	char version_string[128] = "EMVCO L1 Test App v";

	strcat(version_string, VERSION);

	gtk_label_set_text(GTK_LABEL(lblApplication), version_string);

	gtk_main();

	return 0;
}
