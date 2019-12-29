idf_component_register(
	SRCS 
		src/AudioStream.cpp 
		src/control_i2s.cpp 
		src/data_waveforms.c 
		src/effect_calibration.cpp 
		src/effect_compressor.cpp 
		lib/dr_wav/dr_wav.h
		src/effect_delay.cpp
		src/effect_envelope.cpp
		src/effect_multiply.cpp
		src/input_i2s.cpp
		src/mixer.cpp
		src/output_i2s.cpp
		#src/play_sdmmc_wav.cpp
		#src/record_flash.cpp
		src/record_psram.cpp
		src/synth_dc.cpp
		src/synth_sine.cpp
	INCLUDE_DIRS include
	PRIV_INCLUDE_DIRS "lib/dr_wav")
					
# Edit following two lines to set component requirements (see docs)
#set(COMPONENT_REQUIRES )
#set(COMPONENT_PRIV_REQUIRES )

#set(COMPONENT_SRCS "src" "lib/dr_wav")
#set(COMPONENT_ADD_INCLUDEDIRS "include")
#set(COMPONENT_PRIV_INCLUDEDIRS "lib/dr_wav")

#register_component()