#pragma once

#include <vector>
#include <utility>
#include <string>
#include <unordered_map>

#include "Trie.h"

#define MAX_ITEM 1000000

const int SUCCESS = 0;
const int FAILURE = 1;

struct sortItem
{
	std::vector<int> * data;
	int size;

	sortItem(std::vector<int> * data)
	{
		this->data = data;
		size = data->size();
	}

	sortItem(){}
	~sortItem(){}
};

class SimSearcher
{
public:
	SimSearcher();
	~SimSearcher();

	std::unordered_map<std::string, std::vector<int> *> m_map_ED;
	Trie* m_tree_ED;
	Trie* m_tree_Jaccard;
	std::vector<std::vector<std::string> > m_string_grams;
	std::vector<int> m_gram_length[257];
	char** m_string_list;
	int* m_string_size, *searchQueue, *searchList;
	int m_q;
	int m_idx;
	int m_min_gram;
	int d[257][257];
	sortItem* lists;

	int createIndexED(const char *filename, unsigned q);
	int createIndexJaccard(const char* filename, unsigned q);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
	int DP(const char *query, int item, int length_query, int thres);
};

