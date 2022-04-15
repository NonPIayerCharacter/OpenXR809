/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "common/framework/platform_init.h"
// XR809 sysinfo is used to save configuration to flash
#include "common/framework/sysinfo.h"
#include "common/framework/net_ctrl.h"
#include "serial.h"
#include "shared/src/new_cfg.h"
#include "shared/src/new_pins.h"
#include "shared/src/mqtt/new_mqtt.h"
#include "shared/src/new_common.h"

#include "kernel/os/os.h"


OSStatus rtos_create_thread( beken_thread_t* thread, 
							uint8_t priority, const char* name, 
							beken_thread_function_t function,
							uint32_t stack_size, beken_thread_arg_t arg ) {
    OSStatus err = kNoErr;
	int new_priority;
	// TODO: translate
	new_priority = OS_THREAD_PRIO_CONSOLE;

	err = OS_ThreadCreate(thread,
		                name,
		                function,
		                arg,
		                new_priority,
		                stack_size);

	return err;
}

OSStatus rtos_delete_thread( beken_thread_t* thread ) {
    return OS_ThreadDelete( thread );
}

#define HELLOWORLD_THREAD_STACK_SIZE	(1 * 1024)
static OS_Thread_t g_helloworld_thread;

char g_xr809_ota_request[256] = { 0 };

void xr809_do_ota_next_frame(const char *s) {
	strcpy(g_xr809_ota_request,s);
}
//void os_strcpy(char *dst, const char *src) {
//	strcpy(dst,src);
//}
//void *os_malloc(int s) {
//	return malloc(s);
//}
//void os_free(void *f) {
//	free(f);
//}
//static void helloworld_task(void *arg)
//{
//	printf("helloworld_task start...\n");
//
//	while (1) {
//		OS_Sleep(1);
//		MQTT_RunEverySecondUpdate();
//		g_upTime++;
//		if(g_xr809_ota_request[0] != 0) {
//			printf("XR809 main loop now is starting OTA!\n");
//			OS_MSleep(10);
//			printf("URL %s\n",g_xr809_ota_request);
//			OS_MSleep(10);
//			cmd_ota_http_exec(xr809_do_ota_next_frame);
//			break;
//		}
//	}
//
//	printf("helloworld_task exit\n");
//	OS_ThreadDelete(&g_helloworld_thread);
//}

#define MAX_DUMP_BUFF_SIZE 256
char dump_buffer[MAX_DUMP_BUFF_SIZE];



static char g_ipStr[32];
const char *getMyIp() {
	strcpy(g_ipStr,inet_ntoa(g_wlan_netif->ip_addr));

	return g_ipStr;
}
const char *getMACStr(char *macstr) {
	unsigned char mac[32];
	WiFI_GetMacAddress((char *)mac);
	sprintf(macstr,"%02X%02X%02X%02X%02X%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	return macstr;
}
void bk_printf(char *format, ...){
    va_list vp;

    va_start(vp, format);
    vsnprintf(dump_buffer, MAX_DUMP_BUFF_SIZE, format, vp);
    va_end(vp);

	printf("%s\r\n",dump_buffer);
}

int main(void)
{
	int res;
	sysinfo_t *inf;

	platform_init();

	serial_init(SERIAL_UART_ID, 115200, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1, 1);

	serial_start();

	OS_MSleep(10);

	res = sysinfo_init();
	if(res != 0) {
		printf("sysinfo_init error - %i!\n\r",res);
	}
	res = sysinfo_load();
	if(res != 0) {
		printf("sysinfo_load error - %i!\n\r",res);
	}
	inf = sysinfo_get();
	if(inf == 0) {
		printf("sysinfo_get returned 0!\n\r");
	}
	if(inf->checksum != sysinfo_checksum(inf)) {
		printf("sysinfo checksum invalid, resetting!\n\r");
		inf->wlan_sta_param.ssid[0] = 0;
		inf->wlan_sta_param.psk[0] = 0;
		memset(&inf->pins,0,sizeof(inf->pins));
		strcpy(inf->mqtt_param.brokerName,"brokerName");
		strcpy(inf->mqtt_param.userName,"userName");
		strcpy(inf->mqtt_param.hostName,"192.168.0.123");
		strcpy(inf->mqtt_param.pass,"P@ssw0rd");
		inf->mqtt_param.port = 1883;
		sysinfo_save_wrapper();
	} else {
		printf("sysinfo checksum OK, loading!\n\r");
	}
	OS_MSleep(10);

	printf("PRJCONF_SYSINFO_ADDR %i\n\r",PRJCONF_SYSINFO_ADDR);
	printf("SYSINFO_SSID_LEN_MAX %i\n\r",SYSINFO_SSID_LEN_MAX);
	OS_MSleep(10);
	printf("inf->wlan_sta_param.ssid %s\n\r",inf->wlan_sta_param.ssid);
	printf("inf->wlan_sta_param.psk %s\n\r",inf->wlan_sta_param.psk);
			
	OS_MSleep(10);

void user_main(void);

	user_main();

	return 0;
}
