// Coding done by Gautham Kumar R S (A20512380). I have implemented FIFO, LFU, LRU, Clock.

#include<stdio.h>
#include<stdlib.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <math.h>

int hit = 0,clkptr = 0;

// In the buffer pool, this structure represents one page frame.
typedef struct Page
{
	SM_PageHandle data; 
	PageNumber pageNum; 
	int isPageDirty, currentClientCount ; 
	int referenceNumber, hitNumber ;   
} PageFrame;

// Every time a page frame is added to the buffer pool, the general "hit" count is increased.

int sizeOfBuffer = 0;
int pageReadCount = 0, diskWriteCount = 0;
int leastFrequentlyUsedPointer = 0,totalDiskWriteCount = 0;

// helperFunctions
// Adding pageFrame data to the disk's page file
extern void writeBlockToDisk(BM_BufferPool *const bm, PageFrame *pageFrame, int pageFrameIndex)
{
	SM_FileHandle fh;
	openPageFile((*bm).pageFile, &fh);
	writeBlock(pageFrame[pageFrameIndex].pageNum, &fh, pageFrame[pageFrameIndex].data);
	totalDiskWriteCount = totalDiskWriteCount+1;
}
// putting together page frames for new pages
extern void setNewPage(PageFrame *pageFrame, PageFrame *page, int index) {

	int checkvariable = 1;
	if (checkvariable == 1)
	{
	pageFrame[index].isPageDirty = page->isPageDirty;
	pageFrame[index].data = (*page).data;
	pageFrame[index].currentClientCount = (*page).currentClientCount;	
	pageFrame[index].pageNum = page->pageNum;
	}	
}

// Replacement Strategy
#pragma region Strategies
// Define FIFO Function
extern void FIFO(BM_BufferPool *const bufferManagement, PageFrame *page)
{
	PageFrame *pageFrame = (PageFrame *) (*bufferManagement).mgmtData;
	
	int itr = 0;
	int frontIndex = pageReadCount % sizeOfBuffer;

	while(itr < sizeOfBuffer)
	{
		if(pageFrame[frontIndex].currentClientCount != 0)
		{
			frontIndex++;
			if(frontIndex % sizeOfBuffer == 0) {
				frontIndex = 0;
			}
			else continue;
		}
		else
		{
			if(pageFrame[frontIndex].isPageDirty == 1)
			{
				SM_FileHandle fileHandle;
				openPageFile(bufferManagement->pageFile, &fileHandle);
				writeBlock(pageFrame[frontIndex].pageNum, &fileHandle, pageFrame[frontIndex].data);
				diskWriteCount++;
			}

			setNewPage(pageFrame, page, frontIndex);
			break;
			
		}
		itr++;
	}
}

// Define LFU ucntion
extern void LFU(BM_BufferPool *const bufferManagement, PageFrame *page)
{
	PageFrame *pageFrame = (PageFrame *) (*bufferManagement).mgmtData;
	
	int itr = 0;
	int bufferIndex = 0;
	int leastFrequentlyUsedIndex, leastFrequentlyUsedReference;
	leastFrequentlyUsedIndex = leastFrequentlyUsedPointer;	
	
	while(itr < sizeOfBuffer)
	{
		if(pageFrame[leastFrequentlyUsedIndex].currentClientCount == 0)
		{
			leastFrequentlyUsedIndex = (leastFrequentlyUsedIndex + itr) % sizeOfBuffer;
			leastFrequentlyUsedReference = pageFrame[leastFrequentlyUsedIndex].referenceNumber;
			break;
		}
		itr++;
	}

	itr = (leastFrequentlyUsedIndex + 1) % sizeOfBuffer;

	while(bufferIndex < sizeOfBuffer)
	{
		if(pageFrame[itr].referenceNumber < leastFrequentlyUsedReference)
		{
			leastFrequentlyUsedIndex = itr;
			leastFrequentlyUsedReference = pageFrame[itr].referenceNumber;
		}
		itr = (itr + 1) % sizeOfBuffer;
		bufferIndex++;
	}
			
	if(pageFrame[leastFrequentlyUsedIndex].isPageDirty == 1)
	{
		SM_FileHandle fileHandle;
		openPageFile(bufferManagement->pageFile, &fileHandle);
		writeBlock(pageFrame[leastFrequentlyUsedIndex].pageNum, &fileHandle, pageFrame[leastFrequentlyUsedIndex].data);
		diskWriteCount++;
	}
	
	// updating the page frame with the content of the new page
	setNewPage(pageFrame, page, leastFrequentlyUsedIndex);
	leastFrequentlyUsedPointer = leastFrequentlyUsedIndex + 1;
}

// Define LRU function
extern void LRU(BM_BufferPool *const bufferManagement, PageFrame *page)
{	
	PageFrame *pageFrame = (PageFrame *) bufferManagement->mgmtData;
	SM_FileHandle fileHandle;
	int itr = 0;
	int leastHitIndex, leastHitNumber;

	while(itr < sizeOfBuffer)
	{
		if(pageFrame[itr].currentClientCount == 0)
		{
			leastHitIndex = itr;
			leastHitNumber = pageFrame[itr].hitNumber;
			break;
		}
		itr++;
	}	

	itr = leastHitIndex + 1;

	while(itr < sizeOfBuffer)
	{
		if(pageFrame[itr].hitNumber < leastHitNumber)
		{
			leastHitIndex = itr;
			leastHitNumber = pageFrame[itr].hitNumber;
		}
		itr++;
	}

	if(pageFrame[leastHitIndex].isPageDirty == 1)
	{
		openPageFile(bufferManagement->pageFile, &fileHandle);
		writeBlock(pageFrame[leastHitIndex].pageNum, &fileHandle, pageFrame[leastHitIndex].data);
		diskWriteCount++;
	}
	pageFrame[leastHitIndex].hitNumber = (*page).hitNumber;
	pageFrame[leastHitIndex].currentClientCount = page->currentClientCount;
	pageFrame[leastHitIndex].data = (*page).data;
	pageFrame[leastHitIndex].pageNum = page->pageNum;
	pageFrame[leastHitIndex].isPageDirty = (*page).isPageDirty;	
	
}

// Define CLOCK function
extern void CLOCK(BM_BufferPool *const bufferManagement, PageFrame *page)
{	
	PageFrame *pageFrame = (PageFrame *) bufferManagement->mgmtData;
	SM_FileHandle fileHandle;
	while(true)
	{
		if(clkptr % sizeOfBuffer == 0) {
			clkptr = 0;
		}

		if(pageFrame[clkptr].hitNumber != 0)
		{
			pageFrame[clkptr].hitNumber = 0;
			clkptr++;
		}
		else
		{
			if(pageFrame[clkptr].isPageDirty == 1)
			{
				openPageFile(bufferManagement->pageFile, &fileHandle);
				writeBlock(pageFrame[clkptr].pageNum, &fileHandle, pageFrame[clkptr].data);
				diskWriteCount++;
			}
			
			setNewPage(pageFrame, page, clkptr);
			clkptr++;
			break;		
		}
	}
}

// Coding done by Anudeep Gadige (A20513979) I have implemented markDirty, unpinPage, forcePage, pinPage

// PAGE MANAGEMENT FUNCTIONS 

// When the client modifies the data on a page, this function flags the page as dirty
extern RC markDirty (BM_BufferPool *const bufferM, BM_PageHandle *const page)
{
PageFrame *pgFrame = (PageFrame *)bufferM->mgmtData;
int index = 0;
while (index < sizeOfBuffer) {
	// If the current page needs to be flagged as "dirty," 
	// then set the isPageDirty of that page to 1 to indicate that it has been modified.
	if (pgFrame[index].pageNum == page->pageNum) {
		pgFrame[index].isPageDirty = 1;
		return RC_OK;
	}		
	index++;
	}
return RC_ERROR;
} 

// The purpose of this function is to remove a page from memory by "unpinning" it.
extern RC unpinPage (BM_BufferPool *const bufferM, BM_PageHandle *const page)
{	
	PageFrame *pgFrame = (PageFrame *)bufferM->mgmtData;
	int index = 0;
	while (index < sizeOfBuffer) {  // Iterating all the pages
	// If the current page is the one that needs to be unpinned, then decrement the currentClientCount  
	//(indicating that the client has finished working on the page) and exit the loop.
	if (pgFrame[index].pageNum == page->pageNum) {
		pgFrame[index].currentClientCount --;
		break;
	}	
	index++;	
}
	return RC_OK;
} 

// This function writes the contents of the modified pages back to the page file on disk
extern RC forcePage (BM_BufferPool *const bufferM, BM_PageHandle *const page)
{
PageFrame *pgFrame = (PageFrame *)bufferM->mgmtData;
int index = 0;
while (index < sizeOfBuffer) {
	if (pgFrame[index].pageNum == page->pageNum) {
    if (pgFrame[index].isPageDirty == 1) {
        SM_FileHandle fileHandle;
        openPageFile(bufferM->pageFile, &fileHandle);
	int pgNumber = pgFrame[index].pageNum;
        writeBlock(pgNumber, &fileHandle, pgFrame[index].data);
        // After writing the modified page to disk, indicate that it is no longer "dirty"
        pgFrame[index].isPageDirty = 0;
        // Increment the writeCount, which keeps track of the number of writes performed by the buffer manager.
        diskWriteCount = diskWriteCount +1;
    }
}
	index++;
}
return RC_OK;
}

extern RC pinPage (BM_BufferPool *const bufferM, BM_PageHandle *const page, 
	    const PageNumber pgNumber)
{
// It pins a page with page number pageNum to the buffer pool.
// If the buffer pool is at maximum capacity, the function will employ the appropriate page replacement strategy to replace a page currently in memory with the newly pinned page. 
	PageFrame *pgFrame = (PageFrame *)bufferM->mgmtData;
	// Checking if buffer pool is empty and this is the first page to be pinned
	int isEmpty =-1;
	if(pgFrame[0].pageNum == isEmpty){
		// reading a page from disk and initializing the content of the corresponding page frame within the buffer pool.
		SM_FileHandle fileHandle;
		openPageFile(bufferM->pageFile, &fileHandle);
		pgFrame[0].data = (SM_PageHandle) malloc(PAGE_SIZE);
		int pgNumberx = pgNumber;
		ensureCapacity(pgNumberx,&fileHandle);
		readBlock(pgNumber, &fileHandle, pgFrame[0].data);
		const PageNumber pgNumber1 = pgNumber;
		pgFrame[0].pageNum = pgNumber1;
		pgFrame[0].currentClientCount  = pgFrame[0].currentClientCount +1;
		hit = 0;
		pageReadCount = hit; 
		pgFrame[0].hitNumberber = hit;	
		int zero = 0;
		pgFrame[0].referenceNumber = zero;
		page->pageNum = pgNumber;
		page->data = pgFrame[0].data;
		return RC_OK;		
	}else{	
	int index = 0;
	bool checkIfBufferFull = true;
	while(index < sizeOfBuffer && checkIfBufferFull){
	if(pgFrame[index].pageNum != -1){
		// Check if page is in memory
		if(pgFrame[index].pageNum == pgNumber){
			// Increasing currentClientCount  
			checkIfBufferFull = false;
			pgFrame[index].currentClientCount =pgFrame[index].currentClientCount +1;
			hit=hit+1; // Incrementing hit (hit is used by LRU algorithm to determine the least recently used page)

	switch(bufferM->strategy){
	case RS_CLOCK:
		// hitNumber = 1 shows that this was the last page frame examined 
		pgFrame[index].hitNumber = 1;
		break;
	case RS_LFU:
		// Incrementing referenceNumber to add one more to count of number of times the page is used (referenced)
		pgFrame[index].referenceNumber++;
		break;
	case RS_LRU:
		// Least recently used
		pgFrame[index].hitNumber = hit;
		break;
	default:
		break;
}
			page->data = pgFrame[index].data;
			page->pageNum = pgNumber;
			clockPointer= clockPointer+1;
			break;
		}		
	} else {
		SM_FileHandle fileHandle;
		openPageFile(bufferM->pageFile, &fileHandle);
		pgFrame[index].data = (SM_PageHandle) malloc(PAGE_SIZE);
		int pgNumber2 = pgNumber;
		readBlock(pgNumber2, &fileHandle, pgFrame[index].data);
		int oneNumber= 1;
		pgFrame[index].pageNum = pgNumber2;
		pgFrame[index].currentClientCount  = oneNumber;
		int zero = 0;
		pgFrame[index].referenceNumber = zero;
		pageReadCount = pageReadCount +1;
		hit= hit+1; // Incrementing hit (hit is used by LRU algorithm to determine the LRU page)

	switch(bufferM->strategy){
	case RS_LRU:
		pgFrame[index].hitNumber = hit;
		break;
	case RS_CLOCK:
		pgFrame[index].hitNumber = 1;
		break;
	default:
		break;
}
		int pgNumber3 = pgNumber;
		page->pageNum = pgNumber3;
		page->data = pgFrame[index].data;

		checkIfBufferFull = false;
		break;
	}
	index++;
}
		// If checkIfBufferFull is true, then the buffer is full and we must replace an existing page using page replacement strategy
		if(checkIfBufferFull){
			// Creates a new page to store data read from the file.
			PageFrame *newPg = (PageFrame *) malloc(sizeof(PageFrame));		
			
			// Reading page from disk and initializing page frame's content in the buffer pool
			SM_FileHandle fileHandle;
			openPageFile(bufferM->pageFile, &fileHandle);
			int pgNumber4 = pgNumber;
			newPg->data = (SM_PageHandle) malloc(PAGE_SIZE);
			readBlock(pgNumber4, &fileHandle, newPg->data);
			newPg->pageNum = pgNumber4;
			int zero = 0;
			newPg->isPageDirty = zero;		
			newPg->currentClientCount  = 1;
			int zero1 = 0;
			newPg->referenceNumber = zero1;
			pageReadCount =pageReadCount +1;
			hit=hit+1;

switch(bufferM->strategy){
	case RS_LRU:
		// LRU algorithm uses the value of hit to determine the least recently used page
		newPg->hitNumber = hit;
		break;
	case RS_CLOCK:
		// hitNumber = 1 to indicate that this was the last page frame examined (added to the buffer pool)
		newPg->hitNumber = 1;
		break;
	default:
		break;
}
	page->pageNum = pgNumber;
	page->data = newPg->data;			

if (bufferM->strategy == RS_FIFO) {
						FIFO(bufferM, newPg);
}else if (bufferM->strategy == RS_LRU) {
	LRU(bufferM, newPg);
}else if (bufferM->strategy == RS_CLOCK) {
	CLOCK(bufferM, newPg);
}else if (bufferM->strategy == RS_LFU) {
	LFU(bufferM, newPg);
}else if (bufferM->strategy == RS_LRU_K) {
	printf("\n The algorithm LRU-k is not implemented");
}else {
	printf("\nNo Algorithm implemented\n");
}

						
	}		
	return RC_OK;
	}	
}




// Coding Done by Preeti Sharma(A20493653).I have implemented Buffer Pool Functions and Statistics functions

// ***** BUFFER POOL FUNCTIONS ***** //

/* 
   This function creates and initializes a buffer pool with numPages page frames.
   pageFileName stores the name of the page file whose pages are being cached in memory.
   strategy represents the page replacement strategy (FIFO, LRU, LFU, CLOCK) that will be used by this buffer pool
   stratData is used to pass parameters if any to the page replacement strategy
*/

extern RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		  const int numPages, ReplacementStrategy strategy, 
		  void *stratData)
{
	(*bm).pageFile = (char *)pageFileName;
	(*bm).numPages = numPages;
	(*bm).strategy = strategy;

	// Reserver memory space = number of pages x space required for one page
	PageFrame *p = malloc(sizeof(PageFrame) * numPages);
	
	// sizeOfBuffer is the total number of pages in memory or the buffer pool.
	sizeOfBuffer = numPages;	
	int k=0;

	// Intilalizing all pages in buffer pool. The values of fields (variables) in the page is either NULL or 0
	while( k < sizeOfBuffer)
	{
		p[k].hitNumber = 0;
		p[k].data = NULL;
		p[k].referenceNumber = 0;
		p[k].currentClientCount = 0;
		p[k].pageNum = -1;
		p[k].isPageDirty = 0;
		k++;
	}
	bm->mgmtData = p;
	diskWriteCount = clkptr; 
	clkptr = leastFrequentlyUsedPointer;
	leastFrequentlyUsedPointer= 0;
	return RC_OK;
		
}

// Shutdown i.e. close the buffer pool, thereby removing all the pages from the memory and freeing up all resources and releasing some memory space.
extern RC shutdownBufferPool(BM_BufferPool *const bm)
{
	PageFrame *pf = (PageFrame *)bm->mgmtData;
	// Write all dirty pages (modified pages) back to disk
	forceFlushPool(bm);

	int k=0;	
	while( k < sizeOfBuffer)
	{
		// If currentClientCount != 0, it means that the contents of the page was modified by some client and has not been written back to disk.
		int z = pf[k].currentClientCount;
		if(z != 0)
		{
			return RC_PINNED_PAGES_IN_BUFFER;
		}
	k++;
	}

	// Releasing space occupied by the page
	free(pf);
	bm->mgmtData = NULL;
	return RC_OK;
}


// This function writes all the dirty pages (having currentClientCount = 0) to disk

extern RC forceFlushPool(BM_BufferPool *const bm)
{
	PageFrame *pf = (PageFrame *)bm->mgmtData;
	
	int k=0;
	char result = RC_OK;
	// Store all dirty pages (modified pages) in memory to page file on disk	
	while(k < sizeOfBuffer)
	{
		if (pf[k].currentClientCount == 0 && pf[k].isPageDirty == 1) 
		{
    		SM_FileHandle filehandle;
    		// Open page file on disk
    		if (openPageFile(bm->pageFile, &filehandle) == RC_OK) 
			{
        	// Write block of data to page file on disk
        		if (writeBlock(pf[k].pageNum, &filehandle, pf[k].data) == RC_OK) 
				{
            		// Mark the page not dirty
            		pf[k].isPageDirty = 0;
            		// Increase the diskWriteCount to record the number of writes done by the buffer manager
            		diskWriteCount++;
        		}
        		// Close page file
        		closePageFile(&filehandle);
    		}
		}
	k++;
	}	
	return result;
}


// ***** STATISTICS FUNCTIONS ***** //


// This function returns an array of page numbers.
extern PageNumber *getFrameContents (BM_BufferPool *const bm)
{
	PageFrame* pageFrame = (PageFrame*) bm->mgmtData;
	PageNumber* contents = (PageNumber*) malloc(sizeof(PageNumber) * sizeOfBuffer);
	

	int k;
	// Iterating through all the pages in the buffer pool and setting frameContents' value to pageNum of the page
	for(k=0;k< sizeOfBuffer;k++) {
		if (pageFrame[k].pageNum != -1) {
    		contents[k] = pageFrame[k].pageNum;
		} 
		else {
    		contents[k] = NO_PAGE;
		}

	}
	return contents;
}

// This function returns an array of bools, each element represents the dirtyBit of the respective page.
extern bool *getDirtyFlags (BM_BufferPool *const bm)
{
	int k = 0;
	PageFrame *pageFrame = (PageFrame*) bm->mgmtData;
	bool *dF = (bool*) malloc(sizeof(bool) * sizeOfBuffer);
	

	// Iterating through all the pages in the buffer pool and setting dirtyFlags' value to TRUE if page is dirty else FALSE
	while(k< sizeOfBuffer)
	{
		if (pageFrame[k].isPageDirty == 1) {
    		dF[k] = true;
		} 
		else {
    		dF[k] = false;
		}

	k++;
	}	
	return dF;
}

// This function returns an array of ints (of size numPages) where the ith element is the fix count of the page stored in the ith page frame.
extern int *getFixCounts (BM_BufferPool *const bm)
{
	
	PageFrame *pageFrame= (PageFrame *)bm->mgmtData;
	int *fc = malloc(sizeof(int) * sizeOfBuffer);

	int k;
	// Iterating through all the pages in the buffer pool and setting fixCounts' value to page's fixCount
	for(k=0;k < sizeOfBuffer;k++)
	{
		if (pageFrame[k].currentClientCount != -1) 
		{
    		fc[k] = pageFrame[k].currentClientCount;
		} 
		else 
		{
    		fc[k] = 0;
		}

	}	
	return fc;
}



// This function returns the number of pages that have been read from disk since a buffer pool has been initialized.
extern int getNumReadIO (BM_BufferPool *const bm)
{
	// Adding one because with start pageReadCount with 0.
	return (pageReadCount + 1);
}

// This function returns the number of pages written to the page file since the buffer pool has been initialized.
extern int getNumWriteIO (BM_BufferPool *const bm)
{
	return diskWriteCount;
}
