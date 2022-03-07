#include "CommonLexer/Node.h"

namespace CommonLexer
{
	Node::Node(Lex& lex)
	    : m_Lex(&lex), m_Rule(0U), m_Span({ 0, 0 }) {}
	Node::Node(Lex& lex, std::uint32_t rule)
	    : m_Lex(&lex), m_Rule(rule), m_Span({ 0, 0 }) {}

	void Node::setRule(std::uint32_t rule)
	{
		m_Rule = rule;
	}

	void Node::setSourceSpan(SourceSpan span)
	{
		m_Span = span;
	}

	void Node::addChild(const Node& child)
	{
		m_Children.push_back(child);
	}

	void Node::addChild(Node&& child)
	{
		m_Children.push_back(std::move(child));
	}

	void Node::addChildren(const Node& node)
	{
		m_Children.reserve(m_Children.size() + node.m_Children.size());
		for (auto& child : node.m_Children)
			m_Children.push_back(child);
	}

	void Node::addChildren(Node&& node)
	{
		m_Children.reserve(m_Children.size() + node.m_Children.size());
		for (auto& child : node.m_Children)
			m_Children.push_back(std::move(child));
		node.m_Children.clear();
	}

	Node* Node::getChild(std::size_t index)
	{
		return index < m_Children.size() ? &m_Children[index] : nullptr;
	}

	const Node* Node::getChild(std::size_t index) const
	{
		return index < m_Children.size() ? &m_Children[index] : nullptr;
	}
} // namespace CommonLexer