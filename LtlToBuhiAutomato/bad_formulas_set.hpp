//
//  bad_formulas_set.hpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 12.12.2021.
//

#ifndef bad_formulas_set_hpp
#define bad_formulas_set_hpp

#include "ltl.h"

#include <vector>


class FormulaSet {
public:
    std::vector<const model::ltl::Formula*> *set;
    
    FormulaSet(std::vector<const model::ltl::Formula*>* init):set(init) {}
    bool insert(const model::ltl::Formula* formula);
    
};

#endif /* bad_formulas_set_hpp */
