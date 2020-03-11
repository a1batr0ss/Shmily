#include <global.h>
#include <stdio.h>
#include <all_syscall.h>
#include <string.h>
#include <builtin_cmd.h>
#include "user.h"

static unsigned int allocate_uid();
bool user_is_exists(char *username);

void useradd(char *username, char *password)
{
	if (user_is_exists(username)) {
		printf("user: %s is already exists.\n", username);
		return;
	}

	unsigned int uid = allocate_uid();
	char buf[128] = {0};
	int fd = open("/etc/passwd");

	sprintf(buf, "%d:%s:%s\n", uid, username, password);
	write(fd, buf, strlen(buf), file_io::APPEND);
	close(fd);
}

void userdel(char *username)
{
	if (!user_is_exists(username)) {
		printf("User: %s is not exists.\n", username);
		return;
	}

	mkfile("/etc/passwd_");

	char buf[128] = {0};
	char buf_will_write[128] = {0};
	char *real_uname = NULL;
	int fd_src = open("/etc/passwd");
	int fd_dst = open("/etc/passwd_");
	while (!eof(fd_src)) {
		memset(buf, 0, 128);
		memset(buf_will_write, 0, 128);
		readline(fd_src, buf);
		// printf("%s", buf);
		memcpy(buf_will_write, buf, strlen(buf));

		unsigned int colon_cnt = 0;
		for (int i=0; i<strlen(buf); i++) {
			if ((':' == buf[i]) && (0 == colon_cnt)) {
				real_uname = &buf[i+1];
				colon_cnt++;
			} else if ((':' == buf[i]) && (1 == colon_cnt)) {
				buf[i] = 0;
			}
		}

		if (strcmp(username, real_uname))
			continue;

		write(fd_dst, buf_will_write, strlen(buf_will_write), file_io::APPEND);
	}
	close(fd_src);
	close(fd_dst);

	rmfile("/etc/passwd");
	mv_file("/etc/passwd_", "/etc/passwd");
}

bool user_check(char *username, char *password)
{
	char buf[64] = {0};
	char *real_uname = NULL;
	char *real_pwd = NULL;
	int fd = open("/etc/passwd");

	while (!eof(fd)) {
		readline(fd, buf);

		buf[strlen(buf)-1] = 0;  /* The '\n' */

		unsigned int colon_cnt = 0;
		for (int i=0; i<strlen(buf); i++) {
			if ((':' == buf[i]) && (0 == colon_cnt)) {
				real_uname = &buf[i+1];
				colon_cnt++;
			} else if ((':' == buf[i]) && (1 == colon_cnt)) {
				buf[i] = 0;
				real_pwd = &buf[i+1];
			}
		}

		if (strcmp(username, real_uname) && strcmp(password, real_pwd)) {
			close(fd);
			return true;
		}
		memset(buf, 0, 64);
	}
	close(fd);
	return false;
}

bool user_is_exists(char *username)
{
	char buf[128] = {0};
	char *real_uname = NULL;
	int fd = open("/etc/passwd");

	while (!eof(fd)) {
		readline(fd, buf);

		/* Split the username */
		unsigned int colon_cnt = 0;
		for (int i=0; i<strlen(buf); i++) {
			if ((':' == buf[i]) && (0 == colon_cnt)) {
				real_uname = &buf[i+1];
				colon_cnt++;
			} else if ((':' == buf[i]) && (1 == colon_cnt)) {
				buf[i] = 0;
			}
		}

		if (strcmp(real_uname, username)) {
			close(fd);
			return true;
		}
		memset(buf, 0, 128);
	}
	close(fd);
	return false;
}

/* Bug: Should read from /etc/passwd to decide the next uid. */
static unsigned int allocate_uid()
{
	static unsigned int uid = 99;
	uid++;

	return uid;
}

