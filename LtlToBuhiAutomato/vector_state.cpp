//
//  vector_state.cpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 12.12.2021.
//

#include "vector_state.hpp"

bool VectorState::next(std::vector<const model::ltl::Formula*>* trues) {
    if (state >= maxState) {
        return false;
    }
    bool bufferVec[set->size()];
    for (int i = 0 ; i < set->size(); i++) {
        bufferVec[i] = false;
    }
    
    int buffer = state;
    int index = 0;
    
    while (buffer != 0) {
        if (buffer%2 == 1) {
            bufferVec[index] = true;
        }
        buffer /= 2;
        ++index;
    }
    
    --index;
    
    for(;index>=0;--index){
        if (bufferVec[index]) {
            trues->push_back((*set)[index]);
        }
    }
    
    state++;
    return true;
}
