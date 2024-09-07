#include "StaticBuffer.h"
// the declarations for this class can be found at "StaticBuffer.h"




unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];




StaticBuffer::StaticBuffer() {
  int k=0;
 for(int i=0;i<4;i++)
 {
    unsigned char buffer [BLOCK_SIZE];
    Disk::readBlock(buffer,i);
    for(int j=0;j<BLOCK_SIZE;j++)
      blockAllocMap[k++]=buffer[j];
  }
  
  
  
  for (int bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++) {
    metainfo[bufferIndex].free = true;
    metainfo[bufferIndex].dirty = false;
    metainfo[bufferIndex].timeStamp = -1;
    metainfo[bufferIndex].blockNum = -1;
    
  }
}


int StaticBuffer::getFreeBuffer(int blockNum) {
  if (blockNum < 0 || blockNum >= DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }
  int allocatedBuffer=-1;
  for(int i=0;i<BUFFER_CAPACITY;i++)
    {
      if(metainfo[i].free==true)
      {
        allocatedBuffer=i;
        break;
      }
    }
  
  if(allocatedBuffer!=-1)
  {
  metainfo[allocatedBuffer].free = false;
  metainfo[allocatedBuffer].blockNum = blockNum;

  return allocatedBuffer;
  }
  int index;
  int max=-2;
  for(int i=0;i<BUFFER_CAPACITY;i++)
  {
     if(metainfo[i].timeStamp<max)
     {
        index=i;
        max=metainfo[i].timeStamp;
     }
   }
   if(metainfo[index].dirty==1)
    {
      Disk::writeBlock(blocks[index],metainfo[index].blockNum);
    }
      metainfo[index].blockNum=blockNum;
      metainfo[index].free = false;
      metainfo[index].dirty = false;
      metainfo[index].timeStamp = metainfo[index].timeStamp+1;
      return index;
    
      
      
}


int StaticBuffer::getBufferNum(int blockNum) {
  if(!(blockNum>=0&&blockNum<DISK_BLOCKS))
   return E_OUTOFBOUND ;
   for(int i=0;i<BUFFER_CAPACITY;i++)
    {
      if(metainfo[i].free==false)
      {
        if(metainfo[i].blockNum==blockNum)
         return i;
      }
    }
  
  return E_BLOCKNOTINBUFFER;
}



int StaticBuffer::setDirtyBit(int blockNum){
    int bn=getBufferNum(blockNum);
    
    if(bn==E_BLOCKNOTINBUFFER)
        return E_BLOCKNOTINBUFFER;
    if(bn==E_OUTOFBOUND)
        return E_OUTOFBOUND;
        
        metainfo[bn].dirty=true;
        return SUCCESS;

    }

StaticBuffer::~StaticBuffer() {

    for(int i=0;i<4;i++)
    {
       unsigned char buffer[BLOCK_SIZE];
       for(int j=0;j<2014;j++)
         {
           buffer[j]=blockAllocMap[2014*i+j];
         }
       Disk::writeBlock(buffer,i);
    }
   
   for (int bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++) 
     {
       if(metainfo[bufferIndex].free==false && metainfo[bufferIndex].dirty==true)
         Disk::writeBlock(blocks[bufferIndex],metainfo[bufferIndex].blockNum);
  
}
}
