#include "proto.h"

void settings_mode (void) {
  char * line;
  char * setting;
  char * value;
  char * new_value;
  unsigned setting_number, setting_length;
  puts("Enter a setting to view its value, or a setting and a value to change it.");
  puts("Enter ? for help, or a blank line to exit configuration mode.");
  while (1) {
    printf("config> ");
    line = read_line(stdin);
    if (!*line) {
      free(line);
      return;
    } else if (!strcmp(line, "?")) {
      free(line);
      settings_help();
      continue;
    }
    setting_length = strcspn(line, " \t");
    setting = malloc(setting_length + 1);
    memcpy(setting, line, setting_length);
    setting[setting_length] = 0;
    value = line + setting_length;
    value += strspn(value, " \t");
    if (!*value) value = NULL;
    for (setting_number = 0; setting_entries[setting_number].name; setting_number ++)
      if (!strcmp(setting_entries[setting_number].name, setting)) break;
    if (setting_entries[setting_number].name) {
      new_value = setting_entries[setting_number].handler(value);
      if (new_value)
        printf("%s %s %s\n", setting, value ? "set to" : "is", new_value);
      else
        printf("%s is not a valid value for %s\n", value, setting);
      free(new_value);
    } else
      printf("Unknown setting '%s'. Type ? for help, or press Enter to exit.\n", setting);
    free(setting);
    free(line);
  }
}

void parse_configuration_line (const char * line) {
  char ** kvp = split_by(line, ","); // key_value_pairs is too long, so kvp will do
  if (!kvp) return;
  unsigned count = string_array_size(kvp), current;
  unsigned setting_number;
  char * value;
  char * result;
  for (current = 0; current < count; current ++) {
    value = strchr(kvp[current], '=');
    if (!value) error_exit(1, "no value given in configuration string: %s", kvp[current]);
    *(value ++) = 0;
    for (setting_number = 0; setting_entries[setting_number].name; setting_number ++)
      if (!strcmp(setting_entries[setting_number].name, kvp[current])) break;
    if (!setting_entries[setting_number].name) error_exit(1, "unknown setting: %s", kvp[current]);
    result = setting_entries[setting_number].handler(value);
    if (!result) error_exit(1, "invalid value for %s: %s", kvp[current], value);
    free(result);
  }
  destroy_string_array(kvp);
}

void settings_help (void) {
  unsigned setting_number;
  for (setting_number = 0; setting_entries[setting_number].name; setting_number ++)
    printf("%-" COMMAND_LENGTH "s - %s\n", setting_entries[setting_number].name, setting_entries[setting_number].description);
  puts("Enter a setting to view its value, or a setting and a value to change it.");
  puts("Enter ? for help, or a blank line to exit configuration mode.");
}

char * headers_setting_handler (const char * value) {
  unsigned numeric_value;
  if (!value)
    numeric_value = global_settings.insert_replacement_comment;
  else if (!strcmp(value, "off"))
    numeric_value = 0;
  else if (!strcmp(value, "on"))
    numeric_value = 1;
  else
    return NULL;
  global_settings.insert_replacement_comment = numeric_value;
  return duplicate_string(numeric_value ? "on" : "off");
}

char * indent_setting_handler (const char * value) {
  unsigned numeric_value;
  if (!value)
    numeric_value = global_settings.indent_lines;
  else if (!strcmp(value, "none"))
    numeric_value = 0;
  else if (!strcmp(value, "tab"))
    numeric_value = 15;
  else if ((*value >= '0') && (*value <= '9') && (!value[1]))
    numeric_value = *value - '0';
  else
    return NULL;
  global_settings.indent_lines = numeric_value;
  if (numeric_value > 9) return duplicate_string("tab");
  if (!numeric_value) return duplicate_string("none");
  char * result = malloc(2);
  *result = '0' + numeric_value;
  result[1] = 0;
  return result;
}

char * code_labels_setting_handler (const char * value) {
  unsigned numeric_value;
  if (!value)
    numeric_value = global_settings.code_labels;
  else if (!strcmp(value, "off"))
    numeric_value = 0;
  else if (!strcmp(value, "on"))
    numeric_value = 1;
  else
    return NULL;
  global_settings.code_labels = numeric_value;
  return duplicate_string(numeric_value ? "on" : "off");
}

char * data_labels_setting_handler (const char * value) {
  unsigned numeric_value;
  if (!value)
    numeric_value = global_settings.data_labels;
  else if (!strcmp(value, "off"))
    numeric_value = 0;
  else if (!strcmp(value, "exact"))
    numeric_value = 1;
  else if (!strcmp(value, "on"))
    numeric_value = 2;
  else
    return NULL;
  global_settings.data_labels = numeric_value;
  return duplicate_string(numeric_value ? ((numeric_value == 1) ? "exact" : "on") : "off");
}
