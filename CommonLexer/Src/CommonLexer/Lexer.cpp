#include "CommonLexer/Lexer.h"
#include "CommonLexer/Source.h"

namespace CommonLexer
{
	Lexer::Lexer()
	    : m_RNG(std::random_device {}()) {}

	Lex Lexer::lexSource(ISource* source)
	{
		return lexSource(source, source->getCompleteSpan());
	}

	Lex Lexer::lexSource(ISource* source, SourceSpan span)
	{
		Lex  lex { *this, source };
		auto rule = getRule(m_MainRule);
		if (rule && source)
		{
			auto& root = lex.getRoot();
			root.setRule(rule->getID());

			MatcherState       state { &lex, source, span };
			MatcherScopedState scopedState { &root };
			rule->match(state, scopedState, span);
		}
		return lex;
	}

	void Lexer::registerRule(std::unique_ptr<IRule>&& rule)
	{
		auto id = newRuleID();
		rule->setID(id);
		m_Rules.insert({ id, std::move(rule) });
	}

	void Lexer::setMainRule(const std::string& rule)
	{
		m_MainRule = rule;
	}

	[[nodiscard]] std::uint32_t Lexer::getRuleID(const std::string& rule) const
	{
		for (auto& r : m_Rules)
		{
			if (r.second->getName() == rule)
				return r.first;
		}
		return 0;
	}

	[[nodiscard]] IRule* Lexer::getRule(std::uint32_t ruleID)
	{
		auto itr = m_Rules.find(ruleID);
		return itr != m_Rules.end() ? itr->second.get() : nullptr;
	}

	std::uint32_t Lexer::newRuleID()
	{
		std::uint32_t id = m_RNG();

		while (id == 0 || m_Rules.find(id) != m_Rules.end())
			id = m_RNG();

		return id;
	}
} // namespace CommonLexer