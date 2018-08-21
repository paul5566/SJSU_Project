
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ON	"1"
#define OFF	"0"
#define DELAY 0.5

#define WIFI_IF "wlx4c0fc716fbaf"
#define WIFI(st) system("echo "st" > /sys/class/edm/gpio/USB_OTG2/value")

#define station_txt "/etc/wificonfig/station_ssid_pwd.txt"
#define ap_n_txt "./n_ssid_pwd.txt" 
#define ap_g_txt  "./g_ssid_pwd.txt"





static void setup_ssid_pwd(int variable)
{
	FILE *fp;
	char buffer_ssid [32] = {0};
	char buffer_pwd [16] = {0};
	/*
	   get the ssid account
	*/
	printf("enter the Account\n");
	fgets(buffer_ssid, sizeof(buffer_ssid), stdin);
	buffer_ssid[strlen(buffer_ssid) - 1] = '\n';//deletd the keyin "enter"
	size_t ssid_length = strlen(buffer_ssid);
	/*
	   get the pwd password
	*/
	printf("enter thr passwpord\n");
	fgets(buffer_pwd, sizeof(buffer_pwd), stdin);
	if(strlen(buffer_pwd) < 8){
		printf("*** the length of password is not long enough ***\n");
	}
	buffer_pwd[strlen(buffer_pwd) - 1] = '\n';// the stdin would create 
	size_t pwd_length = strlen(buffer_pwd);

	/*
		char *array[] = {station_txt,ap_n_txt, ap_g_txt};
		fp = fopen(station_txt,"w+");
	*/
	switch (variable)   {
		case 1:
			/*write to the station.txt*/
			fp = fopen(, "w+");
			fwrite(buffer_ssid , 1, ssid_length, fp);
			fwrite(buffer_pwd, 1, pwd_length, fp);
			fclose(fp);//be careful 
			break;
		case 2:
			/*write to the n_mode.txt*/	
			fp = fopen(ap_n_txt, "w+");
			fwrite(buffer_ssid , 1, ssid_length, fp);
			fwrite(buffer_pwd, 1, pwd_length, fp);
			fclose(fp);
			break;
		case 3:
			/*write to the g_mode.txt*/
			fp = fopen(ap_g_txt, "w+");
			fwrite(buffer_ssid , 1, ssid_length, fp);
			fwrite(buffer_pwd, 1, pwd_length, fp);
			fclose(fp);
			break;
		default:
			printf("The option is not available\n");
			break;
	}
}
/*

Target: store the account and password to the buffer

(a) *fp to open the conf file(.txt) to read the account and password

(b) memcpy to the buffer and check the length

(c) *ssid ,*pwd to store the buffer in to the account and password

*/


/* get ssid_pwd  */
static int fillin_ssid_pwd(char *ssid,char *pwd,int a)
{
	FILE *fp;
	char buf[128] = {0};
	size_t rlen;
	//read the station's ssid & pwd 
	if ( a == 1){
		fp = fopen(station_txt, "r");
		if(!fp){
			printf("fopen ssid_pwd.txt failed\n");
			return -1;
		}
	}
	// read the ap n mode's ssid & pwd
	else if ( a == 2){
		fp = fopen(ap_n_txt, "r");
		if(!fp){
			printf("fopen ssid_pwd.txt failed\n");
			return -1;
		}
	}
	// read the ap g's ssid & pwd
	else {
		fp = fopen(ap_g_txt, "r");
		if(!fp){
			printf("fopen ssid_pwd.txt failed\n");
			return -1;
		}
	}
	//TODO: error check
	rlen = fread(buf, 1, sizeof(buf), fp);
	buf[rlen] = '\0';

	int pos = 0;

	for(pos = 0; pos < rlen; pos++){
		if (buf[pos] == '\n')
			break;
	}

	memcpy(ssid, buf, pos+1);
	ssid[pos+2] = '\0';
	char *string_ptr;
	int count = 0;
	for(string_ptr = (buf+pos-2); *string_ptr != '\0'; string_ptr++){
		count ++;
	}
	memcpy(pwd, &buf[pos+1],sizeof(pwd)/sizeof(pwd[0]) - count);
	return 0;
}

int main(void)
{
	char *array[] = {station_txt,ap_n_txt, ap_g_txt};
	fp = fopen(station_txt,"w+");

	char buffer[128];
	printf("enter the input\n");
	scanf("%s",buffer);
	printf("the input is %s\n",buffer);
	return 0;
}
