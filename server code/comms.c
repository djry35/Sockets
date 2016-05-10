//bonus TODO:
//broadcast to other users the logout and login
//extra checks for login to see if they're in already
//send msg to specific user
#include "sockets.h"

int login(msg_t msg, msg_t* retMsg)
{
	FILE* fp = fopen("accts.txt", "r");
	if(!fp)
	{
		printf("File read error\n");
		retMsg->type = ERROR;
		return -1;
	}
	
	char username[33];
	char password[9];
	while(1)
	{
		fscanf(fp, "%s %s", username, password);
		if(feof(fp))
		{
			printf("Invalid credentials.\n");
			retMsg->type = ERROR;
			strcpy(retMsg->arg0, "ERROR");
			fclose(fp);
			return -1;
		}
		if(strcmp(username, msg.arg1) == 0 && strcmp(password, msg.arg2) == 0)
		{
			fclose(fp);
			retMsg->type = LOGIN;
			strcpy(retMsg->arg0, "OK");
			strcpy(retMsg->arg1, username);
			strcpy(retMsg->arg2, password);
			strcpy(retMsg->user, username);
			return 0;
		}
	}
	//uhhhhh
	return -2;
}

int sendMsg(msg_t msg, msg_t* retMsg)
{
	retMsg->type = SEND;
	strcpy(retMsg->arg0, "OK");
	strcpy(retMsg->arg1, msg.arg1);
	strcpy(retMsg->user, msg.user);
	return 0;
}

int newUser(msg_t msg, msg_t* retMsg)
{
	FILE* fp = fopen("accts.txt", "r");
	if(!fp)
	{
		printf("File read error\n");
		retMsg->type = ERROR;
		return -1;
	}
	
	char username[33];
	char password[9];
	while(1)
	{
		fscanf(fp, "%s %s", username, password);
		if(feof(fp))
		{
			fclose(fp);
			break;
		}
		if(strcasecmp(username, msg.arg1) == 0)
		{
			printf("Error: user exists.\n");
			retMsg->type = ERROR;
			strcpy(retMsg->arg0, "ERROR");
			fclose(fp);
			return -1;
		}
	}
	//requires a little maneuvering to get this right.
	fp = fopen("accts.txt", "a");
	if(!fp)
	{
		printf("file didn't open\n");
		retMsg->type = ERROR;
		return -1;
	}
	printf("args: %s %s\n", msg.arg1, msg.arg2);
	if(fprintf(fp, "%s %s\n", msg.arg1, msg.arg2) < 0)
	{
		printf("error writing.\n");
		retMsg->type = ERROR;
		return -1;
	}
	retMsg->type = LOGIN;
	strcpy(retMsg->arg0, "OK");
	strcpy(retMsg->arg1, msg.arg1);
	strcpy(retMsg->arg2, msg.arg2);
	strcpy(retMsg->user, msg.arg1);
	fclose(fp);
	return 0;
}

int logout(msg_t msg, msg_t* retMsg)
{
	//log?
	retMsg->type = LOGOUT;
	strcpy(retMsg->arg0, "OK");
	strcpy(retMsg->arg1, msg.arg1);
	strcpy(retMsg->user, msg.user);
	return 0;
}
