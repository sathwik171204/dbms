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


int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **buffPtr) {
  
  int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

  if (bufferNum == E_BLOCKNOTINBUFFER) {
    bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);

    if (bufferNum == E_OUTOFBOUND) {
      return E_OUTOFBOUND;
    }

    Disk::readBlock(StaticBuffer::blocks[bufferNum], this->blockNum);
  }

  
  *buffPtr = StaticBuffer::blocks[bufferNum];

  return SUCCESS;
}
/* used to get the slotmap from a record block
NOTE: this function expects the caller to allocate memory for *slotMap
*/
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
    
