#ifndef TERMINAL_H
#define TERMINAL_H

void init_terminal(void);
void terminal_put(char c);
void terminal_write(const char *s);
void terminal_write_hex(uint32_t n);
void terminal_write_dec(uint32_t n);

#endif