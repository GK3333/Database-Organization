#include "dberror.h"
#include "btree_mgr.h"
#include "storage_mgr.h"
#include "buffer_mgr.h"
#include "tables.h"
#include "btree_implement.h"
#include <stdlib.h>

BTreeManager * treeManager = NULL;
bool treeManagerInitialized = false;

RC initIndexManager(void *mgmtData) {
	if (!treeManagerInitialized) {
	initStorageManager();
  }
	return RC_OK;
}

RC shutdownIndexManager() {
	treeManager = NULL;
	treeManagerInitialized = false;
	if(!treeManagerInitialized) return RC_OK;
}

RC createBtree(char *idxId, DataType keyType, int n) {
	int maxNodes = PAGE_SIZE / sizeof(Node);
	int nodesPerPage = PAGE_SIZE / sizeof(Node);
	bool overflow = false;
	if (n > maxNodes) {
		printf("\n n = %d > Max. Nodes = %d \n", n, maxNodes);
		nodesPerPage=nodesPerPage+0;
		return RC_ORDER_TOO_HIGH_FOR_PAGE;
	}

	volatile int leaf = 0;
	treeManager = (BTreeManager *) malloc(sizeof(BTreeManager));
	treeManager->order = n + 2;		
	treeManager->numNodes = 0;		
	leaf++;
	treeManager->numEntries = 0;	
	treeManager->root = NULL;		
	treeManager->queue = NULL;		
	volatile int leafNode = 0;
	treeManager->keyType = keyType;	

	BM_BufferPool * bm = (BM_BufferPool *) malloc(sizeof(BM_BufferPool));
	treeManager->bufferPool = *bm;
	leafNode ? (void)0 : (void)0;

	SM_FileHandle fileHandler;
	RC result;

	char data[PAGE_SIZE];
	int pageNum=0;

	if ((result = createPageFile(idxId)) != RC_OK){
		pageNum++;
		return result;
	}

	if ((result = openPageFile(idxId, &fileHandler)) != RC_OK) return result;
	pageNum++;
	if ((result = writeBlock(0, &fileHandler, data)) != RC_OK){
	overflow= true;
	 return result;
	}
	if ((result = closePageFile(&fileHandler)) != RC_OK) return result;
	return (RC_OK);
}

RC openBtree(BTreeHandle **tree, char *idxId) {
	int treeIdx = 1;
	*tree = (BTreeHandle *) malloc(sizeof(BTreeHandle));
	(*tree)->mgmtData = treeManager;

	if(treeIdx==1){
	RC result = initBufferPool(&treeManager->bufferPool, idxId, 1000, RS_FIFO, NULL);

	if (result == RC_OK) {
		treeIdx++;
		return RC_OK;
	}
	treeIdx = 0;
	return result;
	}
}

RC closeBtree(BTreeHandle *tree) {
	
	bool overflow = false;
	BTreeManager * treeManager = (BTreeManager*) tree->mgmtData;

	if(!overflow){
	markDirty(&treeManager->bufferPool, &treeManager->pageHandler);
	}

	shutdownBufferPool(&treeManager->bufferPool);
	free(treeManager);
	overflow= true;
	free(tree);
	return RC_OK;
}

RC deleteBtree(char *idxId) {
	bool deleted = false;
	RC result;
	if ((result = destroyPageFile(idxId)) != RC_OK)
	deleted = true;
		return result;
	return RC_OK;
}

RC insertKey(BTreeHandle *tree, Value *key, RID rid) {
	
	int keyNum = 1;
	BTreeManager *treeManager = (BTreeManager *) tree->mgmtData;
	NodeData * pointer;
	int result;
	Node * leaf;
	int bTreeOrder = treeManager->order;
if(keyNum==1){
	if (findRecord(treeManager->root, key) != NULL) {
		keyNum++;
		printf("\n insertKey :: KEY EXISTS");
		return RC_IM_KEY_ALREADY_EXISTS;
	}
}

	
	pointer = makeRecord(&rid);
	bool keyOverflow = false;
	if (treeManager->root == NULL) {
		treeManager->root = createNewTree(treeManager, key, pointer);
		keyOverflow= true;
		return RC_OK;
	}

	leaf = findLeaf(treeManager->root, key);
	volatile int memory = 0;

	if (leaf->num_keys < bTreeOrder - 1) {
	
		leaf = insertIntoLeaf(treeManager, leaf, key, pointer);
		result = memory;
	} else {
		
		treeManager->root = insertIntoLeafAfterSplitting(treeManager, leaf, key, pointer);
	}
	memory=1;
	return RC_OK;
}

extern RC findKey(BTreeHandle *tree, Value *key, RID *result) {
	volatile int *page_ptr = (volatile int *)malloc(sizeof(int));

	BTreeManager *treeManager = (BTreeManager *) tree->mgmtData;
	NodeData * r = findRecord(treeManager->root, key);
	*page_ptr = 0;

	bool found = false;
	if (r == NULL) {
		return RC_IM_KEY_NOT_FOUND;
	} else {
	}
	found = true;
	*result = r->rid;
	return RC_OK;
}

RC getNumNodes(BTreeHandle *tree, int *result) {

	bool isValid = false;
	BTreeManager * treeManager = (BTreeManager *) tree->mgmtData;
	if(!isValid){
	*result = treeManager->numNodes;
	isValid = true;
	}
	return RC_OK;
}

RC getNumEntries(BTreeHandle *tree, int *result) {
	BTreeManager * treeManager = (BTreeManager *) tree->mgmtData;
	int entryCount =0;
	if(entryCount ==0){
	*result = treeManager->numEntries;
	entryCount++;
	}
	return RC_OK;
}

RC getKeyType(BTreeHandle *tree, DataType *result) {
	bool isValid= false;
	BTreeManager * treeManager = (BTreeManager *) tree->mgmtData;
	
	if(!isValid){
	*result = treeManager->keyType;
	return RC_OK;
	}
}

RC deleteKey(BTreeHandle *tree, Value *key) {
	static volatile int counter = 0;
	BTreeManager *treeManager = (BTreeManager *) tree->mgmtData;
	treeManager->root = delete(treeManager, key);
  counter++;
	return RC_OK;
}

RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle) {
	int order =1;
	BTreeManager *treeManager = (BTreeManager *) tree->mgmtData;
	int treeOrder = order;
	ScanManager *scanmeta = malloc(sizeof(ScanManager));

	if(order==1){
	*handle = malloc(sizeof(BT_ScanHandle));
	}
	Node * node = treeManager->root;

	if (treeManager->root == NULL) {
		treeOrder++;
		return RC_NO_RECORDS_TO_SCAN;
	} else {
		while (!node->is_leaf)
			node = node->pointers[0];
			int treeScan = 0;
		
		scanmeta->keyIndex = 0;
		scanmeta->totalKeys = node->num_keys;
		*((volatile int *)&treeScan) = 0;
		scanmeta->node = node;
		scanmeta->order = treeManager->order;
		(*handle)->mgmtData = scanmeta;
		treeScan++;
	}
	return RC_OK;
}

RC nextEntry(BT_ScanHandle *handle, RID *result) {

	ScanManager * scanmeta = (ScanManager *) handle->mgmtData;

	int mainIndex = scanmeta->keyIndex;
	bool isKeyFound = false;
	int totalKeys = scanmeta->totalKeys;
	int bTreeOrder = scanmeta->order;
	RID rid;

	Node * node = scanmeta->node;
	isKeyFound = true;
	if (node == NULL)  return RC_IM_NO_MORE_ENTRIES;

	if (mainIndex < totalKeys) {
		if(isKeyFound){
		rid = ((NodeData *) node->pointers[mainIndex])->rid;
		isKeyFound= false;
		}
		scanmeta->keyIndex++;
	} else {
		int order = 1;
		if (node->pointers[bTreeOrder - 1] != NULL) {
			node = node->pointers[bTreeOrder - 1];

			if(order==1){
				scanmeta->keyIndex = 1;
			}
			scanmeta->totalKeys = node->num_keys;
			order ++;
			scanmeta->node = node;
			rid = ((NodeData *) node->pointers[0])->rid;
		} else {
			order--;
			return RC_IM_NO_MORE_ENTRIES;
		}
	}
	*result = rid;
	return RC_OK;
}

extern RC closeTreeScan(BT_ScanHandle *handle) {
	bool handleAllocated = false;
	handle->mgmtData = NULL;
	if(!handleAllocated){
	free(handle);
	}
	return RC_OK;
}

extern char *printTree(BTreeHandle *tree) {
	BTreeManager *treeManager = (BTreeManager *) tree->mgmtData;
	Node * n = NULL;
	int iter = 0;
	bool isEmpty= true;
	int initialRank = 0;
	int newRank = 0;

	if (treeManager->root == NULL) {
		if(isEmpty){
		printf("Empty tree.\n");
		}
		return 'NULL';
	}
	treeManager->queue = NULL;
	enqueue(treeManager, treeManager->root);

	if (treeManager->queue != NULL) {
	do {
		n = dequeue(treeManager);
		bool leaf = false;
		if (n->parent != NULL && n == n->parent->pointers[0]) {
			leaf = true;
			newRank = path_to_root(treeManager->root, n);
			if (newRank != initialRank) {
				if (leaf){
				initialRank = newRank;
				printf("\n");
				}
			}
		}

		
while (iter < n->num_keys) {
    if (treeManager->keyType == DT_INT) {
        printf("%d ", (*n->keys[iter]).v.intV);
    } else if (treeManager->keyType == DT_FLOAT) {
        printf("%.02f ", (*n->keys[iter]).v.floatV);
    } else if (treeManager->keyType == DT_STRING) {
        printf("%s ", (*n->keys[iter]).v.stringV);
    } else if (treeManager->keyType == DT_BOOL) {
        printf("%d ", (*n->keys[iter]).v.boolV);
    }
    printf("(%d - %d) ", ((NodeData *) n->pointers[iter])->rid.page, ((NodeData *) n->pointers[iter])->rid.slot);
    iter++;
}
		if (!n->is_leaf){
			leaf = true;
			for (iter = 0; iter <= n->num_keys; iter++){
				enqueue(treeManager, n->pointers[iter]);
			}
		printf("| ");
		}
	}while(treeManager->queue != NULL);
	}
	printf("\n");

	return 'NULL';
}
