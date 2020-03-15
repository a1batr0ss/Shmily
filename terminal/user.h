#ifndef __TERMINAL_USER_H__
#define __TERMINAL_USER_H__

struct user {
	int uid;
	char username[32];
};

int user_check(char *username, char *password);
void useradd(char *username, char *password);
void userdel(char *username);

#endif

