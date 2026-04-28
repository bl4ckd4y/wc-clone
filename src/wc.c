/*
 * wc — клон утилиты Unix wc.
 * Считает в файле строки (-l), слова (-w) и байты (-c).
 * Если флаги не указаны — выводит все три.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    long lines;
    long words;
    long bytes;
} counts_t;

typedef struct
{
    int show_lines;
    int show_words;
    int show_bytes;
    int any_flag;
    int arg_i;
} options_t;

static options_t options_init(void)
{
    options_t opt = {0, 0, 0, 0, 1};
    return opt;
}

static int parse_flags(int argc, char *argv[], options_t *opt)
{
    for (; opt->arg_i < argc; opt->arg_i++)
    {
        if (argv[opt->arg_i][0] == '-' && argv[opt->arg_i][1] != '\0')
        {
            for (int i = 1; argv[opt->arg_i][i] != '\0'; i++)
            {
                if (argv[opt->arg_i][i] == 'l')
                {
                    opt->show_lines = 1;
                    opt->any_flag = 1;
                }
                else if (argv[opt->arg_i][i] == 'w')
                {
                    opt->show_words = 1;
                    opt->any_flag = 1;
                }
                else if (argv[opt->arg_i][i] == 'c')
                {
                    opt->show_bytes = 1;
                    opt->any_flag = 1;
                }
                else
                {
                    fprintf(stderr, "invalid option -- %s\n", argv[opt->arg_i]);
                    return 1;
                }
            }
        }
        if (argv[opt->arg_i][0] != '-')
            break;
    }

    if (!opt->any_flag)
    {
        opt->show_lines = 1;
        opt->show_words = 1;
        opt->show_bytes = 1;
    }
    return 0;
}

static counts_t counts_init(void)
{
    counts_t count = {0, 0, 0};
    return count;
}

static void print_counts(const counts_t *c, const char *filename, int show_lines, int show_words, int show_bytes)
{
    if (show_lines)
        printf("%8ld", c->lines);
    if (show_words)
        printf("%8ld", c->words);
    if (show_bytes)
        printf("%8ld", c->bytes);
    printf(" %s\n", filename);
}

static int count_file(FILE *fp, counts_t *c)
{
    int ch;
    int in_word = 0;

    c->lines = 0;
    c->words = 0;
    c->bytes = 0;

    while ((ch = fgetc(fp)) != EOF)
    {
        c->bytes++;

        if (ch == '\n')
        {
            c->lines++;
        }

        if (isspace(ch))
        {
            in_word = 0;
        }
        else if (in_word == 0)
        {
            c->words++;
            in_word = 1;
        }
    }

    return 0;
}

static int process_one_file(const char *progname, const char *filename, options_t opt, counts_t *out)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "%s: %s: cannot open file\n", progname, filename);

        return 1;
    }

    if (count_file(fp, out) != 0)
    {
        fprintf(stderr, "%s: %s: read error\n", progname, filename);
        fclose(fp);
        return 1;
    }
    fclose(fp);

    print_counts(out, filename, opt.show_lines, opt.show_words, opt.show_bytes);
    return 0;
}

static void add_counts(counts_t *total, counts_t c)
{
    total->lines += c.lines;
    total->words += c.words;
    total->bytes += c.bytes;
}

int main(int argc, char *argv[])
{
    options_t opt = options_init();

    if (parse_flags(argc, argv, &opt) != 0)
        return 1;

    if (opt.arg_i >= argc)
    {
        fprintf(stderr, "Usage: %s [-lwc] FILE...\n", argv[0]);
        return 1;
    }

    int exit_code = 0;
    counts_t total = counts_init();
    int file_count = 0;

    for (; opt.arg_i < argc; opt.arg_i++)
    {
        counts_t c;
        if (process_one_file(argv[0], argv[opt.arg_i], opt, &c) != 0)
        {
            exit_code = 1;
            continue;
        }
        add_counts(&total, c);
        file_count++;
    }

    if (file_count > 1)
    {
        print_counts(&total, "total", opt.show_lines, opt.show_words, opt.show_bytes);
    }

    return exit_code;
}
