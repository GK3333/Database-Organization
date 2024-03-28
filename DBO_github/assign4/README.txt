******************
To run the code
******************

1) Using TerminalGo to Project root (assign4) .

2) Type ls to list the files and check that we are in the correct directory.

3) Type "make clean" to delete old compiled .o files.

4) Type "make" to compile all project files including "test_assign4_1.c" file 

5) Type "make run" to run "test_assign4_1.c" file.

********************
SOLUTION DESCRIPTION
********************

MakeFile was made using following tutorial -
http://mrbook.org/blog/tutorials/make/

By releasing any reserved space whenever possible and minimizing the use of variables, we have made this B+ Tree with proper memory management in mind. We have added support for additional datatypes and tested them in addition to the functions listed in btree_mgr.h. 

***********************************************
1. CUSTOM B+ TREE FUNCTIONS (btree_implement.h)
***********************************************

These functions have bee defined to perform insert/delete/find/print operations on our B+ Tree.

findLeaf(...)
--> The leaf node which have the entry with the specified key in the parameter is found by this function.
--> This function is used is to both insert an element and to find an entry.

findRecord(...)
--> This function looks for an entry in our B+ Tree that matches the key specified in the parameter.
--> If key is present in the tree then it will return a record otherwise returns null.

makeRecord(...)
--> This function is used to create a new record element which contain a RID.

insertIntoLeaf(...)
--> A new pointer to the record and its corresponding key are inserted into a leaf by this function.
--> The modified leaf node is returned

createNewTree(...)
--> When the first element or entry is added to the B+ tree, this function will create a new tree.

createNode(...)
--> A new general node is produced by this function, and it can be modified to act as a leaf, internal, or root node.

createLeaf(...)
-->  A new leaf node is created by this function.

insertIntoLeafAfterSplitting(...)
--> This function splits a leaf node in half by inserting a new key and pointer to a new record into it, which exceeds the order of the tree.
After splitting, the tree is adjusted to maintain the B+ Tree's characteristics.

insertIntoNode(...)
--> With no violation of the B+ tree properties, this function adds a new key and a pointer to a node into a node.

insertIntoNodeAfterSplitting(...)
--> This function causes a non-leaf node to split into two by adding a new key and pointer to a node, which causes the node's size to exceed the order.

insertIntoParent(...)
--> With the help of this function, a new leaf or internal node is added to the B+ tree.
--> After insertion, the tree's root is returned.

insertIntoNewRoot(..)
--> This function adds the appropriate key to the newly created roots of the two subtrees.

getLeftIndex(...)
--> To findRecord the index of the parent's pointer to the node to the left of the key to be inserted, use the insertIntoParent(..) function.

adjustRoot(...)
--> After a record is removed from the B+ Tree, this function adjusts the root and keeps the B+ Tree's properties.

mergeNodes(...)
--> With the help of this function, a node that was too small due to deletion is combined (merged) with a nearby node that can accommodate the extra entries without going over the limit.

redistributeNodes(...)
--> When one node shrinks after being deleted but its neighbor is too large to append the small node's entries without going over the maximum, this function redistributes the entries between the two nodes.

deleteEntry(...)
--> By using this function, a B+ tree entry is removed.
--> It then makes all necessary adjustments to preserve the B+ tree properties after removing the record from the leaf that has the specified key and pointer.

delete(...)
--> The entry/record having the specified key is deleted by this function.

removeEntryFromNode(...)
--> This function removes a record having the specified key from the the specified node.

getNeighborIndex(...)
--> If there is a nearest neighbor (sibling) to the left of a node, this function returns its index.
--> If the node is the leftmost child and this is not the case, it returns -1.

******************************************
2. INITIALIZING AND SHUTDOWN INDEX MANAGER
******************************************

The index manager is set up using these functions, and it is shut down using them to release all the acquired resources.

initIndexManager(...)
--> The index manager is initialized by this function.
--> To initialize the storage manager, we call the initStorageManager(...) function of the storage manager. 

shutdownIndexManager(...)
--> This function de-allocates all of the resources assigned to the index manager and shuts down the index manager.
- It releases all memory and resources that the Index Manager was utilizing.
--> We de-allocate memory by setting the treeManager data structure pointer to NULL and calling the C function free().

**********************************
3. B+ TREE INDEX RELATED FUNCTIONS
**********************************
These functions are used to create or delete a b-tree index.

createBtree(...)
--> This function creates a new B+ Tree.
--> It initializes the TreeManager structure which stores additional information of our B+ Tree.
--> We initialize the buffer manager, create a buffer pool using Buffer Manager and also create a page with the specified page name "idxId" using Storage Manager.

openBtree(...)
--> This function opens an existing B+ Tree which is stored on the file specified by "idxId" parameter.
--> We retrieve our TreeManager and initialize the Buffer Pool.

closeBtree(...)
--> This function closes the B+ Tree.
--> It marks all the pages dirty so that they can be written back to disk by the Buffer Manager.
--> It then shuts down the buffer pool and frees up all the allocated resources.

deleteBtree(....)
--> This function deletes the page file having the specified file name "idxId" in the parameter. It uses Storage Manager for this purpose.

***********************************
4. ACCESS INFORMATION ABOUT B+ TREE
***********************************
These functions enable us to obtain details about our B+ Tree, such as its nodes and keys, such as their number.

getNumNodes(...)
--> The number of nodes in our B+ Tree is returned by this function.
--> Our TreeManager structure's "numNodes" variable contains this data. We simply return this data, then.

getNumEntries(...)
--> Our B+ Tree's number of entries, records, and keys is returned by this function.
--> We keep this data in the "numEntries" variable of our TreeManager structure. We simply return this data, then.

getKeyType(...) 
--> The datatype of the keys being stored in our B+ Tree is returned by this function.
--> In the "keyType" variable of our TreeManager structure, we store this data. We simply return this data, then.

******************************
5. ACCESSING B+ TREE FUNCTIONS
******************************

These procedures are used to locate, add, and remove keys from/from a specified B+ Tree. Additionally, we can use the openTreeScan, nextEntry, and closeTreeScan methods to iteratively scan through every entry of a B+ Tree in sorted order.

findKey(...)
--> The key entered as a parameter will be looked up using this method's B+ Tree search.
--> If the key specified in the query is found in an entry, we store the RID (value) for that key in the memory location indicated by the "result" parameter.
--> This is accomplished by calling the findRecord(..) method. We return error code RC_IM_KEY_NOT_FOUND if findRecord(..) returns NULL, which indicates that the key is not present in the B+ Tree.

insertKey(...)
--> This function creates a new entry or record with the key and RID that are specified.
--> We start by looking for the given key in the B+ Tree. If it is located, the error code RC_IM_KEY_ALREADY_EXISTS is returned.
If not, a NodeData structure is created and the RID is stored there.
--> We determine if the tree's root is empty. When we call createNewTree(..), a new B+ Tree is created and this entry is added to the tree if it is empty.
We find the leaf node where this entry can be inserted if our tree has a root element, meaning the tree was already there.
--> Once the leaf node has been identified, we then determine whether there is space for the new entry. If so, calling insertIntoLeaf(...), which executes the insertion.
--> We call insertIntoLeafAfterSplitting(...) to split the leaf node if it is full before inserting the entry.

deleteKey(...)
--> This function deletes the B+ Tree entry or record associated with the given "key."
--> As previously described, we call our B+ Tree method delete(...). In order to maintain the B+ Tree properties, this function removes the entry/key from the tree and modifies the tree as necessary.

openTreeScan(...)
--> With the help of this function, the scan that is used to scan the B+ Tree entries in the sorted key order is initialized.
--> This function sets up the ScanManager structure, which stores additional data needed to carry out the scan operation. 
--> We return error code RC_NO_RECORDS_TO_SCAN if the root node of the B+ Tree is NULL.

nextEntry(...)
--> The B+ Tree entries are traversed using this function.
    The "result" parameter points to a memory location where the record details, or RID, are stored.
--> Once every entry has been scanned and no more entries are available, we return an error code. RC_IM_NO_MORE_ENTRIES;

closeTreeScan(...)
--> By terminating the scan mechanism, this function releases resources.

**********************************
6. DEBUGGING AND TEST FUNCTIONS
*******************************

These operations serve the purpose of debugging.  

--> printTree(...) This function outputs the B+ Tree.
