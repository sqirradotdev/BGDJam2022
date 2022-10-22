add_library(Physac
    src/physac.h
    src/physac.c
)

target_link_libraries(Physac raylib)
target_include_directories(Physac PUBLIC src)
