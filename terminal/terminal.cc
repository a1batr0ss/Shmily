#include <stdio.h>
#include <string.h>

char cur_user[64];
char cur_dir[64];

void print_shell();

void init_terminal()
{
	strcpy(cur_user, "shmily");
	strcpy(cur_dir, "~");  /* tentatively, it should get from filesystem by message-passing. */

	print_shell();

	return;
}

void print_shell()
{
	printf("%s@localhost:%s$", cur_user, cur_dir);
}

void handler_input(char *line)
{
	printf("get a line %s\n", line);
}

