#include "unit_tests.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#ifdef __linux__
#include <wait.h>
#endif

# define BGRN "\033[1;32m"
# define CRESET "\033[0m"
# define BRED "\033[1;31m"
# define YEL "\033[0;33m"

specs_state_t  __specs__ = {
	.passed  = 0,
	.failed  = 0,
};

bool run_test(bool (*test)(void), const char *title) {
	struct timespec start_time, end_time;
    fprintf(stderr, " running: %s%35s%s -> ", YEL, title, CRESET);
    int ret;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    pid_t pid = fork();
    if (pid == -1)
        assert(0 && "Internal error");
    else if (pid == 0) {
        alarm(TIMEOUT_LIMIT);
        if (test())
            exit(0);
        exit(1);
    }

    while (waitpid(pid, &ret, 0) != -1)
        continue;

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double execution_time_ms = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                               (end_time.tv_nsec - start_time.tv_nsec) / 1e6;

    if (WEXITSTATUS(ret))
        goto test_ko;

    if (WIFSIGNALED(ret)) {
        fprintf(stderr, "SIG %s, ", strsignal(WTERMSIG(ret)));
        goto test_ko;
    }

	__specs__.passed++;
    fprintf(stderr, "(%f ms) " BGRN "OK %s\n", execution_time_ms, CRESET);
    return true;

test_ko:
	__specs__.failed++;
    fprintf(stderr, "(%f ms) " BRED "KO %s\n", execution_time_ms, CRESET);
    return false;
}
