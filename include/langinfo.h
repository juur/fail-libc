#ifndef _LANGINFO_H
#define _LANGINFO_H

#include <locale.h>
#include <nl_types.h>

char *nl_langinfo(nl_item);
char *nl_langinfo_l(nl_item, locale_t);

/* queryable via nl_langinfo - do we want to match Linux/glibc? */

#define CODESET 0

#endif
