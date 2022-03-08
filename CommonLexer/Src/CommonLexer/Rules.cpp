#include "CommonLexer/Rules.h"
#include "CommonLexer/Node.h"

namespace CommonLexer
{
	MatcherRule::MatcherRule(const std::string& name, std::unique_ptr<IMatcher>&& matcher, bool createNode)
	    : IRule(name), m_Matcher(std::move(matcher)), m_CreateNode(createNode) {}

	MatcherRule::MatcherRule(std::string&& name, std::unique_ptr<IMatcher>&& matcher, bool createNode)
	    : IRule(std::move(name)), m_Matcher(std::move(matcher)), m_CreateNode(createNode) {}

	MatcherRule::MatcherRule(MatcherRule&& move) noexcept
	    : IRule(std::move(move)), m_Matcher(std::move(move.m_Matcher)), m_CreateNode(move.m_CreateNode) {}

	MatchResult MatcherRule::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		state.setCurrentRule(this, span.m_Start);
		if (m_CreateNode)
		{
			Node               currentNode { *state.m_Lex, getID() };
			MatcherScopedState newScopedState { &currentNode };

			auto result = m_Matcher->match(state, newScopedState, span);
			if (result.m_Status == EMatchStatus::Success)
			{
				currentNode.setSourceSpan(result.m_Span);
				scopedState.m_ParentNode->addChild(std::move(currentNode));
			}
			scopedState.addMessages(newScopedState);
			return result;
		}
		else
		{
			return m_Matcher->match(state, scopedState, span);
		}
	}

	CallbackRule::CallbackRule(const std::string& name, Callback&& callback)
	    : IRule(name), m_Callback(std::move(callback)) {}

	CallbackRule::CallbackRule(std::string&& name, Callback&& callback)
	    : IRule(std::move(name)), m_Callback(std::move(callback)) {}

	CallbackRule::CallbackRule(CallbackRule&& move) noexcept
	    : IRule(std::move(move)), m_Callback(std::move(move.m_Callback)) {}

	MatchResult CallbackRule::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		return m_Callback(state, scopedState, span);
	}
} // namespace CommonLexer