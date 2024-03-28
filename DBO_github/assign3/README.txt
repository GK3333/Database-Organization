******************
RUNNING THE SCRIPT
******************

1) Go to Project root

2) run "make clean" to delete all old compiled .o files.

3) run "make" to compile all project files including "test_assign3_1.c" file 

4) run "make run" to run "test_assign3_1.c" file.

********************
SOLUTION DESCRIPTION
********************

*************************************
1. TABLE AND RECORD MANAGER FUNCTIONS
*************************************

The record manager related functions used in initializing and shutting down the record manager with help of BufferPool and StorageManager. 

initRecordManager:
--> This function is used to initialize the record manager.

shutdownRecordManager:
--> This function will shuts down the record manager and releases all of the resources that have been allocated.


createTable:
--> This function will Create a table with name as provided in the parameter 'name' in the schema provided in the parameter 'schema'.

openTable:
--> It will open the table having name provided by the parameter 'name', with attributes name, datatype, size.

closeTable:
--> As given by the parameter 'rel' it will close the table.

deleteTable:
--> This function will delete the table with name specified by the parameter 'name'.

getNumTuples:
--> This function will return the no. of tuples present in the table.

*******************
2. RECORD FUNCTIONS
*******************

Operation on records will be done by using below functions.

insertRecord:
--> A record in the table will be inserted with the help of this function.

deleteRecord:
-->A record with Record ID 'id' is deleted with help of this function.

updateRecord:
--> This function will update a record in the table referred by the parameter "rel".

getRecord(....)
--> A record having Record ID "id" is retrieved by this function.

*****************
3. SCAN FUNCTIONS
*****************

Scan related functions are used to retrieve all tuples from a table that fulfill a certain condition.

startScan:
--> This will function will start the scan.

next:
--> This function will return the next tuple which meets the given condition.

closeScan: 
--> This function will terminate the close scan.

*******************
4. SCHEMA FUNCTIONS
*******************

Schema functions will return the size in bytes of records for a given schema and create a new schema. 

getRecordSize:
--> This function will return the size of a record in the specified schema.

freeSchema:
--> This function will delete the schema given by the parameter schema.

createSchema:
--> This function will create a new schema in memory with the provided arguments.

**********************
5. ATTRIBUTE FUNCTIONS
**********************

This function is used to access the features of record.

createRecord:
--> This function will add a new record in the schema.

attrOffset:
--> This Function will set the offset.

freeRecord:
--> This function will release the memory space allocated to the 'record'.

getAttr:
--> This function is used to obtain an attribute from the given record in the specified schema.

setAttr:
--> This function will set the attribute value in the record in the specified schema.