/* Definitions the scanner shares with the parser
    Michael L. Scott, 2008-2019.
*/

typedef enum {t_read, t_write, t_id, t_literal, t_gets,
                t_add, t_sub, t_mul, t_div, t_lparen, t_rparen, t_eof} token;

#define MAX_TOKEN_LEN 100
extern char token_image[MAX_TOKEN_LEN];

extern token scan();
