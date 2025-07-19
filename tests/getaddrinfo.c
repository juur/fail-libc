#define _XOPEN_SOURCE 700

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <err.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
        errx(EXIT_FAILURE, "missing nodename");

    const struct addrinfo addrinfo = {
        .ai_family = AF_INET,
    };

    int rc;
    struct addrinfo *result;

    rc = getaddrinfo(argv[1], NULL, &addrinfo, &result);

    if (rc != 0) {
        warnx("getaddrinfo failed: %s", gai_strerror(rc));
    } else {
        printf("got result: 0x%08x\n", ((struct sockaddr_in *)result->ai_addr)->sin_addr);
    }

    if (result)
        freeaddrinfo(result);
    return EXIT_SUCCESS;
}
