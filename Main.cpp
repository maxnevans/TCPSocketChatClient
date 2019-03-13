#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <string.h>
#include <stdio.h>
#include <exception>
#include <windows.h>
#include "inc/MessangerController.h"

#define MSG_SIZE_BYTES 100000
#define BUFFER_SIZE_BYTES MSG_SIZE_BYTES

void clear()
{
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}

struct ThreadData
{
	MessangerController* mc;
};

DWORD WINAPI UpdaterThread(LPVOID lpParam)
{
	ThreadData* td = (ThreadData*)lpParam;

	while (td->mc->update())
	{
		if (td->mc->hasMessage())
		{
			char* text = (char*)malloc(MSG_SIZE_BYTES);
			char* from = (char*)malloc(MSG_SIZE_BYTES);
			td->mc->getMessage(from, MSG_SIZE_BYTES, text, MSG_SIZE_BYTES);
			printf("[%s]: %s\n", from, text);
			free(text);
			free(from);
		}
	}

	return 0;
}

int main()
{
	try
	{
		// Console Incoding
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);

		// Connecting to the server
		char serverName[256];
		char serverPort[256];
		printf("Server: ");
		scanf_s("%s %s", serverName, 256, serverPort, 256);
		MessangerController* mc = new MessangerController(serverName, serverPort);

		// Creating separate thread to handle update
		ThreadData td;
		td.mc = mc;
		HANDLE hCT = CreateThread(0, 0, UpdaterThread, &td, 0, 0);
		if (hCT == NULL) throw new std::exception("unable to create updater thread");

		// Signing in
		char username[256];
		char password[256];
		printf("Login and password: ");
		scanf_s("%s %s", username, 256, password, 256);
		bool logged = mc->login(username, password);
		if (!logged) throw new std::exception("failed to login");
		printf("Sign in successfully!\n");

		// Main cycle
		bool isDone = false;
		while (!isDone)
		{
			char message[MSG_SIZE_BYTES];
			scanf_s("%s", message, MSG_SIZE_BYTES);
			if (strcmp(message, "/exit") == 0)
			{
				isDone = true;
				TerminateThread(hCT, 0);
				CloseHandle(hCT);
				delete mc;
			}
			else if (strcmp(message, "/message") == 0)
			{
				printf("User: ");
				scanf_s("%s", message, MSG_SIZE_BYTES);
				char sendTo[MSG_SIZE_BYTES];
				strcpy_s(sendTo, MSG_SIZE_BYTES, message);
				printf("Message: ");
				clear();
				scanf_s("%[^\n]s", message, MSG_SIZE_BYTES);
				mc->sendMessage(sendTo, message);
			}
			else if (strcmp(message, "/upload") == 0)
			{
				printf("User: ");
				scanf_s("%s", message, MSG_SIZE_BYTES);
				char sendTo[MSG_SIZE_BYTES];
				strcpy_s(sendTo, MSG_SIZE_BYTES, message);
				printf("Choose file: ");
				clear();
				scanf_s("%[^\n]s", message, MSG_SIZE_BYTES);
				mc->uploadFile(sendTo, message);
			}
			else if (strcmp(message, "/download") == 0)
			{
				printf("File: ");
				scanf_s("%s", message, MSG_SIZE_BYTES);
				char pathToSave[MSG_SIZE_BYTES];
				printf("Where to save: ");
				scanf_s("%s", pathToSave, MSG_SIZE_BYTES);
				mc->downloadFile(message, pathToSave);
			}
		}
		
	}
	catch (std::exception* error)
	{
		printf("%s\n", error->what());
	}
	catch (...)
	{
		printf("Undefined error happend!\n");
	}
	system("pause");
	return 0;
}