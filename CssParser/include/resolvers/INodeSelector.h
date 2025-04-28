#pragma once

#include <string>
#include <vector>
#include <span>
#include <optional>

class INodeSelector {
public:
	using ClassIterator_t = std::vector<std::string>::const_iterator;

	virtual ~INodeSelector() {}

	virtual std::string GetTag() const = 0;
	virtual std::string GetId() const = 0;

	virtual std::optional<std::string> GetAttribute(const std::string& key) const = 0;

	virtual INodeSelector* GetParent() const = 0;
	virtual std::span<const INodeSelector* const> GetChildren() const = 0;
	virtual int GetIndexWithinParent() const = 0;

	virtual bool HasClass(const std::string&) const = 0;
	// Pseudo-class APIs
	virtual bool IsHovered() const = 0;
	virtual bool IsFocused() const = 0;
	virtual bool IsActive() const = 0;
};