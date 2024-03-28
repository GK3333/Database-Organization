#include <stdio.h>
#include <stdlib.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <math.h>

typedef struct Pg
{
	SM_PageHandle data;
	PageNumber pageNum;
	bool isThePgDirty;
	int cCount;
	int hitNumber;
	int refNum;
} PageFrame;

int bufferSize = 0;

bool foundBuffer = true; 
int numPgCount = 0;
bool hitFound = true;  
int totalDiskWriteCount = 0;
int hit = 0;
int clockPointer = 0;
double tempdiskWrites = 0; 

extern void writeBlockToDisk(BM_BufferPool *const bm, PageFrame *pageFrame, int pageFrameIndex)
{
	SM_FileHandle fh;
	bool pageOpen = false;  
	if(!pageOpen){ 
	extern int globBuffer;
	openPageFile(bm->pageFile, &fh);
	(void)globBuffer;
	}  
	
	writeBlock(pageFrame[pageFrameIndex].pageNum, &fh, pageFrame[pageFrameIndex].data);
	pageOpen=true;
	if(1){
	totalDiskWriteCount++;
	}
}

extern void setNewPageToPageFrame(PageFrame *pageFrame, PageFrame *page, int pageFrameIndex)
{
	int pageIndex = 1;
	pageFrame[pageFrameIndex].data = page->data;
	extern int globBuffer;
	(void)globBuffer; 
	pageFrame[pageFrameIndex].pageNum = page->pageNum;
	pageFrame[pageFrameIndex].isThePgDirty = page->isThePgDirty;
	int pageFrames = pageIndex;
	pageFrame[pageFrameIndex].cCount = page->cCount;
	pageFrame[pageFrameIndex].hitNumber = page->hitNumber;
	extern int globBuffer;
	(void)globBuffer;
	pageFrames=pageFrames+0;
}

extern PageFrame getNewPageInstance (BM_BufferPool *const bm, const PageNumber pageNum) {
	PageFrame *newPage = (PageFrame *)malloc(sizeof(PageFrame));
	extern int globBuffer;
	(void)globBuffer;  
	SM_FileHandle fh;
	openPageFile(bm->pageFile, &fh);
	(void)0; 
	newPage->data = (SM_PageHandle)malloc(PAGE_SIZE);
	ensureCapacity(pageNum, &fh);
	readBlock(pageNum, &fh, newPage->data);
	int newPageCount = pageNum; 
	newPage->pageNum = pageNum;
	newPage->isThePgDirty = false;
	newPageCount =0; 
	newPage->cCount = 1;
	newPage->refNum = 0;
	newPageCount++;
	newPage->hitNumber = hit;
	hitFound = true; 
	return *newPage;
}

extern void FIFO(BM_BufferPool *const bm, PageFrame *page)
{
	PageFrame *pageFrame = (PageFrame *)bm->mgmtData;
	extern int globBuffer;
	(void)globBuffer; 
	int iterVar = 0;
	int currentIndex = numPgCount % bufferSize;
	int tempIndex= 0; 

	
	while (iterVar < bufferSize)
	{
		
		if (pageFrame[currentIndex].cCount != 0)
		{
			currentIndex++;
			extern int globBuffer;
			(void)globBuffer;
			
			if (currentIndex % bufferSize == 0)
			{
				
				currentIndex = 0;
				tempIndex++; 
			}
			else{

				currentIndex = currentIndex;
				tempIndex=currentIndex;
			}
		}
		else
		{
			
			if (pageFrame[currentIndex].isThePgDirty == true)
			{
				writeBlockToDisk(bm, pageFrame, currentIndex);
			}
			extern int globBuffer;
			(void)globBuffer;  
			setNewPageToPageFrame(pageFrame, page, currentIndex);
			break;
		}
		iterVar++;
	}
}

extern void LRU(BM_BufferPool *const bm, PageFrame *page)
{
	PageFrame *pageFrame = (PageFrame *)bm->mgmtData;
		bool found = false;
	int iterVar = 0, leastHitIndex, leastHitNum;
	leastHitIndex = 0;
	 extern int globBuffer;
	(void)globBuffer;  
	leastHitNum = pageFrame[0].hitNumber;

	
	for (iterVar = leastHitIndex + 1; iterVar < bufferSize; iterVar++)
	{
		if (pageFrame[iterVar].hitNumber < leastHitNum)
		{
			if(!found){   
			leastHitIndex = iterVar;
			extern int globBuffer;
			(void)globBuffer;
			leastHitNum = pageFrame[iterVar].hitNumber;
			}
		}
	}

	
	if (pageFrame[leastHitIndex].isThePgDirty == true)
	{
		writeBlockToDisk(bm, pageFrame, leastHitIndex);
		found = true;
	}

	
	setNewPageToPageFrame(pageFrame, page, leastHitIndex);
}

extern void CLOCK(BM_BufferPool *const bm, PageFrame *page)
{
	PageFrame *pageFrame = (PageFrame *)bm->mgmtData;
	foundBuffer= true;  
	while (foundBuffer)
	{
		
		if (clockPointer % bufferSize == 0)
			clockPointer = 0;

		
		if (pageFrame[clockPointer].hitNumber == 0)
		{
			tempdiskWrites++;
			
			if (pageFrame[clockPointer].isThePgDirty == true)
			{
				writeBlockToDisk(bm, pageFrame, clockPointer);
			}

			
			hitFound = true;
			setNewPageToPageFrame(pageFrame, page, clockPointer);
			clockPointer++;
			break;
		}
		else
		{	
			extern int globBuffer;
(void)globBuffer;  
			
			pageFrame[clockPointer].hitNumber = 0;
			clockPointer++;
		}
	}
	extern int globBuffer;
	(void)globBuffer; 
}
 
extern RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
						 const int numberOfPages, ReplacementStrategy strategy,
						 void *stratData)
{
		extern int globBuffer;
	
	PageFrame *page = malloc(sizeof(PageFrame) * numberOfPages);

	bufferSize = numberOfPages;
	int tempPool = 0;
	int iterVar = 0;
	bool pageIsNew = false;  

	while (iterVar < bufferSize)
	{
		page[iterVar].data = NULL;
		tempPool++;  
		page[iterVar].pageNum = -1;
		page[iterVar].isThePgDirty = false;
		pageIsNew = true;  
		page[iterVar].cCount = 0;
		page[iterVar].hitNumber = 0;
		extern int globBuffer;
(void)globBuffer;
		page[iterVar].refNum = 0;
		iterVar++;
	}

	bm->mgmtData = page;
	bm->pageFile = (char *)pageFileName;
	tempPool=0; 
	bm->numPages = numberOfPages;
	bm->strategy = strategy;

	totalDiskWriteCount = clockPointer = 0;
	(void)globBuffer; 
	return RC_OK;
}

extern RC shutdownBufferPool(BM_BufferPool *const bm)
{
	PageFrame *pageFrame = (PageFrame *)bm->mgmtData;
	
	extern int globBuffer;
(void)globBuffer;
	forceFlushPool(bm);

	int iterVar = 0;
	bool found = false;
	while (iterVar < bufferSize)
	{
		
		if (pageFrame[iterVar].cCount != 0)
		{
			found = true;
			return RC_PINNED_PAGES_IN_BUFFER;
		}
		iterVar++;
	}

	free(pageFrame);
	found = false;
	bm->mgmtData = NULL;
	return RC_OK;
}

extern RC forceFlushPool(BM_BufferPool *const bm)
{
	PageFrame *pageFrame = (PageFrame *)bm->mgmtData;
	int iterVar = 0;
	extern int globBuffer;
	
	while (iterVar < bufferSize)
	{
		
		if (pageFrame[iterVar].cCount == 0 && pageFrame[iterVar].isThePgDirty == true)
		{
			int iterations = 0;
			SM_FileHandle fh;
			openPageFile(bm->pageFile, &fh);
			(void)globBuffer;
			writeBlock(pageFrame[iterVar].pageNum, &fh, pageFrame[iterVar].data);
			iterations++;
			pageFrame[iterVar].isThePgDirty = false;
			totalDiskWriteCount++;
		}
		iterVar++;
	}
	return RC_OK;
}

//PAGE MANAGEMENT FUNCTIONS 

RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	PageFrame *pgFrame = (PageFrame *)bm->mgmtData;

	for (int iterator = 0; iterator < bufferSize; iterator++)
{
	if (page->pageNum == pgFrame[iterator].pageNum)
	{
		bool pgDirty = pgFrame[iterator].isThePgDirty;
		pgDirty = true;
		return RC_OK;
	}
}
	return RC_OK;
}

RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	PageFrame *pgFrame = (PageFrame *)bm->mgmtData;

	for (int iterator = 0; iterator < bufferSize; iterator++)
{
	
		pgFrame[iterator].cCount--;
		
}

	return RC_OK;
}

RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	PageFrame *pgFrame = (PageFrame *)bm->mgmtData;

	for (int iterator = 0; iterator < bufferSize; iterator++)
{
	if (page->pageNum ==  pgFrame[iterator].pageNum)
	{
		writeBlockToDisk(bm, pgFrame, iterator);
		bool pgIsDirty = pgFrame[iterator].isThePgDirty;
		pgIsDirty = false;
	}
}

	return RC_OK;
}

RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page,
				  const PageNumber pageNum)
{
	PageFrame *pgF = (PageFrame *)bm->mgmtData;
	int firstPg = pgF[0].pageNum;
	if (firstPg == -1)
{
	hit = 0;
	numPgCount = 0;
	SM_FileHandle fhandl;
	openPageFile(bm->pageFile, &fhandl);
	pgF[0].data = (SM_PageHandle)calloc(1, PAGE_SIZE);
	ensureCapacity(pageNum, &fhandl);
	readBlock(pageNum, &fhandl, pgF[0].data);
	pgF[0].pageNum = pageNum;
	pgF[0].cCount = 1;
	pgF[0].hitNumber = hit;
	pgF[0].refNum = 0;
	page->pageNum = pgF[0].pageNum;
page->data = pgF[0].data;
	return RC_OK;
}


	
	bool buffPoolFull = true;
	int zero = 0;
	int iterVar = zero;
	for (int iterVar = 0; iterVar < bufferSize; iterVar++) {
    int PgNum = pgF[iterVar].pageNum;
    if (PgNum == -1) {
        SM_FileHandle fhandl;
        openPageFile(bm->pageFile, &fhandl);
        pgF[iterVar].data = (SM_PageHandle)calloc(1,PAGE_SIZE);
        readBlock(pageNum, &fhandl, (SM_PageHandle)calloc(1,PAGE_SIZE));
        pgF[iterVar].pageNum = pageNum;
        pgF[iterVar].refNum = 0;
        pgF[iterVar].cCount = 1;
        
        numPgCount = numPgCount + 1;
        hit = hit + 1;
        pgF[iterVar].hitNumber = (bm->strategy == RS_LRU) ? hit : 1;

        page->pageNum = pageNum;
        page->data = pgF[iterVar].data;
        buffPoolFull = false;
        break;
    }

    int numOfPg = pgF[iterVar].pageNum;
    if (numOfPg == pageNum) {
        buffPoolFull = false;
        hit = hit + 1;
        pgF[iterVar].cCount++;
    
        pgF[iterVar].hitNumber = 1;
       

        page->pageNum = pageNum;
        page->data = pgF[iterVar].data;
        break;
    }
}


	if (buffPoolFull == true)
	{

		PageFrame *newPage = (PageFrame *)malloc(sizeof(PageFrame));
		SM_FileHandle fileHand;
		openPageFile(bm->pageFile, &fileHand);
		readBlock(pageNum, &fileHand, (SM_PageHandle)calloc(1,PAGE_SIZE));
		newPage->pageNum = pageNum;
		newPage->refNum = 0;
		newPage->cCount = 1;
		numPgCount = numPgCount + 1;
		hit = hit + 1;
		newPage->hitNumber = (bm->strategy == RS_LRU) ? hit : 1;

if (bm->strategy == RS_CLOCK) {
    CLOCK(bm, newPage);
} else if (bm->strategy == RS_FIFO) {
    FIFO(bm, newPage);
} else if (bm->strategy == RS_LRU) {
    LRU(bm, newPage);
} else {
    printf("\n We have not implemented any strategy\n");
}

	}
	return RC_OK;
}

// STATISTICS FUNCTIONS 

PageNumber *getFrameContents(BM_BufferPool *const bm)
{
	PageNumber *frameContents = calloc(bufferSize, sizeof(PageNumber));;
	PageFrame *pgFrame = (PageFrame *)bm->mgmtData;

	for (int iterVar = 0; iterVar < bufferSize; iterVar++)
{
 
		frameContents[iterVar] = pgFrame[iterVar].pageNum;
        
  
}

	return RC_OK;
}

bool *getDirtyFlags(BM_BufferPool *const bm)
{
	bool *isThePgDirtyFlags = calloc(1, bufferSize);
	PageFrame *pgFrame = (PageFrame *)bm->mgmtData;

	int iterVar = 0;
	while (iterVar < bufferSize)
	{
    	isThePgDirtyFlags[iterVar] = false;
    	if (true == pgFrame[iterVar].isThePgDirty)
    	{
        	isThePgDirtyFlags[iterVar] = true;
    	}
    iterVar++;
}

	return RC_OK;
}

int *getFixCounts(BM_BufferPool *const bm)
{
	int *fixCounts = malloc(bufferSize * 4);
	PageFrame *pageFrame = (PageFrame *)bm->mgmtData;
	for (int iterVar = 0; iterVar < bufferSize; iterVar++)
	{
    fixCounts[iterVar] = 0;
    if (-1 != pageFrame[iterVar].cCount)
    {
        fixCounts[iterVar] = pageFrame[iterVar].cCount;
    }
	}
	return RC_OK;
}

int getNumReadIO(BM_BufferPool *const bm)
{
	int totNumOfPgs = numPgCount + 1;
	return (totNumOfPgs);
}

int getNumWriteIO(BM_BufferPool *const bm)
{
	return RC_OK;
}

