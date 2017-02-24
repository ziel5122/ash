#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define INPUT_BUFFER_SIZE 1024;
#define TOKEN_BUFFER_SIZE 64;

char *EXIT = "exit";
char *DELIMITER = " \t\r\n\a";
char *PROMPT = "ash> ";

char *growBuffer1(char *buffer, int buf_size);

char **growBuffer2(char **buffer, int buf_size);
char **tokenize(char *input);

void loop(void);

int main(int argc, char *argv[]) {
	//load config files

	loop();	

	//perform any shutdown/cleanup

	return 0;
}

int execute(char **tokens) {
	if (tokens[0] == NULL) return 1;
	if (strcmp(tokens[0], EXIT) == 0) exit(0);

	int status;
	pid_t pid = fork(), wpid;

	if (pid == 0) { //child process
		if (execvp(tokens[0], tokens) == -1) {
			perror("ash");
		}
		exit(1);
	} else if (pid < 0) { //error forking
		perror("ash");
	} else {
		int wc = wait(NULL);
/*
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
*/
	}

	return 1;
}

char *getInput(void) {
	char *buffer = malloc(1);
	int buf_size = 0, c, position = 0;
	
	printf("%s", PROMPT);
	
	while (1) {
		if (position >= buf_size) {
			buf_size += INPUT_BUFFER_SIZE;
			buffer = growBuffer1(buffer, buf_size);
		}

		c = getchar();
		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}

		position++;
	}
}

char *growBuffer1(char *buffer, int buf_size) {
	buffer = realloc(buffer, buf_size);
	if (!buffer) {
		fprintf(stderr, "ash: allocation error\n");
		exit(1);
	}
	return buffer;
}

char **growBuffer2(char **buffer, int buf_size) {
	buffer = realloc(buffer, buf_size);
	if (!buffer) {
		fprintf(stderr, "ash: allocation error\n");
		exit(1);
	}
	return buffer;
}

void loop(void) {
	char *input;
	char **tokens;
	int status;

	status = 1;

	do {
		input = getInput();
		tokens = tokenize(input);
		status = execute(tokens);
	
		free(input);
		free(tokens);
	} while (status);
}

char **tokenize(char *input) {
	char *token;
	char **tokens = malloc(sizeof(char*));
	int buf_size = 0, position = 0;

	token = strtok(input, DELIMITER);
	while (token != NULL) {
		if (position >= buf_size) {
			buf_size += TOKEN_BUFFER_SIZE;
			tokens = growBuffer2(tokens, buf_size);
		}

		tokens[position] = token;
		position++;	
		token = strtok(NULL, DELIMITER);
	}	
	tokens[position] = NULL;
	
	return tokens;
}	
