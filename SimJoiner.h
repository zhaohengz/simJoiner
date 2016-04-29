#ifndef __EXP2_SIMJOINER_H__
#define __EXP2_SIMJOINER_H__

#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <algorithm>


template <typename IDType, typename SimType>
struct JoinResult {
    IDType id1;
    IDType id2;
    SimType s;
};

typedef JoinResult<unsigned, double> JaccardJoinResult;
typedef JoinResult<unsigned, unsigned> EDJoinResult;

#define MAX_THRESHOLD 100

const int SUCCESS = 0;
const int FAILURE = 1;

class SimJoiner {
public:
    SimJoiner();
    ~SimJoiner();

    int m_idx;
    int d[257][257];
    std::unordered_map<std::string, std::vector<int>*> m_map_ED[260][15];
    int m_substring_len[260][MAX_THRESHOLD];
    std::vector<std::string> m_string;
    int joinJaccard(const char *filename1, const char *filename2, double threshold, std::vector<JaccardJoinResult> &result);
    int joinED(const char *filename1, const char *filename2, unsigned threshold, std::vector<EDJoinResult> &result);
    int createIndexED(const char* filename, unsigned threshold);
	int searchED(const char* query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> >& result);
	int DP(const char *query, int item, int length_query, int thres);
	void generateCandidates(const char* query, unsigned loc, unsigned length, unsigned threshold, std::vector<int>& candidates);
};

#endif
