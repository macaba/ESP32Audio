//Example: SineEnvelope

#include "Audio.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// GUItool: begin automatically generated code
AudioInputI2S            i2sInput1;      //xy=215,226
AudioSynthWaveformSine   sine1;          //xy=306,401
AudioEffectCalibration   calibrationInL;   //xy=374,212
AudioEffectCalibration   calibrationInR; //xy=375,246
AudioEffectEnvelope      envelope1;      //xy=497,398
AudioEffectCalibration   calibrationOutR; //xy=809,245
AudioEffectCalibration   calibrationOutL; //xy=811,211
AudioOutputI2S           i2sOutput1;     //xy=985,229
AudioConnection          patchCord1(i2sInput1, 0, calibrationInL, 0);
AudioConnection          patchCord2(i2sInput1, 1, calibrationInR, 0);
AudioConnection          patchCord3(sine1, envelope1);
AudioConnection          patchCord4(envelope1, calibrationOutL);
AudioConnection          patchCord5(envelope1, calibrationOutR);
AudioConnection          patchCord6(calibrationOutR, 0, i2sOutput1, 1);
AudioConnection          patchCord7(calibrationOutL, 0, i2sOutput1, 0);
AudioControlI2S          i2s;           //xy=553,55
AudioControlPCM3060      pcm3060;      //xy=697,60
// GUItool: end automatically generated code


void audioTask( void * parameter )
{
  AudioStream *p;  
  for(;;){
    p->update_all();    
  }
}

void noteTask( void * parameter )
{
  for(;;){
    envelope1.noteOn();
    vTaskDelay(50 / portTICK_PERIOD_MS);
    envelope1.noteOff();
    vTaskDelay(950 / portTICK_PERIOD_MS);
  }
}

extern "C" {
   void app_main();
}

void app_main()
{
    AudioMemory(10);
    i2s.default_codec_rx_tx_24bit();
    vTaskDelay(1000/portTICK_RATE_MS);
    pcm3060.init();
    sine1.frequency(440);
    sine1.amplitude(0.1);
    calibrationOutL.calibrate(0, 0.0156, 10, 11.34);
    calibrationOutR.calibrate(0, 0.069, 10, 11.372);
    calibrationInL.calibrate(0, -0.000757, 1, 0.958241);
    calibrationInR.calibrate(0, -0.000757, 1, 0.958241);    //Made up these values
 
    xTaskCreatePinnedToCore(audioTask, "AudioTask", 10000, NULL, configMAX_PRIORITIES - 1, NULL, 1);
    xTaskCreatePinnedToCore(noteTask, "NoteTask", 1000, NULL, 2, NULL, 1);
}