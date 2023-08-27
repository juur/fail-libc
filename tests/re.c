#define _XOPEN_SOURCE 700

#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *init_re = "(.{1,4}):(.*):(.+):(\\+?)(.*)";

int main(int argc, char *argv[])
{
	if (argc < 2)
		exit(1);

	regex_t re;
	int ret;

	if ((ret = regcomp(&re, (argc > 2) ? argv[1] : init_re, 0)) != 0) {
		perror("regcomp");
		exit(1);
	}

	regmatch_t rem[10];
	memset(rem, 0, sizeof(rem));

	const char *string = (argc > 2) ? argv[2] : argv[1];

	int rc = regexec(&re, string, 10, rem, 0);
	printf("regexec returned %d\n", rc);

	for (int i = 0; i < 10; i++) {
		printf("match[%d] {%d,%d} ", i, rem[i].rm_so, rem[i].rm_eo);
		if (rem[i].rm_eo)
			fwrite(string + rem[i].rm_so, rem[i].rm_eo - rem[i].rm_so, 1, stdout);
		printf("\n");

	}

	exit(0);
}
