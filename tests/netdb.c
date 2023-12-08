#define _XOPEN_SOURCE 700

#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>

int main(int argc, char *argv[])
{
	if (argc < 2 || argc > 3)
		errx(EXIT_FAILURE, 
				"Usage: netdb COUNT\n"
				"       netdb NAME [PROTO]");


	if (argc == 3) {
		struct servent *ret;

		setservent(1);
		if ((ret = getservbyname(argv[1], argv[2])) == NULL)
			errx(EXIT_FAILURE, "No such service");

		printf("Found: %s/%s %d\n",
				ret->s_name,
				ret->s_proto,
				ret->s_port);
        endservent();
	} else if (isdigit(*argv[1])) {
		int val = atoi(argv[1]);
		struct servent *ret;
		setservent(1);
		for (int i = 0; i < val; i++) {
			if ((ret = getservent()) == NULL)
                goto done;

			printf("Found: %s/%s %d ",
				ret->s_name,
				ret->s_proto,
				ret->s_port);

            if (ret->s_aliases)
                for (int i = 0; ret->s_aliases[i] != NULL; i++)
                    printf("aka [%d]:%s ", i, ret->s_aliases[i]);
            printf("\n");
		}
done:
		endservent();
	} else {
		struct servent *ret;

		if ((ret = getservbyname(argv[1], NULL)) == NULL)
			errx(EXIT_FAILURE, "No such service");

		printf("Found: %s/%s %d\n",
				ret->s_name,
				ret->s_proto,
				ret->s_port);
		endservent();
	}

    exit(EXIT_SUCCESS);
}
