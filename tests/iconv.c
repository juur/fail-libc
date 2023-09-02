#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <uchar.h>
#include <iconv.h>
#include <err.h>
#include <locale.h>

int main(void)
{
	unsigned char u8_U10348[] = {
		0xF0, 0x90, 0x8D, 0x88
	};

	char32_t u32_U10348[] = {
		0x00010348,
	};

	char32_t u32_buf[4] = {0};
	unsigned char u8_buf[8] = {0};

	memset(u32_buf, 0, sizeof(u32_buf));
	memset(u8_buf, 0, sizeof(u8_buf));

	setlocale(LC_ALL, "");

	printf("utf8 -> utf32\n\n");

	iconv_t conv = iconv_open("utf-32", "utf-8");

	if (conv == (iconv_t)-1) {
		err(EXIT_FAILURE, "iconv_open");
	}

	if (iconv(conv, NULL, NULL, NULL, NULL) == (size_t)-1)
		err(EXIT_FAILURE, "iconv");

	size_t ret;
	size_t inb = sizeof(u8_U10348);
	size_t outb = sizeof(u32_buf);
	char *src = (char *)u8_U10348;
	char *dst = (char *)u32_buf;

	printf("iconv(%p, %p=%2x, %2x, %p=%2x, %2x)\n",
			conv,
			src,
			(unsigned char)*src,
			inb,
			dst,
			(unsigned char)*dst,
			outb);

	ret = iconv(conv, &src, &inb, &dst, &outb);
	if (ret == (size_t)-1)
		err(EXIT_FAILURE, "iconv");

	printf("u8_src:   ");
	for (unsigned i = 0; i < sizeof(u8_U10348); i++)
		printf("%02x ", u8_U10348[i]);
	printf("\n");

	printf("u32_buf:  ");
	for (unsigned i = 0; i < sizeof(u32_buf)/sizeof(char32_t); i++)
		printf("%08x ", u32_buf[i]);
	printf("\n");

	printf("u32_test: %0lx\n", u32_U10348[0]);
	iconv_close(conv);

	printf("\nutf32 -> utf8\n\n");

	if ((conv = iconv_open("utf-8", "utf-32")) == (iconv_t)-1)
		err(EXIT_FAILURE, "iconv_open");

	if (iconv(conv, NULL, NULL, NULL, NULL) == (size_t)-1)
		err(EXIT_FAILURE, "iconv");

	inb = sizeof(u32_U10348);
	outb = sizeof(u8_buf);
	src = (char *)u32_U10348;
	dst = (char *)u8_buf;

	printf("iconv(%p, %p=%2x, %2x, %p=%2x, %2x)\n",
			conv,
			src,
			(unsigned char)*src,
			inb,
			dst,
			(unsigned char)*dst,
			outb);

	ret = iconv(conv, &src, &inb, &dst, &outb);
	if (ret == (size_t)-1)
		err(EXIT_FAILURE, "iconv");

	printf("u32_src:  ");
	for (unsigned i = 0; i < sizeof(u32_U10348)/sizeof(char32_t); i++)
		printf("%08x ", u32_U10348[i]);
	printf("\n");

	printf("u8_buf:   ");
	for (unsigned i = 0; i < sizeof(u8_buf); i++)
		printf("%02x ", (unsigned char)u8_buf[i]);
	printf("\n");

	printf("u8_test:  ");
	for (unsigned i = 0; i < sizeof(u8_U10348); i++)
		printf("%02x ", u8_U10348[i]);
	printf("\n");

	iconv_close(conv);

	exit(EXIT_SUCCESS);
}
