#include<iostream>
#include "BlockBuffer.h"
#include<bits/stdc++.h>
#include <cstdlib>
#include <cstring>

BlockBuffer::BlockBuffer(int blockNum){
 this->blockNum=blockNum;
 }
 RecBuffer::RecBuffer(int blockNum):
 BlockBuffer::BlockBuffer(blockNum){}
int BlockBuffer::getHeader(struct HeadInfo *head) {

  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;   // return any errors that might have occured in the process
  }

  

  memcpy(&head->numSlots, bufferPtr + 24, 4);
  
  memcpy(&head->numEntries, bufferPtr + 16, 4);
    memcpy(&head->numAttrs, bufferPtr + 20, 4);
    memcpy(&head->rblock, bufferPtr + 12, 4);
    memcpy(&head->lblock, bufferPtr + 8, 4);

    return SUCCESS;
}

int RecBuffer::getRecord(union Attribute *rec, int slotNum) {
  // ...
  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }
  struct HeadInfo head;
  this->getHeader(&head);
  int attrCount=head.numAttrs;
  int slotCount=head.numSlots;
  int recordSize=attrCount*ATTR_SIZE;
  unsigned char* slotPointer=bufferPtr+HEADER_SIZE+slotCount+(recordSize*slotNum);
  memcpy(rec,slotPointer,recordSize);
  return SUCCESS;
}



int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char ** buffPtr) {
    int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

     if (bufferNum!=E_BLOCKNOTINBUFFER)
        {
          StaticBuffer::metainfo[bufferNum].timeStamp=0;
          for(int i=0;i<BUFFER_CAPACITY;i++)
          {
            if(i!=bufferNum && StaticBuffer::metainfo[i].free==false)
            StaticBuffer::metainfo[bufferNum].timeStamp+=1;
          }
        }
     else
        {
           bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);
           if(bufferNum== E_OUTOFBOUND)
             return E_OUTOFBOUND;
           Disk::readBlock(StaticBuffer::blocks[bufferNum],this->blockNum);
           

           
        }
        *buffPtr = StaticBuffer::blocks[bufferNum];
        
        return SUCCESS;

}
/* used to get the slotmap from a record block
NOTE: this function expects the caller to allocate memory for *slotMap
*/


int BlockBuffer::setHeader(struct HeadInfo *head){

    unsigned char *bufferPtr;
    int ret= loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS)
      return ret;
 
    // cast bufferPtr to type HeadInfo*
    struct HeadInfo *bufferHeader = (struct HeadInfo *)bufferPtr;

    bufferHeader->numSlots = head->numSlots;
    bufferHeader->numEntries = head->numEntries;
    bufferHeader->blockType = head->blockType;
	bufferHeader->pblock = head->pblock;
    bufferHeader->rblock = head->rblock;
    bufferHeader->lblock = head->lblock;
    bufferHeader->numAttrs = head->numAttrs;
    
    ret=StaticBuffer::setDirtyBit(this->blockNum);
    if(ret!=SUCCESS)
    return ret;
    
     return SUCCESS;
}

int RecBuffer::getSlotMap(unsigned char *slotMap) {
  unsigned char *bufferPtr;

  // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr().
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }

  struct HeadInfo head;
  
    this->getHeader(&head);

  int slotCount = head.numSlots;

  
  unsigned char *slotMapInBuffer = bufferPtr + HEADER_SIZE;

  
    memcpy(slotMap,slotMapInBuffer,slotCount);
  return SUCCESS;
}
int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType) {

    double diff;
    if (attrType == STRING)
         diff = strcmp(attr1.sVal, attr2.sVal);

     else
       diff = attr1.nVal - attr2.nVal;

   
    if (diff > 0)  return 1;
    if (diff < 0)  return -1;
    else  return 0;
    }
    
    
    
    
int RecBuffer::setRecord(union Attribute *rec, int slotNum) {
    unsigned char *bufferPtr;
    

    int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;   // return any errors that might have occured in the process
  } 
    struct HeadInfo head;
    this->getHeader(&head);
  
    int attrCount=head.numAttrs;
    int slotCount=head.numSlots;
    
     if(slotNum>=slotCount || slotNum<0)
     return E_OUTOFBOUND;
     
     int recordSize=ATTR_SIZE *attrCount;
      
     bufferPtr=bufferPtr + HEADER_SIZE + (slotNum*recordSize)+slotCount;
     memcpy(bufferPtr,rec,recordSize);
  
    
    if(StaticBuffer::setDirtyBit(this->blockNum)!=SUCCESS)
       return 0;
       
       return SUCCESS;
}


int BlockBuffer::getFreeBlock(int blockType){

    // iterate through the StaticBuffer::blockAllocMap and find the block number
    // of a free block in the disk.
	int bufferNum = -1;
	for (int i = 0; i < DISK_BLOCKS; i++) {
		if (StaticBuffer::blockAllocMap[i] == UNUSED_BLK) {
			bufferNum = i;
			break;
		}
	}

    // if no block is free, return E_DISKFULL.
	if (bufferNum == -1) {
		return E_DISKFULL;
	}

    // set the object's blockNum to the block number of the free block.
	this->blockNum = bufferNum;

    // find a free buffer using StaticBuffer::getFreeBuffer() .
	bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);

    // initialize the header of the block passing a struct HeadInfo with values
    // pblock: -1, lblock: -1, rblock: -1, numEntries: 0, numAttrs: 0, numSlots: 0
    // to the setHeader() function.
	HeadInfo head;
	head.pblock = -1;
	head.lblock = -1;
	head.rblock = -1;
	head.numEntries = 0;
	head.numAttrs = 0;
	head.numSlots = 0;
	this->setHeader(&head);

    // update the block type of the block to the input block type using setBlockType().
	this->setBlockType(blockType);

    // return block number of the free block.
	return this->blockNum;
}
int BlockBuffer::setBlockType(int blockType){

    unsigned char *bufferPtr;
    
       int ret=loadBlockAndGetBufferPtr(&bufferPtr);
       
       if(ret!=SUCCESS)
        return ret;
        
     *((int32_t *)bufferPtr) = blockType;

    StaticBuffer::blockAllocMap[this->blockNum]=blockType;
    
    

    ret=StaticBuffer::setDirtyBit(this->blockNum);
    
        

    return ret;
}


BlockBuffer::BlockBuffer(char blockType){
     int ret;
     if(blockType=='R'){
         ret=getFreeBlock(REC);
     }
     
     
     if(ret==SUCCESS)
     this->blockNum=ret;
     
}


int RecBuffer::setSlotMap(unsigned char *slotMap) {
    unsigned char *bufferPtr;
      int ret= loadBlockAndGetBufferPtr(&bufferPtr);

     if (ret!= SUCCESS)
     return ret;
        
        HeadInfo head;
        getHeader(&head);
        
    
    int numSlots = head.numSlots;
    
    memcpy(bufferPtr + HEADER_SIZE,slotMap,numSlots);
    StaticBuffer::setDirtyBit(ret);

     return SUCCESS;
}


int BlockBuffer::getBlockNum(){

    return this->blockNum;
}
RecBuffer::RecBuffer() : BlockBuffer('R'){}
    
