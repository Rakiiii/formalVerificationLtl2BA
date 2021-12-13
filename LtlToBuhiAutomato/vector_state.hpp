//
//  vector_state.hpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 12.12.2021.
//

#ifndef vector_state_hpp
#define vector_state_hpp

#include "ltl.h"

#include <vector>
#include <cmath>

class VectorState {
private:
    std::vector<const model::ltl::Formula*> *set;
    int state;
    int maxState;
public:
    VectorState(std::vector<const model::ltl::Formula*>* init) : set(init), state(0) { maxState = (int)pow(2.0,init->size()); }
    bool next(std::vector<const model::ltl::Formula*>* trues);
};

#endif /* vector_state_hpp */
