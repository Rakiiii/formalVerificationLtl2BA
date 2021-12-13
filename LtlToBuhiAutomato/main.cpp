//
//  main.cpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 01.12.2021.
//

#include "ltl_converter.hpp"

using namespace model::ltl;
using namespace model::fsm;

int ConvertNode::nodeNumber = -1;

bool printStates = false;
bool printRequermentForTransition = false;
bool printTransitions = true;
bool printFinal = true;

int main() {
    
    auto formula = (F(G(P("p")))) >> (!X(P("q")));
    std::cout << "Formula: " << std::endl;
    std::cout << formula << std::endl << std::endl;
    
    LtlConverter converter;
    
    auto automaton = converter.convert(formula);
    
    std::cout << "Automaton: " << std::endl;
    std::cout << *automaton << std::endl;
    
    
    if (printStates) {
        for(auto state : converter.states){
            std::cout << state->stateLabel << " = { ";
            for(auto f : *(state->trues)) {
                if(f->kind() != model::ltl::Formula::NOT)std::cout<<*f<<" ; ";
            }
            std::cout<< " } "<<std::endl;
        }
    }
    
    if (printRequermentForTransition) {
        for(auto state : converter.states){
            std::cout << state->stateLabel << " : req = { ";
            for(auto f : state->req) {
                std::cout<<*f<<" ; ";
            }
            std::cout<<" } unreq = { ";
            for(auto f : state->unreq) {
                std::cout<<*f<<" ; ";
            }
            std::cout<< " } "<<std::endl;
        }
    }
    
    if (printTransitions) {
        for(auto state : converter.states){
            std::cout << state->stateLabel << " = { ";
            for(auto f : state->connectedStates) {
                std::cout<<f->stateLabel<<" ; ";
            }
            std::cout<< " } "<<std::endl;
        }
    }
    
    return 0;
}
