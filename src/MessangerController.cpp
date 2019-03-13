#include "..\inc\MessangerController.h"
#include "..\inc\Message.h"
#include <exception>
#include <string.h>
#include <stdio.h>

MessangerController::MessangerController(const char * domain, const char * port)
{
	this->username = nullptr;
	this->messageBufferer = new MessageBufferer(domain, port);
	this->fd = nullptr;
	this->messageOrder = nullptr;
}

MessangerController::~MessangerController()
{
	delete this->messageBufferer;
}

bool MessangerController::login(const char * login, const char * password)
{
	this->username = login;

	//Sending request to the server
	Message msg(this->A_LOGIN, this->username, (BYTE*)password, (UINT)strlen(password));
	this->messageBufferer->sendMessage(&msg, true);

	bool logged;
	while (!this->hasLoginMessage(&logged));

	return strcmp(msg.getAction(), this->A_LOGIN) == 0
		&& strcmp(msg.getSign(), this->username) == 0
		&& logged;
}

void MessangerController::logout()
{
	//Sending request to the server
	Message msg(this->A_LOGOUT, this->username, 0, 0);
	this->messageBufferer->sendMessage(&msg, true);

	this->username = nullptr;
}

void MessangerController::uploadFile(const char * toUser, const char * filename)
{
	this->fd = new FileData();
	int err = fopen_s(&this->fd->descFile, filename, "rb");
	if (err) throw new std::exception("open file error");

	try
	{
		const long filesize = (fseek(this->fd->descFile, 0, SEEK_END), ftell(this->fd->descFile));
		rewind(this->fd->descFile);

		this->fd->framesSent = 0;
		this->fd->framesTotal = filesize / MAX_FRAME_SIZE;
		if (filesize % MAX_FRAME_SIZE) this->fd->framesTotal++;

		int frameCountToRead = this->fd->framesTotal > 1 ? MAX_FRAME_SIZE : filesize;
		long leftToRead = filesize;

		BYTE* buffer = (BYTE*)malloc(MAX_BUFFER_SIZE);
		if (!buffer) throw new std::exception("malloc fail");
		try
		{
			// Sending file header
			UINT filenamePos = (UINT)strlen(filename);
			char c;
			while ((c = *(filename + --filenamePos) != '/' || c != '\\') && filenamePos);
			filenamePos++;
			Message msg(this->A_FILE_START, this->username, (BYTE*)filename, (UINT)strlen(filename));
			this->messageBufferer->sendMessage(&msg);

			// Sending file body
			while (this->fd->framesSent < this->fd->framesTotal)
			{
				UINT read = (UINT)fread_s(buffer, MAX_BUFFER_SIZE, sizeof(BYTE), frameCountToRead, this->fd->descFile);
				if (read != frameCountToRead) throw new std::exception("file read less then should be");

				Message msg(this->A_FILE_PART, this->username, buffer, frameCountToRead);
				this->messageBufferer->sendMessage(&msg);
				this->fd->framesSent++;

				leftToRead -= frameCountToRead;
				frameCountToRead -= leftToRead % MAX_BUFFER_SIZE;
			}

			// Sending file footer
			msg.to(this->A_FILE_END, this->username, (BYTE*)&filesize, sizeof(filesize));
			this->messageBufferer->sendMessage(&msg);

		}
		catch (...)
		{
			free(buffer);
			throw;
		}

		free(buffer);
		this->messageBufferer->send();
	}
	catch (...)
	{
		fclose(this->fd->descFile);
		delete this->fd;
		this->fd = nullptr;
		throw;
	}

	fclose(this->fd->descFile);
	delete this->fd;
	this->fd = nullptr;
}

void MessangerController::downloadFile(const char * filename, const char * pathToSave)
{
	Message msg(this->A_FILE_GET, this->username, (BYTE*)filename, (UINT)strlen(filename));
	this->messageBufferer->sendMessage(&msg, true);

	while (this->hasFileStarted());

	this->fd = new FileData();
	int err = fopen_s(&this->fd->descFile, pathToSave, "wb");
	if (err) throw new std::exception("open file error");

	const BYTE* buffer;
	UINT bufferLength;
	while (buffer = this->getFilePart(&bufferLength))
	{
		UINT wrote = (UINT)fwrite((void*)buffer, sizeof(BYTE), bufferLength, this->fd->descFile);
		if (wrote != bufferLength) throw new std::exception("failed to write file");
	}
}

void MessangerController::getFileStatus(PFileStatus pFDStruct)
{
	if (this->fd)
	{
		pFDStruct->framesTotal = this->fd->framesTotal;
		pFDStruct->framesSent = this->fd->framesSent;
	}
	else
	{
		pFDStruct->framesTotal = pFDStruct->framesSent = 0;
	}
}

void MessangerController::sendMessage(const char * toUser, const char * text)
{
	Message msg(this->A_MESSAGE, toUser, (BYTE*)text, (UINT)strlen(text));
	this->messageBufferer->sendMessage(&msg, true);
}

bool MessangerController::update()
{
	this->messageBufferer->recieve();
	Message msg;
	MessageNode* localMessageOrder = nullptr;;
	while (this->messageBufferer->tryMessage(&msg))
	{
		// Add messages to the local order
		MessageNode *tempMessageNode = new MessageNode();
		tempMessageNode->next = nullptr;
		tempMessageNode->message = new Message(msg);
		if (localMessageOrder)
		{
			MessageNode* itr = localMessageOrder;
			while (itr->next) itr = itr->next;
			itr->next = tempMessageNode;
		}
		else
		{
			localMessageOrder = tempMessageNode;
		}
	}

	// Add local message order to main order
	MessageNode* itr = this->messageOrder;
	if (itr)
	{
		while (itr->next) itr = itr->next;
		itr->next = localMessageOrder;
	}
	else
	{
		this->messageOrder = localMessageOrder;
	}
	return true;
}

bool MessangerController::hasMessage()
{
	PMessageNode tml = this->messageOrder;
	while (tml)
	{
		if (strcmp(tml->message->getAction(), this->A_MESSAGE) == 0) return true;
		tml = tml->next;
	}
	return false;
}

bool MessangerController::hasFile()
{
	PMessageNode tml = this->messageOrder;
	while (tml)
	{
		if (strcmp(tml->message->getAction(), this->A_FILE) == 0) return true;
		tml = tml->next;
	}
	return false;
}

void MessangerController::getMessage(char* fromUser, int fromUserLength, char* text, int textLength)
{
	PMessageNode tml = this->messageOrder;
	PMessageNode prevNode = nullptr;
	while (tml)
	{
		if (strcmp(tml->message->getAction(), this->A_MESSAGE) == 0)
		{
			strcpy_s(fromUser, fromUserLength, tml->message->getSign());
			strcpy_s(text, textLength, (char*)tml->message->getContent());
			if (prevNode)
			{
				prevNode->next = tml->next;
			}
			else
			{
				this->messageOrder = this->messageOrder->next;
			}
			delete tml->message;
			delete tml;
			break;
		}
		prevNode = tml;
		tml = tml->next;
	}
}

void MessangerController::getFile(char * filename, int filenameLength, int* filesize)
{

}

bool MessangerController::hasLoginMessage(bool* logged)
{
	PMessageNode tml = this->messageOrder;
	while (tml)
	{
		if (strcmp(tml->message->getAction(), this->A_LOGIN) == 0)
		{
			*logged = tml->message->getContent();
			return true;
		}
		tml = tml->next;
	}
	return false;
}

bool MessangerController::hasFileStarted()
{
	PMessageNode tml = this->messageOrder;
	while (tml)
	{
		if (strcmp(tml->message->getAction(), this->A_FILE_START) == 0)
		{
			return true;
		}
		tml = tml->next;
	}
	return false;
}

const BYTE* MessangerController::getFilePart(UINT* length)
{
	const BYTE* retVal = nullptr;
	PMessageNode tml = this->messageOrder;
	PMessageNode prevNode = nullptr;
	while (tml)
	{
		if (strcmp(tml->message->getAction(), this->A_FILE_PART) == 0)
		{
			retVal = tml->message->getContent();
			*length = tml->message->getContentLength();
			if (prevNode)
			{
				prevNode->next = tml->next;
			}
			else
			{
				this->messageOrder = this->messageOrder->next;
			}
			//delete tml->message;
			delete tml;
			break;
		}
		prevNode = tml;
		tml = tml->next;
	}
	return retVal;
}
