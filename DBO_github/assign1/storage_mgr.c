#include "storage_mgr.h"
#include "dberror.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
//#include<sys/types.h>
//#include<unistd.h>
//#include<string.h>
//#include<math.h> 

FILE *filePointer;
int sizeOfChar = sizeof(char);

void initStorageManager(void){
	//Initialize the Storage Manager.
}

RC createPageFile(char *fileName){
	//opening file in read and write mode.
    filePointer = fopen(fileName, "w+");
    if(filePointer != NULL){
    	//creating empty page.
		SM_PageHandle emptyPage = (SM_PageHandle)calloc(PAGE_SIZE, sizeOfChar);
		//writing empty page.
		if (fwrite(emptyPage,sizeOfChar,PAGE_SIZE,filePointer)> PAGE_SIZE)
		{
			printf("Write done");
		}
		else
		{
			printf("Write fail");
		}
		//closing a file.
		fclose(filePointer);
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
	filePointer = fopen(fileName,"r");
	//checking if the file is opened successfully or not.
    if(filePointer!=NULL)
	{
		//setting filehandle's file name.
    	fileHandlePointer->fileName = fileName;
    	//settting filehandle's currentposition.
    	fileHandlePointer->curPagePos = fseek(filePointer,0,SEEK_SET);
    	//setting filehandle's total no of pages.
    	struct stat infoOfFile;
		fstat(fileno(filePointer), &infoOfFile);
		fileHandlePointer->totalNumPages = infoOfFile.st_size/ PAGE_SIZE;
    	fclose(filePointer);
    	return RC_OK;
    }
	else
	{
		return RC_FILE_NOT_FOUND;
	}
}


RC closePageFile (SM_FileHandle *fileHandlePointer) {
	//checking if the file is opened successfully or not.
	if(filePointer == NULL)
    {
		return RC_FILE_NOT_FOUND;
	}
	else{
		//closing the file.
		filePointer = NULL;
		return RC_OK;
		}
}
    


RC destroyPageFile (char *fileName) {
	//opening file in read mode.
	filePointer = fopen(fileName, "r");
	//checking if the file is opened successfully or not.
	if(filePointer != NULL)
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
	filePointer = fopen(fileHandlePointer->fileName, "r");
	if(filePointer != NULL)
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
			fseek(filePointer,seek,SEEK_SET);
			fread(pageHandlePointer,sizeOfChar, PAGE_SIZE,filePointer);
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


RC writeBlock (int pageNum, SM_FileHandle *fileHandlePointer, SM_PageHandle pageHandlePointer) {
	//opening file in read mode.
	filePointer = fopen(fileHandlePointer->fileName, "r+");
	if(filePointer != NULL)
	{
			//to determine if the pageNumber is more than 0 and less than totalNumPages.
		if (pageNum > fileHandlePointer->totalNumPages || pageNum < 0)
		{
			return RC_WRITE_FAILED;
		}
		else{
			//the pointer's alignment with the file stream. If the fseek() function returns 0, the seek will be successful.
			fseek(filePointer, (pageNum * PAGE_SIZE), SEEK_SET);
			//pageHandlePointer's content should be written to the pageF stream.
		    fwrite(pageHandlePointer, sizeOfChar, strlen(pageHandlePointer), filePointer);
		    // To align the file stream's cursor location with the current page position.
		    fileHandlePointer->curPagePos = ftell(filePointer);
			// Closing file stream.
			fclose(filePointer);
			return RC_OK;
		}
	}
	else{
		return RC_FILE_NOT_FOUND;
	}
	
}


RC writeCurrentBlock (SM_FileHandle *fileHandlePointer, SM_PageHandle pageHandlePointer) {
	//setting the pointer to the current page position.
	fileHandlePointer->totalNumPages++;
	return writeBlock(fileHandlePointer->curPagePos / PAGE_SIZE, fileHandlePointer, pageHandlePointer);
}


RC appendEmptyBlock (SM_FileHandle *fileHandlePointer) {
	if (filePointer == NULL)
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
			SM_PageHandle emptyBlock = (SM_PageHandle)calloc(PAGE_SIZE, sizeOfChar);
			if(fseek(filePointer, 0, SEEK_END) == 0 ) {
				fwrite(emptyBlock, sizeOfChar, PAGE_SIZE, filePointer);
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
	filePointer = fopen(fileHandlePointer->fileName, "a");
	while(numberOfPages > fileHandlePointer->totalNumPages){
		appendEmptyBlock(fileHandlePointer);
	}	
	fclose(filePointer);
	return RC_OK;
}

