#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "lined.h"
#include "term.h"
#include "cli.h"

#ifdef KICKC

static const char commands[] = {
  'h','e','l','p', 0,
  'e','c','h','o', 0,
  'p','a','r','s','e',0,
  'c','l','e','a','r', 0,
  'v','e','r','s','i','o','n', 0,
  'u','p','t','i','m','e', 0,
  'l','o','g','o','u','t', 0,
  'e','x','i','t', 0,
   0 // end marker
};

#else

static const char commands[] = {
  "help\0"
  "echo\0"
  "parse\0"
  "clear\0"
  "version\0"
  "uptime\0" 
  "logout\0"
  "exit\0"
  "\0" // end marker

  // TODO
  //"cat\0" "cp\0" "df\0" "dd\0" "ls\0" "mkfs\0"
  //"mv\0" "rm\0" "tail\0" "touch\0" "hd\0" "tetris\0"
};

#endif

static int8_t xstrncmp(const char *s1, const char *s2, uint8_t n) {
  uint8_t i;

  for (i=0; i<n; i++) {
    if (s1[i] != s2[i]) {
      return ((s1[i]>s2[i]) ? 1 : -1);
    }

    if (!s1[i] || !s2[i]) break;
  }

  return (0);
}

static int8_t xstrcmp(const char *s1, const char *s2) {
  return (xstrncmp(s1, s2, 255));
}

static uint8_t parse(char *cmd, char **argv, uint8_t size) {
  uint8_t argc = 0, args = (size / sizeof (argv[0]));
  char *s = cmd, *t = s + strlen(s) - 1;
  uint8_t quote = 0, first = 1;

  while ((t >= s) && (*t && (*t == ' '))) *t-- = 0; // trim end

  memset(argv, 0, size);

  while (*s) {
    if (first) {
      first = 0;

      // skip whitespace
      while (*s && (*s == ' ')) s++;

      // skip quotation mark
      if (*s && (*s == '"')) { s++; quote ^= 1; }

      // start next arg
      *argv++ = s;

      if (++argc == args) {
        return (argc);
      }
    }

    if (*s == '"') {
      *s++ = 0; // remove and skip
      if (*s == '"') first = 1;
      quote ^= 1;
    } else if ((!quote) && (*s == ' ')) {
      *s++ = 0; // remove and skip
      first = 1;
    } else {
      s++;
    }
  }

  return (argc);
}

static void cmd_help(uint8_t argc, char **argv) {
  const char *ptr = commands;

  cprintf("available commands are:\r\n");

  while (*ptr) {
    cprintf("  %s\r\n", ptr);
    ptr += strlen(ptr) + 1;
  }
}

static void cmd_parse(uint8_t argc, char **argv) {
  uint8_t i;

  for (i=0; i<argc; i++) {
    cprintf("argv[%i]='%s'\r\n", (int)i, argv[i]);
  }
}

static void cmd_echo(uint8_t argc, char **argv) {
  uint8_t i;

  for (i=1; i<argc; i++) {
    cprintf("%s", argv[i]);
    if (i < argc - 1) cprintf(" ");
  }
  cprintf("\r\n");
}

static void cmd_version(uint8_t argc, char **argv) {
  cprintf("push, version " VERSION "\r\n");
}

static void cmd_clear(uint8_t argc, char **argv) {
  term_clear_screen();
}

static void cmd_uptime(uint8_t argc, char **argv) {
  cprintf("uptime: not implemented\r\n");
}

void lined_complete_cb(lined_t *l) {
#ifdef HAVE_COMPLETION
  const char *ptr = commands;
  const char *c = l->buf;
  uint8_t spaces = 0;

  // remove all the leading spaces
  while (c && (*c == ' ')) {
    spaces++;
    c++;
  }

  // TAB completion requires at least one character
  if (strlen(c) < 1) return;

  if (!xstrncmp(c, "echo", 4)) {
    lined_completion_add(l, "echo foo bar");
  }

  while (*ptr) {
    if (!xstrncmp(ptr, c, strlen(c))) {
      lined_completion_add(l, ptr);
    }

    ptr += strlen(ptr) + 1;
  }
#endif
}

#ifdef HAVE_HINTS
const char *term_hint_cb(lined_t *l) {
  const char *c = l->buf;

  // remove all the leading spaces
  while (c && (*c == ' ')) c++;

  if (!xstrcmp(c, "parse")) return ("[<arg1> <arg2> ...]");
  if (!xstrcmp(c, "echo"))  return ("<text>");

  return (NULL);
}
#endif

uint8_t cli_exec(char *cmd) {
  char *argv[8];
  uint8_t argc;

  argc = parse(cmd, argv, sizeof (argv));

  if (argc == 0) return (0);

  if (!xstrcmp(argv[0], "exit") || !xstrcmp(argv[0], "logout")) {
    return (1); // exit
  } else if (!xstrcmp(argv[0], "help")) {
    cmd_help(argc, argv);
  } else if (!xstrcmp(argv[0], "echo")) {
    cmd_echo(argc, argv);
  } else if (!xstrcmp(argv[0], "parse")) {
    cmd_parse(argc, argv);
  } else if (!xstrcmp(argv[0], "clear")) {
    cmd_clear(argc, argv);
  } else if (!xstrcmp(argv[0], "version")) {
    cmd_version(argc, argv);
  } else if (!xstrcmp(argv[0], "uptime")) {
    cmd_uptime(argc, argv);
  } else {
    cprintf("%s: command not found\r\n", argv[0]);
  }

  return (0);
}
