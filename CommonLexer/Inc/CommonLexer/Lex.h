#pragma once

#include "Message.h"
#include "Node.h"

#include <utility>
#include <vector>

namespace CommonLexer
{
	class Lexer;
	class ISource;

	struct Lex
	{
	public:
		Lex(Lexer& lexer, ISource* source) : m_Lexer(&lexer), m_Source(source), m_Root(*this) {}

		void setMessages(std::vector<Message>&& messages) { m_Messages = std::move(messages); }

		[[nodiscard]] auto  getLexer() const { return m_Lexer; }
		[[nodiscard]] auto  getSource() const { return m_Source; }
		[[nodiscard]] auto& getRoot() { return m_Root; }
		[[nodiscard]] auto& getRoot() const { return m_Root; }
		[[nodiscard]] auto& getMessages() { return m_Messages; }
		[[nodiscard]] auto& getMessages() const { return m_Messages; }

	private:
		Lexer*   m_Lexer;
		ISource* m_Source;
		Node     m_Root;

		std::vector<Message> m_Messages;
	};
} // namespace CommonLexer