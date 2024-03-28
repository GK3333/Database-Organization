#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<math.h>

#include "storage_mgr.h"

FILE *pageFile;

void initStorageManager(void){
	//Initialize the Storage Manager.
}

RC createPageFile(char *fileName){
	//opening file in read and write mode.
    pageFile = fopen(fileName, "w+");
    if(pageFile != NULL){
    	//creating empty page.
		SM_PageHandle emptyPage = (SM_PageHandle)calloc(PAGE_SIZE, 1);
		//writing empty page.
		if (fwrite(emptyPage,1,PAGE_SIZE,pageFile)> PAGE_SIZE)
		{
			printf("Write done");
		}
		//closing a file.
		fclose(pageFile);
		//free space.
		free(emptyPage);
		return RC_OK;
    }
    else{
		return RC_FILE_NOT_FOUND;
    	}
}

RC openPageFile(char *fileName,SM_FileHandle*fileHandlePointer){
	//opening file in read mode.
	pageFile = fopen(fileName,"r");
	//checking if the file is opened successfully or not.
    if(pageFile!=NULL)
	{
		//setting filehandle's file name.
    	fileHandlePointer->fileName = fileName;
    	//settting filehandle's currentposition.
    	fileHandlePointer->curPagePos = fseek(pageFile,0,SEEK_SET);
    	//setting filehandle's total no of pages.
    	struct stat infoOfFile;
		fstat(fileno(pageFile), &infoOfFile);
		fileHandlePointer->totalNumPages = infoOfFile.st_size/ PAGE_SIZE;
    	fclose(pageFile);
    	return RC_OK;
    }
	else
	{
		return RC_FILE_NOT_FOUND;
	}
}

RC closePageFile (SM_FileHandle *fileHandlePointer) {
	//checking if the file is opened successfully or not.
	if(pageFile == NULL)
    {
		return RC_FILE_NOT_FOUND;
	}
	else{
		//closing the file.
		pageFile = NULL;
		return RC_OK;
		}
}
    


RC destroyPageFile (char *fileName) {
	//opening file in read mode.
	pageFile = fopen(fileName, "r");
	//checking if the file is opened successfully or not.
	if(pageFile != NULL)
	{
		remove(fileName);
		return RC_OK;
	}
	else{
		return RC_FILE_NOT_FOUND;
	}
	
}

RC readBlock (int pageNum, SM_FileHandle *fileHandlePointer, SM_PageHandle pageHandlePointer) {
	//opening file in read mode.
	pageFile = fopen(fileHandlePointer->fileName, "r");
	if(pageFile != NULL)
	{
		if(pageNum < 0 || pageNum > fileHandlePointer->totalNumPages)
		{
			printf("Page does not exist");
			return RC_READ_NON_EXISTING_PAGE;
		}
		else
		{
			int seek= pageNum * PAGE_SIZE;
			// the fseek is used to seek the given page 
			//SEEK_SET points last page
			fseek(pageFile,seek,SEEK_SET);
			fread(pageHandlePointer,1, PAGE_SIZE,pageFile);
			//fileHandlePointer->curPagePos = pageNum;
			return RC_OK; 
		}
	}
	else
	{
		return RC_FILE_NOT_FOUND;
		printf("\n  file is not found");
	}
}

int getBlockPos (SM_FileHandle *fileHandlePointer) 
{
	//getting the current block. 
	if(fileHandlePointer == NULL)
	{
		printf("file is not found");
		return RC_FILE_NOT_FOUND;
	}
	else
	{
		//This determines whether the file reference is null before executing this block if it is not.
		printf("The current page position block is executed");
		int current_position;
		current_position = fileHandlePointer->curPagePos;
		return current_position;
	}
}

RC readFirstBlock (SM_FileHandle *fileHandlePointer, SM_PageHandle pageHandlePointer) {
	//assigning the values. 
	return readBlock(0, fileHandlePointer, pageHandlePointer);
	
}


RC readPreviousBlock (SM_FileHandle *fileHandlePointer, SM_PageHandle pageHandlePointer) {
	//To read the previous block, move the pointer to the current page position and deduct one.
	return readBlock((fileHandlePointer->curPagePos / PAGE_SIZE) - 1, fileHandlePointer, pageHandlePointer);
}


RC readCurrentBlock (SM_FileHandle *fileHandlePointer, SM_PageHandle pageHandlePointer) {
	//To read the current block, move the pointer to the appropriate page position.
	return readBlock(fileHandlePointer->curPagePos / PAGE_SIZE, fileHandlePointer, pageHandlePointer);
}


RC readNextBlock (SM_FileHandle *fileHandlePointer, SM_PageHandle pageHandlePointer){
	//To read the following block, move the pointer to the current page position and increase the value by 1.
	return readBlock((fileHandlePointer->curPagePos / PAGE_SIZE) + 1, fileHandlePointer, pageHandlePointer);
}


RC readLastBlock (SM_FileHandle *fileHandlePointer, SM_PageHandle pageHandlePointer){
	//advancing the cursor to the previous page number and deducting one.
	return readBlock(fileHandlePointer->totalNumPages - 1, fileHandlePointer, pageHandlePointer);
}

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	if (fHandle->totalNumPages < pageNum)
	{
		if (pageNum < 0)
		{
			return RC_WRITE_FAILED;
		}
		
	}
		
	pageFile = fopen(fHandle->fileName, "r+");
	int totPgSize = pageNum * PAGE_SIZE;
	if(pageNum != 0) { 
		fHandle->curPagePos = totPgSize;
		fclose(pageFile);
		writeCurrentBlock(fHandle, memPage);	
	} 
	else {	
		fseek(pageFile, totPgSize, SEEK_SET);	
		int iterator = 0;
while (iterator < PAGE_SIZE) {
    if (feof(pageFile)) {
        appendEmptyBlock(fHandle);
    }
    fputc(memPage[iterator], pageFile);
    iterator++;
}

		fHandle->curPagePos = ftell(pageFile); 
		fclose(pageFile);
	}
	return RC_OK;
}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	if (fHandle != NULL)
	{
		fseek(pageFile, fHandle->curPagePos, SEEK_SET);
		if (pageFile)
		{
			fwrite(memPage, 1, strlen(memPage), pageFile);
		}
		else
		{
			return RC_FILE_NOT_FOUND;
		}
		
		
	}
	else
	{
		return RC_ERROR;
	}
	return RC_OK;
}


RC appendEmptyBlock (SM_FileHandle *fileHandlePointer) {
	if (pageFile == NULL)
	{
		return RC_FILE_NOT_FOUND;
	}
	else
	{
		if (fileHandlePointer == NULL)
		{
			return RC_FILE_HANDLE_NOT_INIT;
		}
		else{
			//creating an empty block. 
			SM_PageHandle emptyBlock = (SM_PageHandle)calloc(PAGE_SIZE, 1);
			if(fseek(pageFile, 0, SEEK_END) == 0 ) {
				fwrite(emptyBlock, 1, PAGE_SIZE, pageFile);
			} else {
				free(emptyBlock);
				return RC_WRITE_FAILED;
			}
			//free(emptyBlock);
			//updating the total number of pages.
			fileHandlePointer->totalNumPages++;
			return RC_OK;
		}
	}	
}

RC ensureCapacity (int numberOfPages, SM_FileHandle *fileHandlePointer) {
	//// Opening file in append mode.
	pageFile = fopen(fileHandlePointer->fileName, "a");
	while(numberOfPages > fileHandlePointer->totalNumPages){
		appendEmptyBlock(fileHandlePointer);
	}	
	fclose(pageFile);
	return RC_OK;
}