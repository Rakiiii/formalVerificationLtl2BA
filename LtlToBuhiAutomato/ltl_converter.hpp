//
//  ltl_converter.hpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 01.12.2021.
//

#ifndef ltl_converter_hpp
#define ltl_converter_hpp

#include <iostream>

#include <vector>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <utility>

#include "ltl.h"
#include "fsm.h"
#include "convert_node.hpp"
#include "bad_formulas_set.hpp"
#include "vector_state.hpp"
#include "state.hpp"

const std::string FALSE_PROP = *(new std::string("false"));
const std::string TRUE_PROP = *(new std::string("true"));

class LtlConverter {
private:
    int lastIndex = 0;
    std::map<std::string,int> indexMap{};
    void copyNodes(ConvertNode* newNode, ConvertNode* currentNode);
    const model::ltl::Formula& trivializeX(const model::ltl::Formula& formula) const;
    const model::ltl::Formula& trivialize(const model::ltl::Formula& formula) const;
    
    void constructAllSubFormulas(const model::ltl::Formula& formula,FormulaSet *subFormulas);
    
    void addInverse(FormulaSet *subFormulas);
    
    void findAtoms(std::vector<const model::ltl::Formula*> *subFormulas,FormulaSet *atoms);
    void findPureAtoms(std::vector<const model::ltl::Formula*> *atoms,FormulaSet *pureAtoms);
    void findFalseAtoms(std::vector<const model::ltl::Formula*> *atoms,std::vector<const model::ltl::Formula*> *falses,FormulaSet *trues);
    
    void findPures(std::vector<const model::ltl::Formula*> *subFormulas,FormulaSet *pureAtoms,FormulaSet *trues, std::set<std::string> &marks);
    
    bool findInFormulas(std::vector<const model::ltl::Formula*> *searchSet, const model::ltl::Formula* expected);
    
    void constructStates(const model::ltl::Formula& formula);
    
    void constructRules(std::vector<const model::ltl::Formula*> *subFormulas);
    
    void constructTransitions();
    
    void findStartStates(const model::ltl::Formula& formula);
    
    void constructLabels();
    
    void constructAutomaton(model::fsm::Automaton *automaton);
    
    void printState(std::vector<const model::ltl::Formula*> *trues,std::vector<const model::ltl::Formula*> * falses);
    void printTransitionMarks(std::set<std::string> *marks);
    
    std::vector<ConvertNode*>* expand(ConvertNode* currentNode, std::vector<ConvertNode*>* nodes);
public:
    std::vector<State*> states;
    
    model::fsm::Automaton* convert(const model::ltl::Formula& formula);
};

#endif /* ltl_converter_hpp */
