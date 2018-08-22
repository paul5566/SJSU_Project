/*
 * 	Copyright (C) 2018 Paul5566
 *	This software may be distributed under the terms of the BSD license.
 */

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
#define ap_n_txt "/etc/wificonfig/n_ssid_pwd.txt" 
#define ap_g_txt  "/etc/wificonfig/g_ssid_pwd.txt"
#define parser_log "/var/log/kern.log"

/*
	trig on the wifi and parser the dmesg in the parser_log(/var/log/kern.log)

	check the parser log "link is not ready" and the wlx(mac address)

*/
static int parser_wifimac(void)  //check_again ,replace the parser_pwd
{
	/*turn on the wifi	*/
	WIFI(ON);	
	/*parser the code*/
	char buffer_wifimac[128] = {0};// be careful
	FILE *fp_wifimac;
	fp_wifimac = fopen(parser_log,"r+");
	if(fp_wifimac == NULL){
		perror("Error opening file");
		return(-1);
	}	
	fseek(fp_wifimac, -64, SEEK_END );
	
	fread(buffer_wifimac,64,1,fp_wifimac);
	//puts(buffer_pwd);
	rewind(fp_wifimac);

	printf("%s",buffer_wifimac);

	char *pwd_check;

	pwd_check = strstr(buffer_wifimac,"link becomes not ready");//sta recv deauth reason code"
	if (pwd_check != NULL){
		fclose(fp_wifimac);
	return 1;
	}
	else	{
		fclose(fp_wifimac);
		return 0;
	}
	fclose(fp_wifimac);// important, don't gorget
}

int main(void)
{
	int wifi_parser_check;
	wifi_parser_check =  parser_wifimac();
	printf("the result is %d",wifi_parser_check);
	return 0;
}


