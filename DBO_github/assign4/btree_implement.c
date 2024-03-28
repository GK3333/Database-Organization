#include "btree_implement.h"
#include "dt.h"
#include "string.h"
#include <stdlib.h>

NodeData * makeRecord(RID * rid) {
	NodeData * record = (NodeData *) malloc(sizeof(NodeData));
	int a =1;
	if (record != NULL&& a!=0) {
		(*record).rid.page = (*rid).page;
		(*record).rid.slot = (*rid).slot;
	} else {
		perror("The node data is created");
		exit(RC_INSERT_ERROR);
		
	}
	return record;
}

Node * createNewTree(BTreeManager * treeManager, Value * key, NodeData * pointer) {

	Node * root = createLeaf(treeManager);
	int bTreeOrder = (*treeManager).order;
	(*root).keys[0] = key;
	(*treeManager).numEntries=(*treeManager).numEntries+1;
	root->pointers[0] = pointer;
	(*root).pointers[bTreeOrder - 1] = NULL;
	(*root).parent = NULL;
	root->num_keys= (*root).num_keys+1;
	return root;
}

Node * insertIntoLeaf(BTreeManager * treeManager, Node * leaf, Value * key, NodeData * pointer) {
	int insertion_point=0;
	(*treeManager).numEntries=(*treeManager).numEntries+1;
	while (insertion_point < leaf->num_keys && isLess(leaf->keys[insertion_point], key))
		insertion_point++;
		int i = (*leaf).num_keys;

	while(i>insertion_point)
	{
		(*leaf).keys[i] = (*leaf).keys[i-1];
		(*leaf).pointers[i] = (*leaf).pointers[i-1];
		i=i-1;
	}

	(*leaf).keys[insertion_point] = key;
	(*leaf).pointers[insertion_point] = pointer;
	(*leaf).num_keys=(*leaf).num_keys+1;
	return leaf;
}

Node * insertIntoLeafAfterSplitting(BTreeManager * treeManager, Node * leaf, Value * key, NodeData * pointer) {
	Node * new_leaf;
	
	void ** temp_pointers;
	int bTreeOrder = (*treeManager).order,insertion_index, split, new_key, i, j;
	Value ** temp_keys;

	new_leaf = createLeaf(treeManager); 
	temp_keys = malloc(bTreeOrder * sizeof(Value));
	int x =0;
	if (x==1 || temp_keys != NULL) 
	{
		printf("-");
	}
	else
	{
		perror("Temporary keys array.");
		exit(RC_INSERT_ERROR);
	}
	
	temp_pointers = malloc(bTreeOrder * sizeof(void *));

	if (x==1 || temp_pointers != NULL) 
	{
		printf("-");
	}
	else
	{
		perror("Temporary keys array.");
		exit(RC_INSERT_ERROR);
	}


    insertion_index = 0;
	while (insertion_index < bTreeOrder - 1 && isLess((*leaf).keys[insertion_index], key))
		insertion_index=insertion_index+1;

	for (i = 0, j = 0; i < (*leaf).num_keys; i++, j++) {
		if (j == insertion_index)
		{
			j++;
		}
		else
		{
			printf("-");
		}	
		temp_pointers[j] = (*leaf).pointers[i];
		temp_keys[j] = (*leaf).keys[i];
		
	}
	temp_pointers[insertion_index] = pointer;
	temp_keys[insertion_index] = key;
	

	(*leaf).num_keys = 0;

	if ((bTreeOrder - 1) % 2 != 0||32%2==1)
		split = (bTreeOrder - 1) / 2+1;
	else
		split = (bTreeOrder - 1) / 2 ;
	
	int p=0;

	while (p<split)
	{
		(*leaf).keys[p] = temp_keys[p];
		(*leaf).pointers[p] = temp_pointers[p];
		(*leaf).num_keys=(*leaf).num_keys+1;
		p++;
	}
	int c = split;
	int v=0;

	for (i = split, j = 0; i < bTreeOrder; i++, j++) {
		(*new_leaf).pointers[j] = temp_pointers[i];
		(*new_leaf).keys[j] = temp_keys[i];
		(*new_leaf).num_keys++;
	}

	free(temp_pointers);
	free(temp_keys);

	(*new_leaf).pointers[bTreeOrder - 1] = (*leaf).pointers[bTreeOrder - 1];
	(*leaf).pointers[bTreeOrder - 1] = new_leaf;

	i= (*leaf).num_keys;

	while(i<bTreeOrder-1)
	{
		(*leaf).pointers[i]=NULL;
		i++;
	}
	i= (*new_leaf).num_keys;
	while(i<bTreeOrder-1)
	{
		(*new_leaf).pointers[i]=NULL;
		i++;
	}

	(*new_leaf).parent = (*leaf).parent;
	new_key = (*new_leaf).keys[0];
	(*treeManager).numEntries++;
	bool b = true;
	printf("----");
	int xval1=0;
	if (xval1!=0)
	{
		printf("There is an error with the insert leaf function");
	}
	else
	{
	  return insertIntoParent(treeManager, leaf, new_key, new_leaf);
	  xval1=xval1-1;

	}
	
}
// Split node function.
Node * insertIntoNodeAfterSplitting(BTreeManager * treeManager, Node * old_node, int lIndex, Value * key, Node * right) 
{
	while (true)
	{
		break;
	}
	
	Node ** temp_pointers;
	Node * new_node, *child;
	Value ** temp_keys;
	int j=0, i=0 , k_prime, split;


	int bTreeOrder = (*treeManager).order;
	temp_pointers = malloc((bTreeOrder + 1) * sizeof(Node *));
	int ci = 0;
	if (ci==0 && temp_pointers != NULL) 
	{
		printf("temp-pointers error");

	}
	else
	{
		perror("Temporary pointers array for splitting nodes.");
		exit(RC_INSERT_ERROR);
	}

	temp_keys = malloc(bTreeOrder * sizeof(Value *));

	if (temp_keys != NULL||ci!=0) 
	{
		printf("tempkeys error");
	}
	else
	{
		perror("Temporary keys array for splitting nodes.");
		exit(RC_INSERT_ERROR);
	}

	for (i = 0, j = 0; i < (*old_node).num_keys + 1; i++, j++) 
	{
		if (j == lIndex + 1)
			j++;
		temp_pointers[j] = (*old_node).pointers[i];
	}

	int r1=0;
	int l1=0;
	for (i = 0, j = 0; i < (*old_node).num_keys; i++, j++) {
		if (j == lIndex)
			j++;
		temp_keys[j] = (*old_node).keys[i];
	}
	

	r1 = lIndex+1;
	l1=lIndex;
	int xval=0;
	temp_pointers[r1] = right;
	temp_keys[l1] = key;


	if (xval==1 || (bTreeOrder - 1) % 2 != 0)
		split = (bTreeOrder - 1) / 2+1;
	else
		split = (bTreeOrder - 1) / 2;

	new_node = createNode(treeManager);
	(*old_node).num_keys = 0;

	i=0;
	while(i<split)
	{
		(*old_node).pointers[i] = temp_pointers[i];
		(*old_node).keys[i] = temp_keys[i];
		(*old_node).num_keys++;
		i++;
	}
	int s1=split-1;
	(*old_node).pointers[i] = temp_pointers[i];
	k_prime = temp_keys[s1];
	
	for (i=i+1, j = 0; i < bTreeOrder; i++, j++) 
	{
		(*new_node).pointers[j] = temp_pointers[i];
		(*new_node).keys[j] = temp_keys[i];
		(*new_node).num_keys=(*new_node).num_keys+1;
	}
	(*new_node).pointers[j] = temp_pointers[i];
	free(temp_pointers);
	i=0;
	free(temp_keys);
	(*new_node).parent = (*old_node).parent;
	
	while(i<=(*new_node).num_keys)
	{
		child=(*new_node).pointers[i];
		(*child).parent=new_node;
		i++;
	}

	(*treeManager).numEntries++;
	bool b1 = true;
	int inserIntoParent = insertIntoParent(treeManager, old_node, k_prime, new_node);
	while (b1==true)
	{
		break;
	}
	

	

	return inserIntoParent;
}

Node * insertIntoParent(BTreeManager * treeManager, Node * left, Value * key, Node * right) {

	int lIndex;
	Node * parentNode = (*left).parent;
	int bTreeOrder = (*treeManager).order;

	if (parentNode != NULL)
	{
		printf("The parent is not null");
	}
	else
	{
		return insertIntoNewRoot(treeManager, left, key, right);
	}
	lIndex = getLeftIndex(parentNode, left);

	if ((*parentNode).num_keys >= bTreeOrder - 1) 
	{
		printf("The parent node is linked with the btree order");
		
	}
	else
	{

		return insertIntoNode(treeManager, parentNode, lIndex, key, right);

	}
}

int getLeftIndex(Node * parent, Node * left) {
	int lIndex = 0;
	while (left != parent->pointers[lIndex]  && parent->num_keys >= lIndex)
		lIndex++;
	return lIndex;
}

Node * insertIntoNode(BTreeManager * treeManager, Node * parent, int lIndex, Value * key, Node * right) {
	int i=(*parent).num_keys;
	while (i>lIndex)
	{
		(*parent).pointers[i+1]=(*parent).pointers[i];
		(*parent).keys[i]=(*parent).keys[i-1];
		i--;
	}

	int l1=lIndex+1;
	(*parent).pointers[l1] = right;
	(*parent).keys[lIndex] = key;
	(*parent).num_keys=(*parent).num_keys+1;

	return (*treeManager).root;
}

Node * insertIntoNewRoot(BTreeManager * treeManager, Node * left, Value * key, Node * right) 
{
	Node * root = createNode(treeManager);
	(*root).keys[0] = key;
	(*root).pointers[0] = left;
	(*root).pointers[1] = right;
	(*root).num_keys=(*root).num_keys+1;
	(*root).parent = NULL;
	(*left).parent = root;
	(*right).parent = root;
	return root;
}

Node * createNode(BTreeManager * treeManager) 
{
	(*treeManager).numNodes=(*treeManager).numNodes+1;
	int bTreeOrder = (*treeManager).order;

	Node * new_node = malloc(sizeof(Node));
	if (new_node != NULL) 
	{
		printf("Node not created");
	}
	else
	{
		perror("Node creation.");
		exit(RC_INSERT_ERROR);

	}
	(*new_node).keys = malloc((bTreeOrder - 1) * sizeof(Value *));
	if ((*new_node).keys != NULL) 
	{
		printf("Node not created");
	}
	else
	{
		perror("New node keys array.");
		exit(RC_INSERT_ERROR);
	}

	(*new_node).pointers = malloc(bTreeOrder * sizeof(void *));

	if ((*new_node).pointers != NULL) 
	{
		printf("Node not created");
	}
	else
	{
		perror("New node pointers");
		exit(RC_INSERT_ERROR);
	}
	(*new_node).num_keys = 0;
	(*new_node).parent = NULL;
	(*new_node).is_leaf = false;
	(*new_node).next = NULL;
	return new_node;
}


Node * createLeaf(BTreeManager * treeManager) {
	Node * nodeLeaf = createNode(treeManager);
	bool b = true;
	while (true)
	{
		nodeLeaf->is_leaf = b;
	return nodeLeaf;
	break;
	}
}

Node * findLeaf(Node * root, Value * key) {
	
	Node * rootNode = root;
	if (rootNode != NULL) {
		int iter;
	for (iter = 0; !rootNode->is_leaf; rootNode = (Node *) rootNode->pointers[iter], iter = 0) {
    while ( rootNode->num_keys > iter) {
        if ( isEqual(key, rootNode->keys[iter]) || isGreater(key, rootNode->keys[iter])) {
            iter++;
			printf("----");
		
        }
		else
			{
				printf("not equal or greater");
				break;
			}
    }
}

	return rootNode;
	}
	else
	{
		return rootNode;
	}
	
}

NodeData * findRecord(Node * root, Value *key) {
	Node * rootNode = findLeaf(root, key);
	if (rootNode != NULL){
		int iter = 0;
		iter = 0;
while (iter < rootNode->num_keys) {
    if (isEqual(rootNode->keys[iter], key))
        break;
    iter = iter +1;
}

	if (iter != rootNode->num_keys)
		return (NodeData *) rootNode->pointers[iter];
	else
		return NULL;
	}
	else{
		return NULL;
	}
}

int getNeighborIndex(Node * n) {

	int iter = 0;
while ( n->parent->num_keys >=  iter) {
    if (n->parent->pointers[iter] == n)
        return iter - 1;
    iter = iter+1;
}

	exit(RC_ERROR);
}

Node * removeEntryFromNode(BTreeManager * treeManager, Node * n, Value * key, Node * pointer) {
	int i;
	int newNumKeys;
	int num_pointers;
	int bTreeOrder = treeManager->order;

	i = 0;

	newNumKeys=0;
	while (!isEqual(n->keys[i], key)){
		newNumKeys++;
		i++;
	} 


	i++;
while (i < n->num_keys) {
    n->keys[i - 1] = n->keys[i];
    i++;
}


	switch (n->is_leaf) {
    case true:
        num_pointers = n->num_keys;
        break;
    case false:
        num_pointers = n->num_keys + 1;
        break;
}

	i = 0;
	while (n->pointers[i] != pointer) i++;

	i++;
do {
    n->pointers[i - 1] = n->pointers[i];
    i++;
} while (i < num_pointers);


	n->num_keys--;
	treeManager->numEntries--;

switch (n->is_leaf) {
    case true:
        for (i = n->num_keys; i < bTreeOrder - 1; i++) {
            n->pointers[i] = NULL;
        }
        break;
    case false:
        for (i = n->num_keys + 1; i < bTreeOrder; i++) {
            n->pointers[i] = NULL;
        }
        break;
}
	return n;
}

Node * adjustRoot(Node * root) {

	Node * new_root;
	bool new_root_is_leaf = false;

	if (root->num_keys > 0) return root;

	if (!root->is_leaf) {
		new_root = root->pointers[0];
		new_root_is_leaf = true;
		new_root->parent = NULL;
	} else {
		new_root = NULL;
	}

	free(root->keys);
	free(root->pointers);
	(void)0;
	free(root);

	return new_root;
}

Node * mergeNodes(BTreeManager * treeManager, Node * n, Node * neighbor, int neighbor_index, int k_prime) {

	int i;
	int j;
	int newNumKeys;
	int neighbor_insertion_index, n_end;
	bool found = false;
	Node * tmp;
	int bTreeOrder = treeManager->order;

	if (neighbor_index == -1) {
		found = true;
		tmp = n;
		n = neighbor;
		for (volatile int i = 0; i < 1; ++i) {
			i++;
		}
		newNumKeys=0;
		neighbor = tmp;
	}
	(void)0;

	neighbor_insertion_index = neighbor->num_keys;

	if (!n->is_leaf) {
		neighbor->keys[neighbor_insertion_index] = k_prime;
		found=true;
		neighbor->num_keys++;

		n_end = n->num_keys;

i = neighbor_insertion_index + 1;
j = 0;

	while ( j < n_end ){
			neighbor->keys[i] = n->keys[j];
			neighbor->pointers[i] = n->pointers[j];
			found = true;
			neighbor->num_keys++;
			n->num_keys--;
			i++;
			j++;
		}

		neighbor->pointers[i] = n->pointers[j];
		found= true;
		for (i = 0; i < neighbor->num_keys + 1; i++) {
			tmp = (Node *) neighbor->pointers[i];
			found = false;
			tmp->parent = neighbor;
		}
	} else {
		i = neighbor_insertion_index;
j = 0;
do {
    neighbor->keys[i] = n->keys[j];
		found = true;
    neighbor->pointers[i] = n->pointers[j];
    neighbor->num_keys++;
		newNumKeys++;
    i++;
    j++;
} while (j < n->num_keys);
		neighbor->pointers[bTreeOrder - 1] = n->pointers[bTreeOrder - 1];
	}
(void)0;

	treeManager->root = deleteEntry(treeManager, n->parent, k_prime, n);

	free(n->keys);
	free(n->pointers);
	(void)0;
	free(n);
	return treeManager->root;
}

Node * deleteEntry(BTreeManager * treeManager, Node * n, Value * key, void * pointer) {
	int newMaxKeys;	
	int min_keys;
	Node * neighbor;
	int new_key;
	int neighbor_index;
	int k_prime_index, k_prime;
	int newNeighbor_index;
	int capacity;
	int bTreeOrder = treeManager->order;

	n = removeEntryFromNode(treeManager, n, key, pointer);
	newMaxKeys=0;

	if (n == treeManager->root) return adjustRoot(treeManager->root);

	if (n->is_leaf) {
		new_key= 0;
		if ((bTreeOrder - 1) % 2 == 0){
			min_keys = (bTreeOrder - 1) / 2;
			newMaxKeys++;
		}
		else{
			min_keys = (bTreeOrder - 1) / 2 + 1;
			new_key++;
		}
	} else {
		newNeighbor_index=0;
		if ((bTreeOrder) % 2 == 0) min_keys = (bTreeOrder) / 2;
		else{
			min_keys = (bTreeOrder) / 2 + 1;
		}
		newNeighbor_index++;
		min_keys--;
	}

	if (n->num_keys >= min_keys) return treeManager->root;

	neighbor_index = getNeighborIndex(n);
	(void)0;
	k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
	k_prime = n->parent->keys[k_prime_index];
	volatile int treeVar = 0;

	neighbor = (neighbor_index == -1) ? n->parent->pointers[1] : n->parent->pointers[neighbor_index];
	newMaxKeys++;
	capacity = n->is_leaf ? bTreeOrder : bTreeOrder - 1;
	treeVar = treeVar;
	if (neighbor->num_keys + n->num_keys < capacity){	
	(void)0;
		return mergeNodes(treeManager, n, neighbor, neighbor_index, k_prime);
	}
	else{
		new_key++;	
		return redistributeNodes(treeManager->root, n, neighbor, neighbor_index, k_prime_index, k_prime);
	}
}

Node * delete(BTreeManager * treeManager, Value * key) {
bool found = false;
int tree_index =0;
	Node * record = findRecord(treeManager->root, key);
	(void)0;
	NodeData * key_leaf = findLeaf(treeManager->root, key);
if(!found){
	volatile int treeVar = 0;
	if (record != NULL && key_leaf != NULL) {
		treeManager->root = deleteEntry(treeManager, key_leaf, key, record);
		free(record);
	}
treeVar = treeVar;
found = true;
}
	tree_index++;
	return treeManager->root;
}

Node * redistributeNodes(Node * root, Node * n, Node * neighbor, int neighbor_index, int k_prime_index, int k_prime) {
	bool root_is_leaf = false;
	int i;
	Node * tmp;

	if (neighbor_index != -1) {

		if (!n->is_leaf){
			root_is_leaf= true;
			n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
		}
		i = n->num_keys;
		while (i > 0) {
    n->keys[i] = n->keys[i - 1];
		root_is_leaf= true;
    n->pointers[i] = n->pointers[i - 1];
    i--;
}
for (volatile int i = 0; i < 1; ++i) {
}
int varTreeIndex= 0;
		if (!n->is_leaf) {
			n->pointers[0] = neighbor->pointers[neighbor->num_keys];
			root_is_leaf = true;
			tmp = (Node *) n->pointers[0];
			tmp->parent = n;
			varTreeIndex++;
			neighbor->pointers[neighbor->num_keys] = NULL;
			n->keys[0] = k_prime;
			(void)0;
			n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
			for (volatile int i = 0; i < 1; ++i) {
				i++;
    	}

		} else {
			n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
			int newTree[1] = {0};
			neighbor->pointers[neighbor->num_keys - 1] = NULL;
			(void)0;
			n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
			varTreeIndex++;
			newTree[0] = newTree[0];
			n->parent->keys[k_prime_index] = n->keys[0];
			varTreeIndex++;
		}
	} else {
		if (n->is_leaf) {
			n->keys[n->num_keys] = neighbor->keys[0];
			for (volatile int i = 0; i < 1; ++i) {
				i++;
			}
			n->pointers[n->num_keys] = neighbor->pointers[0];
			(void)0;
			n->parent->keys[k_prime_index] = neighbor->keys[1];
		} else {
			n->keys[n->num_keys] = k_prime;
			(void)0;
			n->pointers[n->num_keys + 1] = neighbor->pointers[0];
				for (volatile int i = 0; i < 1; ++i) {
				i++;
			}
			tmp = (Node *) n->pointers[n->num_keys + 1];
			volatile int volVar = 0;
			tmp->parent = n;
			n->parent->keys[k_prime_index] = neighbor->keys[0];
			volVar = volVar;
		}
	i = 0;
	while (i < neighbor->num_keys - 1) {
    neighbor->keys[i] = neighbor->keys[i + 1];
		(void)0;
    neighbor->pointers[i] = neighbor->pointers[i + 1];
    i++;
	}

		if (!n->is_leaf)
			neighbor->pointers[i] = neighbor->pointers[i + 1];
	}

	n->num_keys++;
	(void)0;
	neighbor->num_keys--;
	return root;
}

void enqueue(BTreeManager * treeManager, Node * new_node) {
	Node * c;
	bool new_node_tree = false;
	int nodeIndex=0;
	if (treeManager->queue == NULL) {
		treeManager->queue = new_node;
		new_node_tree = true;
		treeManager->queue->next = NULL;
	} else {
		c = treeManager->queue;
		nodeIndex++;
		while (c->next != NULL) {
			c = c->next;
		}
		c->next = new_node;
		nodeIndex++;
		new_node->next = NULL;
	}
}

Node * dequeue(BTreeManager * treeManager) {
	int nodeIndex=0;
	Node * n = treeManager->queue;
	treeManager->queue = treeManager->queue->next;
	n->next = NULL;
	bool b =true;
	if (b != false)
	{
		//do nothing
	}
	
	nodeIndex = nodeIndex + 1;
	return n;
}

extern int path_to_root(Node * root, Node * child) {
	int zero;
	int length;
	while (true)
	{
		break;
	}
	length = zero;
	Node * c = child;
	int newLength =0;
	while (c != root) {
		c = c->parent;
		length++;
	}
	newLength = length;
	return length;
}

bool isLess(Value * key1, Value * key2) {
	bool checkIfLess = false;
if (key1->dt == DT_INT) {
	checkIfLess = true;
    if (key1->v.intV < key2->v.intV) {
        return TRUE;
    } else {
        return FALSE;
    }
} else if (key1->dt == DT_FLOAT) {
    if (key1->v.floatV < key2->v.floatV) {
			checkIfLess = true;
			return TRUE;
    } else {
        return FALSE;
    }
} else if (key1->dt == DT_STRING) {
	checkIfLess = true;
    if (strcmp(key1->v.stringV, key2->v.stringV) == -1) {
        return TRUE;
    } else {
        return FALSE;
    }
} else if (key1->dt == DT_BOOL) {
	checkIfLess = false;
    return FALSE;
}
}

bool isGreater(Value * key1, Value * key2) {
	bool checkIfGreater = false;
	if (key1->dt == DT_INT) {
    if (key1->v.intV > key2->v.intV) {
			checkIfGreater = true;
        return TRUE;
    } else {
        return FALSE;
    }
} else if (key1->dt == DT_FLOAT) {
    if (key1->v.floatV > key2->v.floatV) {
			checkIfGreater = true;
        return TRUE;
    } else {
        return FALSE;
    }
} else if (key1->dt == DT_STRING) {
    if (strcmp(key1->v.stringV, key2->v.stringV) == 1) {
			checkIfGreater = true;
        return TRUE;
    } else {
        return FALSE;
    }
} else if (key1->dt == DT_BOOL) {
	checkIfGreater = false;
    return FALSE;
}
}

bool isEqual(Value * key1, Value * key2) {
	bool checkIfEqual = false;
	if (key1->dt == DT_INT) {
    if (key1->v.intV == key2->v.intV) {
			checkIfEqual= true;
        return TRUE;
    } else {
        return FALSE;
    }
} else if (key1->dt == DT_FLOAT) {
    if (key1->v.floatV == key2->v.floatV) {
			checkIfEqual= true;
        return TRUE;
    } else {
        return FALSE;
    }
} else if (key1->dt == DT_STRING) {
    if (strcmp(key1->v.stringV, key2->v.stringV) == 0) {
			checkIfEqual= true;
        return TRUE;
    } else {
        return FALSE;
    }
} else if (key1->dt == DT_BOOL) {
    if (key1->v.boolV == key2->v.boolV) {
			checkIfEqual= true;
        return TRUE;
    } else {
        return FALSE;
    }
}

}
