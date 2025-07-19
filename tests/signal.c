#define _XOPEN_SOURCE 700

#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

static bool running = true;

static void parent_handler(int sig, siginfo_t *info, void *ucontext)
{
    printf("parent_handler: invoked: sig=%lx, siginfo=%lx, ucontxt=%lx\n",
            sig, info, ucontext);
}

static int parent(pid_t child_pid)
{
    int rc;

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_sigaction = parent_handler;
    sa.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
        err(EXIT_FAILURE, "parent: sigaction SIGUSR1");

    printf("parent: going to sleep\n");
    rc = sleep(5);

    if (rc == 0)
        errx(EXIT_FAILURE, "parent: sleep returned before any signals were received");

    if (errno != EINTR)
        errx(EXIT_FAILURE, "parent: sleep returned an error");

    struct timespec tm;
    tm.tv_sec = 5;
    tm.tv_nsec = 1000000;

    printf("parent: sending kill ");
    for (int cnt = 0; cnt < 10; cnt++) {
        printf(".");
        kill(child_pid, SIGUSR1);
        //nanosleep(&tm, NULL);
    }
    printf("\n");
    if ((rc = nanosleep(&tm, NULL)) == -1)
        warn("parent: nanosleep");

    kill(child_pid, SIGUSR2);

    pid_t pid;
    int wstatus = 0;

    pid = waitpid(child_pid, &wstatus, 0);

    if (pid == -1)
        err(EXIT_FAILURE, "parent: waitpid");

    printf("parent: wstatus is %08x\n", wstatus);

    return EXIT_SUCCESS;
}

static void siguser1_handler(int sig, siginfo_t *info, void *)
{
    printf("siguser1_handler: signo:%d code:%d pid:%d errno:%d uid:%d\n",
            sig,
            info->si_code,
            info->si_pid,
            info->si_errno,
            info->si_uid
            );
}

static void siguser2_handler(int, siginfo_t *, void *)
{
    running = false;
}

static int child(pid_t parent_pid)
{
    printf("child: started\n");

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_sigaction = siguser1_handler;
    sa.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
        err(EXIT_FAILURE, "child: sigaction SIGUSR1");

    sa.sa_sigaction = siguser2_handler;

    if (sigaction(SIGUSR2, &sa, NULL) == -1)
        err(EXIT_FAILURE, "child: sigaction SIGUSR2");

    sleep(1);

    printf("child: sending SIGUSR1 to parent\n");
    kill(parent_pid, SIGUSR1);

    printf("child: entering main loop\n");
    while (running)
        if (sleep(5) != 0)
            err(EXIT_FAILURE, "child: sleep");

    return EXIT_SUCCESS;
}

int main(void)
{
    pid_t child_pid, parent_pid;

    parent_pid = getpid();
    child_pid = fork();

    if (child_pid < 0)
        err(EXIT_FAILURE, "main: fork");

    if (child_pid == 0)
        return child(parent_pid);

    return parent(child_pid);
}
