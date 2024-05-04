#ifndef _CANDIDATERESULT_H
#define _CANDIDATERESULT_H

#include <string>
using std::string;

class CandidateResult {
public:
    CandidateResult(const string& word, size_t freq, int dist)
    :_word(word)
    ,_freq(freq)
    ,_dist(dist)
    {}
public: 
    string _word;
    size_t _freq;
    int _dist;
};

// 改为小根堆
inline bool operator<(const CandidateResult& lhs, const CandidateResult& rhs){
    if(lhs._dist != rhs._dist){
        return lhs._dist > rhs._dist;
    }
    else if(lhs._freq != rhs. _freq){
        return lhs._freq > rhs. _freq;
    }
    else{
        return lhs._word > rhs._word;
    }
}

#endif //_CANDIDATERESULT_H
