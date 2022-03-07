#include "CommonLexer/Matcher.h"

namespace CommonLexer
{
	void MatcherState::setCurrentRule(IRule* rule, SourcePoint begin)
	{
		m_CurrentRule = rule;
		m_RuleBegin   = begin;
	}

	void MatcherState::setGroupedValue(const std::string& name, SourceSpan span)
	{
		m_GroupedValues.insert_or_assign(name, span);
	}

	void MatcherState::setGroupedValue(std::string&& name, SourceSpan span)
	{
		m_GroupedValues.insert_or_assign(std::move(name), span);
	}

	SourceSpan MatcherState::getGroupedValue(const std::string& name) const
	{
		auto itr = m_GroupedValues.find(name);
		return itr != m_GroupedValues.end() ? itr->second : SourceSpan { 0, 0 };
	}

	void MatcherScopedState::addMessages(const MatcherScopedState& state)
	{
		m_Messages.reserve(m_Messages.size() + state.m_Messages.size());
		for (auto& message : state.m_Messages)
			m_Messages.push_back(message);
	}

	void MatcherScopedState::addMessages(MatcherScopedState&& state)
	{
		m_Messages.reserve(m_Messages.size() + state.m_Messages.size());
		for (auto& message : state.m_Messages)
			m_Messages.push_back(std::move(message));
		state.m_Messages.clear();
	}

	void MatcherScopedState::addMessages(const std::vector<Message>& messages)
	{
		m_Messages.reserve(m_Messages.size() + messages.size());
		for (auto& message : messages)
			m_Messages.push_back(message);
	}

	void MatcherScopedState::addMessages(std::vector<Message>&& messages)
	{
		m_Messages.reserve(m_Messages.size() + messages.size());
		for (auto& message : messages)
			m_Messages.push_back(std::move(message));
		messages.clear();
	}
} // namespace CommonLexer