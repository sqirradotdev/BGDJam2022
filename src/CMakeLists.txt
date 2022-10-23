add_executable(src
    constants.h

    main.c

    global_resources.h
    global_resources.c

    core/gameloop.h
    core/gameloop.c
    core/sprite.h
    core/sprite.c
    core/tween.h
    core/tween.c

    util/list.h
    util/list.c
    util/formatter.h
    util/formatter.c

    state/state.h
    state/state.c
    state/state_splash.h
    state/state_splash.c
    state/state_menu.h
    state/state_menu.c
    state/state_main.h
    state/state_main.c

    player.h
    player.c

    crate.c
    crate.h

    chest.h
    chest.c

    hud.h
    hud.c
)

target_link_libraries(src raylib)
if (APPLE)
    target_link_libraries(src "-framework IOKit")
    target_link_libraries(src "-framework Cocoa")
    target_link_libraries(src "-framework OpenGL")
endif()
target_link_libraries(src cLDtk)

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_LIST_DIR}/../resources/ $<TARGET_FILE_DIR:src>/resources/
)
add_dependencies(src copy_resources)

set_target_properties(src PROPERTIES OUTPUT_NAME "bgdjam2022")
