//
//  convert_node.cpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 10.12.2021.
//

#include "convert_node.hpp"

std::string ConvertNode::toString() const {
    std::stringstream stringBuilder;
    
    stringBuilder<<"Node: "<<number<<std::endl;
    stringBuilder<<"Incoming: { ";
    
    for(auto node: Incoming){
        stringBuilder << node->number << "; ";
    }
    
    stringBuilder << " }" << std::endl;
    
    if(Old.empty()){
        stringBuilder << "Old: Empty"<<std::endl;
    }else{
        stringBuilder << "Old: { ";
    
    for(auto formula: Old){
        stringBuilder << *formula << "; ";
    }
    
        stringBuilder << " }" <<std::endl;
    }
    
    if (Next.empty()) {
        stringBuilder << "Next: Empty"<<std::endl;
    } else {
        stringBuilder <<"Next: { ";
    
        for(auto formula: Next){
            stringBuilder << *formula << "; ";
        }
    
        stringBuilder << " }" << std::endl ;
    }

    if (New.empty()) {
        stringBuilder << "New: Empty"<<std::endl;
    } else {
        stringBuilder << "New: { ";
    
        for(auto formula: New){
            stringBuilder << *formula << "; ";
        }
    
        stringBuilder << " }" << std::endl;
    }
    
    return stringBuilder.str();
}
