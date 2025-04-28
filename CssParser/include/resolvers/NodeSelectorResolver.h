
#ifndef NODESELECTORRESOLVER_H_
#define NODESELECTORRESOLVER_H_

#include "selectors/SelectorsHeader.h"

class INodeSelector;

namespace future {

class NodeSelectorResolver {
public:
	NodeSelectorResolver() {}
	virtual ~NodeSelectorResolver() {}
    /**
     * Check Whether a selector can apply to the node
     */
	static bool DoesNodeMatchSelector(const INodeSelector* node, Selector* selector);

private:
	static bool DoesNodeMatchIDSelector(const INodeSelector* node, IdSelector* selector);
	static bool DoesNodeMatchClassSelector(const INodeSelector* node, ClassSelector* selector);
	static bool DoesNodeMatchTypeSelector(const INodeSelector* node, TypeSelector* selector);
	static bool DoesNodeMatchAttributeSelector(const INodeSelector* node, AttributeSelector* selector);
	static bool DoesNodeMatchPseudoSelector(const INodeSelector* node, PseudoSelector* selector);
	static bool DoesNodeMatchSequenceSelector(const INodeSelector* node, SequenceSelector* selector);
	static bool DoesNodeMatchCombineSelector(const INodeSelector* node, CombineSelector* selector);
	static bool DoesNodeMatchGroupSelector(const INodeSelector* node, GroupSelector* selector);
};

}

#endif /* NODESELECTORRESOLVER_H_ */
