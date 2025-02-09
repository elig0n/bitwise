/* Copyright 2019
 * Ramon Fried <ramon.fried@gmail.com>
 */
#include <string.h>
#include "bitwise.h"
#include "shunting-yard.h"

#define MAX_TOKENS 4

static int cmd_clear(char **argv, int argc);
static int cmd_set_width(char **argv, int argc);

struct cmd {
	const char *name;
	int min_args;
	int max_args;
	int (*func)(char **argv, int argc);
};

static struct cmd cmds[] = {
	{"clear", 0, 0, cmd_clear},
	{"width", 1, 1, cmd_set_width},
};

static int get_cmd(const char *cmd_name)
{
	int i = 0;

	for (i = 0; i < ARRAY_SIZE(cmds); i++) {
		LOG("comparing %s to command %s\n", cmd_name, cmds[i].name);
		if (!strncmp(cmds[i].name, cmd_name, strlen(cmds[i].name)))
			return i;
	}

	return -1;
}

void show_error(Status status)
{
	char *message = NULL;

	switch (status) {
	case ERROR_SYNTAX:
		message = "Syntax error";
		break;
	case ERROR_OPEN_PARENTHESIS:
		message = "Missing parenthesis";
		break;
	case ERROR_CLOSE_PARENTHESIS:
		message = "Extra parenthesis";
		break;
	case ERROR_UNRECOGNIZED:
		message = "Unknown character";
		break;
	case ERROR_NO_INPUT:
		message = "Empty expression";
		break;
	case ERROR_UNDEFINED_FUNCTION:
		message = "Unknown function";
		break;
	case ERROR_FUNCTION_ARGUMENTS:
		message = "Missing function arguments";
		break;
	case ERROR_UNDEFINED_CONSTANT:
		message = "Unknown constant";
		break;
	default:
		message = "Unknown error";
	}

	werase(cmd_win);
	mvwprintw(cmd_win, 0, 0, "%s", message);
	append_to_history(message, TYPE_OUTPUT_ERROR);
	wrefresh(cmd_win);
}

static int parse_cmd(char *cmdline)
{
	static char *tokens[MAX_TOKENS];
	int cmd_entry;
	int i = 0;
	int rc;
	uint64_t result;

	LOG("got command: %s\n", cmdline);

	/* First let's see if this is a command */
	cmd_entry = get_cmd(cmdline);
	if (cmd_entry >= 0) {
		/* It looks like a command, let's tokenize this */
		append_to_history(cmdline, TYPE_INPUT_COMMAND);

		tokens[i] = strtok(cmdline, " ");
		LOG("%s\n", tokens[i]);
		do {
			i++;
			tokens[i] = strtok(NULL, " ");
			LOG("%s\n", tokens[i]);
		} while (tokens[i] != NULL && (i) < MAX_TOKENS);

		LOG("Finished tokenizing %d tokens\n", i);

		if ((i - 1 >= cmds[cmd_entry].min_args) &&
		    (i - 1 <= cmds[cmd_entry].max_args))
			rc = cmds[cmd_entry].func(&tokens[1], i - 1);
		else {
			werase(cmd_win);
			mvwprintw(cmd_win, 0, 0, "%s: Invalid parameter(s)", tokens[0]);
			wrefresh(cmd_win);
			LOG("Invalid parameters\n");
			return -1;
		}
	} else {
		append_to_history(cmdline, TYPE_INPUT_EXPRESSION);
		Status status = shunting_yard(cmdline, &result);
		if (status != OK) {
			show_error(status);
			return -1;
		} else {
			char result_string[32];

			g_val = result;
			sprintf(result_string, "= 0x%lx", result);
			update_binary();
			update_fields(-1);
			append_to_history(result_string, TYPE_OUTPUT_RESULT);
		}
	}

	return 0;
}

static int readline_input_avail(void)
{
	return g_input_avail;
}

static int readline_getc(FILE *dummy)
{
	g_input_avail = false;
	return g_input;
}

static void got_command(char *line)
{
	int rc;
	/* Handle exit commands immediately */
	if (!line) {
		/* Handle CTL-D */
		g_leave_req = true;
		return;
	}

	if (strcmp("q", line) == 0)
		g_leave_req = true;

	if (*line)
		add_history(line);

	rc = parse_cmd(line);
	free(line);

	active_win = last_win;
	if (active_win == FIELDS_WIN) {
		curs_set(1);
		set_active_field(false);
		wrefresh(fields_win);

	} else if (active_win == BINARY_WIN) {
		position_binary_curser(0, bit_pos);
		curs_set(0);
		wrefresh(binary_win);
	}

	if (!rc) {
		keypad(stdscr, FALSE);
		werase(cmd_win);
		wrefresh(cmd_win);
	}

	return;
}

void readline_redisplay(void)
{
	if (active_win != COMMAND_WIN)
		return;

	werase(cmd_win);
	mvwprintw(cmd_win, 0, 0, "%s%s", rl_display_prompt, rl_line_buffer);
	wmove(cmd_win, 0, rl_point + 1);
	wrefresh(cmd_win);
}


void init_readline(void)
{
	rl_catch_signals = 0;
	rl_catch_sigwinch = 0;
	rl_deprep_term_function = NULL;
	rl_prep_term_function = NULL;
	rl_change_environment = 0;
	rl_getc_function = readline_getc;
	rl_input_available_hook = readline_input_avail;
	rl_redisplay_function = readline_redisplay;
	rl_bind_key('\t', rl_insert);
	rl_callback_handler_install(":", got_command);

}

void deinit_readline(void)
{
	rl_callback_handler_remove();
}

void process_cmd(int ch)
{
	int new_char;

	LOG("Process cmd %u\n", ch);

	if (ch == 27) {
		nodelay(get_win(active_win), true);
		new_char = wgetch(get_win(active_win));
		nodelay(get_win(active_win), false);

		if (new_char == ERR) {
			LOG("IT's a real escape\n");
			active_win = last_win;
			if (active_win == FIELDS_WIN) {
				curs_set(1);
				set_active_field(false);
				wrefresh(fields_win);
			} else if (active_win == BINARY_WIN) {
				curs_set(0);
				position_binary_curser(0, bit_pos);
				wrefresh(binary_win);
			}
			keypad(stdscr, FALSE);
			werase(cmd_win);
			wrefresh(cmd_win);
			return;
		} else
			ungetch(new_char);

	}

	/* If TAB || BACKSPACE (to -1) */
	if (ch == '\t' || (ch == 127 && !rl_point)) {
		LOG("Detected exit cmd\n");
		active_win = last_win;
		if (active_win == FIELDS_WIN) {
			curs_set(1);
			set_active_field(false);
			wrefresh(fields_win);
		} else if (active_win == BINARY_WIN) {
			curs_set(0);
			position_binary_curser(0, bit_pos);
			wrefresh(binary_win);
		}
		keypad(stdscr, FALSE);
		werase(cmd_win);
		wrefresh(cmd_win);
		return;
	}

	g_input = ch;
	g_input_avail = true;
	rl_callback_read_char();
}

static int cmd_clear(char **argv, int argc)
{
	flush_history();
	update_history_win();
	return 0;
}

static int cmd_set_width(char **argv, int argc)
{
	LOG("cmd_set_width: argc %d\n", argc);
	if (!strcmp("64", argv[0]))
		set_fields_width(64);
	else if (!strcmp("32", argv[0]))
		set_fields_width(32);
	else if (!strcmp("16", argv[0]))
		set_fields_width(16);
	else if (!strcmp("8", argv[0]))
		set_fields_width(8);
	else
		return -1;

	unpaint_screen();
	paint_screen();

	return 0;
}

