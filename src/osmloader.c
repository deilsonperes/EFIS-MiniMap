#include "osmloader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define _pause system("pause");
/* global variables */
char *getNode_valString;

void getBounds(char*, osm*);
static inline void getNode(char*, osm*, int);
static inline void getWay(FILE*, char*, osm*, int);
static inline unsigned long long getOSMValueULL(const char *, const char *);
static inline long double getOSMValueLD(const char *, const char *);

void osmLoad(char* fileName, osm **mapData)
{
	FILE* osm_file = fopen(fileName, "rb");
	if(osm_file == NULL)
	{
		printf("Could not open file \'%s\'\n", fileName);
		return;
	}

	printf("Loading OSM data from file '%s'.\n", fileName);
	time_t time = clock();

	*mapData = (osm*) malloc(sizeof(osm));
	osm *ptrMapData = *mapData;
	ptrMapData->numNodes 		= 0;
	ptrMapData->numWays 			= 0;
	ptrMapData->numTags 			= 0;
	ptrMapData->numRelations 	= 0;

	getNode_valString = (char*) calloc(32, sizeof(char));
	char *lineBuffer 	= (char*) calloc(1024, sizeof(char)),
		  *tagName 		= (char*) calloc(32, sizeof(char)),
		  *tagStart,*valueStart;

	int numNodeAlloc		= 1000,
		 numWayAlloc 		= 1000,
		 numRelationAlloc = 1000,
		 numTagAlloc 		= 1000;

	// allocate memory for structs
	ptrMapData->nodes 	 = (node*) malloc(sizeof(node) * numNodeAlloc);
	ptrMapData->ways 		 = (way*) malloc(sizeof(way) * numWayAlloc);
	ptrMapData->relations = (relation*) malloc(sizeof(relation) * numRelationAlloc);
	ptrMapData->tags 		 = (tag*) malloc(sizeof(tag) * numTagAlloc);
	getNode_valString 	 = (char*) calloc(64, sizeof(char));

	while(1)
	{
		fgets(lineBuffer, 1024, osm_file);
		if(feof(osm_file))
		{
			break;
		}
		if(tagStart = strstr(lineBuffer, "<") + 1)
		{
			// if next char is '/' this is a tag ending
			if(tagStart[1] == '/')
			{
				;//printf("Tag ending: %s", lineBuffer);
			} else {
				if(!strncmp(tagStart, "bounds", 6))
				{
					getBounds(tagStart, ptrMapData);
				} else
				if(!strncmp(tagStart, "node", 4))
				{
					// TODO: implement
					// check if there is enough memory
					if(ptrMapData->numNodes >= numNodeAlloc)
					{
						numNodeAlloc += 1000;
						ptrMapData->nodes = realloc(ptrMapData->nodes, numNodeAlloc * sizeof(node));
					}
					getNode(lineBuffer, ptrMapData, ptrMapData->numNodes++);
				} else
				if(!strncmp(tagStart, "way", 3))
				{
					if(ptrMapData->numWays >= numWayAlloc)
					{
						numWayAlloc += 1000;
						ptrMapData->ways = (way*) realloc(ptrMapData->ways, numWayAlloc);
					}
					getWay(osm_file,lineBuffer, ptrMapData, ptrMapData->numWays++);
				} else
				if(!strncmp(tagName, "relation", 8))
				{
					ptrMapData->numRelations++;
				} else
				if(!strncmp(tagName, "tag", 3))
				{
					ptrMapData->numTags++;
				}
			}
		} else
			printf("Broken line: %s", lineBuffer);
	}
	
	printf("Nodes: %d | Ways: %d | Relations: %d\n", ptrMapData->numNodes, ptrMapData->numWays, ptrMapData->numRelations);

	// free used memory
	free(lineBuffer);
	free(getNode_valString);
	free(tagName);
	time = ((clock() - time));
	printf("Loading took %lldms\n", time);
}

static inline unsigned long long getOSMValueULL(const char *src, const char *key)
{
	static char buffer[64];
	if(src == 0 || key == 0 || src[0] == 0 || key[0] == 0)
	{
		printf("Error: invalid query.(src:%s, key:%s)\n", src, key);
		return 0;
	}

	char *start = strstr(src, key);
	if(start == 0)
	{
		printf("key '%s' not found\n", key);
		return 0;
	}
	start += strlen(key) + 2;
	char *end = strstr(start, "\"");

	return strtoull(start, &end, 10);
}

static inline long double getOSMValueLD(const char *src, const char *key)
{
	static char buffer[64];
	if(src == 0 || key == 0 || src[0] == 0 || key[0] == 0)
	{
		printf("Error: invalid query.(src:%s, key:%s)\n", src, key);
		return 0;
	}

	char *start = strstr(src, key);
	if(start == 0)
	{
		printf("key '%s' not found\n", key);
		return 0;
	}
	start += strlen(key) + 2;
	char *end = strstr(start, "\"");

	return strtod(start, &end);
}

void getBounds(char *src, osm *in)
{
	in->minLat = getOSMValueLD(src, "minlat");
	in->minLon = getOSMValueLD(src, "minlon");
	in->maxLat = getOSMValueLD(src, "maxlat");
	in->maxLon = getOSMValueLD(src, "maxlon");
	
	in->deltaLat = in->maxLat - in->minLat;
	in->deltaLon = in->maxLon - in->minLon;

	printf("Bounds:\n\tminLat: %lf\n\tminLon: %lf\n\tmaxLat: %lf\n\tmaxLon: %lf\n\tdLat: %lf\n\tdLon: %lf\n", in->minLat, in->minLon, in->maxLat, in->maxLon, in->deltaLat, in->deltaLon);
}

static inline void getNode(char *src, osm *osmData, int index)
{
	node *thisNode = &osmData->nodes[index];
	thisNode->id = getOSMValueULL(src, "id");
	thisNode->latitude = 2.0f * ((getOSMValueLD(src, "lat") - osmData->minLat) / osmData->deltaLat) - 1.0f;
	thisNode->longitude = 2.0f * ((getOSMValueLD(src, "lon") - osmData->minLon) / osmData->deltaLon) - 1.0f;
}

static inline void getWay(FILE *osmFile, char* lineBuffer, osm *mapData, int index)
{
	unsigned int numNodeAlloc = 100;
	id_t currId = 0;

	mapData->ways[index].nodes = malloc(numNodeAlloc * sizeof(node*));
	node **nodeList = mapData->ways[index].nodes;
	int *numNodes = &mapData->ways[index].numNodes;
	*numNodes = 0;

	while(!strstr(lineBuffer, "</way>"))
	{
		if(strstr(lineBuffer, "<nd"))
		{
			if(*numNodes >= numNodeAlloc)
			{
				numNodeAlloc += 100;
				nodeList = realloc(nodeList, numNodeAlloc * sizeof(node*));
				if(nodeList == NULL)
				{
					printf("ERROR ALLOCATING MEMORY");
					_pause
				}
			}

			currId = getOSMValueULL(lineBuffer, "ref");

			for(int n = 0; n < mapData->numNodes; n++)
			{
				if(mapData->nodes[n].id == currId)
				{
					nodeList[*numNodes] = &mapData->nodes[n];
					*numNodes += 1;
					//printf("found node %d: (%llu<%Lf>)\n", *numNodes, mapData->nodes[n].id, mapData->nodes[n].latitude);
					//_pause
				}
			}
		} else
		if(strstr(lineBuffer, "<way"))
		{
			mapData->ways[index].id = getOSMValueULL(lineBuffer, "id");
		}
		fgets(lineBuffer, 1024, osmFile);
		if(feof(osmFile)) break;
	}
	*numNodes -= 1;
	nodeList = realloc(nodeList, *numNodes * sizeof(node*));
}

void osmPrintWay(way *wayptr)
{
	printf("Way '%llu':\n", wayptr->id);
	node *nd = *wayptr->nodes;
	for(int n = 0; n < wayptr->numNodes; n++)
	{
		printf("\t%llu: <%Lf, %Lf>,\n", nd->id, nd->longitude, nd->latitude);
		nd++;
	}
}

/* TODO: implement properly */
void osmCacheSave(char* cacheFileName, osm* mapData)
{
	FILE *cacheFile = fopen(cacheFileName, "wb");
	// write osm sections to file
	/*
	file structure:
		* bounds data
		  [

		  ]
		* num ways
		  [way data block]
		* num nodes
		  [node data block]
		* num tags
		  [tag data block]
		* num relations
		  [relation data block]
	*/
	// 6 floats
	fwrite(&mapData->minLat, sizeof(double), 6, cacheFile);
	// 1 int
	fwrite(&mapData->numWays, sizeof(int), 1, cacheFile);
	// (way struct) * numWays
	//fwrite(&mapData->ways, sizeof(way), mapData->numWays, cacheFile);
	// (node struct) * numNodes
	fwrite(mapData->nodes, sizeof(way), mapData->numNodes, cacheFile);
	fclose(cacheFile);
}

/* TODO: implement properly */
void osmCacheLoad(char *cacheFileName, osm **mapData)
{
	FILE *cacheFile = fopen(cacheFileName, "rb");

	*mapData = malloc(sizeof(osm));
	// read bounds
	fread(*mapData, sizeof(double), 6, cacheFile);
	fclose(cacheFile);
	return;
}

void osmFree(osm **osmData)
{
	printf("Cleaning up osmloader memory...\n");
	// TODO: IMPLEMENT! -> clean up all memory used by osm struct and its children
	printf("\tCleaning ways...\n");
	free((*osmData)->ways);
	printf("\tCleaning nodes...\n");
	free((*osmData)->nodes);
	printf("\tCleaning relations...\n");
	free((*osmData)->relations);

	free(*osmData);
	printf("\tDone!\n");
}