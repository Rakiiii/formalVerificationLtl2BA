//
//  ltl_converter.cpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 01.12.2021.
//

#include "ltl_converter.hpp"

using namespace model::ltl;

const model::ltl::Formula& LtlConverter::trivialize(const model::ltl::Formula& formula) const {
    switch (formula.kind()) {
        case Formula::Kind::NOT:
            if (formula.arg().kind() == Formula::Kind::NOT) {
                return LtlConverter::trivialize(formula.arg().arg());
            } else if (formula.arg().kind() == Formula::Kind::ATOM && formula.arg().prop() == TRUE_PROP) {
                return P(FALSE_PROP);
            } else if (formula.arg().kind() == Formula::Kind::ATOM && formula.arg().prop() == FALSE_PROP) {
                return P(TRUE_PROP);
            } else {
                return !(LtlConverter::trivialize(formula.arg()));
            }
        case Formula::Kind::AND:
            return LtlConverter::trivialize(formula.lhs()) && LtlConverter::trivialize(formula.rhs());
        case Formula::Kind::OR:
            return LtlConverter::trivialize(formula.lhs()) || LtlConverter::trivialize(formula.rhs());
        case Formula::Kind::IMPL:
            return !LtlConverter::trivialize(formula.lhs()) || LtlConverter::trivialize(formula.rhs());
        case Formula::Kind::X:
            if (trivializeX(formula).kind() == Formula::Kind::X) {
                return X(trivialize(trivializeX(formula).arg()));
            } else {
                return trivialize(trivializeX(formula));
            }
        case Formula::Kind::G:
            return !(U ( P(TRUE_PROP) , ( !(LtlConverter::trivialize(formula.arg() ) ) ) ) );
        case Formula::Kind::F:
            return U ( P(TRUE_PROP) , LtlConverter::trivialize(formula.arg())  );
        case Formula::Kind::U:
            return U ( LtlConverter::trivialize(formula.lhs()) , LtlConverter::trivialize(formula.rhs()) );
        case Formula::Kind::R:
            return !(U ( !LtlConverter::trivialize(formula.lhs()) , !LtlConverter::trivialize(formula.rhs()) ) );
        case Formula::Kind::ATOM:
            return formula;
    }
}

const model::ltl::Formula& LtlConverter::trivializeX(const model::ltl::Formula& formula) const {
    switch (formula.arg().kind()) {
        case Formula::Kind::NOT:
            return !(X(formula.arg().arg()));
        case Formula::Kind::AND:
            return X(formula.arg().lhs()) && X(formula.arg().rhs());
        case Formula::Kind::OR:
            return X(formula.arg().lhs()) || X(formula.arg().rhs());
        case Formula::Kind::IMPL:
            return formula;
        case Formula::Kind::X:
            return X(trivializeX(formula.arg()));
        case Formula::Kind::G:
            return formula;
        case Formula::Kind::F:
            return formula;
        case Formula::Kind::U:
            return U(X(formula.arg().lhs()), X(formula.arg().rhs()));
        case Formula::Kind::R:
            return formula;
        case Formula::Kind::ATOM:
            if (formula.arg().prop() == TRUE_PROP) {
                return P(TRUE_PROP);
            } else if(formula.arg().prop() == FALSE_PROP) {
                return P(FALSE_PROP);
            } else{
                return formula;
            }
    }
}

//придумать что делать со структурой данных
void LtlConverter::constructAllSubFormulas(const model::ltl::Formula& formula,FormulaSet *subFormulas) {
    subFormulas->insert(&formula);
    if (formula.isBinary()) {
        constructAllSubFormulas(formula.lhs(), subFormulas);
        constructAllSubFormulas(formula.rhs(), subFormulas);
    } else {
        if (formula.kind() != Formula::Kind::ATOM) {
            constructAllSubFormulas(formula.arg(), subFormulas);
        }
    }
}

void LtlConverter::addInverse(FormulaSet *subFormulas) {
    std::vector<const model::ltl::Formula*> inversedVec;
    FormulaSet inversed(&inversedVec);
    for(auto formula = subFormulas->set->begin(); formula != subFormulas->set->end(); ++formula) {
        if ((*formula)->kind() == Formula::Kind::NOT) {
            inversed.insert(&((*formula)->arg()));
        } else if((*formula)->kind() == Formula::Kind::ATOM && (*formula)->prop() == TRUE_PROP){
            inversed.insert(&P(FALSE_PROP));
        }else if((*formula)->kind() == Formula::Kind::ATOM && (*formula)->prop() == FALSE_PROP){
            inversed.insert(&P(TRUE_PROP));
        }else {
            inversed.insert(&(!(**formula)));
        }
    }
    
    for(auto formula = inversedVec.begin(); formula != inversedVec.end(); ++formula) {
        subFormulas->insert(*formula);
    }
}

void LtlConverter::findAtoms(std::vector<const model::ltl::Formula*> *subFormulas,FormulaSet *atoms) {
    for(auto formula = subFormulas->begin(); formula != subFormulas->end(); ++formula) {
        switch ((*formula)->kind()) {
            case Formula::Kind::X:
                atoms->insert(*formula);
                atoms->insert(&(*formula)->arg());
                break;
            case Formula::Kind::ATOM:
                atoms->insert(*formula);
                break;
            case Formula::Kind::NOT:
                if ((*formula)->arg().kind() == model::ltl::Formula::ATOM || (*formula)->arg().kind() == model::ltl::Formula::X) {
                    atoms->insert(*formula);
                    atoms->insert(&(*formula)->arg());
                }
                break;
            default:
                break;
        }
    }
}

void LtlConverter::findPureAtoms(std::vector<const model::ltl::Formula*> *atoms,FormulaSet *pureAtoms) {
    for(auto formula = atoms->begin(); formula != atoms->end(); ++formula) {
        switch ((*formula)->kind()) {
            case Formula::Kind::NOT:
                break;
            case Formula::Kind::ATOM:
                if ((*formula)->prop() != FALSE_PROP && (*formula)->prop() != TRUE_PROP) {
                    pureAtoms->insert(*formula);
                }
                break;
            default:
                pureAtoms->insert(*formula);
                break;
        }
    }
}

void LtlConverter::findFalseAtoms(std::vector<const model::ltl::Formula*> *atoms,std::vector<const model::ltl::Formula*> *trues,FormulaSet *falses) {
    for(auto formula = atoms->begin(); formula != atoms->end(); ++formula) {
        switch ((*formula)->kind()) {
            case Formula::Kind::NOT:
                if (findInFormulas(trues, &(*formula)->arg())) {
                    falses->insert(*formula);
                }
                break;
            default:
                if ((*formula)->prop() != TRUE_PROP && (*formula)->prop() != FALSE_PROP) {
                    if (!findInFormulas(trues, *formula)) {
                        falses->insert(*formula);
                    }
                }
                break;
        }
    }
}

bool LtlConverter::findInFormulas(std::vector<const model::ltl::Formula*> *searchSet, const model::ltl::Formula* expected) {
    for(auto subFormula = searchSet->begin(); subFormula != searchSet->end(); ++subFormula) {
        if ((**subFormula) == (*expected)) {
            return true;
        }
    }
    return false;
}

void LtlConverter::findPures(std::vector<const model::ltl::Formula*> *subFormulas,FormulaSet *falses,FormulaSet *trues, std::set<std::string> &marks) {
    bool flag = true;
    while(flag) {
        flag = false;
        for(auto subFormula = subFormulas->begin(); subFormula != subFormulas->end(); ++subFormula) {
            switch ((*subFormula)->kind()) {
                case model::ltl::Formula::ATOM:
                    break;
                case model::ltl::Formula::NOT:
                    if (findInFormulas(falses->set, &(*subFormula)->arg())) {
                        bool res = trues->insert(*subFormula);
                        flag = flag || res;
                    } else if (findInFormulas(trues->set, &(*subFormula)->arg())) {
                        bool res = falses->insert(*subFormula);
                        flag = flag || res;
                    }
                    break;
                case model::ltl::Formula::AND:
                    if (findInFormulas(trues->set, &(*subFormula)->lhs()) && findInFormulas(trues->set, &(*subFormula)->rhs())) {
                        bool res = trues->insert(*subFormula);
                        flag = flag || res;
                    } else if (findInFormulas(falses->set, &(*subFormula)->lhs()) || findInFormulas(falses->set, &(*subFormula)->rhs())) {
                        bool res = falses->insert(*subFormula);
                        flag = flag || res;
                    }
                    break;
                case model::ltl::Formula::OR:
                    if (findInFormulas(trues->set, &(*subFormula)->lhs()) || findInFormulas(trues->set, &(*subFormula)->rhs())) {
                        bool res = trues->insert(*subFormula);
                        flag = flag || res;
                    } else if (findInFormulas(falses->set, &(*subFormula)->lhs()) && findInFormulas(falses->set, &(*subFormula)->rhs())) {
                        bool res = falses->insert(*subFormula);
                        flag = flag || res;
                    }
                    break;
                case model::ltl::Formula::IMPL:
                    break;
                case model::ltl::Formula::X:
                    break;
                case model::ltl::Formula::G:
                    break;
                case model::ltl::Formula::F:
                    break;
                case model::ltl::Formula::U:
                    if (findInFormulas(trues->set, &(*subFormula)->rhs())) {
                        bool res = trues->insert(*subFormula);
                        flag = flag || res;
                    } else if (findInFormulas(trues->set, &(*subFormula)->lhs()) && findInFormulas(falses->set, &(*subFormula)->rhs())) {
                        auto newStateTrues = new std::vector<const model::ltl::Formula*>();
                        newStateTrues->reserve(trues->set->size());
                        std::copy(trues->set->begin(), trues->set->end(), std::back_inserter(*newStateTrues));
                        FormulaSet newTruesSet(newStateTrues);
                        newTruesSet.insert(*subFormula);
                        
                        auto newStateFalses = new std::vector<const model::ltl::Formula*>();
                        newStateFalses->reserve(falses->set->size());
                        std::copy(falses->set->begin(), falses->set->end(), std::back_inserter(*newStateFalses));
                        FormulaSet newFalsesSet(newStateFalses);
                        
                        auto newState = new State(newStateTrues,newStateFalses);
                        //не корректно ибо состояние добовляется после этой операции (передавать в метод?)
                        newState->transitionMarks = marks;
                        states.push_back(newState);
                        
                        findPures(subFormulas,&newTruesSet,&newFalsesSet,marks);
                    }
                    break;
                case model::ltl::Formula::R:
                    break;
            }
        }
    }
}

void LtlConverter::constructRules(std::vector<const model::ltl::Formula*> *subFormulas) {
    std::vector<const model::ltl::Formula*> temporalOperators;
    FormulaSet tempSet(&temporalOperators);
    
    for(auto subF = subFormulas->begin(); subF != subFormulas->end(); ++subF) {
        if ((*subF)->kind() == Formula::Kind::U || (*subF)->kind() == Formula::Kind::X) {
            tempSet.insert(*subF);
        }
    }
    
    for (auto state = states.begin(); state != states.end(); ++state) {
        std::stringstream stringBuilder;
        
        for(auto temp = temporalOperators.begin(); temp != temporalOperators.end(); ++temp) {
            if ((*temp)->kind() == Formula::Kind::U) {
                
                bool leftSide = findInFormulas((*state)->trues, *temp);
                
                bool leftSideAnd = findInFormulas((*state)->trues, &(*temp)->rhs());
                
                bool rightSideAnd = findInFormulas((*state)->trues, &(*temp)->lhs());
                
                if (leftSide && !leftSideAnd && rightSideAnd) {
                    (*state)->reqSet.insert(*temp);
                } else if (!leftSide && rightSideAnd) {
                    (*state)->unreqSet.insert(*temp);
                }
                
                if (!leftSide || leftSideAnd) {
                    stringBuilder.str("");
                    stringBuilder<<*temp;
                    (*state)->fLabels.push_back(stringBuilder.str());
                }
            } else if ((*temp)->kind() == Formula::Kind::X) {
                if(findInFormulas((*state)->trues, *temp)) {
                    (*state)->reqSet.insert(&(*temp)->arg());
                } else {
                    (*state)->unreqSet.insert(&(*temp)->arg());
                }
            }
        }
    }
}

void LtlConverter::constructTransitions() {
    for (auto state = states.begin(); state != states.end(); ++state) {
        for (auto transition = states.begin(); transition != states.end(); ++transition) {
            
            bool sholdBeConnected = true;
            
            for(auto req = (*state)->req.begin(); req != (*state)->req.end(); ++req) {
                sholdBeConnected = sholdBeConnected && findInFormulas((*transition)->trues,*req);
            }
            
            for(auto unreq = (*state)->unreq.begin(); unreq != (*state)->unreq.end(); ++unreq) {
                sholdBeConnected = sholdBeConnected && findInFormulas((*transition)->falses,*unreq);
            }
            
            if (sholdBeConnected) {
                (*state)->connectedStates.push_back(*transition);
            }
        }
    }
}

void LtlConverter::findStartStates(const model::ltl::Formula& formula) {
    for (auto state = states.begin(); state != states.end(); ++state) {
        (*state)->isStartState = findInFormulas((*state)->trues, &formula);
    }
}

void LtlConverter::constructStates(const model::ltl::Formula& formula) {
    const Formula* newF = &formula;
    const Formula* oldF = &formula;
    
    do {
        oldF = newF;
        newF = &trivialize(*oldF);
    }while(!((*oldF) == (*newF)));
    
    std::vector<const model::ltl::Formula*> subFormulas;
    FormulaSet subFormulasSet(&subFormulas);
    
    constructAllSubFormulas(*newF, &subFormulasSet);
    
    addInverse(&subFormulasSet);
    
    std::vector<const model::ltl::Formula*> atoms;
    FormulaSet atomsSet(&atoms);
    
    findAtoms(&subFormulas, &atomsSet);
    
    std::vector<const model::ltl::Formula*> pureAtoms;
    FormulaSet pureAtomsSet(&pureAtoms);
    
    findPureAtoms(&atoms, &pureAtomsSet);
    
    bool isAtomsContainsTrue = findInFormulas(&atoms, &P(TRUE_PROP));
    bool isAtomsContainsFalse = findInFormulas(&atoms, &P(FALSE_PROP));
    
    std::vector<const model::ltl::Formula*> trues;
    VectorState state(&pureAtoms);
    
    while (state.next(&trues)) {
        
        if(isAtomsContainsTrue) trues.push_back(&P(TRUE_PROP));
        
        std::vector<const model::ltl::Formula*> falses;
        FormulaSet falsesSet(&falses);
        
        if(isAtomsContainsFalse) falses.push_back(&P(FALSE_PROP));
        
        auto trMarks = std::set<std::string>{};
        std::stringstream stringBuilder;
        
        for(auto f : trues) {
            stringBuilder.str("");
            stringBuilder << *f;
            trMarks.insert(stringBuilder.str());
        }
        
        findFalseAtoms(&atoms, &trues, &falsesSet);
        
        FormulaSet truesSet(&trues);
        findPures(&subFormulas, &falsesSet, &truesSet, trMarks);
        
        auto newStateTrues = new std::vector<const model::ltl::Formula*>();
        newStateTrues->reserve(trues.size());
        std::copy(trues.begin(), trues.end(), std::back_inserter(*newStateTrues));
        FormulaSet newTruesSet(newStateTrues);
        
        auto newStateFalses = new std::vector<const model::ltl::Formula*>();
        newStateFalses->reserve(falses.size());
        std::copy(falses.begin(), falses.end(), std::back_inserter(*newStateFalses));
        FormulaSet newFalsesSet(newStateFalses);
        
        auto newState = new State(newStateTrues,newStateFalses);
        
        newState->transitionMarks = trMarks;
        
        states.push_back(newState);
        
        trues.clear();
    }
    
    constructLabels();
    
    constructRules(&subFormulas);
    
    constructTransitions();
    
    findStartStates(*newF);
}

void LtlConverter::constructLabels() {
    std::stringstream stringBuilder;
    for (int index = 0; index < states.size(); ++index) {
        stringBuilder.str("");
        stringBuilder<<"S"<<index;
        states[index]->stateLabel = stringBuilder.str();
    }
}

void LtlConverter::constructAutomaton(model::fsm::Automaton *automaton) {
    for (auto state = states.begin(); state != states.end(); ++state) {
        automaton->add_state((*state)->stateLabel);
        if ((*state)->isStartState) {
            automaton->set_initial((*state)->stateLabel);
        }
    }
    for (auto state = states.begin(); state != states.end(); ++state) {
        for (auto transition = (*state)->connectedStates.begin(); transition != (*state)->connectedStates.end(); ++transition) {
            automaton->add_trans((*state)->stateLabel, (*state)->transitionMarks, (*transition)->stateLabel);
        }
        if (!(*state)->fLabels.empty()) {
            for(auto label : (*state)->fLabels) {
                automaton->set_final((*state)->stateLabel,(unsigned int)std::hash<std::string>{}(label));
            }
        }
    }
}

model::fsm::Automaton* LtlConverter::convert(const model::ltl::Formula& formula) {
    //    ConvertNode* initNode = new ConvertNode();
    //    ConvertNode* n = new ConvertNode();
    //    auto nodes = new std::vector<ConvertNode*>();
    //
    //    n->Incoming.push_back(initNode);
    //    auto triv = trivialize(trivialize(formula));
    //    n->New.push_back(&triv);
    //    return expand(n, nodes);
    auto automaton = new model::fsm::Automaton();
    
    constructStates(formula);
    
    constructAutomaton(automaton);
    
    return automaton;
}

std::vector<ConvertNode*>* LtlConverter::expand(ConvertNode* currentNode, std::vector<ConvertNode*>* nodes) {
    
    std::cout<< std::endl << "Current node: "<< std::endl <<currentNode->toString()<<std::endl;
    
    if (currentNode->New.empty()) {
        
        for(int i = 0; i < nodes->size(); i++) {
            
            auto node = (*nodes)[i];
            if (node->Old == currentNode->Old && node->Next == currentNode->Next) {
                
                std::cout<<"current in nodes"<<std::endl;
                
                node->Incoming.reserve(currentNode->Incoming.size());
                std::copy(currentNode->Incoming.begin(),currentNode->Incoming.end(),std::back_inserter(node->Incoming));
                
                return nodes;
            }
        }
        
        
        std::cout<<"current not in nodes"<<std::endl;
        
        ConvertNode* newNode = new ConvertNode();
        
        newNode->Incoming.push_back(currentNode);
        
        newNode->New.reserve(currentNode->Next.size());
        std::copy(currentNode->Next.begin(),currentNode->Next.end(),std::back_inserter(newNode->New));
        
        nodes->push_back(currentNode);
        
        return expand(newNode, nodes);
    } else {
        
        auto n = currentNode->New[currentNode->New.size()-1];
        
        std::cout<<"choosed formule: "<< *n <<std::endl;
        
        currentNode->New.erase(--currentNode->New.end());
        
        for(int i = 0; i < currentNode->Old.size(); i++) {
            
            auto func = currentNode->Old[i];
            if (n == func) {
                
                std::cout<<"n in Old "<<std::endl;
                
                return expand(currentNode,nodes);
            }
        }
        
        if (n->prop() == FALSE_PROP) {
            
            std::cout<<"n is false"<<std::endl;
            
            return nodes;
        }
        
        auto not_n = !(*n);
        for(int i = 0; i < currentNode->Old.size(); i++) {
            
            auto func = currentNode->Old[i];
            if (not_n == (*func)) {
                
                std::cout<<"!n in Old"<<std::endl;
                
                return nodes;
            }
        }
        
        if (
            n->kind() == Formula::Kind::ATOM ||
            (n->kind() == Formula::Kind::NOT && n->arg().kind() == Formula::Kind::ATOM) ||
            n->prop() == TRUE_PROP
            ) {
                
                std::cout<<"n is true or ATOM or !ATOM"<<std::endl;
                
                ConvertNode* newNode = new ConvertNode();
                
                copyNodes(newNode, currentNode);
                
                newNode->Old.push_back(n);
                
                return LtlConverter::expand(newNode, nodes);
            } else if (n->kind() == Formula::Kind::OR) {
                
                std::cout<<"n is OR "<<std::endl;
                
                ConvertNode *firstNewNode = new ConvertNode(), *secondNewNode = new ConvertNode();
                
                copyNodes(firstNewNode, currentNode);
                copyNodes(secondNewNode, currentNode);
                
                firstNewNode->Old.push_back(n);
                firstNewNode->New.push_back(&(n->lhs()));
                
                secondNewNode->Old.push_back(n);
                secondNewNode->New.push_back(&(n->rhs()));
                
                return expand(secondNewNode, expand(firstNewNode,nodes));
            } else if (n->kind() == Formula::Kind::U) {
                
                std::cout<<"n is Until "<<std::endl;
                
                ConvertNode *firstNewNode = new ConvertNode(), *secondNewNode = new ConvertNode();
                
                copyNodes(firstNewNode, currentNode);
                copyNodes(secondNewNode, currentNode);
                
                firstNewNode->Old.push_back(n);
                firstNewNode->New.push_back(&(n->lhs()));
                firstNewNode->Next.push_back(n);
                
                secondNewNode->Old.push_back(n);
                secondNewNode->New.push_back(&(n->rhs()));
                
                return expand(secondNewNode, expand(firstNewNode,nodes));
            } else if (n->kind() == Formula::Kind::AND) {
                
                std::cout<<"n is AND"<<std::endl;
                
                ConvertNode* newNode = new ConvertNode();
                
                copyNodes(newNode, currentNode);
                
                newNode->Old.push_back(n);
                newNode->New.push_back(&(n->lhs()));
                newNode->New.push_back(&(n->rhs()));
                
                return expand(newNode, nodes);
            } else {
                
                std::cout<<"n is X"<<std::endl;
                
                ConvertNode *newNode = new ConvertNode();
                
                copyNodes(newNode, currentNode);
                
                newNode->Old.push_back(n);
                newNode->Next.push_back(&(n->arg()));
                
                return expand(newNode, nodes);
            }
    }
    
    return nodes;
}

void LtlConverter::copyNodes(ConvertNode* newNode, ConvertNode* currentNode) {
    newNode->Incoming.reserve(currentNode->Incoming.size());
    std::copy(currentNode->Incoming.begin(),currentNode->Incoming.end(),std::back_inserter(newNode->Incoming));
    
    newNode->New.reserve(currentNode->New.size());
    std::copy(currentNode->New.begin(),currentNode->New.end(),std::back_inserter(newNode->New));
    
    newNode->Next.reserve(currentNode->New.size());
    std::copy(currentNode->Next.begin(),currentNode->Next.end(),std::back_inserter(newNode->Next));
    
    
    newNode->Old.reserve(currentNode->Old.size());
    std::copy(currentNode->Old.begin(),currentNode->Old.end(),std::back_inserter(newNode->Old));
}
