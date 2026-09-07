#ifndef _LIMITS_H
#define _LIMITS_H

// These are the static limits placed on the internal structures
// The limits exist to minimize the use of the heap, and aid performance
// and scalability of the framework.

// MAX_FIELD_DATA_SIZE has a significant effect on the size of the CProxy object
#define MAX_FIELD_DATA_SIZE 2048// max length of field value buffer (number of WCHARs)

// ENTITY_BANKS * ENTITY_PER_BANK = max number of rows returned from a query
#define ENTITY_PER_BANK 100		// max entities per entity bank in EntityQuery
#define ENTITY_BANKS 100		// max number of entity banks in EntityQuery

// Not a significant in dynamic memory overhead (i.e. only used on stack)
#define FIELDNAME_MAXSIZE 128	// max size of a field name

#endif