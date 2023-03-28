#include <limine.h>

#include "common.h"

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.
static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

static struct limine_terminal *terminal;

void init_terminal(void)
{
    // Ensure we got a terminal
    if (terminal_request.response == NULL
     || terminal_request.response->terminal_count < 1) {
        hcf();
    }

    terminal = terminal_request.response->terminals[0];
}

void terminal_write(const char *s)
{
    terminal_request.response->write(terminal, s, strlen(s));
}

void terminal_put(char c)
{
    char s[2];
    s[0] = c;
    s[1] = '\0';
    terminal_write(s);
}

void terminal_write_hex(uint32_t n)
{
    int32_t tmp;

    terminal_write("0x");

    char no_zeros = 1;

    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && no_zeros != 0)
        {
            continue;
        }

        if (tmp >= 0xA)
        {
            no_zeros = 0;
            terminal_put(tmp-0xA+'a' );
        }
        else
        {
            no_zeros = 0;
            terminal_put( tmp+'0' );
        }
    }

    tmp = n & 0xF;
    if (tmp >= 0xA)
    {
        terminal_put(tmp-0xA+'a');
    }
    else
    {
        terminal_put(tmp+'0');
    }

}

void terminal_write_dec(uint32_t n)
{

    if (n == 0)
    {
        terminal_put('0');
        return;
    }

    int32_t acc = n;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    terminal_write(c2);
}
