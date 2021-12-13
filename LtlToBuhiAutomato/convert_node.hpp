//
//  convert_node.hpp
//  LtlToBuhiAutomato
//
//  Created by Евгений Сударский on 10.12.2021.
//

#ifndef convert_node_hpp
#define convert_node_hpp

#include <vector>
#include <sstream>
#include <string>
#include "ltl.h"

class ConvertNode {
public:
    static int nodeNumber;
    int number = nodeNumber++;
    std::vector<ConvertNode*> Incoming;
    std::vector<const model::ltl::Formula*> Old;
    std::vector<const model::ltl::Formula*> New;
    std::vector<const model::ltl::Formula*> Next;
    
    std::string toString() const;
};

#endif /* convert_node_hpp */
