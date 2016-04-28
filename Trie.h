#ifndef __TRIE_H__
#define __TRIE_H__ 

#include <vector>
#include <cstring>
#include <cstdlib>

#define ASCIISIZE 127

struct TrieNode
{
	int m_child_nodes[128];
	std::vector<int> m_list; 

	TrieNode();
	~TrieNode();
};

class Trie
{

	TrieNode* m_pool;
	std::vector<int> m_empty_list;
	int m_idx;

public:
	Trie();
	~Trie();

	void insert(char* str, int idx, int length);
	void insert(const char* str, int idx);	
	std::vector<int> * searchStr(char* str, int length);
	std::vector<int> * searchStr(const char* str);

};




#endif