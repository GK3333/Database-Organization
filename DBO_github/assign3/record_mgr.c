#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"

const ReplacementStrategy DEFAULT_REPLACEMENT_STRATEGY = RS_LRU;
const int firstPgPos = 1;
const int firstPgNum = 0; 
BM_PageHandle *pageHandle;
const int numOfPgsInBufferPool = 100;
RecordManager *recordManager;
const int firstSlotPos = 0;
BM_BufferPool *bufferPool;
const int attrsize = 20;


int *incrementPointer(char *pointer, int offset)
{
	int x =0;
	if(x==0)
	{
		pointer = pointer + offset;
	}
	return pointer;
}
int getFreeSlotIndex(char *data, int recordSize)
{
	int totalSlots,index,offset;
	totalSlots= PAGE_SIZE / recordSize;
	for(index=0;index < totalSlots;index++)
	{
		offset = index * recordSize;
		if (data[offset] != '#')
			return index;
	}
	return -1;
}

extern RC initRecordManager(void *mgmtData)
{
	int checkval =0;
	if(checkval!=1)
	{
		initStorageManager();
	}
	return RC_OK;
}
extern RC shutdownRecordManager()
{
	if(recordManager != NULL)
	{
	  recordManager = NULL;
	}
	free(recordManager);
	return RC_OK;
}
extern RC createTable(char *name, Schema *schema)
{
	int result;
	int const NUMBER_OF_TUPLES = 0;
	char data[PAGE_SIZE], *pageHandle = data;
	SM_FileHandle fileHandle; 
	recordManager = (RecordManager *)malloc(sizeof(RecordManager));
	bool success = true;    
	bufferPool = &recordManager->bufferPool;

	initBufferPool(bufferPool, name, numOfPgsInBufferPool, DEFAULT_REPLACEMENT_STRATEGY, NULL);
	success = false;  
	int iter=0,tableCreationAttributes[] = {NUMBER_OF_TUPLES, firstPgPos, (*schema).numAttr, (*schema).keySize};
	int tableCreationAttributesSize = sizeof(tableCreationAttributes) / sizeof(tableCreationAttributes[0]);
	
	for (iter=0;iter < tableCreationAttributesSize;iter++)
	{
		*(int *)pageHandle = tableCreationAttributes[iter];
		pageHandle = incrementPointer(pageHandle, sizeof(int));
	}
	for(iter=0;iter<(*schema).numAttr;iter++)
	{
		strncpy(pageHandle, (*schema).attrNames[iter], attrsize);
		pageHandle = incrementPointer(pageHandle, attrsize);
		*(int *)pageHandle = (int)(*schema).typeLength[iter];
		pageHandle = incrementPointer(pageHandle, sizeof(int));
		*(int *)pageHandle = (int)(*schema).dataTypes[iter];
		pageHandle = incrementPointer(pageHandle, sizeof(int));
		
	}

bool isExceptionPresent = true;
if ((result = createPageFile(name)) != RC_OK) {
   ;
		(void)0;
} else if ((result = openPageFile(name, &fileHandle)) != RC_OK) {
	;
} else if ((result = writeBlock(firstPgNum, &fileHandle, data)) != RC_OK) {
	 (void)fwrite("", 0, 1, NULL);
  
} else if ((result = closePageFile(&fileHandle)) != RC_OK) {
  (void)puts("");
} else {
    isExceptionPresent = false;
}

	if (isExceptionPresent == true)
	{
		(void)scanf("%*s", "");   
		return result;
	}

	return RC_OK;
}

extern RC openTable(RM_TableData *rel, char *name)
{
	(void)puts("");  
	int iter=0;
	pageHandle = &recordManager->pageHandle;
	int numberOfAttributes;
	bufferPool = &recordManager->bufferPool;
	pinPage(bufferPool, pageHandle, firstPgNum);
	bool hasAttribute = false;    
	pageHandle = (char *)(*recordManager).pageHandle.data;
	(*recordManager).totalRecordsInTable = *(int *)pageHandle;
	pageHandle = incrementPointer(pageHandle, sizeof(int));
	(*recordManager).firstFreePage.page = 1;
	(*recordManager).firstFreePage.slot = 0;

	pageHandle = incrementPointer(pageHandle, sizeof(int));
    numberOfAttributes = *(int *)pageHandle;
	hasAttribute= true;    
	pageHandle = incrementPointer(pageHandle, sizeof(int));
	Schema *schema;
    if(hasAttribute) {   
		schema = (Schema *)malloc(sizeof(Schema));
	}   
	(*schema).numAttr = numberOfAttributes;
	(*schema).attrNames = (char **)calloc(1,sizeof(char *) * numberOfAttributes);
	(*schema).dataTypes = (DataType *)calloc(1,sizeof(DataType) * numberOfAttributes);
	(*schema).typeLength = (int *)calloc(1,sizeof(int) * numberOfAttributes);


	while(iter<numberOfAttributes)
	{
		schema->attrNames[iter] = (char *)malloc(attrsize);
		iter++;

	}
	iter =0;
	while(iter<(*schema).numAttr)
	{
		strncpy(schema->attrNames[iter], pageHandle, attrsize);
		pageHandle = incrementPointer(pageHandle, attrsize);
		(*schema).dataTypes[iter] = *(int *)pageHandle;
		pageHandle = incrementPointer(pageHandle, sizeof(int));
		(*schema).typeLength[iter] = *(int *)pageHandle;
		pageHandle = incrementPointer(pageHandle, sizeof(int));

		iter++;

	}

	(*rel).schema = schema;
	(*rel).mgmtData = recordManager;
	(*rel).name = name;
	
	unpinPage(bufferPool, pageHandle);
	forcePage(bufferPool, pageHandle);

	return RC_OK;
}

extern RC closeTable(RM_TableData *rel)
{
	RecordManager *recordManager = (*rel).mgmtData;
	bufferPool = &recordManager->bufferPool;
	int iv= 0;
	if(iv==0)
	{
		shutdownBufferPool(bufferPool);
	}
	return RC_OK;
}
extern int getNumTuples(RM_TableData *rel)
{
	RecordManager *recordManager = (*rel).mgmtData;
	return (*recordManager).totalRecordsInTable;
}
extern RC deleteTable(char *name)
{
	destroyPageFile(name);
	return RC_OK;
}

extern RC insertRecord(RM_TableData *rel, Record *record)
{
	char *data, *recordPointer;
	bufferPool = &recordManager->bufferPool;
	pageHandle = &recordManager->pageHandle;
	recordManager = (*rel).mgmtData;
	int y=1, recordSize = getRecordSize((*rel).schema);
	RID *recordID = &record->id;
	(*recordID).slot = -1;
	bool isSearchFirstTime = true;
	
	while ((*recordID).slot == -1)
	{
		if ((y!=1 || !isSearchFirstTime))
		{
			unpinPage(bufferPool, pageHandle);
			(*recordID).page++;
		}

		else if (y==1 && isSearchFirstTime)
		{
			(*recordID).page = (*recordManager).firstFreePage.page;
			isSearchFirstTime = false;
		}

		pinPage(bufferPool, pageHandle, (*recordID).page);

		data = (*recordManager).pageHandle.data;

		(*recordID).slot = getFreeSlotIndex(data, recordSize);
	}

	recordPointer = data;

	if (markDirty(bufferPool, pageHandle))
	{
		RC_message = "A Fail message";
		return RC_MARK_DIRTY_FAILED;
	}

	int recordOffset ;
	recordOffset  = (*recordID).slot * recordSize;
	recordPointer = incrementPointer(recordPointer, recordOffset);
	*recordPointer = '#';
	recordPointer++;

	memcpy(recordPointer, (*record).data + 1, recordSize - 1);

	if (unpinPage(bufferPool, pageHandle))
	{
		RC_message = "Fail message 2";
		return RC_UNPIN_PAGE_FAILED;
	}

	(*recordManager).totalRecordsInTable++;

	pinPage(bufferPool, pageHandle, 0);

	return RC_OK;
}

extern RC deleteRecord(RM_TableData *rel, RID id)
{
	RecordManager *recordManager = (*rel).mgmtData;
	bufferPool = &recordManager->bufferPool;
	pageHandle = &recordManager->pageHandle;
	pinPage(bufferPool, pageHandle, id.page);

	recordManager->firstFreePage.page = id.page;

	char *pageData = (*recordManager).pageHandle.data;

	int pov = 1 , recordSize = getRecordSize((*rel).schema),recordOffset = id.slot * recordSize;
	
	pageData = incrementPointer(pageData, recordOffset);
	if (markDirty(bufferPool, pageHandle))
	{ 
		if(pov == 1)
		{
			RC_message = "A Fail message";
		}
		return RC_MARK_DIRTY_FAILED;
	}

	if (unpinPage(bufferPool, pageHandle))
	{
		if(pov == 1)
		{
			return RC_UNPIN_PAGE_FAILED;

		}	
	}
	return RC_OK;
}

extern RC updateRecord(RM_TableData *rel, Record *record)
{
	bufferPool = &recordManager->bufferPool;
	pageHandle = &recordManager->pageHandle;
	recordManager = (*rel).mgmtData;
	RID id = (*record).id;
	pinPage(bufferPool, pageHandle, id.page);
	char *pageData = (*recordManager).pageHandle.data;
	int pov = 0 ,recordSize = getRecordSize((*rel).schema), recordOffset = id.slot * recordSize;
	pageData = incrementPointer(pageData, recordOffset);
	pageData= pageData+1;
	memcpy(pageData, (*record).data + 1, recordSize);

	if (markDirty(bufferPool, pageHandle))
	{ 
		if(pov == 0)
		{
			RC_message = "A fail message";
		}
		return RC_MARK_DIRTY_FAILED;
	}

	if (unpinPage(bufferPool, pageHandle))
	{
		if(pov == 0)
		{
			return RC_UNPIN_PAGE_FAILED;

		}	
		(void)0;  
	}
	return RC_OK;
}


extern RC getRecord(RM_TableData *rel, RID id, Record *record)
{
	bufferPool = &recordManager->bufferPool;
	bool found = false;  
	pageHandle = &recordManager->pageHandle;
	recordManager = (*rel).mgmtData;
	if(found == false){  
	(*record).id = id;
	} 
	int pageNumber = id.page;

	if (pinPage(bufferPool, pageHandle, pageNumber) != RC_OK)
	{
		RC_message = "Pin page has failed: ";
		return RC_PIN_PAGE_FAILED;
		found = true;  
	}

	int recordSize = getRecordSize(rel->schema);
	int newRecordSize = recordSize+1;  
	int recordOffset = id.slot * recordSize;
	char *dataPointer = recordManager->pageHandle.data;
	dataPointer = incrementPointer(dataPointer, recordOffset);
	newRecordSize++;  
	char *data = record->data;
	data++;
	if(found==true || found==false){  
	memcpy(data, dataPointer + 1, recordSize);
	}  

	if (unpinPage(bufferPool, pageHandle) != RC_OK)
	{
		found = true;   
		RC_message = "Unpin Page has failed";
		return RC_UNPIN_PAGE_FAILED;
	}

	return RC_OK;
}

extern RC startScan(RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
{
	bool scanDone = false;  
	
	if (cond == NULL)
	{
		(void)memcpy(NULL, NULL, 0);  
		return RC_SCAN_CONDITION_NOT_FOUND;
	}

	if(scanDone==false){  
	openTable(rel, "ScanTable");
	}  

	RecordManager *scanManager, *tableManager;
	tableManager = rel->mgmtData;
	scanDone = true;   
	tableManager->totalRecordsInTable = attrsize;
	int scanCounter = 0; 

	scanManager = (RecordManager *)malloc(sizeof(RecordManager));
	scan->mgmtData = scanManager;

	RID scanManagerRecordID = scanManager->recordID;
	if (scanDone){ 
	scanManagerRecordID.page = firstPgPos;
	scanManagerRecordID.slot = firstSlotPos;
	} 

	scanManager->scanCount = 0;
	scanCounter++ ; 
	scanManager->condition = cond;
	scan->rel = rel;

	return RC_OK;
}

extern RC next(RM_ScanHandle *scan, Record *record)
{
	RecordManager *scanManager;
RecordManager *scanTableManager;
int y=0;
Schema *schema;

scanManager = (RecordManager *)(scan->mgmtData);
int num=0;
scanTableManager = (RecordManager *)(scan->rel->mgmtData);
schema = (Schema *)(scan->rel->schema);

	if(y==0){
	if (scanManager->condition == NULL)
	{
		return RC_SCAN_CONDITION_NOT_FOUND;
	}
	}


Value *result = (Value *)malloc(sizeof(Value));

char *data;

	int sizeOfRecord = getRecordSize(schema);

	int placeholderRecord = sizeOfRecord+0;   
	int totalSlotsCount = PAGE_SIZE / sizeOfRecord;
    totalSlotsCount = totalSlotsCount+placeholderRecord - placeholderRecord;    

	int scanCount = scanManager->scanCount;
	int totalRecordsInTable = scanTableManager->totalRecordsInTable;
    totalRecordsInTable=totalRecordsInTable+0;
	if (totalRecordsInTable == 0) return RC_RM_NO_MORE_TUPLES;
    RID scanManagerRecordID = scanManager->recordID;

	while (scanCount <= totalRecordsInTable)
	{

		bool isTrue = true;  

		if(isTrue){    
		if (scanCount > 0)
		{
			scanManager->recordID.slot++;

			if(isTrue){	   
			if (scanManager->recordID.slot >= totalSlotsCount)
			{
				scanManager->recordID.slot = firstSlotPos;
				int pageCount= scanCount;
				scanManager->recordID.page++;
			}
			}    
		}
		else
		{
			scanManager->recordID.page = firstPgPos;
			isTrue = true;   
			scanManager->recordID.slot = firstSlotPos;
		}
		}   

		if(isTrue){  
        pinPage(&scanTableManager->bufferPool, &scanManager->pageHandle, scanManager->recordID.page);
        }   

		data = scanManager->pageHandle.data;
		int x=0;
		int recordOffset = scanManager->recordID.slot * sizeOfRecord;
        data = incrementPointer(data, recordOffset);

		
		bool flag=true; 
		record->id.page = scanManager->recordID.page;

		if(flag==true) { 

		 record->id.slot = scanManager->recordID.slot;
		}

		char *dataPointer = record->data;
		dataPointer=dataPointer+1;   

		memcpy(dataPointer, data + 1, sizeOfRecord);
		scanManager->scanCount++;
		scanCount=scanCount+1;  

		evalExpr(record, schema, scanManager->condition, &result);

		if (result->v.boolV == TRUE)
		{
			scanCount=scanCount+1;
			unpinPage(&scanTableManager->bufferPool, &scanManager->pageHandle);
			scanCount=scanCount-1;
			return RC_OK;
		}

	}

	unpinPage(&scanTableManager->bufferPool, &scanManager->pageHandle);
    scanManager->recordID.page = firstPgPos;
	int rec_page =0;     
	scanManager->recordID.slot = firstSlotPos;
	rec_page++;         
	scanManager->scanCount = 0;
	return RC_RM_NO_MORE_TUPLES;
}

extern RC closeScan(RM_ScanHandle *scan)
{
	{int temp=0; (void)temp;}     
	RecordManager *scanManager = scan->mgmtData;
	RecordManager *recordManager = scan->rel->mgmtData;
	bool scanCompleted = false;  
	RID scanManagerRecordID = scanManager->recordID;

	
	if(!scanCompleted){ 

	if (scanManager->scanCount > 0)
	{
		
		int scanCounter = scanManager->scanCount;   	
		unpinPage(&recordManager->bufferPool, &scanManager->pageHandle);
		scanManagerRecordID.page = firstPgPos;
		scanCounter=scanCounter+1;
		scanManagerRecordID.slot = firstSlotPos;
		scanManager->scanCount = 0;
	}
	}  

	
	scanManager = NULL;
	int scanCounter=0;  
	free(scanManager);
	return RC_OK;
}

int getRecordSize(Schema *schema)
{
	int recordSize = 0;
	int scanCounter=0;   
	int sizeOfAttribute = 0;
	scanCounter=0;    
	int iter = 0;

	
	for (iter = 0; iter < schema->numAttr; iter++){
		bool foundForward=true;   
		
			if (schema->dataTypes[iter] == DT_STRING){
				sizeOfAttribute = schema->typeLength[iter];
				if(!foundForward){};  
		}else if (schema->dataTypes[iter] == DT_INT){
				foundForward= false; 
				sizeOfAttribute = sizeof(int);
		}else if (schema->dataTypes[iter] == DT_FLOAT){
				sizeOfAttribute = sizeof(float);
				recordSize= scanCounter; 
		}else if (schema->dataTypes[iter] == DT_BOOL){
				sizeOfAttribute = sizeof(bool);
				recordSize=recordSize-scanCounter;  
		}
		recordSize = recordSize + sizeOfAttribute;
		}
		recordSize = ++ recordSize;
	return recordSize;
}

extern Schema *createSchema(int numberofAttribute, char **attributeNames, DataType *dataTypes, int *typeLength, int sizeOfKey, int *keys)
{
	{int temp=0; (void)temp;}     
	Schema *schema = (Schema *)malloc(sizeof(Schema));
	schema->typeLength = typeLength;
	int sizeOfnewKey=0;   
	schema->attrNames = attributeNames;
	schema->keyAttrs = keys;
	schema->keySize = sizeOfKey;
	int newKeySize = sizeOfnewKey;   
	schema->numAttr = numberofAttribute;
	schema->dataTypes = dataTypes;
	return schema;

}

extern RC freeSchema(Schema *schema)
{
	if(true) {
		free(schema);
	}
	return RC_OK;
}

extern RC createRecord(Record **record, Schema *schema)
{
	{int temp=0; (void)temp;}     
	Record *newRecord = (Record *)malloc(sizeof(Record));

	int newSize = 0; 
	int sizeOfRecord = getRecordSize(schema);
	newRecord->data = (char *)malloc(sizeOfRecord);

	newSize=newSize+1;  

	newRecord->id.page = newRecord->id.slot = -1;
	char *dataPointer = newRecord->data;
	if(newSize>0) {  
		*dataPointer = '-';
	++dataPointer;   
	}   
	char esc = '\0';   

	*dataPointer = esc; 

	*record = newRecord;

	return RC_OK;
}

RC attrOffset(Schema *schema, int attributeNumber, int *result)
{
	int iter = 0;
	int newSize = 0;     
	int sizeOfAttribute = 0;
	*result = 1;
	int attributeList[3] = {DT_INT, DT_FLOAT, DT_BOOL};
	char types[3] ={'float','bool','double'};  
	char datatypeList[3][10] = {"int", "float", "bool"};

	for (iter=0; iter< attributeNumber; iter++){
		if (schema->dataTypes[iter] == DT_STRING)
		{	
			sizeOfAttribute = schema->typeLength[iter];
			newSize++;  
		}
		else if (schema->dataTypes[iter] == DT_INT)
		{
			sizeOfAttribute=sizeOfAttribute-(newSize*0);   
			sizeOfAttribute = sizeof(int);
		}
		else if (schema->dataTypes[iter] == DT_FLOAT)
		{
			if (newSize>0|| newSize<0){  
			sizeOfAttribute = sizeof(float);
			newSize--; 
			}  
		}
		else if (schema->dataTypes[iter] == DT_BOOL)
		{
			sizeOfAttribute = sizeof(bool);
		}
		newSize++; 
		*result = *result + sizeOfAttribute;
	}
	return RC_OK;
}


extern RC freeRecord(Record *record)
{
	if(true) free(record)   ;  
	return RC_OK;
}

extern RC getAttr(Record *record, Schema *schema, int attributeNumber, Value **value)
{
	{int temp=0; (void)temp;}     
	int offset = 0;
	int newSize= 0;  
	int attributeSize = 0;

	attrOffset(schema, attributeNumber, &offset);

	Value *attribute = (Value *)malloc(sizeof(Value));

	for(int temp=0;temp<newSize;temp++) {   
		newSize++;  
	}  
	char *dataPointer = record->data;
	dataPointer = incrementPointer(dataPointer, offset);

	if (attributeNumber == 1)
	{
		schema->dataTypes[attributeNumber] = 1;
	}
	else{
		if(newSize==0){
		schema->dataTypes[attributeNumber] = schema->dataTypes[attributeNumber];
		}
	}


switch (schema->dataTypes[attributeNumber])
{
case DT_STRING:
{
{int temp=0; (void)temp;}     
int length = schema->typeLength[attributeNumber];

	{int temp=0; (void)temp;}     

int newSize = length+1;   
attribute->v.stringV = (char *)malloc(length + 1);
	strncpy(attribute->v.stringV, dataPointer, length);
	{int temp=0; (void)temp;}     
	attribute->v.stringV[length] = '\0';
	newSize--;  
	attribute->dt = DT_STRING;
	break;
}

case DT_INT:
{
	int value = 0;
	attributeSize = sizeof(int);
	int newValue =value;  
	memcpy(&value, dataPointer, attributeSize);
	attribute->v.intV = value;
	attribute->dt = DT_INT;
	value--;  
	break;
}

case DT_FLOAT:
{
	float value;
	attributeSize = sizeof(float);
	memcpy(&value, dataPointer, attributeSize);
	int newAttributeSize = sizeof(double);    
	attribute->v.floatV = value;
	newAttributeSize = sizeof(double);   
	attribute->dt = DT_FLOAT;
	break;
}

case DT_BOOL:
{
	bool value;
	attributeSize = sizeof(bool);
	bool isValid = false;  
	memcpy(&value, dataPointer, attributeSize);
	attribute->v.boolV = value;
	isValid = true;  
	attribute->dt = DT_BOOL;
	break;
}

default:
{
	printf("Not defined\n");
	break;
}
}

	*value = attribute;
	return RC_OK;
}

RC setAttr(Record *record, Schema *schema, int attributeNumber, Value *value)
{
	int offset = 0;
	int scanCount=0;  
	if(scanCount==0){   
		attrOffset(schema, attributeNumber, &offset);
	}    

	char *dataPointer = record->data;
	dataPointer = incrementPointer(dataPointer, offset);


switch(schema->dataTypes[attributeNumber]){
	case DT_STRING:
	{
	 
		int length = schema->typeLength[attributeNumber];
		int boolValue= false;  
		strncpy(dataPointer, value->v.stringV, length);
		int offset =0;
		if (boolValue ==false) offset = schema->typeLength[attributeNumber];  
		dataPointer = incrementPointer(dataPointer, offset);
		break;
	}
	case DT_INT:{
		*(int *)dataPointer = value->v.intV;
		dataPointer = incrementPointer(dataPointer, sizeof(int));
		break;
	}
	case DT_FLOAT:
	{

			*(float *)dataPointer = value->v.floatV;
		dataPointer = incrementPointer(dataPointer, sizeof(float));
		break;
	}
	case DT_BOOL:
	{

		*(bool *)dataPointer = value->v.boolV;
		dataPointer = incrementPointer(dataPointer, sizeof(bool));
		break;
	}
	default:
	{
		printf("provided datatype does not have a Serializer \n");	
		break;
	}
}
	return RC_OK;
	}

//coding done by Gautham, Anudeep and Preeti
