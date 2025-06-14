set(SRCS
    main.c
    argparse.h
    argparse.c
)

target_sources(whcli PRIVATE ${SRCS})
