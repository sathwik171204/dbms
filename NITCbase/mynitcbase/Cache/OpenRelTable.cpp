#include "OpenRelTable.h"
#include <cstring>
#include <cstdlib>

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable() {

  
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
    tableMetaInfo[i].free=true;
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
  OpenRelTable::tableMetaInfo[RELCAT_RELID].free=false;
  strcpy(OpenRelTable::tableMetaInfo[RELCAT_RELID].relName,RELCAT_RELNAME);
  

  Attribute attrCatRecord1[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(attrCatRecord1, RELCAT_SLOTNUM_FOR_ATTRCAT);

  struct RelCacheEntry a2;
  RelCacheTable::recordToRelCatEntry(attrCatRecord1,&a2.relCatEntry);
  a2.recId.block = RELCAT_BLOCK;
  a2.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;

 
  RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = a2;
  OpenRelTable::tableMetaInfo[ATTRCAT_RELID].free=false;
  strcpy(OpenRelTable::tableMetaInfo[ATTRCAT_RELID].relName,ATTRCAT_RELNAME);

  
  
  /*relCatBlock.getRecord(relCatRecord, 2);

  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = 2;

 
  RelCacheTable::relCache[2] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[2]) = relCacheEntry;
  OpenRelTable::tableMetaInfo[2].free=false;
  strcpy(OpenRelTable::tableMetaInfo[2].relName,"Students");*/


  
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
  
  
  /*head=NULL;
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
  

  AttrCacheTable::attrCache[2] = head;*/
  
  
  
  
  

}



int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

   if (strcmp(relName, RELCAT_RELNAME)==0)
   return RELCAT_RELID;
   if (strcmp(relName, ATTRCAT_RELNAME)==0)
   return ATTRCAT_RELID;
   for(int i=0;i<MAX_OPEN;i++)
            {
              if(tableMetaInfo[i].free==false)
               {
                  if(strcmp(tableMetaInfo[i].relName,relName)==0)
                    return i;
               }
            }
  return E_RELNOTOPEN;
   
   }


int OpenRelTable::getFreeOpenRelTableEntry() {
   for(int i=0;i<MAX_OPEN;i++)
   {
      if(tableMetaInfo[i].free)
      return i;
   }

  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/
    return E_CACHEFULL;
  // if found return the relation id, else return E_CACHEFULL.
}






int OpenRelTable::closeRel(int relId) {
  if (relId==0 || relId==1) {
    return E_NOTPERMITTED;
  }

  if ( 0 > relId || relId>= MAX_OPEN ) {
    return E_OUTOFBOUND;
  }

  if (OpenRelTable::tableMetaInfo[relId].free) {
    return E_RELNOTOPEN;
  }
  
  if(RelCacheTable::relCache[relId]->dirty==true)
  {
     RelCatEntry relCatEntry;
     union Attribute record[RELCAT_NO_ATTRS];
     
     RelCacheTable::getRelCatEntry(relId, &relCatEntry);
		RelCacheTable::relCatEntryToRecord(&relCatEntry, record);
     RecId recId = RelCacheTable::relCache[relId]->recId;
     RecBuffer relCatBlock(recId.block);
      relCatBlock.setRecord(record, recId.slot);
   }
   free(RelCacheTable::relCache[relId]);
   AttrCacheEntry* entry = AttrCacheTable::attrCache[relId], * temp = nullptr;
	while (entry) {
		temp = entry;
		entry = entry->next;
		free(temp);
	}
   
     
  
   RelCacheTable::relCache[relId]=nullptr;
   AttrCacheTable::attrCache[relId]=nullptr;
   tableMetaInfo[relId].free=true;
   
  return SUCCESS;
}



 int OpenRelTable::openRel( char relName[ATTR_SIZE]) {


  int relId=getRelId(relName);
  
  if(relId>=0){
      return relId;
  }
  
  
  int freeId=OpenRelTable::getFreeOpenRelTableEntry();

  /* find a free slot in the Open Relation Table
     using OpenRelTable::getFreeOpenRelTableEntry(). */

  if (freeId==E_CACHEFULL){
    return E_CACHEFULL;
  }

  // let relId be used to store the free slot.
   relId=freeId;

  /* Setting up Relation Cache entry for the relation */
    union Attribute a10;
    strcpy(a10.sVal,relName);
    RelCacheEntry relCatBuf;
    RelCacheTable::getRelCatEntry(0,&relCatBuf.relCatEntry);
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    
    RecId r=BlockAccess::linearSearch(0,(char*)"RelName",a10, EQ);
    
    
    
    
    
  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/

  // relcatRecId stores the rec-id of the relation relName in the Relation Catalog.
  RecId relcatRecId=r;

  if (relcatRecId.block ==-1 && relcatRecId.slot==-1) {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
  }
  
  
  
    union Attribute a7[6];
    RecBuffer r4(relcatRecId.block);
    r4.getRecord(a7, relcatRecId.slot);
  struct RelCacheEntry a2;
  RelCacheTable::recordToRelCatEntry(a7,&a2.relCatEntry);
  a2.recId=relcatRecId;
  
 
  RelCacheTable::relCache[relId] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[relId]) = a2;
  OpenRelTable::tableMetaInfo[relId].free=false;
    
    
    
    
    
  /* read the record entry corresponding to relcatRecId and create a relCacheEntry
      on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
      update the recId field of this Relation Cache entry to relcatRecId.
      use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
    NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  */

  /* Setting up Attribute Cache entry for the relation */

  // let listHead be used to hold the head of the linked list of attrCache entries.
  
  
  
  RelCacheTable::resetSearchIndex(1);
  
      struct AttrCacheEntry* head=nullptr;
      struct AttrCacheEntry* cur=nullptr;
      
      int numAttrs = a2.relCatEntry.numAttrs;
      
  for(int i=0;i< numAttrs;i++)

  {
      /* let attrcatRecId store a valid record id an entry of the relation, relName,
      in the Attribute Catalog.*/
      RecId attrcatRecId=BlockAccess::linearSearch(1, (char *)"RelName" ,a10,EQ);
      union Attribute rec[6];
      RecBuffer r8(attrcatRecId.block);
      r8.getRecord(rec, attrcatRecId.slot);
      AttrCacheEntry b;
      AttrCacheTable::recordToAttrCatEntry(rec,&b.attrCatEntry);
      b.recId=attrcatRecId;
      struct AttrCacheEntry* b1=(struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
      *b1=b;
      if(head==nullptr)
      {
          head=b1;
          }
       else
       {
         cur->next=b1;
         }
       cur=b1;
      }
      cur->next=nullptr;
      AttrCacheTable::attrCache[relId] = head;
  tableMetaInfo[relId].free=false;
  strcpy(tableMetaInfo[relId].relName,relName);
  // update the relIdth entry of the tableMetaInfo with free as false and
  // relName as the input.

  return relId;
      
   
   }
      
      
      
      
      
      
      
      
      

   



OpenRelTable::~OpenRelTable() {

  // close all open relations (from rel-id = 2 onwards. Why?)
  for (int i = 2; i < MAX_OPEN; ++i) {
    if (!tableMetaInfo[i].free) {
      OpenRelTable::closeRel(i); // we will implement this function later
    }
  }

  // free the memory allocated for rel-id 0 and 1 in the caches
}
