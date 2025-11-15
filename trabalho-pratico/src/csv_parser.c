#include <stddef.h>
#include "csv_parser.h"


int csv_split_quoted(
    char *s,//linha que se transforma em tokens
    char sep,// o separador entre os tokens é a ','
    char **out,//array onde ficam os ponteiros para o inicio de cada campo; ex: iaca
    int max_out//número máximo de carateres
) 
{
    if (!s || !out || max_out <= 0) return 0;
    int n = 0;
    int inq = 0;
    char *r = s;
    char *w = s;
    out[n++] = w;
    while (*r) {
        char c = *r;
        if (c == '"') {
            if (inq && r[1] == '"') { 
                *w++ = '"';
                r += 2;
                continue;
            }
            inq = !inq; 
            r++;
            continue;
        }
        if (!inq && c == sep) {
            *w++ = '\0';
            if (n < max_out) out[n++] = w;
            r++;
            continue;
        }
        *w++ = c;
        r++;
    }
    *w = '\0';
    return n;
}

