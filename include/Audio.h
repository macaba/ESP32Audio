#ifndef Audio_h_
#define Audio_h_

// include all the library headers, so a sketch can use a single
// #include <Audio.h> to get the whole library
//
//

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "control_afs22.h"
#include "control_i2s.h"
#include "control_pcm3060.h"
#include "effect_calibration.h"
#include "effect_compressor.h"
#include "effect_delay.h"
#include "effect_envelope.h"
#include "effect_multiply.h"
#include "input_i2s.h"
#include "mixer.h"
#include "output_i2s.h"
#include "play_sdmmc_wav.h"
#include "record_psram.h"
#include "synth_dc.h"
#include "synth_sine.h"

#endif