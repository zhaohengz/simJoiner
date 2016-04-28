#include "SimJoiner.h"
#include "SimSearcher.h"

using namespace std;

SimJoiner::SimJoiner() {
}

SimJoiner::~SimJoiner() {
}

int SimJoiner::joinJaccard(const char *filename1, const char *filename2, double threshold, vector<JaccardJoinResult> &result) {
    result.clear();
    return SUCCESS;
}

int SimJoiner::joinED(const char *filename1, const char *filename2, unsigned threshold, vector<EDJoinResult> &result) {
    result.clear();
    searcher.createIndexED(filename2, 3);
    ifstream fp(filename1);
    char line[260];
    const int buffer_size = 260;
    vector<pair<unsigned, unsigned> > resultED;
    int id1 = 0;
    while(fp.getline(line, buffer_size))
    {
    	searcher.searchED(line, threshold, resultED);
    	for (auto& it:resultED)
    	{
    		EDJoinResult temp;
    		temp.id1 = id1;
    		temp.id2 = it.first;
    		temp.s = it.second;
    		result.push_back(temp);
    	}
    	id1++;
    }
    return SUCCESS;
}
