file(GLOB_RECURSE UI_SOURCES src/*.c)
file(GLOB_RECURSE UI_IMAGE image/*.c)

idf_component_register(	SRCS ${UI_SOURCES}
                        SRCS ${UI_IMAGE}
                        INCLUDE_DIRS . src image
                        PRIV_REQUIRES lvgl)