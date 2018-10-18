#include <M5Stack.h>
#include <Audio.h>
#include <AudioStream.h>
#include "cpuStats.h"

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine4;          //xy=229,369
AudioSynthWaveformSine   sine1;          //xy=237,211
AudioSynthWaveformSine   sine3;          //xy=237,317
AudioSynthWaveformSine   sine2;          //xy=243,264
AudioMixer4              mixer1;         //xy=423,221
AudioEffectEnvelope      envelope1;      //xy=566,291
AudioOutputAnalog        dac1;           //xy=695,240
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
  while(1){
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
  while(1){
    p->update_all();    
  }
}

void statsTask ( void * parameter){          //Baseline: 0.75%. 1 Sine into DAC: 6.5%
  const float maxTicksPerUpdate = (((float)F_CPU) / 44100.0f) * 128.0f;
  while(1)
  {
    int sine1Clocks = sine1.clocksPerUpdate;
    int sine2Clocks = sine2.clocksPerUpdate;
    int sine3Clocks = sine3.clocksPerUpdate;
    int sine4Clocks = sine4.clocksPerUpdate;
    int mixer1Clocks = mixer1.clocksPerUpdate;
    int envelope1Clocks = envelope1.clocksPerUpdate;
    float sine1Load = 100.0f * ((float)sine1Clocks / maxTicksPerUpdate);
    float sine2Load = 100.0f * ((float)sine2Clocks / maxTicksPerUpdate);
    float sine3Load = 100.0f * ((float)sine3Clocks / maxTicksPerUpdate);
    float sine4Load = 100.0f * ((float)sine4Clocks / maxTicksPerUpdate);
    float sine1LoadMax = 100.0f * ((float)sine1.clocksPerUpdateMax / maxTicksPerUpdate);
    float sine2LoadMax = 100.0f * ((float)sine1.clocksPerUpdateMax / maxTicksPerUpdate);
    float sine3LoadMax = 100.0f * ((float)sine1.clocksPerUpdateMax / maxTicksPerUpdate);
    float sine4LoadMax = 100.0f * ((float)sine1.clocksPerUpdateMax / maxTicksPerUpdate);
    float mixer1Load = 100.0f * ((float)mixer1Clocks / maxTicksPerUpdate);
    float mixer1LoadMax = 100.0f * ((float)mixer1.clocksPerUpdateMax / maxTicksPerUpdate);
    float envelope1Load = 100.0f * ((float)envelope1Clocks / maxTicksPerUpdate);
    float envelope1LoadMax = 100.0f * ((float)envelope1.clocksPerUpdateMax / maxTicksPerUpdate);
    float audioNonBlockingTotalLoad = sine1Load + sine2Load + sine3Load + sine4Load + mixer1Load + envelope1Load;
    float audioNonBlockingTotalLoadMax = sine1LoadMax + sine2LoadMax + sine3LoadMax + sine4LoadMax + mixer1LoadMax + envelope1LoadMax;
  
    if( xSemaphoreTake( xSemaphoreScreen, ( TickType_t ) 10 ) == pdTRUE )
    {
      int w[3] = {6,13,8};
      M5.Lcd.setCursor(200, 0);
      M5.Lcd.printf("%i ms uptime\r\n\r\n", millis());
      M5.Lcd.printf("CPU 0 %5.2f%% [%5.2f%% max]  \r\n", cpu0Load, cpu0LoadMax);
      M5.Lcd.printf("CPU 1 %5.2f%% [%5.2f%% max]  \r\n\r\n", cpu1Load, cpu1LoadMax);
      M5.Lcd.printf("Audio %5.2f%% [%5.2f%% max]  \r\n\r\n", audioNonBlockingTotalLoad, audioNonBlockingTotalLoadMax);
      M5.Lcd.printf(" %-10s %6s %7s %6s  \r\n", "Object", "CPU %", "Clocks", "Max %"); 
      M5.Lcd.printf(" --------------------------------\r\n");
      M5.Lcd.printf(" %-10s %6.2f %7i %6.2f%  \r\n", "Sine 1", sine1Load, sine1Clocks, sine1LoadMax);
      M5.Lcd.printf(" %-10s %6.2f %7i %6.2f%  \r\n", "Sine 2", sine2Load, sine2Clocks, sine2LoadMax);
      M5.Lcd.printf(" %-10s %6.2f %7i %6.2f%  \r\n", "Sine 3", sine3Load, sine3Clocks, sine3LoadMax);
      M5.Lcd.printf(" %-10s %6.2f %7i %6.2f%  \r\n", "Sine 4", sine4Load, sine4Clocks, sine4LoadMax);
      M5.Lcd.printf(" %-10s %6.2f %7i %6.2f%  \r\n", "Mixer 1", mixer1Load, mixer1Clocks, mixer1LoadMax);
      M5.Lcd.printf(" %-10s %6.2f %7i %6.2f%  \r\n", "Envelope 1", envelope1Load, envelope1Clocks, envelope1LoadMax);
      xSemaphoreGive(xSemaphoreScreen);
    }
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}
