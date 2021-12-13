//
//  state.hpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 12.12.2021.
//

#ifndef state_hpp
#define state_hpp

#include <vector>
#include <set>
#include "bad_formulas_set.hpp"

#include "ltl.h"

class State {
public:
    bool isStartState = false;
    std::string stateLabel = "";
    std::vector<const model::ltl::Formula*> *trues;
    std::vector<const model::ltl::Formula*> *falses;
    std::vector<const model::ltl::Formula*> req;
    std::vector<const model::ltl::Formula*> unreq;
    FormulaSet reqSet;
    FormulaSet unreqSet;
    std::vector<std::string> fLabels;
    std::vector<State*> connectedStates;
    std::set<std::string> transitionMarks{};
    State(std::vector<const model::ltl::Formula*> *t,std::vector<const model::ltl::Formula*> *f) : trues(t), falses(f), reqSet(&req), unreqSet(&unreq) {}
};

#endif /* state_hpp */
