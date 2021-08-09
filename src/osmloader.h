#ifndef _MY_OSM_LOADER_
#define _MY_OSM_LOADER_

typedef unsigned long long id_t;

/**
 *	A tag consists of two items, a key and a value. Tags describe specific features
 * of map elements (nodes, ways, or relations) or changesets. Both items are free 
 * format text fields, but often represent numeric or other structured items.
 * from: https://wiki.openstreetmap.org/wiki/Tags
 */
typedef struct tag {
	char key;
	char value;
} tag;

/**
 * Nodes can be used on their own to define point features. When used in this way,
 * a node will normally have at least one tag to define its purpose. Nodes may have
 * multiple tags and/or be part of a relation. For example, a telephone box may be
 * tagged simply with amenity=telephone, or could also be tagged with operator=*. 
 * from: https://wiki.openstreetmap.org/wiki/Node
 */
typedef struct node {
	id_t	id;			// 64-bit integer number ≥ 1
	long double latitude;	// decimal number ≥ −90.0000000 and ≤ 90.0000000 with 7 decimal places
	long double longitude;	// decimal number ≥ −180.0000000 and ≤ 180.0000000 with 7 decimal places
	//TODO: implement tags in nodes later
 //tag** 		tags;			// A set of key/value pairs, with unique key
} node; // 16 bytes per node

typedef struct way {
	id_t	 id;
	int	 numNodes;		// Number of nodes in this way
	node **nodes;			// array of pointers to nodes that belong in this way
   //TODO: implement tags in ways later
 	//tag**			tags;
} way;

// TODO: implement remaining items
typedef struct relation {
	id_t  id;
	way  *ways;
	node *nodes;
} relation;

typedef struct osm {
	/* bounds */
	double minLat, 
			 minLon, 
			 maxLat, 
			 maxLon,
			 deltaLat,
			 deltaLon;
	
	int numWays;
	way *ways;
	
	int numNodes;
	node *nodes;
	
	int numTags;
	tag *tags;

	int numRelations;
	relation *relations;
} osm;

void osmLoad(char* fileName, osm **mapData);
void osmPrintWay(way *wayptr);
void osmCacheSave(char* cacheFileName, osm* mapData);
void osmFree(osm **osmData);

#endif//_MY_OSM_LOADER_