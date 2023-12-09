#define _XOPEN_SOURCE 700

void parse_resolv_config(void);

int main(void)
{
    parse_resolv_config();
    return 0;
}
