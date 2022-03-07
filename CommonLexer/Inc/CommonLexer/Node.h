#pragma once

#include "SourceSpan.h"

#include <cstdint>

#include <vector>

namespace CommonLexer
{
	class ISource;
	struct Lex;

	struct Node
	{
	public:
		explicit Node(Lex& lex);
		Node(Lex& lex, std::uint32_t rule);

		void setRule(std::uint32_t rule);
		void setSourceSpan(SourceSpan span);
		void addChild(const Node& child);
		void addChild(Node&& child);
		void addChildren(const Node& node);
		void addChildren(Node&& node);

		[[nodiscard]] Node*       getChild(std::size_t index);
		[[nodiscard]] const Node* getChild(std::size_t index) const;
		[[nodiscard]] auto&       getLex() const { return *m_Lex; }
		[[nodiscard]] auto        getRule() const { return m_Rule; }
		[[nodiscard]] auto        getSpan() const { return m_Span; }
		[[nodiscard]] auto&       getChildren() const { return m_Children; }

	private:
		Lex*              m_Lex;
		std::uint32_t     m_Rule;
		SourceSpan        m_Span;
		std::vector<Node> m_Children;
	};
} // namespace CommonLexer