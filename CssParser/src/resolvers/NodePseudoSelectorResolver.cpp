
#include "resolvers/NodePseudoSelectorResolver.h"
#include "resolvers/INodeSelector.h"
#include "utils/StringUtil.h"

#include <string>

namespace future {  
    inline static std::set<std::string> skippedPseudoClasses = {
        "link",
        "visited",
        "enable",
        "disable",
        "checked",
        "indeterminate",
        "target",
    };

    static void NodeIndexCounter(const INodeSelector* node, bool& stop, TraverseUtilStruct* userData);
    static void NodeFullIndexCounter(const INodeSelector* node, bool& stop, TraverseUtilStruct* userData);
    static void NodeTypeEqualIndexCounter(const INodeSelector* node, bool& stop, TraverseUtilStruct* userData);
    static void NodeFullTypeEqualIndexCounter(const INodeSelector* node, bool& stop, TraverseUtilStruct* userData);

    
    bool NodePseudoSelectorResolver::DoesNodeMatchPseudo(const INodeSelector *node, future::PseudoSelector *selector)
    {
        if (!node || !selector) {
            return false;
        }
        std::string ruleData = selector->getPseudoData();
        if (ruleData.empty()) {
            return false;
        }

        if (skippedPseudoClasses.contains(ruleData)) {
            return true;
        }

        switch (strhash(ruleData.c_str())) 
        {
        case strhash("root"):
            return !node->GetParent();

        case strhash("nth-child"):
            return IndexMatchesParameter(selector, IndexOfSiblings(node));

        case strhash("nth-last-child"):
            return IndexMatchesParameter(selector, LastIndexOfSiblings(node));

        case strhash("nth-of-type"):
            return IndexMatchesParameter(selector, IndexEqualTypeOfSiblings(node));

        case strhash("nth-last-of-type"):
            return IndexMatchesParameter(selector, LastIndexEqualTypeOfSiblings(node));

        case strhash("first-child"):
            return IndexOfSiblings(node) == 1;

        case strhash("last-child"):
            return LastIndexOfSiblings(node) == 1;

        case strhash("first-of-type"):
            return IndexEqualTypeOfSiblings(node) == 1;

        case strhash("last-of-type"):
            return LastIndexEqualTypeOfSiblings(node) == 1;

        case strhash("only-child"):
            return IndexOfSiblings(node) == 1 && LastIndexOfSiblings(node) == 1;

        case strhash("only-of-type"):
            return IndexEqualTypeOfSiblings(node) == 1 && LastIndexEqualTypeOfSiblings(node) == 1;

        case strhash("empty"):
            return !node->GetChildren().size();

        case strhash("hover"):
            return node->IsHovered();

        case strhash("focus"):
            return node->IsFocused();
        
        case strhash("active"):
            return node->IsActive();

        case strhash("lang"): {
            auto langAttr = node->GetAttribute("lang");
            PseudoSelector::Parameter* p = selector->getParameter();
            return p && langAttr && p->pString == langAttr;
        }
        }

        return true;
    }
    

    // private functions
    
    int NodePseudoSelectorResolver::IndexOfSiblings(const INodeSelector* node)
    {
        TraverseUtilStruct baseInfo(node);
        bool ret = TraverseElementNodeSiblings(node, NodeIndexCounter, &baseInfo);
        return ret ? baseInfo.idx : 0;
    }
    
    int NodePseudoSelectorResolver::IndexEqualTypeOfSiblings(const INodeSelector* node)
    {
        TraverseUtilStruct baseInfo(node);
        bool ret = TraverseElementNodeSiblings(node, NodeTypeEqualIndexCounter, &baseInfo);
        return ret ? baseInfo.idx : 0;
    }
    
    int NodePseudoSelectorResolver::LastIndexOfSiblings(const INodeSelector* node)
    {
        TraverseUtilStruct baseInfo(node);
        bool ret = TraverseElementNodeSiblings(node, NodeFullIndexCounter, &baseInfo);
        return ret ? baseInfo.totalCount - baseInfo.idx + 1 : 0;
    }
    
    int NodePseudoSelectorResolver::LastIndexEqualTypeOfSiblings(const INodeSelector* node)
    {
        TraverseUtilStruct baseInfo(node);
        bool ret = TraverseElementNodeSiblings(node, NodeFullTypeEqualIndexCounter, &baseInfo);
        return ret ? baseInfo.totalCount - baseInfo.idx + 1 : 0;
    }
    
    bool NodePseudoSelectorResolver::TraverseElementNodeSiblings(const INodeSelector* node, traverseAction ac, TraverseUtilStruct* baseInfo)
    {
        INodeSelector* parent = node->GetParent();
        if (!parent) {
            return false;
        }
        bool stop = false;
        auto children = parent->GetChildren();
        for (const auto& sibling : children) {
            ac(sibling, stop, baseInfo);
            if (stop) {
                break;
            }
        }
        return true;
    }
    
    bool NodePseudoSelectorResolver::IndexMatchesParameter(future::PseudoSelector* selector, int idx)
    {
        auto parameter = selector->getParameter();
        if (!parameter) {
            return false;
        }

        bool match = false;
        switch (parameter->type) {
            case PseudoSelector::ParameterType::NUMBER: {
                match = idx == parameter->pNumber;
                break;
            }
            case PseudoSelector::ParameterType::STRING: {
                match = false;
                break;
            }
            case PseudoSelector::ParameterType::POLYNOMIAL: {
                match = IndexMatchesPolynomial(parameter, idx);
                break;
            }
            case PseudoSelector::ParameterType::IDENT: {
                PseudoSelector::Parameter p;
                p.polynomial.coefficient = 2;
                if (StringUtil::tolower(parameter->pString) == "even") {
                    p.polynomial.constant = 0;
                    match = IndexMatchesPolynomial(&p, idx);
                } else if (StringUtil::tolower(parameter->pString) == "odd") {
                    p.polynomial.constant = 1;
                    match = IndexMatchesPolynomial(&p, idx);
                } else {
                    match = false;
                }
                break;
            }
                
            default:
                break;
        }
        return match;
    }
    
    bool NodePseudoSelectorResolver::IndexMatchesPolynomial(PseudoSelector::Parameter *parameter, int idx)
    {
        if (idx < parameter->polynomial.constant) {
            return false;
        }
        return !((idx - parameter->polynomial.sign * parameter->polynomial.constant) % parameter->polynomial.coefficient);
    }   
    

    // callbacks

    void NodeIndexCounter(const INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo)
    {
        baseInfo->idx += 1;
        if (node == baseInfo->selfNode) {
            stop = true;
        }
    }
    
    void NodeFullIndexCounter(const INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo)
    {
        baseInfo->totalCount += 1;
        if (node == baseInfo->selfNode) {
            baseInfo->idx = baseInfo->totalCount;
        }
    }
    
    void NodeTypeEqualIndexCounter(const INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo)
    {
        if (node->GetTag() == baseInfo->selfNode->GetTag()) {
            baseInfo->idx += 1;
        }
        if (node == baseInfo->selfNode) {
            stop = true;
        }
    }
    
    void NodeFullTypeEqualIndexCounter(const INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo)
    {
        if (node->GetTag() == baseInfo->selfNode->GetTag()) {
            baseInfo->totalCount += 1;
        }
        if (node == baseInfo->selfNode) {
            baseInfo->idx = baseInfo->totalCount;
        }
    }
}
