#include "SimSearcher.h"
#include <fstream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <iostream>

#define MAX_LENGTH 256
#define EPS 1E-6

using namespace std;

bool operator < (const sortItem& a, const sortItem& b)
{
	return a.size < b.size;	
}


int min(int a, int b)
{
	if (a > b) return b;
	return a;
}

int max(int a, int b)
{
	if (a > b) return a;
	return b;
}

double max(double a, double b)
{
	if (a > b) return a;
	return b;
}

SimSearcher::SimSearcher()
{
	//m_tree_ED = new Trie();
	//m_tree_Jaccard = new Trie();
	m_string_size = new int[MAX_ITEM];
	m_string_list = new char*[MAX_ITEM];
	//searchQueue = new int[MAX_ITEM];
    //searchList = new int[MAX_ITEM];
    lists = new sortItem[MAX_ITEM];
   	//m_min_gram = 400;

	for (int i = 0; i < 257; i++)
	{
		d[i][0] = i;
	}

	for (int j = 0; j < 257; j++)
	{
		d[0][j] = j;
	}
}

SimSearcher::~SimSearcher()
{
	//delete m_tree_ED;
	//delete m_tree_Jaccard;
	//delete[] m_string_list;
	//delete[] m_string_size;
	delete[] searchQueue;
	delete[] searchList;
}

int SimSearcher::createIndexED(const char *filename, unsigned q)
{
	m_q = q;
	m_idx = 0;
	m_map_ED.clear();
	ifstream fp(filename);
    char line[260];
    char temp[10];
    memset(temp, 0, sizeof(temp));
    const int buffer_size = 260;
    while(fp.getline(line, buffer_size))
    {
    	//cout << line << endl;
        int length = strlen(line);
        m_string_list[m_idx] = new char[257];
        memcpy(m_string_list[m_idx], line, length);
        m_string_size[m_idx] = length;
      	for (int i = 0; i < length - q + 1; i++)
       	{
       		//create ED
       		memcpy(temp, line + i, m_q);
       		temp[m_q] = 0;
       		auto it = m_map_ED.find(temp);
       		if (it == m_map_ED.end())
       		{
       			std::vector<int>* tempVec = new vector<int>;
       			tempVec->push_back(m_idx);
       			m_map_ED[temp] = tempVec;
       		}
       		else
       		{
       			it->second->push_back(m_idx);
       		}
       	}
        m_idx++;
    }
	return SUCCESS;
}

int SimSearcher::createIndexJaccard(const char *filename, unsigned q)
{
	m_q = q;
	m_idx = 0;
	ifstream fp(filename);
    char line[260];
    const int buffer_size = 260;
    while(fp.getline(line, buffer_size))
    {
    	m_string_grams.push_back(std::vector<std::string>());
    	//cout << line << endl;
        int length = strlen(line);
        m_string_list[m_idx] = new char[257];
        memcpy(m_string_list[m_idx], line, length);
        m_string_size[m_idx] = length;
      	for (int i = 0; i < length - q + 1; i++)
       	{
       		//create ED
       		m_tree_ED->insert(line + i, m_idx, m_q);
       	}
       	//create Jaccard
       	string temp;
       	for (int i = 0; i < length; i++)
       	{
       		if (line[i] == ' ')
       		{
       			m_string_grams[m_idx].push_back(temp);
       			temp = "";
       			while (line[i] == ' ')
       			{
       				i++;
       			}	
       			i--;
       		}
       		else
       		{
       			temp += line[i];
       		}
       	}
       	if (temp.size() > 0) m_string_grams[m_idx].push_back(temp);
       	
       	std::sort(m_string_grams[m_idx].begin(), m_string_grams[m_idx].end());
       	auto it = std::unique(m_string_grams[m_idx].begin(), m_string_grams[m_idx].end());
       	m_string_grams[m_idx].resize(std::distance(m_string_grams[m_idx].begin(), it));

       	m_gram_length[m_string_grams[m_idx].size()].push_back(m_idx);
       	if (m_string_grams[m_idx].size() < m_min_gram)
       	{
       		m_min_gram = m_string_grams[m_idx].size();
       	}

       	for (auto str = m_string_grams[m_idx].begin(); str != m_string_grams[m_idx].end(); str++)
       	{
       		m_tree_Jaccard->insert(str->c_str(), m_idx);
       	}
        m_idx++;
    }

	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result)
{
	result.clear();
	std::vector<std::string> query_grams;
	int length_query = strlen(query);

	string temp;
    for (int i = 0; i < length_query; i++)
    {
      	if (query[i] == ' ')
       	{
       		query_grams.push_back(temp);
       		temp = "";
       		while (query[i] == ' ')
       		{
       			i++;
       		}	
       		i--;
       	}
       	else
       	{
       		temp += query[i];
       	}
    }
    if (temp.size() > 0) query_grams.push_back(temp);
    std::sort(query_grams.begin(), query_grams.end());
    auto it = std::unique(query_grams.begin(), query_grams.end());
    query_grams.resize(std::distance(query_grams.begin(), it));

    int grams_query = query_grams.size();

	int t = max(threshold * grams_query, double(grams_query + m_min_gram)) * threshold / (1 + threshold);

	std::vector<int> freqList;
	std::vector<int>* ptr;
	int count = 0;
	for (int i = 0; i < grams_query; i++)
	{
		ptr = m_tree_Jaccard->searchStr(query_grams[i].c_str());
		if (ptr != NULL) 
		{
			/*sortItem item((std::vector<int>*)ptr);
			if (item.size > 0) lists[lists_idx++] = item;*/
			for (auto it = ptr->begin(); it != ptr->end(); it++)
			{
				freqList.push_back(*it);
				count++;
			}
			std::inplace_merge(freqList.begin(), freqList.begin() + count - ptr->size(), freqList.end());
		}
	}

	count = 0;
	int pivot = -1;
	for (int i = 0; i < freqList.size(); i++)
	{
		if (pivot = -1) 
		{
			pivot = freqList[i];
			count = 1;
			continue;
		}
		if (pivot == freqList[i])
		{
			count++;
		}
		else
		{
			double freq = count;
			double jaccard = freq / (grams_query + i - freq);
			if (jaccard - threshold > -EPS)result.push_back(std::make_pair(pivot, jaccard));
			count = 0;
			pivot = freqList[i];
		}
		
	}

	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result)
{
	result.clear();
	int thres = threshold;
	int length_query = strlen(query);
	int t = length_query - m_q + 1 - thres * m_q;
	int dps;
	
	if (t <= 0)
	{
		for (int i = 0; i < m_idx; i++)
		{
			dps = DP(query, i, length_query, thres);
	    	if (dps >= 0) result.push_back(std::make_pair(i, dps));
		}
		return SUCCESS;
	}


	int lists_idx = 0;
	void* ptr;
	//char line [257];
	//memcpy(line, query, length_query + 1);
	char temp[10];
    

	for (int i = 0; i < length_query - m_q + 1; i++)
    {
    	memcpy(temp, (char*) query + i, m_q);
    	temp[m_q] = 0;
       	ptr = m_map_ED[temp];
       	//char temp = line[i + m_q];
       	//line[i + m_q] = 0;
       	//auto it = m_map.find(line + i);
		if (ptr != NULL) 
		{
			sortItem item((std::vector<int>*)ptr);
			if (item.size > 0) lists[lists_idx++] = item;
		}
    }

    std::sort(lists, lists + lists_idx);
    std::vector<int> verifyList;

	for (int i = 0; i < lists_idx - t + 1; i++)
	{
	  	for (int j = 0; j < lists[i].size; j++)
	  	{
	   		int temp = (*(lists[i].data))[j];
	   			
	   		verifyList.push_back(temp);
	   		//dps = DP(query, temp, length_query, thres);
	   	}
	}

	std::sort(verifyList.begin(), verifyList.end());
	auto it = std::unique(verifyList.begin(), verifyList.end());
	verifyList.resize(std::distance(verifyList.begin(), it));
	for (int i = 0; i < verifyList.size(); i++)
	{
		int temp = verifyList[i];
		dps = DP(query, temp, length_query, thres);
		if (dps >= 0) result.push_back(std::make_pair(temp, dps));
	}

	//auto it = std::unique(result.begin(), result.end());
	//result.resize(std::distance(result.begin(),it));

	/*for (int i = 0; i < length_query - m_q + 1; i++)
    {
       	ptr = m_tree_ED->searchStr((char*)query + i, m_q);
		if (ptr != NULL) 
		{
			sortItem item((std::vector<int>*)ptr);
			//if (item.size > 0) lists[lists_idx++] = item;
		}
    } */
	return SUCCESS;
}

int SimSearcher::DP(const char* query, int item, int length_query, int thres)
{
	//int d[257][2 * thres + 2];
	char* str = m_string_list[item];
	int length_str = m_string_size[item];
	if (abs(length_str - length_query) > thres) return -1;

	bool exceed = true;
	for (int i = 1; i <= length_str; i++)
	{
		exceed = true;
		int j1 = max(1, i - thres);
		int j2 = min(length_query, i + thres);
		for (int j = j1; j <= j2; j++)
		{
			int t = (str[i - 1] == query[j - 1])? 0 : 1;
			int temp1 = d[i-1][j] + 1;
			int temp2 = d[i][j-1] + 1;
			if (j + 1 - i > thres) temp1 = MAX_LENGTH;
			if (i + 1 - j > thres) temp2 = MAX_LENGTH;
			d[i][j] = min(temp1, temp2);
			d[i][j] = min(d[i][j], d[i-1][j-1] + t);
			if (d[i][j] <= thres) exceed = false;
		}
		if (d[i][0] <= thres) exceed = false;
		if (exceed) return -1;
	}
	if (d[length_str][length_query] > thres) return -1;
	return d[length_str][length_query];
}
