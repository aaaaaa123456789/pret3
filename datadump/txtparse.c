#include "proto.h"

char * parse_buffer (const unsigned char * buffer, unsigned buffer_length) {
  unsigned char shift_state = 0;
  unsigned char prev_value;
  char * result = malloc(1);
  *result = 0;
  unsigned length = 0;
  unsigned pos;
  char temp[8];
  for (pos = 0; pos < buffer_length; pos ++)
    switch (shift_state) {
      case 0:
        if (text_table[buffer[pos]])
          concatenate(&result, &length, text_table[buffer[pos]], NULL);
        else
          shift_state = buffer[pos];
        break;
      case 1:
      case 2:
      case 3:
        shift_state --;
        sprintf(temp, "%hhu", buffer[pos]);
        concatenate(&result, &length, "{", shift_state[(const char * []) {"COLOR", "HIGHLIGHT", "SHADOW"}], " ",
                    (buffer[pos] > 15) ? temp : colors[buffer[pos]], "}", NULL);
        shift_state = 0;
        break;
      case 4:
        prev_value = buffer[pos];
        shift_state = 5;
        break;
      case 5:
        concatenate(&result, &length, "{COLOR_HIGHLIGHT_SHADOW ", (prev_value > 15) ? temp : colors[prev_value], " ",
                    (buffer[pos] > 15) ? temp : colors[buffer[pos]], "}", NULL);
        shift_state = 0;
        break;
      case 6:
        concatenate(&result, &length, buffer[pos] ? "{BIG}" : "{SMALL}", NULL);
        shift_state = 0;
        break;
      case 8:
        sprintf(temp, "%hhu", buffer[pos]);
        concatenate(&result, &length, "{PAUSE ", temp, "}", NULL);
        shift_state = 0;
        break;
      case 11:
        prev_value = buffer[pos];
        shift_state = 111;
        break;
      case 111:
        sprintf(temp, "%02hhX%02hhX", buffer[pos], prev_value);
        concatenate(&result, &length, "{PLAY_BGM 0x", temp, "}", NULL);
        shift_state = 0;
        break;
      case 12:
        if (buffer[pos] < 250)
          concatenate(&result, &length, text_table[buffer[pos]], NULL);
        else if (buffer[pos] == 250)
          concatenate(&result, &length, "\xe2\x9e\xa1", NULL);
        else if (buffer[pos] == 251)
          concatenate(&result, &length, "+", NULL);
        shift_state = 0;
        break;
      case 13:
        sprintf(temp, "%hhu", buffer[pos]);
        concatenate(&result, &length, "{SHIFT_TEXT ", temp, "}", NULL);
        shift_state = 0;
        break;
      case 16:
        prev_value = buffer[pos];
        shift_state = 116;
        break;
      case 116:
        sprintf(temp, "%02hhX%02hhX", buffer[pos], prev_value);
        concatenate(&result, &length, "{PLAY_SE 0x", temp, "}", NULL);
        shift_state = 0;
        break;
      case 0xf7:
        sprintf(temp, "%hhu", buffer[pos]);
        concatenate(&result, &length, "{DYNAMIC ", temp, "}", NULL);
        shift_state = 0;
        break;
      case 0xf8:
        concatenate(&result, &length, "{", joypad_buttons[buffer[pos]], "}", NULL);
        shift_state = 0;
        break;
      case 0xf9:
        concatenate(&result, &length, "{", buffer[pos] < 0xD0 ? extra_symbols[buffer[pos]] : emoji[buffer[pos] - 0xD0], "}", NULL);
        shift_state = 0;
        break;
      case 0xfc:
        switch (buffer[pos]) {
          case 1: case 2: case 3: case 4: case 6: case 8: case 12: case 13: case 16:
            shift_state = buffer[pos];
            break;
          case 0:
            concatenate(&result, &length, "{NAME_END}", NULL);
            break;
          case 9:
            concatenate(&result, &length, "{PAUSE_UNTIL_PRESS}", NULL);
            break;
          case 21: case 22: case 23: case 24:
            concatenate(&result, &length, ((const char * []) {"{ENLARGE}", "{SET_TO_DEFAULT_SIZE}", "{PAUSE_MUSIC}", "{RESUME_MUSIC}"})[buffer[pos] - 21], NULL);
            break;
          default:
            sprintf(temp, "%hhu", buffer[pos]);
            concatenate(&result, &length, "{UNKNOWN ", temp, "}", NULL);
            break;
        }
        if (shift_state == 0xfc) shift_state = 0;
        break;
      case 0xfd:
        if ((buffer[pos] < 16) && buffers[buffer[pos]])
          concatenate(&result, &length, "{", buffers[buffer[pos]], "}", NULL);
        else {
          sprintf(temp, "%hhu", buffer[pos]);
          concatenate(&result, &length, "{STRING ", temp, "}", NULL);
        }
        shift_state = 0;
        break;
    }
  if (shift_state) {
    free(result);
    return NULL;
  }
  result = realloc(result, length + 1);
  result[length] = 0;
  return result;
}
