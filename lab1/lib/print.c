#include "print.h"
#include "sbi.h"

void putc(char c) { sbi_putchar(c); }

void puts(char *s)
{
    while ((*s) != '\0')
    {
        sbi_putchar(*s);
        s++;
    }
}

void puti(int x)
{
    int digit = 1;
    int tmp = x;
    while (tmp >= 10)
    {
        digit *= 10;
        tmp /= 10;
    }
    while (digit >= 1)
    {
        sbi_putchar('0' + x / digit);
        x %= digit;
        digit /= 10;
    }
}