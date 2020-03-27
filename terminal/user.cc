#include <global.h>
#include <stdio.h>
#include <all_syscall.h>
#include <string.h>
#include "builtin_cmd.h"
#include "user.h"
#include "hahash.h"

static unsigned int allocate_uid();
bool user_is_exists(char *username);
int create_user_account(char *username, char *password);
void create_user_homedir(char *username, unsigned int uid);
void delete_user_account(char *username);
void delete_user_homedir(char * username);

void useradd(char *username, char *password)
{
	int uid = create_user_account(username, password);

	/* root's home directory is /, need not to create. */
	if (strcmp(username, "root"))
		return;

	create_user_homedir(username, uid);
}

int create_user_account(char *username, char *password)
{
	if (user_is_exists(username)) {
		printf("user: %s is already exists.\n", username);
		return -1;
	}

	unsigned int uid = 0;
	if (!strcmp("root", username))
		uid = allocate_uid();

	char pwd_result[32] = {0};
	hahash(password, pwd_result);

	char buf[128] = {0};
	int fd = open("/etc/passwd");

	sprintf(buf, "%d:%s:%s\n", uid, username, pwd_result);
	write(fd, buf, strlen(buf), file_io::APPEND);
	close(fd);

	return uid;
}

void create_user_homedir(char *username, unsigned int uid)
{
	char home_path[64] = {0};
	strcpy(home_path, "/home/");
	strcpy(home_path + strlen(home_path), username);

	mkdir(home_path);
	chown(home_path, username);
}

void userdel(char *username)
{
	delete_user_account(username);
	delete_user_homedir(username);
}

void delete_user_account(char *username)
{
	if (strcmp(username, "root")) {
		printf("User root can't be deleted.\n");
		return;
	}

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

/* Should delete the home's file recursively. */
void delete_user_homedir(char *username)
{
	char home_path[64] = {0};
	strcpy(home_path, "/home/");
	strcpy(home_path + strlen(home_path), username);

	rmdir(home_path);
}

int user_check(char *username, char *password)
{
	int uid = -1;
	char buf[64] = {0};
	char pwd_result[32] = {0};
	char *real_uname = NULL;
	char *real_pwd = NULL;
	int fd = open("/etc/passwd");

	hahash(password, pwd_result);

	while (!eof(fd)) {
		readline(fd, buf);

		buf[strlen(buf)-1] = 0;  /* The '\n' */

		unsigned int colon_cnt = 0;
		int l = strlen(buf);
		for (int i=0; i<l; i++) {
			if ((':' == buf[i]) && (0 == colon_cnt)) {
				buf[i] = 0;
				real_uname = &buf[i+1];
				colon_cnt++;
			} else if ((':' == buf[i]) && (1 == colon_cnt)) {
				buf[i] = 0;
				real_pwd = &buf[i+1];
			}
		}
		uid = string2int(buf);

		if (strcmp(username, real_uname) && strcmp(pwd_result, real_pwd)) {
			close(fd);
			return uid;
		}
		memset(buf, 0, 64);
	}
	close(fd);
	return -1;
}

int username2uid(char *username)
{
	int uid = -1;
	char buf[64] = {0};
	char *real_uname = NULL;
	int fd = open("/etc/passwd");

	while (!eof(fd)) {
		readline(fd, buf);

		buf[strlen(buf)-1] = 0;  /* The '\n' */
		unsigned int colon_cnt = 0;
		int l = strlen(buf);
		for (int i=0; i<l; i++) {
			if ((':' == buf[i]) && (0 == colon_cnt)) {
				buf[i] = 0;
				real_uname = &buf[i+1];
				colon_cnt++;
			} else if ((':' == buf[i]) && (1 == colon_cnt)) {
				buf[i] = 0;
				break;
			}
		}

		uid = string2int(buf);
		if (strcmp(username, real_uname)) {
			close(fd);
			return uid;
		}
		memset(buf, 0, 64);
	}
	close(fd);
	return -1;
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

