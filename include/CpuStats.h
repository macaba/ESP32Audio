#ifndef cpustats_h_
#define cpustats_h_

#include <esp_freertos_hooks.h>

#ifdef __cplusplus
extern "C" {
#endif

volatile uint32_t idlectrl0 = 0;
float cpu0Load;
float cpu0LoadMax;
void cpu0( void * parameter )   //Gets a new CPU load approx. every 1 second
{
  const uint32_t cycleCount = 21818182;   //240MHz / 11
  uint32_t t00, t01;
  
  t00 = xthal_get_ccount();
  for(uint32_t i = 0; i < cycleCount; i++) {
    idlectrl0++;
  }
  t01 = xthal_get_ccount();
  
  uint32_t cycles_expected = 11 * cycleCount;
  uint32_t cycles_used = t01 - t00;
  
  cpu0Load = 100.0f * (cycles_used - cycles_expected) / cycles_used;
  if(cpu0Load > cpu0LoadMax)
    cpu0LoadMax = cpu0Load;
}

volatile uint32_t idlectrl1 = 0;
float cpu1Load;
float cpu1LoadMax;
void cpu1( void * parameter )   //Gets a new CPU load approx. every 1 second
{
  const uint32_t cycleCount = 21818182;   //240MHz / 11
  uint32_t t00, t01;
  
  t00 = xthal_get_ccount();
  for(uint32_t i = 0; i < cycleCount; i++) {
    idlectrl1++;
  }
  t01 = xthal_get_ccount();
  
  uint32_t cycles_expected = 11 * cycleCount;
  uint32_t cycles_used = t01 - t00;
  
  cpu1Load = 100.0f * (cycles_used - cycles_expected) / cycles_used;
  if(cpu1Load > cpu1LoadMax)
    cpu1LoadMax = cpu1Load;
}

void StartMonitoringCPU0()
{
  esp_register_freertos_idle_hook_for_cpu((esp_freertos_idle_cb_t)cpu0, 0);
}

void StartMonitoringCPU1()
{
  esp_register_freertos_idle_hook_for_cpu((esp_freertos_idle_cb_t)cpu1, 1);
}

//uint32_t inline IRAM_ATTR cycles()
//{
//    uint32_t ccount;
//    __asm__ __volatile__ ( "rsr     %0, ccount" : "=a" (ccount) );
//    return ccount;
//}

#ifdef __cplusplus
}
#endif

#endif