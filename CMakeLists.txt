#idf_component_register(SRCS "src" "lib/dr_wav"
#                    INCLUDE_DIRS "include"
#                    PRIV_INCLUDE_DIRS "lib/dr_wav")
					
# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "src" "lib/dr_wav")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "lib/dr_wav")

register_component()