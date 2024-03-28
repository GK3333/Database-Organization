cs525-s23-group-21-main
-----------------------
1)Gautham Kumar Ramappa Shanmugam
2)Anudeep Gadige
3)Preeti sharma


HOW TO RUN
----------

a) Go to Project root (assign1) using Terminal.

b) We can verify that we are in the right directory by typing ls to list the files.

c) "make clean" in order to delete old compiled .o files.

d) "make" to compile all project files.

e) "make run_test" to run "test_assign1_1.c" file.

FUNCTIONS USED
--------------

initStorageManager()-> The file stream object will be initialized by this function by being set to NULL.

createPageFile()-> The filename parameter of this function is used to create a file with that name.
We use fopen() in the 'w+' mode, which opens a new file for reading and writing, to create a new file.
If the file creation process was successful, this method will return RC OK; otherwise, it will return RC FILE NOT FOUND.

openPageFile()-> To update all meta data parameters of the project fHandle, including Total number of pages, Current page position, and File name, by opening the file produced in the previous method.
We use fopen() in the 'r' mode to open files, which creates a new file for reading.
We return RC FILE NOT FOUND if the file could not be opened; otherwise, we return RC OK.

closePageFile()-> In order to open a new file for reading and verify that the page file is not NULL, we utilize the fopen() function in the "r" mode.
The fclose() function is used to close files, and if it is successful, it returns 0.
		
destroyPageFile()-> In order to open a new file for reading and verify that the page file is not NULL, we utilize the fopen() function in the "r" mode.
The fclose() function is used to close files, and if it is successful, it returns 0.
Using the remove() C method, we can remove the file now displayed in memory.

readBlock()-> We check that the page number is accurate. The page number shouldn't range between 0 and the number of pages in the document.
We use fseek() to find the requested place using the valid file reference.
If fseek() is successful, the data is read and stored in the memPage at the supplied page number.

getBlockPos()-> The curPagePos property of FileHandle determines the current page position, which is returned by this function.

readFirstBlock()-> We use the readBlock function with the pageNum option set to 0 to read the first block.

readCurrentBlock()-> We use the readBlock function with the currentPageNumber parameter to read the current block.

readNextBlock()-> We invoke readBlock with the pageNum parameter set to currentPageNumber + 1 to read the following block. 

readPreviousBlock()-> We use readBlock with the pageNum parameter set to currentPageNumber - 1 to read the preceding block.         

readLastBlock()-> We use readBlock with the pageNum parameter set to totalNumPages - 1 to read the final block.

writeBlock()-> We check that the page number is accurate. No page number should be less than 0 or greater than the total number of pages.
We use fseek() to find the requested place using the valid file reference.
If fseek() is successful, the information from the requested page number is written to and stored in the memPage.

writeCurrentBlock()->To write the current block, we call the writeBlock with the pageNum parameter as currentPageNumber.                 

ensureCapacity()-> Check to see if the number of pages exceeds the entire number of pages, and then use the appendEmptyBlock function to append that many empty blocks.

