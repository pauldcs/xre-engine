
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define EXECUTE_AND_CHECK_SIGNAL(expr, signalNumber) \
    ({ \
        pid_t id = fork(); \
        int result; \
        \
        if (id == -1) { \
            perror("fork"); \
            exit(EXIT_FAILURE); \
        } \
        \
        if (id == 0) { \
            expr; \
            exit(EXIT_SUCCESS); \
        } else { \
            int status; \
            while (waitpid(id, &status, 0) != -1); \
            \
            result = (WIFSIGNALED(status) && WTERMSIG(status) == signalNumber); \
        } \
        \
        result; \
    })
