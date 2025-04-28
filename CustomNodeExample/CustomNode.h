#pragma once

#include <resolvers/INodeSelector.h>
#include "utils/StringUtil.h"
#include "StyleMap.h"

#include <map>

class CustomNode : public INodeSelector
{
public:
	CustomNode(std::string tag, std::string id, std::string classes,
		std::map<std::string, std::string> attributes, std::string inlineStyle,
		std::vector<CustomNode*> children)
		: tag(tag), id(id), classes(future::StringUtil::split(classes, ' ')),
		attributes(attributes), inlineStyle(inlineStyle)
	{
		int idx = 0;
		for (const auto& child : children)
		{
			child->SetParent(this, idx++);
			this->children.push_back(child);
		}
	}

	virtual ~CustomNode()
	{
		for (const auto& child : children)
			delete child;
	}

	void SetParent(CustomNode* parent, int indexWithinParent)
	{
		this->parent = parent;
		this->indexWithinParent = indexWithinParent;
	}

	std::string GetIdDesc() { return !id.empty() ? ("#" + id) : ""; }
	std::string GetClassesDesc() { return !classes.empty() ? ("(" + future::StringUtil::join(classes, ' ') + ")") : ""; }

	// INodeSelector Implementation

	virtual std::string GetTag() const override
	{
		return tag;
	}

	virtual std::string GetId() const override
	{
		return id;
	}	
	
	virtual bool HasClass(const std::string& cl) const override
	{
		return std::find(classes.begin(), classes.end(), cl) != classes.end();
	}

	virtual std::optional<std::string> GetAttribute(const std::string& key) const override
	{
		if (attributes.contains(key))return  attributes.at(key);
		return std::nullopt;
	}

	bool hovered;
	virtual bool IsHovered() const override {
		return hovered;
	}

	virtual INodeSelector* GetParent() const override
	{
		return parent;
	}

	virtual
		std::span<const INodeSelector* const> GetChildren() const override
	{
		return { reinterpret_cast<const INodeSelector* const*>(children.data()) , children.size() };
	}

	virtual int GetIndexWithinParent()const  override
	{
		return indexWithinParent;
	}

	// Node Members

	std::string tag;
	std::string id;
	std::vector<std::string> classes;
	std::map<std::string, std::string> attributes;
	std::string inlineStyle;

	CustomNode* parent = nullptr;
	std::vector<CustomNode*> children;

	int indexWithinParent = 0;

	StyleMap styles;

	// Inherited via INodeSelector
	bool IsFocused() const override;
	bool IsActive() const override;
};
