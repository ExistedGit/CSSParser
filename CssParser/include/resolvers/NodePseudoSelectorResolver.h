
#ifndef NODEPSEUDOSELECTORRESOLVER_H_
#define NODEPSEUDOSELECTORRESOLVER_H_

#include <map>
#include <stdio.h>

#include "selectors/PseudoSelector.h"

class INodeSelector;

namespace future {
    class PseudoSelector;

    struct TraverseUtilStruct {
        const INodeSelector* selfNode;
        int idx;
        int totalCount;

        TraverseUtilStruct(const INodeSelector* n)
        {
            selfNode = n;
            idx = 0;
            totalCount = 0;
        }
    };

    class NodePseudoSelectorResolver {
    public:
        static bool DoesNodeMatchPseudo(const INodeSelector*, PseudoSelector*);

    private:
        typedef void(*traverseAction)(const INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo);

        static std::map<std::string, bool>& GetDynamicPseudoClassMap();
        
        static bool TraverseElementNodeSiblings(const INodeSelector*node, traverseAction ac, TraverseUtilStruct* baseInfo);
        
        static int  IndexOfSiblings(const INodeSelector* node);
        static int  IndexEqualTypeOfSiblings(const INodeSelector* node);        
        static int  LastIndexOfSiblings(const INodeSelector* node);
        static int  LastIndexEqualTypeOfSiblings(const INodeSelector* node);
        
        static bool IndexMatchesParameter(future::PseudoSelector* selector, int idx);        
        static bool IndexMatchesPolynomial(PseudoSelector::Parameter* parameter, int idx);
    };
}

#endif /* NODEPSEUDOSELECTORRESOLVER_H_ */
