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
#include "serial.h"

#include "kernel/os/os.h"

#include "prv_config.h"

#define HELLOWORLD_THREAD_STACK_SIZE	(1 * 1024)
static OS_Thread_t g_helloworld_thread;

//void os_strcpy(char *dst, const char *src) {
//	strcpy(dst,src);
//}
//void *os_malloc(int s) {
//	return malloc(s);
//}
//void os_free(void *f) {
//	free(f);
//}
int g_upTime = 0;
int Time_getUpTimeSeconds()
{
	return g_upTime;
}
static void helloworld_task(void *arg)
{
	printf("helloworld_task start...\n");

	while (1) {
		OS_Sleep(1);
		g_upTime++;
	}

	printf("helloworld_task exit\n");
	OS_ThreadDelete(&g_helloworld_thread);
}
static void connectToWiFi()
{
	const char *ssid = DEFAULT_WIFI_SSID;    /* set your AP's ssid */
	const char *psk = DEFAULT_WIFI_PASS; /* set your AP's password */

	/* set ssid and password to wlan */
	wlan_sta_set((uint8_t *)ssid, strlen(ssid), (uint8_t *)psk);

	/* start scan and connect to ap automatically */
	wlan_sta_enable();

	//OS_Sleep(60);
	at_dump("ok set wifii\n\r");
}

#define MAX_DUMP_BUFF_SIZE 256

char dump_buffer[MAX_DUMP_BUFF_SIZE];

void addLog(char *format, ...){
    int len;
    va_list vp;

    va_start(vp, format);
    len = vsnprintf(dump_buffer, MAX_DUMP_BUFF_SIZE, format, vp);
    va_end(vp);

	printf("%s\r\n",dump_buffer);
}

void addLogAdv(int level, int feature, char *format, ...){
    int len;
    va_list vp;

    va_start(vp, format);
    len = vsnprintf(dump_buffer, MAX_DUMP_BUFF_SIZE, format, vp);
    va_end(vp);

	printf("%s\r\n",dump_buffer);
}

int main(void)
{
	platform_init();

	serial_init(SERIAL_UART_ID, 115200, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1, 1);

	serial_start();

	OS_Sleep(1);

	printf("Serial test from main!\n\r");

	OS_Sleep(1);

	connectToWiFi();

	OS_Sleep(1);

	start_tcp_http();

	OS_Sleep(1);


	/* start helloworld task */
	if (OS_ThreadCreate(&g_helloworld_thread,
		                "helloworld",
		                helloworld_task,
		                NULL,
		                OS_THREAD_PRIO_CONSOLE,
		                HELLOWORLD_THREAD_STACK_SIZE) != OS_OK) {
		printf("create serial task failed\n");
		return 1;
	}

	return 0;
}
