let g:ale_c_cc_options .= " -ffreestanding -isystem include -Iinclude -nostdinc -Wall -Wextra -std=c11 -isystem /usr/lib/gcc/x86_64-redhat-linux/11/include -Wmissing-field-initializers"
set path=.,include,
let g:indentLine_enabled = 0
set sw=4 ts=4 expandtab
