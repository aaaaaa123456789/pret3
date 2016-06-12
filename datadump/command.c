#include "proto.h"

unsigned char get_command (const char * prompt, unsigned mask) {
  char * line;
  unsigned char command;
  while (1) {
    printf("%s", prompt);
    line = read_line(stdin);
    if (!(*line && strcmp(line, "?") && strcmp(line, "help"))) {
      print_command_help(mask);
      free(line);
      continue;
    }
    for (command = 0; commands[command].name; command ++) {
      if (!(mask & (1 << command))) continue;
      if (!strcmp(line, commands[command].name) || (commands[command].alias && !strcmp(line, commands[command].alias))) {
        free(line);
        return command;
      }
    }
    free(line);
    printf("Invalid command, enter ? for help.\n");
  }
}

void print_command_help (unsigned mask) {
  unsigned char command;
  for (command = 0; commands[command].name; command ++) {
    if (!(mask & (1 << command))) continue;
    printf("%-" COMMAND_LENGTH "s - ", commands[command].name);
    if (commands[command].alias) printf("(alias: %s) ", commands[command].alias);
    printf("%s\n", commands[command].help_text);
  }
  printf("%-" COMMAND_LENGTH "s - %s\n", "help", "(alias: ?) displays this help");
}
