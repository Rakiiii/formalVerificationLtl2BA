//
//  bad_formulas_set.cpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 12.12.2021.
//

#include "bad_formulas_set.hpp"

bool FormulaSet::insert(const model::ltl::Formula* formula) {
    for(auto elem = set->begin(); elem != set->end(); ++elem) {
        if ((**elem) == (*formula)) {
            return false;
        }
    }
        
    this->set->push_back(formula);
    return true;
}
