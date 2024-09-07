#include "RelCacheTable.h"

#include <cstring>
RelCacheEntry* RelCacheTable::relCache[MAX_OPEN];


int RelCacheTable::getRelCatEntry(int relId, RelCatEntry* relCatBuf) {
  if (relId < 0 || relId >= MAX_OPEN) {
    return E_OUTOFBOUND;
  }

  
  if (relCache[relId] == nullptr) {
    return E_RELNOTOPEN;
  }

  
  *relCatBuf = relCache[relId]->relCatEntry;

  return SUCCESS;
}


void RelCacheTable::recordToRelCatEntry(union Attribute record[RELCAT_NO_ATTRS],RelCatEntry* relCatEntry) {
  strcpy(relCatEntry->relName, record[RELCAT_REL_NAME_INDEX].sVal);
  relCatEntry->numAttrs = (int)record[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
  relCatEntry->numRecs=(int)record[RELCAT_NO_RECORDS_INDEX].nVal;
  relCatEntry->firstBlk=(int)record[RELCAT_FIRST_BLOCK_INDEX].nVal;
  relCatEntry->lastBlk=(int)record[RELCAT_LAST_BLOCK_INDEX].nVal;
  relCatEntry->numSlotsPerBlk=(int)record[RELCAT_NO_SLOTS_PER_BLOCK_INDEX].nVal;
  
}


int RelCacheTable::getSearchIndex(int relId, RecId* searchIndex) {

  if(relId<0||relId>=MAX_OPEN)
     return E_OUTOFBOUND;
     

  
  if (relCache[relId] == nullptr)
      return E_RELNOTOPEN;
  
  *searchIndex = relCache[relId]->searchIndex;
  return SUCCESS;
}


int RelCacheTable::setSearchIndex(int relId, RecId* searchIndex) {

  
    if(relId<0||relId>=MAX_OPEN)
     return E_OUTOFBOUND;
  
     if (relCache[relId] == nullptr)
      return E_RELNOTOPEN;
  
     relCache[relId]->searchIndex=*searchIndex;
  return SUCCESS;
}

int RelCacheTable::resetSearchIndex(int relId) {
  // use setSearchIndex to set the search index to {-1, -1}
   RecId searchIndex;
   searchIndex.block=-1;
   searchIndex.slot=-1;
   return RelCacheTable::setSearchIndex(relId,&searchIndex);
}




int RelCacheTable::setRelCatEntry(int relId, RelCatEntry *relCatBuf) {

  if(relId<0 || relId>=MAX_OPEN) {
    return E_OUTOFBOUND;
  }

  if(relCache[relId]==nullptr) {
    return E_RELNOTOPEN;
  }
  
  memcpy(&relCache[relId]->relCatEntry,relCatBuf,sizeof(RelCacheEntry));
  relCache[relId]->dirty=true;
  return SUCCESS;
  

}




void RelCacheTable::relCatEntryToRecord(RelCatEntry* relCatEntry, union Attribute record[RELCAT_NO_ATTRS]) {
  
  strcpy(record[RELCAT_REL_NAME_INDEX].sVal, relCatEntry->relName);

  record[RELCAT_NO_ATTRIBUTES_INDEX].nVal = (double)relCatEntry->numAttrs;
  record[RELCAT_NO_RECORDS_INDEX].nVal = (double)relCatEntry->numRecs;
  record[RELCAT_FIRST_BLOCK_INDEX].nVal = (double)relCatEntry->firstBlk;
  record[RELCAT_LAST_BLOCK_INDEX].nVal = (double)relCatEntry->lastBlk;
  record[RELCAT_NO_SLOTS_PER_BLOCK_INDEX].nVal = (double)relCatEntry->numSlotsPerBlk;

}
