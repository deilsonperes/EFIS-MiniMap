#include "osmloader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define _pause system("pause");
/* global variables */
char *getNode_valString;

void getTagName(char*, char**);
void getBounds(char*, osm**);
static inline void getNode(char*, osm**, int);
static inline void getWay(FILE*, char*, osm**, int);

void osmLoad(char* fileName, osm **mapData)
{
	time_t time = clock();
	*mapData = (osm*) malloc(sizeof(osm));
	osm *ptrMapData = *mapData;
	ptrMapData->numNodes = 0;
	ptrMapData->numWays = 0;
	ptrMapData->numTags = 0;
	ptrMapData->numRelations = 0;

	printf("Loading OSM data from file '%s'.\n", fileName);
	FILE* osm_file = fopen(fileName, "rb");
	if(osm_file == NULL)
	{
		printf("Could not open file \'%s\'\n", fileName);
		return;
	}

	getNode_valString = (char*) calloc(32, sizeof(char));
	char *lineBuffer = (char*) calloc(1024, sizeof(char)),
		  *tagStart,
		  *tagName = (char*) calloc(32, sizeof(char)),
		  
		  *valueStart;
	
	int nodeMemCount = 1000,
		 wayMemCount = 1000,
		 relationMemCount = 1000,
		 tagMemCount = 1000,
		 progCnt = 100;

	// allocate memory for structs
	ptrMapData->nodes = (node*) malloc(sizeof(node) * nodeMemCount);
	ptrMapData->ways = (way*) malloc(sizeof(way) * wayMemCount);
	ptrMapData->relations = (relation*) malloc(sizeof(relation) * relationMemCount);
	ptrMapData->tags = (tag*) malloc(sizeof(tag) * tagMemCount);
	getNode_valString = (char*) calloc(64, sizeof(char));

	fgets(lineBuffer, 1024, osm_file);
	while(!feof(osm_file))
	{
		if(tagStart = strstr(lineBuffer, "<"))
		{
			// if next char is '/' this is a tag ending
			if(tagStart[1] == '/')
			{
				;//printf("Tag ending: %s", lineBuffer);
			} else {
				getTagName(tagStart, &tagName);
				if(!strcmp(tagName, "bounds"))
				{
					getBounds(tagStart, &ptrMapData);
				} else
				if(!strcmp(tagName, "node"))
				{
					if(ptrMapData->numNodes >= nodeMemCount)
					{
						nodeMemCount += 1000;
						ptrMapData->nodes = (node*) realloc(ptrMapData->nodes, sizeof(node) * nodeMemCount);
					}
					getNode(lineBuffer, &ptrMapData, ptrMapData->numNodes++);
				} else
				if(!strcmp(tagName, "way"))
				{
					if(ptrMapData->numWays >= wayMemCount)
					{
						wayMemCount += 1000;
						ptrMapData->numWays = wayMemCount;
						ptrMapData->ways = realloc(ptrMapData->ways, sizeof(way) * wayMemCount);
					}
					getWay(osm_file, lineBuffer, &ptrMapData, ptrMapData->numWays);
				} else
				if(!strcmp(tagName, "relation"))
				{
					ptrMapData->numRelations++;
				} else
				if(!strcmp(tagName, "tag"))
				{
					ptrMapData->numTags++;
				}
			}
		} else
			printf("Broken line: %s", lineBuffer);

		// read new line here to avoid missing feof()
		fgets(lineBuffer, 512, osm_file);
	}
	
	printf("Nodes: %d | Ways: %d | Relations: %d\n", ptrMapData->numNodes, ptrMapData->numWays, ptrMapData->numRelations);

	// free used memory
	free(lineBuffer);
	free(getNode_valString);
	free(tagName);
	time = ((clock() - time));
	printf("Loading took %lldms\n", time);
}

void getTagName(char *src, char **dest)
{
	while(*src == '<' || *src == '/')
		src++;
	memset(*dest, 0, 32);
	strncpy(*dest, src, strstr(src, " ") - src);
}

static inline void getNode(char *src, osm **osmData, int index)
{
	node *nptr = &(*osmData)->nodes[index];
	
	// id
	char *valueStart = strstr(src, "id=") + 4;
	int valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	nptr->id = atoll(getNode_valString);
	
	// lat
	valueStart = strstr(src, "lat=") + 5;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	nptr->latitude = 2 * ((atof(getNode_valString) - (*osmData)->minLat) / (*osmData)->deltaLat) - 1;
	
	// lon
	valueStart = strstr(src, "lon=") + 5;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	nptr->longitude = 2 * ((atof(getNode_valString) - (*osmData)->minLon) / (*osmData)->deltaLon) - 1;
}

static inline void getWay(FILE *osmFile, char* lineBuffer, osm **mapData, int index)
{
	// a pointer to the list of ways
	way *wayList = (*mapData)->ways;
	int *numWays = &(*mapData)->numWays;
	*numWays += 1;
	int nodesMemCount = 100;

	// allocate initial quantity of memory to be used


	// go through the lines until </node>
	while(!strstr(lineBuffer, "</way>"))
	{
		if(!strstr(lineBuffer, "<nd "))
		{
			
		}
		fgets(lineBuffer, 1024, osmFile);
	}
}

void getBounds(char *src, osm **in)
{
	char *valueStart = strstr(src, "minlat=") + 8;
	int valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	(*in)->minLat = atof(getNode_valString);	
	memset(getNode_valString, 0, valLength);
 
	valueStart = strstr(src, "maxlat=") + 8;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	(*in)->maxLat = atof(getNode_valString);
	memset(getNode_valString, 0, valLength);

	valueStart = strstr(src, "minlon=") + 8;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	(*in)->minLon = atof(getNode_valString);
 	memset(getNode_valString, 0, valLength);

	valueStart = strstr(src, "maxlon=") + 8;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	(*in)->maxLon = atof(getNode_valString);

	(*in)->deltaLat = (*in)->maxLat - (*in)->minLat;
	(*in)->deltaLon = (*in)->maxLon - (*in)->minLon;

	printf("Bounds:\n\tminLat: %lf\n\tminLon: %lf\n\tmaxLat: %lf\n\tmaxLon: %lf\n\tdLat: %lf\n\tdLon: %lf\n", (*in)->minLat, (*in)->minLon, (*in)->maxLat, (*in)->maxLon, (*in)->deltaLat, (*in)->deltaLon);
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