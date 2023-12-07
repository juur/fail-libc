#define _XOPEN_SOURCE 700

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

static void strptime_test(const char *str, const char *fmt)
{
	char *ret;
	struct tm tm = {0};

	ret = strptime(str, fmt, &tm);

	printf("input: <%s>\n", str);

	if (ret == NULL)
		warnx("strptime_test: NULL returned\n");
	else if (*ret != '\0')
		warnx("strptime_test: trailing characters returned: <%s>\n", ret);
	else {
		printf(
				"tm_sec:  %02u\ntm_min:  %02u\ntm_hour: %02u\n"
				"tm_mday: %02u\ntm_mon:  %02u\ntm_year: %04u\n"
				"tm_wday: %02u\ntm_yday: %02u\ntm_dst:  %u\n"
#ifdef _DEFAULT_SOURCE
				"tm_off:  %lu\ntm_tz:   %s\n"
#endif
				,
				tm.tm_sec,
				tm.tm_min,
				tm.tm_hour,
				tm.tm_mday,
				tm.tm_mon,
				tm.tm_year + 1900,
				tm.tm_wday,
				tm.tm_yday,
				tm.tm_isdst
#ifdef _DEFAULT_SOURCE
				,tm.tm_gmtoff,
				tm.tm_zone ? tm.tm_zone : ""
#endif
				);
	}
}

int main(void)
{
	const struct { const char *str; const char *fmt; } test_cases[] = {
		{ "Thu Dec  7 09:29:03 GMT 2023", "%a %b %d %T %Z %Y" },

		{ NULL, NULL }
	};

	for (int i = 0; test_cases[i].str; i++)
		strptime_test(test_cases[i].str, test_cases[i].fmt);

	exit(EXIT_SUCCESS);
}
