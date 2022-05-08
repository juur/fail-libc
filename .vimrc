let g:ale_c_cc_options .= " -ffreestanding -isystem include -Iinclude -nostdinc -Wall -Wextra -pedantic -std=c99 -isystem /opt/rh/gcc-toolset-11/root/usr/lib/gcc/x86_64-redhat-linux/11/include -Wmissing-field-initializers"
set path=.,include,
