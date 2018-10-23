#include <M5Stack.h>
#include <Audio.h>
#include <AudioStream.h>
#include "cpuStats.h"
#include "cpuDisplay.h"

// GUItool: begin automatically generated code
AudioInputI2S            adc1;           //xy=364,669
AudioRecordPSRAM         record1;
AudioOutputI2S           dac1;           //xy=746,663
AudioConnection          patchCord2(adc1, 0, record1, 0);
AudioConnection          patchCord3(record1, 0, dac1, 0);
// GUItool: end automatically generated code

SemaphoreHandle_t xSemaphoreScreen = xSemaphoreCreateMutex();

void setup() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  
  M5.begin();
  M5.Lcd.printf("Synth Testbench (%i MHz)\r\n", F_CPU/1000000);
  M5.Lcd.setCursor(43, 230);
  M5.Lcd.printf("Amp Off. ");
  M5.Lcd.setCursor(135, 230);
  M5.Lcd.printf("Idle. ");
  M5.Lcd.setCursor(227, 230);
  M5.Lcd.printf("Play. ");
  AudioMemory(20);
     
  xTaskCreatePinnedToCore(audioTask, "AudioTask", 10000, NULL, configMAX_PRIORITIES - 1, NULL, 1);
  xTaskCreatePinnedToCore(inputTask, "InputTask", 10000, NULL, 4, NULL, 1);
  xTaskCreatePinnedToCore(statsTask, "StatsTask", 10000, NULL, 3, NULL, 1);
  StartMonitoringCPU0();
  StartMonitoringCPU1();
}

void loop()
{
  vTaskDelay(10000/portTICK_PERIOD_MS);
}

void inputTask( void * parameter ) 
{
  bool ampEnabled = false;
  for(;;){
    M5.update();
        
    if(M5.BtnB.wasPressed()) {
      digitalWrite(5, false);
      record1.startRecord();
      if( xSemaphoreTake( xSemaphoreScreen, ( TickType_t ) 100 ) == pdTRUE )
      {
        M5.Lcd.setCursor(130, 230);
        M5.Lcd.printf("Recording. \r\n");
        xSemaphoreGive(xSemaphoreScreen);
      }
    }
    if(M5.BtnB.wasReleased())
    {
      digitalWrite(5, ampEnabled);
      record1.stopRecord();
      if( xSemaphoreTake( xSemaphoreScreen, ( TickType_t ) 100 ) == pdTRUE )
      {
        M5.Lcd.setCursor(130, 230);
        M5.Lcd.printf("   Idle.  ");
        xSemaphoreGive(xSemaphoreScreen);
      }      
    }
    if(M5.BtnA.wasPressed())
    {
      ampEnabled = !ampEnabled;
      digitalWrite(5, ampEnabled);
      if( xSemaphoreTake( xSemaphoreScreen, ( TickType_t ) 100 ) == pdTRUE )
      {
        M5.Lcd.setCursor(43, 230);
        if(ampEnabled)
          M5.Lcd.printf("Amp On. ");
        else
          M5.Lcd.printf("Amp Off. ");
        xSemaphoreGive(xSemaphoreScreen);
      }
    }

    if(M5.BtnC.wasPressed())
    {
      record1.play();
      if( xSemaphoreTake( xSemaphoreScreen, ( TickType_t ) 100 ) == pdTRUE )
      {
        M5.Lcd.setCursor(227, 230);
        M5.Lcd.printf("Playing. ");
        xSemaphoreGive(xSemaphoreScreen);
      }
    }
       
    vTaskDelay(50/portTICK_PERIOD_MS);
  }
}

void audioTask( void * parameter )
{
  AudioStream *p;  
  for(;;){
    p->update_all();    
  }
}

void statsTask ( void * parameter){
  for(;;)
  {
    if( xSemaphoreTake( xSemaphoreScreen, ( TickType_t ) 10 ) == pdTRUE )
    {
      M5.Lcd.setCursor(200, 0);
      cpuDisplay();
      xSemaphoreGive(xSemaphoreScreen);
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}