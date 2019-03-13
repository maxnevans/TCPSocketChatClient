#pragma once
#include "MessageBufferer.h"
#include "Message.h"
#include <stdio.h>

#define TO_ALL				"\x1\x2\x0"
#define MAX_FRAME_SIZE		0xFFFF
#define MAX_BUFFER_SIZE		MAX_FRAME_SIZE

class MessangerController
{
public:
	typedef struct _FileStatus
	{
		int framesTotal;
		int framesSent;
	} FileStatus, *PFileStatus;
private:
	typedef struct _FileData
	{
		FILE *descFile;
		int framesTotal;
		int framesSent;
	} FileData, *PFileData;
	typedef struct _MessageNode {
		struct _MessageNode* next;
		Message* message;
	} MessageNode, *PMessageNode;
public:
	MessangerController(const char* domain, const char* port);
	~MessangerController();
	bool update();
	bool hasMessage();
	bool hasFile();
	bool login(const char* login, const char* password);
	void logout();
	void uploadFile(const char* toUser, const char* filename);
	void downloadFile(const char* filename, const char* pathToSave);
	void getFileStatus(PFileStatus pFSStruct);
	void sendMessage(const char* toUser, const char* text);	
	void getMessage(char* fromUser, int fromUserLength, char* text, int textLength);
	void getFile(char* filename, int filenameLength, int* filesize);

private:
	bool hasLoginMessage(bool* logged);
	bool hasFileStarted();
	const BYTE* getFilePart(UINT* length);

private:
	static constexpr const char* A_LOGIN		= "login";
	static constexpr const char* A_LOGOUT		= "logout";
	static constexpr const char* A_MESSAGE		= "message";
	static constexpr const char* A_FILE_GET		= "file-get";
	static constexpr const char* A_FILE_PART	= "file-part";
	static constexpr const char* A_FILE_START	= "file-start";
	static constexpr const char* A_FILE_END		= "file-end";
	static constexpr const char* A_FILE			= "file";

	const char* username;
	MessageBufferer* messageBufferer;
	MessageNode* messageOrder;
	
	PFileData fd;
};

