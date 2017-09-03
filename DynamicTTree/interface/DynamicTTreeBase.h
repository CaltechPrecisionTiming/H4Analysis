#ifndef __DYNAMIC_TTREE_BASE__
#define __DYNAMIC_TTREE_BASE__

#include <typeindex>

#include "TFile.h"
#include "TChain.h"

template<typename T> struct argument_type;
template<typename T, typename U> struct argument_type<T(U)> { typedef U type; };

class DynamicTTreeBase
{
public:
    //---ctor---
    DynamicTTreeBase(): currentEntry_(-1) {};

    //---dtor---
    virtual ~DynamicTTreeBase() {};

    //---getters---
    TTree* GetTTreePtr() { return tree_; };

    //---utils---
    bool NextEntry(long int entry=-1);
    long int getCurrentEntry () {return currentEntry_;};
    long int getNEntries () {return tree_->GetEntriesFast();};

protected:
    long int currentEntry_;
    TTree*   tree_;
    static std::map<std::type_index, const char*> type_map;
};

#endif
