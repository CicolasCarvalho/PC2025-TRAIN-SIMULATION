#ifndef PC2025_ENTITY_SIMULATION_DEFS_H
#define PC2025_ENTITY_SIMULATION_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//-debug-info-----------------------------------------------------------------------------------------------------------

#define DEBUG_INFO 1

//-log-utilities--------------------------------------------------------------------------------------------------------

#if DEBUG_INFO
    #define PRINT_FILE_AND_LINE() printf("\033[0;90m%s:%i:\e[0m ", __FILE__, __LINE__)

    #define START_LOG(name) \
                PRINT_FILE_AND_LINE(); \
                printf("%s\n", name)
    #define LOG(...)                                \
                printf(__VA_ARGS__)
    #define END_LOG(name) \
                printf("\n")
                // PRINT_FILE_AND_LINE();
                // printf("'%s'\n", name)

    #define PRINT(...)                             \
                PRINT_FILE_AND_LINE(); \
                printf(__VA_ARGS__);                   \
                printf("\n")
    #define WARN(...)           \
                printf("\033[0;33mWARN:\033[0m ");    \
                PRINT(__VA_ARGS__)
    #define RAISE(...)          \
                printf("\033[0;31mERROR:\033[0m ");    \
                PRINT(__VA_ARGS__);  \
                exit(1)

    #define OUTPUT(...) \
                printf("\033[0;32m>>\033[0m ");        \
                printf(__VA_ARGS__)
#else
    #define PRINT_FILE_AND_LINE()

    #define START_LOG(name)
    #define LOG(...)
    #define END_LOG(name)

    #define PRINT(...)
    #define WARN(...)                               \
                printf("\033[0;33mWARN:\033[0m ");    \
                PRINT_FILE_AND_LINE();              \
                printf(__VA_ARGS__);                   \
                printf("\n")
    #define RAISE(...)          \
                printf("\033[0;31mERROR:\033[0m ");    \
                PRINT_FILE_AND_LINE();              \
                printf(__VA_ARGS__);                   \
                printf("\n"); \
                exit(1)

    #define OUTPUT(...) \
                printf(__VA_ARGS__)
#endif //DEBUG_INFO

//-program-config-------------------------------------------------------------------------------------------------------

#define GET_POSITION(x, y, w) ((y) * (w) + (x))

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TARGET_FPS 120
#define TICK_PER_SECOND 1
#define FPS_BUFFER 10
#define FPS_INTERVAL 1

#define CITY_COUNT 200
#define ENTITY_COUNT 1000

#define SINGLE_THREAD 0b00
#define MULTI_THREAD 0b01
#define MPI 0b10

#define RUN_MODE MPI
#define MULTITHREADING (RUN_MODE & MULTI_THREAD)
#define MPI_MODE (RUN_MODE & MPI)

#define THREAD_COUNT 1

#endif //PC2025_ENTITY_SIMULATION_DEFS_H
