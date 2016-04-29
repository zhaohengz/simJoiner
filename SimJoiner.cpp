#include "SimJoiner.h"


#define MAX_LENGTH 256
#define EPS 1E-6

using namespace std;

SimJoiner::SimJoiner() {

    for (int i = 0; i < 257; i++)
    {
        d[i][0] = i;
    }

    for (int j = 0; j < 257; j++)
    {
        d[0][j] = j;
    }
}

SimJoiner::~SimJoiner() {
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


int SimJoiner::joinJaccard(const char *filename1, const char *filename2, double threshold, vector<JaccardJoinResult> &result) {
    result.clear();
    return SUCCESS;
}

int SimJoiner::joinED(const char *filename1, const char *filename2, unsigned threshold, vector<EDJoinResult> &result) {
    result.clear();
    createIndexED(filename2, threshold);
    ifstream fp(filename1);
    char line[260];
    const int buffer_size = 260;
    vector<pair<unsigned, unsigned> > resultED;
    int id1 = 0;
    while(fp.getline(line, buffer_size))
    {
    	searchED(line, threshold, resultED);
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
    fp.close();
    return SUCCESS;
}

int SimJoiner::createIndexED(const char* filename, unsigned threshold)
{
    for (int i = 0; i < 257; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            m_map_ED[i][j].clear();
            m_substring_len[i][j] = 0;
        }
    }
    m_string.resize(0);
    ifstream fp(filename);
    char line[260];
    char temp[MAX_THRESHOLD];
    const int buffer_size = 260;
    m_idx = 0;
    while(fp.getline(line, buffer_size))
    {
        m_string.push_back(line);
        int length = strlen(line);
        int step_u, step_d;
        if ((length % (threshold + 1)) == 0)
        {
            step_u = step_d = length / (threshold + 1);
        }
        else
        {
            step_d = length / (threshold + 1);
            step_u = step_d + 1;
        }
        temp[step_d] = 0;
        int i = 0;
        while (unsigned(i * step_d) < (length - (length % (threshold + 1) * step_u)))
        {
            memcpy(temp, line + i * step_d, step_d);
            m_substring_len[length][i] = step_d;
            auto it = m_map_ED[length][i].find(temp);
            if (it == m_map_ED[length][i].end())
            {
                vector<int> *tempVec = new vector<int>();
                tempVec->push_back(m_idx);
                m_map_ED[length][i][temp] = tempVec;
            }
            else
            {
                it->second->push_back(m_idx);
            }
            i++;
        }
        int j = 0;
        temp[step_u] = 0;
        while (i * step_d + j * step_u < length)
        {
            memcpy(temp, line + i * step_d + j * step_u, step_u);
            auto it = m_map_ED[length][i + j].find(temp);
            m_substring_len[length][i+j] = step_u;
            if (it == m_map_ED[length][i + j].end())
            {
                vector<int> *tempVec = new vector<int>();
                tempVec->push_back(m_idx);
                m_map_ED[length][i + j][temp] = tempVec;
            }
            else
            {
                it->second->push_back(m_idx);
            }
            j++;
        }
        m_idx ++;
    }
    fp.close();
    /*for (int i = 0; i < 260; i++)
    {
        for (int j = 0; j < MAX_THRESHOLD; j++)
        {
            if (!m_map_ED[i][j].empty())
            {
                std::cout << "length: " << i << "loc: " << j << endl;
                for (auto& it : m_map_ED[i][j])
                {
                    cout << it.first <<  ' ';
                    for (auto& jt : *it.second)
                    {
                        cout << jt << ' ' ;
                    }
                }
                cout << endl;
            }
        }
    }*/
    return SUCCESS;
}

int SimJoiner::searchED(const char* query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> >& result)
{
    result.clear();
    std::vector<int> candidates;
    int length = strlen(query);
    char temp[MAX_THRESHOLD];
    int upperbound = min(length + threshold, 256);
    int lowerbound = max(0, length - threshold);
    for (int i = lowerbound; i <= upperbound; i++)
    {
        int idx = 0;
        int p = 0;
        int delta = length - i;
        while (m_substring_len[i][idx] > 0)
        {
            //cout << m_substring_len[i][idx] << endl;
            int offset = max(p - idx, p + delta - ((int)threshold - idx));
            int offset_bound = min(p + idx, p + delta + ((int)threshold - idx));
            while (offset + m_substring_len[i][idx] <= length)
            {
                if (offset >= 0)
                {
                    memcpy(temp, query + offset, m_substring_len[i][idx]);
                    temp[m_substring_len[i][idx]] = 0;
                    //std::cout << temp << endl;
                    auto it = m_map_ED[i][idx].find(temp);
                    if (it != m_map_ED[i][idx].end())
                    {
                        for (auto& candi : *it->second)
                        {
                            candidates.push_back(candi);
                        }
                    }
                }
                offset++;
                if (offset > offset_bound) break;
            }
            p += m_substring_len[i][idx];
            idx++;
        }
    }

    std::sort(candidates.begin(), candidates.end());
    auto it = std::unique(candidates.begin(), candidates.end());
    candidates.resize(std::distance(candidates.begin(), it));

    for (int i = 0; i < candidates.size(); i++)
    {
        int temp = candidates[i];
        //cout << m_string[temp] << endl;
        int dps = DP(query, temp, length, threshold);
        if (dps >= 0) result.push_back(std::make_pair(temp, dps));
    }

    /*for (auto& it : result)
    {
        cout << it.first << ' ' << it.second << endl;
    }*/
    return SUCCESS;
}

void SimJoiner::generateCandidates(const char* query, unsigned loc, unsigned length, unsigned threshold, std::vector<int>& candidates)
{
    //std::cout << query << endl;
}

int SimJoiner::DP(const char* query, int item, int length_query, int thres)
{
    //int d[257][2 * thres + 2];
    std::string str = m_string[item];
    int length_str = str.size();
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
