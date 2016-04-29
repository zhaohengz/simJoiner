#include "SimJoiner.h"

using namespace std;

int main(int argc, char **argv) {
    SimJoiner joiner;

    vector<EDJoinResult> resultED;
    vector<JaccardJoinResult> resultJaccard;

    unsigned edThreshold = 3;
    double jaccardThreshold = 0.85;

    joiner.joinJaccard(argv[1], argv[2], jaccardThreshold, resultJaccard);
    joiner.joinED(argv[1], argv[2], edThreshold, resultED);
    for (auto& it : resultED)
    {
    	cout << it.id1 << ' ' << it.id2 << ' ' << it.s << endl;
    }

    return 0;
}
