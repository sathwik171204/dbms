#include "BlockAccess.h"

#include <cstring>
RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
    RecId prevRecId;
     RelCacheTable::getSearchIndex(relId,&prevRecId);

       int block,slot;
    // if the current search index record is invalid(i.e. both block and slot = -1)
    if (prevRecId.block == -1 && prevRecId.slot == -1)
    {
        RelCatEntry relCatBuf;
        RelCacheTable::getRelCatEntry(relId,&relCatBuf);
        

         block = relCatBuf.firstBlk;
         slot = 0;
    }
    else
    {
        
        block = prevRecId.block ;
         slot = prevRecId.slot+ 1;
    }

    while (block != -1)
    {
        /* create a RecBuffer object for block (use RecBuffer Constructor for
           existing block) */
         RecBuffer recBuffer(block);
         RelCatEntry relCatBuf;
         RelCacheTable::getRelCatEntry(relId,&relCatBuf);
        
         Attribute record[relCatBuf.numAttrs];
        // get the record with id (block, slot) using RecBuffer::getRecord()
        recBuffer.getRecord(record,slot);
        // get header of the block using RecBuffer::getHeader() function
        HeadInfo header;
             recBuffer.getHeader(&header);
        // get slot map of the block using RecBuffer::getSlotMap() function
          unsigned char slotmap[relCatBuf.numSlotsPerBlk];
          recBuffer.getSlotMap(slotmap);
        // If slot >= the number of slots per block(i.e. no more slots in this block)
       if(slot>=relCatBuf.numSlotsPerBlk)
        {
            // update block = right block of block
               block=header.rblock;
            // update slot = 0
            slot=0;
            continue;  // continue to the beginning of this while loop
        }

        // if slot is free skip the loop
        // (i.e. check if slot'th entry in slot map of block contains SLOT_UNOCCUPIED)
        if(slotmap[slot]==SLOT_UNOCCUPIED)
        {
            // increment slot and continue to the next record slot
            slot+=1;
            continue;
        }

        // compare record's attribute value to the the given attrVal as below:
        /*
            firstly get the attribute offset for the attrName attribute
            from the attribute cache entry of the relation using
            AttrCacheTable::getAttrCatEntry()
        */
        /* use the attribute offset to get the value of the attribute from
           current record */
           AttrCatEntry attrCatBuf;
           AttrCacheTable::getAttrCatEntry(relId,attrName,&attrCatBuf);
           int offset=attrCatBuf.offset;
           int type=attrCatBuf.attrType;

        int cmpVal=compareAttrs(record[offset],attrVal,type);  // will store the difference between the attributes
        // set cmpVal using compareAttrs()

        /* Next task is to check whether this record satisfies the given condition.
           It is determined based on the output of previous comparison and
           the op value received.
           The following code sets the cond variable if the condition is satisfied.
        */
        if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        ) {
            /*
            set the search index in the relation cache as
            the record id of the record that satisfies the given condition
            (use RelCacheTable::setSearchIndex function)
            */
            RecId searchIndex;
            searchIndex.block=block;
            searchIndex.slot=slot;
            RelCacheTable::setSearchIndex(relId,&searchIndex);

            return RecId{block, slot};
        }

        slot++;
    }

    // no record in the relation with Id relid satisfies the given condition
    return RecId{-1, -1};
    }
    
    
    
    int BlockAccess::renameRelation(char oldName[ATTR_SIZE], char newName[ATTR_SIZE]){
      RelCacheTable::resetSearchIndex(RELCAT_RELID); 

    Attribute newRelationName;    
    strcpy(newRelationName.sVal,newName);
    
    RecId r=linearSearch(0, (char*)"RelName" ,newRelationName , EQ);
    if(r.block!=-1 && r.slot!=-1)
        return E_RELEXIST;

       RelCacheTable::resetSearchIndex(0);

    Attribute oldRelationName;    
    strcpy(oldRelationName.sVal,oldName);
    
    RecId r3=linearSearch(0, (char*)"RelName" ,oldRelationName , EQ);
     
     if(r3.block==-1 && r3.block==-1)
        return E_RELNOTEXIST; 
        
        
     RecBuffer r1(RELCAT_BLOCK);
     Attribute rec[6];
     r1.getRecord(rec, r3.slot);
     strcpy(rec[RELCAT_REL_NAME_INDEX].sVal,newName);
     r1.setRecord(rec, r3.slot);
     
     
     
     Attribute recac[6];
     
     RelCacheTable::resetSearchIndex(ATTRCAT_RELID); 

     for(int i=0;i<rec[1].nVal;i++)
     {

          RecId rid=linearSearch(1, (char*)"RelName",oldRelationName , EQ);
          RecBuffer rac(rid.block);
          rac.getRecord(recac, rid.slot);
          strcpy(recac[0].sVal,newName);
          rac.setRecord(recac, rid.slot);
     }
     
     RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
     
    return SUCCESS;
}



int BlockAccess::renameAttribute(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE]) {

       RelCacheTable::resetSearchIndex(0); 

    Attribute relNameAttr;
    strcpy(relNameAttr.sVal,relName);
    
    RecId r=linearSearch(0, (char *)"RelName", relNameAttr, EQ);
    
    if(r.slot== -1 && r.block==-1)
        return E_RELNOTEXIST;
     
      RelCacheTable::resetSearchIndex(1);
      
       

    RecId attrToRenameRecId{-1, -1};
    Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];
    

    
    while (true) {
        
    RecId rac=linearSearch(1,(char *) "RelName", relNameAttr, EQ);
        if(rac.block==-1 && rac.slot==-1)
         break;
         
        RecBuffer r1(rac.block);
        r1.getRecord(attrCatEntryRecord,rac.slot);
          

         if(strcmp(attrCatEntryRecord[1].sVal,oldName)==0)
         {
            attrToRenameRecId = rac;
            
         }
        if (strcmp(attrCatEntryRecord[1].sVal,newName)==0)
             return E_ATTREXIST;
    }

     if (attrToRenameRecId.block == -1 && attrToRenameRecId.slot == -1)
         return E_ATTRNOTEXIST;

    RecBuffer rb(attrToRenameRecId.block);
    Attribute record[ATTRCAT_NO_ATTRS];
    rb.getRecord (record,attrToRenameRecId.slot);
    
    strcpy(record[1].sVal,newName);
    rb.setRecord(record, attrToRenameRecId.slot);

    
    
    return SUCCESS;
}
