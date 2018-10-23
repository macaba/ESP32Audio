#include <M5Stack.h>
#include <Audio.h>
#include <AudioStream.h>
#include "cpuStats.h"
#include "cpuDisplay.h"

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine4;          //xy=229,369
AudioSynthWaveformSine   sine1;          //xy=237,211
AudioSynthWaveformSine   sine3;          //xy=237,317
AudioSynthWaveformSine   sine2;          //xy=243,264
AudioMixer4              mixer1;         //xy=423,221
AudioEffectEnvelope      envelope1;      //xy=566,291
AudioOutputI2S           dac1;           //xy=695,240
AudioInputI2S            adc1;
AudioConnection          patchCord1(sine4, 0, mixer1, 3);
AudioConnection          patchCord2(sine1, 0, mixer1, 0);
AudioConnection          patchCord3(sine3, 0, mixer1, 2);
AudioConnection          patchCord4(sine2, 0, mixer1, 1);
AudioConnection          patchCord5(mixer1, envelope1);
AudioConnection          patchCord6(envelope1, dac1);
// GUItool: end automatically generated code

SemaphoreHandle_t xSemaphoreScreen = xSemaphoreCreateMutex();

void setup() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  
  M5.begin();
  M5.Lcd.printf("Synth Testbench (%i MHz)\r\n", F_CPU/1000000);
  M5.Lcd.setCursor(135, 230);
  M5.Lcd.printf("Note off. \r\n");
  M5.Lcd.setCursor(43, 230);
  M5.Lcd.printf("Amp Off. \r\n");
  AudioMemory(10);

  sine1.frequency(220);
  sine1.amplitude(1);
  sine2.frequency(440);
  sine2.amplitude(1);
  sine3.frequency(880);
  sine3.amplitude(1);
  sine4.frequency(1760);
  sine4.amplitude(1);
  //envelope1.attack(200);
  //envelope1.hold(100);
  //envelope1.decay(100);
  envelope1.release(500);
  mixer1.gainDb(0,-6, false);
  mixer1.gainDb(1,-0, false);
  mixer1.gainDb(2,-6, false);
  mixer1.gainDb(3,-15, false);
     
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
      envelope1.noteOn();
      if( xSemaphoreTake( xSemaphoreScreen, ( TickType_t ) 100 ) == pdTRUE )
      {
        M5.Lcd.setCursor(135, 230);
        M5.Lcd.printf("Note on. \r\n");
        xSemaphoreGive(xSemaphoreScreen);
      }
    }
    if(M5.BtnB.wasReleased())
    {
      envelope1.noteOff();
      if( xSemaphoreTake( xSemaphoreScreen, ( TickType_t ) 100 ) == pdTRUE )
      {
        M5.Lcd.setCursor(135, 230);
        M5.Lcd.printf("Note off. \r\n");
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
          M5.Lcd.printf("Amp On. \r\n");
        else
          M5.Lcd.printf("Amp Off. \r\n");
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