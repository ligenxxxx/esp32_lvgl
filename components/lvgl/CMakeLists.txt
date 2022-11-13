file(GLOB_RECURSE LV_SOURCES src/*.c)
file(GLOB_RECURSE LV_PORTING porting/*.c)
file(GLOB_RECURSE LV_DEMOS demos/*.c)
file(GLOB_RECURSE LV_EXAMPLES examples/*.c)

idf_component_register(	SRCS ${LV_SOURCES}
                        SRCS ${LV_PORTING}
                        SRCS ${LV_DEMOS}
                        SRCS ${LV_EXAMPLES}
                        INCLUDE_DIRS . src porting demos
                        PRIV_REQUIRES lcd)