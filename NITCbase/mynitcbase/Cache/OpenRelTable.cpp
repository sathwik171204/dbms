#include "OpenRelTable.h"
#include <cstring>
#include <cstdlib>

OpenRelTable::OpenRelTable() {

  
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
  }

  
  RecBuffer relCatBlock(RELCAT_BLOCK);

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

  struct RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

 
  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;

  

  Attribute attrCatRecord1[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(attrCatRecord1, RELCAT_SLOTNUM_FOR_ATTRCAT);

  struct RelCacheEntry a2;
  RelCacheTable::recordToRelCatEntry(attrCatRecord1,&a2.relCatEntry);
  a2.recId.block = RELCAT_BLOCK;
  a2.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;

 
  RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = a2;
  
  
  
  relCatBlock.getRecord(relCatRecord, 2);

  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = 2;

 
  RelCacheTable::relCache[2] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[2]) = relCacheEntry;

  
  RecBuffer attrCatBlock(ATTRCAT_BLOCK);
   
  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  struct AttrCacheEntry* head=NULL;
  struct AttrCacheEntry* current=NULL;
  for(int i=0;i<=5;i++)
    {
      attrCatBlock.getRecord(attrCatRecord, i);
      struct AttrCacheEntry attrCacheEntry;
  AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry.attrCatEntry);
  attrCacheEntry.recId.block = RELCAT_BLOCK;
  attrCacheEntry.recId.slot = i;
  struct AttrCacheEntry* a1=(struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
  *a1=attrCacheEntry;
   if(head==NULL)
     head=a1;
     else
     {
       current->next=a1;
     }
     current=a1;
    }
    current->next=nullptr;
  

  AttrCacheTable::attrCache[RELCAT_RELID] = head;
   head=NULL;
   current=NULL;
   for(int i=6;i<=11;i++)
    {
      attrCatBlock.getRecord(attrCatRecord, i);
      struct AttrCacheEntry attrCacheEntry;
  AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry.attrCatEntry);
  attrCacheEntry.recId.block = ATTRCAT_BLOCK;
  attrCacheEntry.recId.slot = i;
  struct AttrCacheEntry* a1=(struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
  *a1=attrCacheEntry;
   if(head==NULL)
     head=a1;
     else
     {
       current->next=a1;
     }
     current=a1;
    }
    current->next=nullptr;
  

  AttrCacheTable::attrCache[ATTRCAT_RELID] = head;
  
  
  head=NULL;
   current=NULL;
   for(int i=12;i<=15;i++)
    {
      attrCatBlock.getRecord(attrCatRecord, i);
      struct AttrCacheEntry attrCacheEntry;
  AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry.attrCatEntry);
  attrCacheEntry.recId.block = ATTRCAT_BLOCK;
  attrCacheEntry.recId.slot = i;
  struct AttrCacheEntry* a1=(struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
  *a1=attrCacheEntry;
   if(head==NULL)
     head=a1;
     else
     {
       current->next=a1;
     }
     current=a1;
    }
    current->next=nullptr;
  

  AttrCacheTable::attrCache[2] = head;

}



int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

   if (strcmp(relName, RELCAT_RELNAME)==0)
   return RELCAT_RELID;
   if (strcmp(relName, ATTRCAT_RELNAME)==0)
   return ATTRCAT_RELID;
   if (strcmp(relName, "Students")==0)
   return 2;
   
   
   
  // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID

  return E_RELNOTOPEN;
}

OpenRelTable::~OpenRelTable() {
  // free all the memory that you allocated in the constructor
}
