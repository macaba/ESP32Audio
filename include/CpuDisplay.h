#ifndef cpudisplay_h_
#define cpudisplay_h_

#include "Audiostream.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef F_CPU
#define F_CPU (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000000U)
#endif

unsigned long IRAM_ATTR millis()
{	
    return (unsigned long) (esp_timer_get_time() / 1000);
}	

void cpuDisplay()
{
    const float maxTicksPerUpdate = (((float)F_CPU) / 44100.0f) * 128.0f;
    AudioStream *p;
    printf("%lu ms uptime\r\n\r\n", millis());
    printf("CPU 0 %5.2f%% [%5.2f%% max]  \r\n", cpu0Load, cpu0LoadMax);
    printf("CPU 1 %5.2f%% [%5.2f%% max]  \r\n\r\n", cpu1Load, cpu1LoadMax);
    int clocks, clocksMax, clocksSec;
    float load, loadMax, loadSec;
    int totalClocks = 0, totalClocksMax = 0;
    printf("                                |     Per Update     |   Per Second   |\r\n"); 
    printf("%-31s %6s %7s %6s %6s %9s\r\n", "Audio Object", "Min %", "Clocks", "Max %", "%", "Clocks"); 
    printf("----------------------------------------------------------------------\r\n");
    for (p = AudioStream::first_update; p; p = p->next_update) {
        if (p->active) {
            if(!p->blocking){
                clocks = p->clocksPerUpdateMin;
                clocksMax = p->clocksPerUpdateMax;
                clocksSec = p->clocksPerSecond;
                load = 100.0f * ((float)clocks / maxTicksPerUpdate);
                loadMax = 100.0f * ((float)clocksMax / maxTicksPerUpdate);
                loadSec = 100.0f * ((float)clocksSec/((float)F_CPU));
                printf("%-31s %6.2f %7i %6.2f %6.2f %9i\r\n", p->name, load, clocks, loadMax, loadSec, clocksSec);
                totalClocks += clocks;
                totalClocksMax += clocksMax;
            }	
            else{
                printf("%-31s %6s %7s %6s %6s %9s\r\n", p->name, "-", "-", "-", "-", "-");
            }		
        }
    }
    printf("                                ---------------------\r\n");
    printf("%31s %6.2f %7i %6.2f\r\n", "", 100.0f * ((float)totalClocks / maxTicksPerUpdate), totalClocks, 100.0f * ((float)totalClocksMax / maxTicksPerUpdate));
}

#ifdef __cplusplus
}
#endif

#endif