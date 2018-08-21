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
 	Target: turn off the station mode and set to 0

	(a)the station mode would excute two process: 
		(1) wpa_supplicant
		(2) dhclient
 	
	(b)	Besides, turn off the hardware sent 0 to gpio USB_OTG2 to turn off the wifi mode
	(c)	Be careful the DELAY time
	(d) Be careful about the fclose have to be excuted 

	Optimized Target

	[X] Printf avoid the buffer

	[X]	Be careful about the fclose have to be settled before the break

	[X] Avoid some duplicated program e.g: in the main function kill station ,kill ap 

	[X] assign the memory must have to be the base of 8

	[X] computer concept (1)error return -1, (2)correct return 0

	[X] set up #define 

	[X] switch from [0],[1],[2]

	[X] rename check_again, rename

	[X] char array[] = { station.txt, ap_n.txt, ap_g.txt }; 

	[X] The printf " *** keyword ***"
*/

static void kill_station(void)
{
	/*kill the process wpa_supplicant*/  //system("pkill wpa_supplicant > /home/mayqueen/wifi_off_record.txt 2>&1");
	system("pkill wpa_supplicant > /dev/null 2>&1");
	/*kill the process dhclient*/ //system("pkill dhclient > /home/mayqueen/wifi_off_record.txt 2>&1");
	system("pkill dhclient > /dev/null 2>&1");
	sleep (DELAY);
	/* Turn off the hardware Wi-Fi*/
	WIFI(OFF);
	/* Delay 0.5 sec*/
	sleep(DELAY);
}

/*
	Target: store the stdin in the buffer and sent to the different conf file(mode.txt)

	(a)"fget" to store the stdin
	(b)store the stdin to the buffer, 
		(1) be careful about the "\n",and deleted the keyin "enter"
	(c)fwrite to the conf
*/

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
			fp = fopen(station_txt, "w+");
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

/*
Target: Generated the wifi.conf

	(a) store the ssid and pwd
	(b) use the snprintf to generate the cmd with ssid and password 
	(c) generate the cmd: "wpa_passpharse" + ssid &pwd to generate the wifi.conf 
*/

static void station_ssid_pwd(void)
{
	char cmd[256] = {0};
	char ssid[64] = {0};
	char pwd[64] = {0};

	fillin_ssid_pwd(ssid, pwd, 1);
	ssid[strlen(ssid) - 1] = '\0';//don't fotget to make the array last element clean
	pwd[strlen(pwd) - 1] = '\0';//don't forget to make the arary last element clean
	//printf("%s(%d): ssid: %s, pwd: %s\n", __func__, __LINE__, ssid, pwd);
	snprintf(cmd, sizeof(cmd), 
			"wpa_passphrase %s %s > /etc/wificonfig/wifi_home.conf",
			ssid, pwd);
	system(cmd);
}

/*
	avoid using a lot of printf function to solve the bug printf and flush

	website link: goo.gl/xf2WyM
*/
static void help(void)
{
	printf("Panzar Wi-Fi Feature v1.0\n"
	       "\tCopyright (c) 2018, Polin Chen <mayqueen@gmail.com> and contributors\n"
	       "This software may be distributed under the terms of the BSD license.\n"
	       "See README for more details.\n"
	       "\nusage:\n"
	       "\tpanzar_wifi [-s] [-sta]/[-ap_n]/[-ap_g]\
			[-c] [-sta]/[-ap_n]/[-ap_g]\
			[-k] \n"
		   "\ndrivers:\n"
		   "\trtl871xdrv,rtl8188eu\n"
		   "\noptions:\n"
	       "\tpanzar_wifi -s -- setup account ans password\n"
	       "\tpanzar_wifi -c -- wifi connect \n"
	       "\tpanzar_wifi -k -- kill all the process of wifi(AP/Station)\n"
	       "\tpanzar_wifi -h -- help\n"
	       "\nexample:\n"
	       "\t-s	-sta 	set up station mode of id and password\n"
	       "\t-s 	-ap_n 	set up 80211 n mode of id and password\n"
	       "\t-s 	-ap_g 	set up 80211 g mode of id and password\n"
	       "\t-c  	-sta 	connect to the station  mode\n"
	       "\t-c  	-ap_n 	excute the 80211 n of AP mode\n"
	       "\t-c  	-ap_g 	excue the 80211 g  of AP mode\n"
	       "\t-k 			kill all the  wifi prcoess\n");
}
/*
	(1)	"link becomes ready" first time
		(o)	account is correct
		(x)	account is not correct
	(2)	"link becomes ready" second time
		(o)	account and password is correct
		(x) account is correctly but the password is not correct
*/

static int check_again(void)  //check_again ,replace the parser_pwd
{
	char buffer_pwd[64] = {0};
	FILE *fp_pwd;
	fp_pwd = fopen(parser_log,"r+");
	if(fp_pwd == NULL){
		perror("Error opening file");
		return(-1);
	}	
	fseek(fp_pwd, -64, SEEK_END );
	
	fread(buffer_pwd,64,1,fp_pwd);
	//puts(buffer_pwd);
	rewind(fp_pwd);

	char *pwd_check;
	
	pwd_check = strstr(buffer_pwd,"link becomes ready");//sta recv deauth reason code"
	if (pwd_check != NULL){
		//printf("\n");
		//printf("password correct\n");
		fclose(fp_pwd);
	return 1;
	}
	else	{
		fclose(fp_pwd);
		return 0;
	}
	fclose(fp_pwd);// important, don't gorget
}



static int parser_account_check()
{
	char buffer[128] = {0};
	FILE *fp;
	fp = fopen(parser_log,"r+");
	if(fp == NULL){
		perror("Error opening file");
		return(-1);
	}	
	fseek(fp,0L,SEEK_END);	// seek to end of file
	fseek( fp, -128, SEEK_END );
	
	//fread(buffer,sz_of_txt/4,1,fp);
	fread(buffer,128,1,fp);
	//puts(ptr);
	rewind(fp);

	char *parsercheck;

	/*
	 *	check the password is correct or not
	 */

	parsercheck = strstr(buffer,"link becomes ready");
	/*
		parsercheck != NULL => log message got "link becomes ready"

		Than,we know the account is correct, but we have to check the password
	*/
	if (parsercheck != NULL){
		int pwd_count = 0;
		int pwd_check;
		while(pwd_count < 5)
		{
			sleep(0.5);
			pwd_check = check_again();
			if(1 == pwd_check){
				sleep(7);
				pwd_check = check_again();
				if(pwd_check == 1)
				{
					printf("*** connection successful ***\n");
					return 1;
					break;
				}
			}
			else	{
				pwd_count ++;
			}
			sleep(DELAY*2);
		}
		if (pwd_count >= 4)
		{
			printf("*** The wifi station connection failed, the password may be not correct ***\n");
			kill_station();
		}
		return 1;		
	}
	else{
		return 0;/* link becomes ready (X)      */
	}
	fclose(fp);// important, don't gorget
}


static void parser_check()
{
	int find;
	int count = 0;
	while(count < 4)
	{
		find = parser_account_check();
		if(1 == find){
			break;
		}
		else {
			count++;
		}
		sleep(5);
	}
	if (count >= 3){

		printf("*** The wifi station connection failed, please check the account setting ***\n");
		kill_station();
	}
}



/*
	Target:excute the station connection
*/
static void station(void)
{
	WIFI(ON);
	sleep(DELAY);

	system("rm /etc/wificonfig/wifi_home.conf");
	station_ssid_pwd();
	sleep(DELAY*4);//be careful about the timing issue
	system("ifconfig wlx4c0fc716fbaf up");
	sleep(DELAY);

	system("wpa_supplicant -i wlx4c0fc716fbaf -c /etc/wificonfig/wifi_home.conf -Dwext & > /etc/wificonfig/record.txt 2>&1");
	parser_check();
	//system("./parser_strstr");
	//printf("wpa_supplicant -i wlx4c0fc716baf -c done\n");

	sleep(DELAY);
	system("dhclient wlx4c0fc716fbaf > /etc/wificonfig/record.txt 2>&1");
	//printf("dhclinet wlx4c716fbaf done/n");
}

/*
 *  Target: excute the create_ap_g mode
 */
static void create_ap_g(void)
{
	/*
	 *
	 * get ssid & pwd and store it in to the buffer
	 *
	 */
	char cmd[256] = {0};
	char ssid[64] = {0};
	char pwd[64] = {0};

	fillin_ssid_pwd(ssid, pwd, 3);
	ssid[strlen(ssid) - 1] = '\0';//don't fotget to make the array last element clean
	pwd[strlen(pwd) - 1] = '\0';//don't forget to make the arary last element clean
	//printf("%s(%d): ssid: %s, pwd: %s\n", __func__, __LINE__, ssid, pwd);
	snprintf(cmd, sizeof(cmd),"create_ap --driver rtl871xdrv --daemon \
			wlx4c0fc716fbaf eth0 %s %s", ssid, pwd);

	/* reset wifi chip */
	WIFI(OFF);
	sleep(DELAY);
	WIFI(ON);
	sleep(DELAY);
	/* Send stop command to an already running create_ap ,use the kill to insead*/
	//system("sudo pkill create_ap");
	/* excute the createap to set the board as the AP mode with 80211n */
	system(cmd);
}
/*
 *  Target: excute the create_ap_n mode
 */
static void create_ap_n(void)
{

	/*
	 *
	 * get ssid & pwd and store it in to the buffer
	 *
	 */
	char cmd[128] = {0};
	char ssid[32] = {0};
	char pwd[32] = {0};

	fillin_ssid_pwd(ssid, pwd, 2);
	ssid[strlen(ssid) - 1] = '\0';//don't fotget to make the array last element clean
	pwd[strlen(pwd) - 1] = '\0';//don't forget to make the arary last element clean
	//printf("%s(%d): ssid: %s, pwd: %s\n", __func__, __LINE__, ssid, pwd);
	snprintf(cmd, sizeof(cmd),"create_ap --driver rtl871xdrv --ieee80211n --daemon \
			wlx4c0fc716fbaf eth0 %s %s", ssid, pwd);
	/* reset wifi chip*/
	WIFI(OFF);
	sleep(DELAY);
	WIFI(ON);
	sleep(DELAY);
	/*Send stop command to an already running create_ap.    */
	//system("sudo pkill create_ap");
	//sleep(DELAY);
	/*excute the createap to set the board as the AP mode with 80211n       */
	system(cmd);
}

static void kill_ap(void)
{
	/* Send stop command to an already running create_ap. */
	system("pkill create_ap");
	WIFI(OFF);
	//printf("end of the kill_ap excution\n");
}

int main(int argc, char **argv)
{
	if(argc < 2){
		printf("Usage: %s [-s/-c/-k/-h]\n", argv[0]);
		return -1;
	}
	if (strcmp(*(argv+1), "-s") == 0){
		//printf("excute the ssid password set up\n");
		if (strcmp(*(argv+2), "-sta") == 0){
			printf("station ssid & password set up\n");
			setup_ssid_pwd(1);
		}
		else if (strcmp(*(argv+2), "-ap_n") == 0){
			printf("AP n mode ssid & password set up\n");
			setup_ssid_pwd(2);
		}
		else if (strcmp(*(argv+2), "-ap_g") == 0){
			printf("AP g mode ssid & password set up\n");
			setup_ssid_pwd(3);
		}
		else {
			printf("The second option is not available\n");
		}			 
	}
	else if(strcmp(*(argv+1), "-c") == 0){
		//printf("excute the connection\n");
		if (strcmp(*(argv+2), "-sta") == 0){
			printf("Excute the station mode connection \n");
			kill_station();
			kill_ap();
			station();
		}
		else if (strcmp(*(argv+2), "-ap_n") == 0){
			printf("Excute the Wi-Fi AP 802.11n mode\n");
			kill_station();
			kill_ap();	
			create_ap_n();	
		}
		else if (strcmp(*(argv+2), "-ap_g") == 0){
			printf("Excute the Wi-Fi AP 802.11g mode\n");
			kill_station();
			kill_ap();
			create_ap_g();
		}
		else {
			printf("The second part of opt is not available\n");
		}			 

	}
	else if(strcmp(*(argv+1), "-k") == 0){
		kill_station();
		kill_ap();
		printf("kill the all connection\n");
	}
	else if(strcmp(*(argv+1), "-h") == 0){
		help();
	}
	return 0;
}
