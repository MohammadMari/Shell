#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


#define INPUT_SIZE 512
char PATH[255];
char ShellName[255] = { "Shell" };

char historyCommands[20][INPUT_SIZE]; // history of last 20 commands inputted by user.
int numHistoryCommands = 0;

bool processCommand(char* command);


// provides array of command, and its arguments
// returns number of commands.

void commandArg(char* command, int* charCounterPtr, int* argCounterPtr, char** args)
{
	int argCounter = *argCounterPtr;
	int charCounter = *charCounterPtr;
	bool passedCommand = false;

	if (!args[argCounter])
		args[argCounter] = malloc(sizeof(char) * INPUT_SIZE);
	//seperate each word
	for (int i = 0; i < strlen(command) + 1; i++)
	{
		if (command[i] == ' ' || command[i] == '\0') //either there is a space or a null thingy.
		{
			charCounter = 0;
			argCounter++;
			args[argCounter] = malloc(sizeof(char) * INPUT_SIZE);

			if (command[i] == ' ') // if its a space we continue.
				continue;
			else if (command[i] == '\0') // if there are no more characters to read, we move on
				break;
		}

		args[argCounter][charCounter] = command[i];
		charCounter++;
	}

	*argCounterPtr = argCounter;
	*charCounterPtr = charCounter;
}

void execCommand(char* command)
{
	int pid = 0;
	//int pid = fork();

	if (pid == -1)
		return;

	if (pid == 0)
	{
		int argCounter = 0; // Number of arguments + command
		int charCounter = 0; // Temporary counter

		//make an array of both the command and all its args
		char* args[INPUT_SIZE];
		args[argCounter] = malloc(sizeof(char) * INPUT_SIZE);
		//path
		strncpy(args[argCounter], "/bin/", 5);
		charCounter = 5;

		commandArg(command, &charCounter, &argCounter, args);

		// execute command
		args[argCounter] = NULL;
		execv(args[0], args);

		// free memory
		for (int i = 0; i < argCounter; i++)
			free(args[i]);
	}
	else
	{
		printf("wait");
		wait(NULL);
	}
}

void ChangeDirectory(char* command)
{
	char newDir[INPUT_SIZE];
	int charCounter = 0;
	int dirSize = 0;
	bool passedCD = false;

	getcwd(newDir, INPUT_SIZE);
	charCounter = dirSize = strlen(newDir);
	for (int i = 0; i < strlen(command); i++)
	{
		if (!passedCD)
		{
			if (command[i] == 'c' && command[i + 1] == 'd')
			{
				passedCD = true;
				i++;
			}
			continue;
		}

		if (command[i] == '\0')
			break;
		newDir[charCounter] = command[i];
		charCounter++;
	}


	newDir[charCounter] = '\0';
	newDir[strcspn(newDir, " ")] = '/';

	if ((charCounter - dirSize) < 1)
	{
		do
		{
			newDir[strlen(newDir) - 1] = 0;
		} while (newDir[strlen(newDir) - 1] != '/');
	}

	chdir(newDir);
}

void myHistory(char* command)
{
	char* args[INPUT_SIZE];
	int argCounter = 0; // Number of arguments + command
	int charCounter = 0; // Temporary counter

	commandArg(command, &charCounter, &argCounter, args);

	if (argCounter > 1)
	{
		for (int i = 1; i < argCounter; i++)
		{
			if (strstr(args[i], "-c"))
			{
				for (int i = 0; i < 20; i++)
				{
					memset(historyCommands[i], 0, sizeof(historyCommands[i]));
					numHistoryCommands = 0;
				}

				return;
			}
			else if (strstr(args[i], "-e"))
			{
				if ((i + 1) >= argCounter)
				{
					printf("Enter number\n");
					return;

				}

				int commandToExecute = atoi(args[i + 1]);
				processCommand(historyCommands[commandToExecute]);
				return;
			}
		}
	}

	int numCommandLoop = 20 < numHistoryCommands ? 20 : numHistoryCommands;

	for (int i = 0; i < numCommandLoop; i++)
		printf("Command %d: %s\n", i + 1, historyCommands[i]);
}

//returns quit
bool processCommand(char* command)
{
	if (strstr(command, "exit"))
		return 1;
	else if (strstr(command, "cd") != 0)
		ChangeDirectory(command);
	else if (strstr(command, "myhistory"))
		myHistory(command);
	else
		execCommand(command);

	return false;
}



// returns back a list of commands
//example:
/*
input: "ls -l; quit"
returns {"ls -l", "quit"}
*/
int processInput(char* input, char** commandList)
{
	int numCommands = 0;
	char* tempCommand = (char*)malloc(sizeof(char) * INPUT_SIZE + 1);
	int tempCounter = 0;
	int spaceCounter = 0;

	for (int i = 0; i < INPUT_SIZE - 1; i++)
	{
		if (input[i] == ';' || input[i] == '\n')
		{
			// we hit the end of a command, now we want to add it to our command list, increment number of commands, and make new space for the next command,
			tempCommand[tempCounter] = '\0';
			commandList[numCommands] = tempCommand;
			numCommands++;
			tempCounter = 0;
			tempCommand = (char*)malloc(sizeof(char) * INPUT_SIZE + 1); //allocate new memory

			if (input[i] == ';')
				continue;
			else
			{
				break;
				free(tempCommand);
			}
		}

		if (i > 0) //ive noticed in the batchfiles provided, there are lots of spaces between semi colons, this is here to fix that.
		{
			if (input[i] == ' ' && (input[i - 1] == ';' || input[i + 1] == ';'))
				continue;
		}
		else if (input[i] == ' ') // space in the beginning, we fix that.
		{
			continue;
		}

		//add letter to string.
		tempCommand[tempCounter] = input[i];
		tempCounter++;
	}


	return numCommands;
}

int main(int argc, char** argv)
{
	char* input; //user input
	char* commandList[20]; // num of commands on current line, ex. exit, cat file1 = 2 commands
	int numCommands = 0; // number of commands

	bool run = true; //used for exit
	bool interactive = argc <= 1; // interactive mode?

	if (interactive)
	{
		//		printf("Enter shell name: ");
		//		fgets(ShellName, 255, stdin);
		//		ShellName[strcspn(ShellName, "\n")] = 0;
	}

	while (run) {

		if (interactive)
		{
			input = (char*)malloc(sizeof(char) * INPUT_SIZE);
			printf("%s: ", ShellName);
			fgets(input, INPUT_SIZE, stdin);
			numCommands = processInput(input, commandList);
		}
		else
		{
			run = false;
			for (int i = 1; i < argc; i++)
			{
				input = argv[i];
				numCommands = processInput(input, commandList);
			}
		}

		for (int i = 0; i < numCommands; i++)
		{
			strcpy(historyCommands[numHistoryCommands % 20], commandList[i]);
			numHistoryCommands++;

			printf("%d\n", numCommands);

			if (processCommand(commandList[i]))
			{
				run = false;
				printf("wa");
			}
			printf("Run: %d\n", getppid());
			free(commandList[i]);
		}


		if (!run)
		{
			exit(EXIT_SUCCESS);
		}

	}

	return 0;
}
