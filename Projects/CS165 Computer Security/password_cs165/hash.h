/*
**  Hash routines
**    
*/

#include <stddef.h>
#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#define HASH_SUPPORTS_DELETE



typedef struct HashTableElement
{
	struct HashTableElement *pPrev;
	struct HashTableElement *pNext;
	char *pData;
}HashTable_Element;

typedef struct  HashTables
{
	//unsigned Hash(const char*szSearchFor);
	unsigned nBuckets;
	unsigned uDatumSize;
	unsigned uCount;
	struct HashTableElement **ppChain;
}HashTable;

extern HashTable     *CreateHashTable(unsigned uTableSize, unsigned uSizeOfData);
extern void   *InsertHashTable(HashTable *pTable, void *pItem);
extern int DeleteHashElement(HashTable *pTable, char *szSearchFor);
extern void   DestroyHashTable(HashTable *pTable);
extern void   *LookupHashTable(HashTable *pTable, char *szSearchFor);

#endif
