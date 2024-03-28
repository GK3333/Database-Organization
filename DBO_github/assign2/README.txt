1. PAGE REPLACEMENT ALGORITHM FUNCTIONS


The page replacement strategy functions implement the FIFO, LRU, LFU, and CLOCK algorithms that are utilized when pinning a page. A page should be substituted from the buffer pool if the buffer pool is full and a new page needs to be pinned. Which page from the buffer pool has to be changed is decided by these page replacement algorithms.

FIFO Function
-------------

The most basic page replacement technique is known as First In First Out (FIFO). With a FIFO system, the first page in the buffer pool is at the front of the line, and if the buffer pool fills up, that page is the one that is replaced first. Upon the identification of the page, we add the new page at that place and write the information from the page frame to the page file on disk.

LFU Function
------------

Least Often Used (LFU) removes the page frame from the buffer pool that has been utilized the fewest (fewest) times out of all the other page frames. This is accomplished using the variable (field) refNum in each page frame. refNum keeps track of how many page frames the client is accessing. So, while utilizing LFU, all we have to do is locate the page frame's position that has the lowest refNum value. After that, we add the new page at that place and write the information from the page frame to the page file on disk.


LRU Function
------------

Among the other page frames in the buffer pool, Least Recently Used (LRU) eliminates the page frame that hasn't been utilized in a while. Each page frame's variable (field) hitNum performs this purpose. hitNum keeps track of how many page frames the client is accessing and pinning. Moreover, the global variable "hit" is utilized for this. So, while employing LRU, all we have to do is locate the page frame's position that has the lowest hitNum number. After that, we add the new page at that place and write the information from the page frame to the page file on disk.

CLOCK Function
--------------

The buffer pool's most recent page frame addition is tracked by the CLOCK algorithm. Also, i use a clockPointer, a counter, to direct the page frames in the buffer pool. We look at the "clockPointer" position to determine when a page has to be changed. Replace the page in that location with the new page if the hitNum for that position's page is not 1. If hitNum = 1, we set hitNum = 0 and advance clockPointer, which means we move to the following page frame to check the same thing. Up until the page can be replaced, this procedure continues. To prevent becoming stuck in an endless loop, we set hitNum = 0.