#include "Trie.h"
#include <cstdio>
#include <iostream>

#define MAXITEM 2000000

TrieNode::TrieNode()
{
	for (int i = 0; i < 128; i++)
	{
		m_child_nodes[i] = 0;
	}
}

TrieNode::~TrieNode()
{

}

Trie::Trie()
{
	m_pool = new TrieNode[MAXITEM];
	m_idx = 1;
}

Trie::~Trie()
{
	delete[] m_pool;
}

void Trie::insert(char* str, int idx, int length)
{
	int node = 0;
	int it = 0;
	while (it < length)
	{
		int index = str[it];
		if (m_pool[node].m_child_nodes[index] == 0) m_pool[node].m_child_nodes[index] = m_idx++;
		node = m_pool[node].m_child_nodes[index];
		it++;
	}
	if (!m_pool[node].m_list.empty() && m_pool[node].m_list[m_pool[node].m_list.size() - 1] == idx) return;
	m_pool[node].m_list.push_back(idx);
}

void Trie::insert(const  char* str, int idx)
{
	int node = 0;
	int it = 0;
	int index = str[it];
	while (index != 0)
	{
		if (m_pool[node].m_child_nodes[index] == 0) m_pool[node].m_child_nodes[index] = m_idx++;
		node = m_pool[node].m_child_nodes[index];
		it++;
		index = str[it];
	}
	if (!m_pool[node].m_list.empty() && m_pool[node].m_list[m_pool[node].m_list.size() - 1] == idx) return;
	m_pool[node].m_list.push_back(idx);
}


std::vector<int> * Trie::searchStr(char* str, int length)
{
	int node = 0;
	int it = 0;
	while (it < length)
	{
		int index = str[it];
		node = m_pool[node].m_child_nodes[index];
		if (node == 0) return NULL;
		it++;
	}
	return &(m_pool[node].m_list);
}

std::vector<int> * Trie::searchStr(const char* str)
{
	int node = 0;
	int it = 0;
	int index = str[it];
	while (index != 0)
	{
		node = m_pool[node].m_child_nodes[index];
		if (node == 0) return NULL;
		it++;
		index = str[it];
	}
	return &(m_pool[node].m_list);
}