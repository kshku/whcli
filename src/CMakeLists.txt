set(SRCS
    main.c
    argparse.h
    argparse.c
)

target_sources(whcli PRIVATE ${SRCS})

install(TARGETS whcli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
