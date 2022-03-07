#include "CommonLexer/Matchers.h"
#include "CommonLexer/Lex.h"
#include "CommonLexer/Lexer.h"
#include "CommonLexer/Node.h"
#include "CommonLexer/Rule.h"

#include <fmt/format.h>

namespace CommonLexer
{
	CombinationMatcher::CombinationMatcher(std::vector<std::unique_ptr<IMatcher>>&& matchers)
	    : m_Matchers(std::move(matchers)) {}

	MatchResult CombinationMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		SourceSpan subSpan { span };
		SourceSpan totalSpan { span.m_Start, span.m_Start };
		for (auto& matcher : m_Matchers)
		{
			auto result = matcher->match(state, scopedState, subSpan);
			switch (result.m_Status)
			{
			case EMatchStatus::Success:
				subSpan.m_Start = totalSpan.m_End = result.m_Span.m_End;
				break;
			case EMatchStatus::Skip: continue;
			case EMatchStatus::Failure: [[fallthrough]];
			default: return { EMatchStatus::Failure, totalSpan };
			}
		}
		return { EMatchStatus::Success, totalSpan };
	}

	OrMatcher::OrMatcher(std::vector<std::unique_ptr<IMatcher>>&& matchers)
	    : m_Matchers(std::move(matchers)) {}

	MatchResult OrMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		std::vector<Message> messages;

		Node        bestNode { *state.m_Lex };
		MatchResult bestResult { EMatchStatus::Failure, { span.m_Start, span.m_Start } };

		for (auto& matcher : m_Matchers)
		{
			Node               tempNode { *state.m_Lex };
			MatcherScopedState newScopedState { &tempNode };
			auto               result = matcher->match(state, newScopedState, span);
			switch (result.m_Status)
			{
			case EMatchStatus::Success:
				if (bestResult.m_Status != EMatchStatus::Success || result.m_Span.length() > bestResult.m_Span.length())
				{
					messages   = std::move(newScopedState.m_Messages);
					bestNode   = std::move(tempNode);
					bestResult = result;
				}
				break;
			case EMatchStatus::Skip:
				if (bestResult.m_Status == EMatchStatus::Failure)
					bestResult = result;
				[[fallthrough]];
			case EMatchStatus::Failure:
			{
				std::size_t len = result.m_Span.length();
				if (len == 0)
					len = 1;
				if (bestResult.m_Status != EMatchStatus::Success && len > bestResult.m_Span.length())
				{
					messages   = std::move(newScopedState.m_Messages);
					bestNode   = std::move(tempNode);
					bestResult = result;
				}
				continue;
			}
			}
		}

		scopedState.addMessages(std::move(messages));
		if (bestResult.m_Status == EMatchStatus::Success)
			scopedState.m_ParentNode->addChildren(std::move(bestNode));
		return bestResult;
	}

	RangeMatcher::RangeMatcher(std::unique_ptr<IMatcher>&& matcher, std::size_t lowerBounds, std::size_t upperBounds)
	    : m_Matcher(std::move(matcher)), m_LowerBounds(lowerBounds), m_UpperBounds(upperBounds) {}

	MatchResult RangeMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		std::size_t matches { 0 };
		SourceSpan  subSpan { span };
		SourceSpan  totalSpan { span.m_Start, span.m_Start };

		MatcherScopedState newScopedState { scopedState.m_ParentNode };

		while (matches < m_UpperBounds)
		{
			auto result = m_Matcher->match(state, newScopedState, subSpan);
			if (result.m_Status != EMatchStatus::Success)
				break;

			subSpan.m_Start = totalSpan.m_End = result.m_Span.m_End;
			++matches;

			if (subSpan.length() == 0)
				break;
		}

		if (totalSpan.m_End < span.m_End)
			scopedState.addMessages(std::move(newScopedState));

		if (matches < m_LowerBounds)
		{
			scopedState.addMessage(fmt::format("Expected at least {} matches but only got {} matches", m_LowerBounds, matches), subSpan.m_End, subSpan, state.m_CurrentRule->getID());
			return { EMatchStatus::Failure, totalSpan };
		}
		return { EMatchStatus::Success, totalSpan };
	}

	OptionalMatcher::OptionalMatcher(std::unique_ptr<IMatcher>&& matcher)
	    : m_Matcher(std::move(matcher)) {}

	MatchResult OptionalMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		auto result = m_Matcher->match(state, scopedState, span);
		switch (result.m_Status)
		{
		case EMatchStatus::Success: return result;
		case EMatchStatus::Skip: [[fallthrough]];
		case EMatchStatus::Failure: [[fallthrough]];
		default: return { EMatchStatus::Skip, result.m_Span };
		}
	}

	NegativeMatcher::NegativeMatcher(std::unique_ptr<IMatcher>&& matcher)
	    : m_Matcher(std::move(matcher)) {}

	MatchResult NegativeMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		Node               tempNode { *state.m_Lex };
		MatcherScopedState newScopedState { &tempNode };

		auto result = m_Matcher->match(state, newScopedState, span);
		switch (result.m_Status)
		{
		case EMatchStatus::Success:
			scopedState.addMessage("Did not expect following sequence", span.m_Start, result.m_Span, state.m_CurrentRule->getID());
			return { EMatchStatus::Failure, result.m_Span };
		case EMatchStatus::Skip: [[fallthrough]];
		case EMatchStatus::Failure: return { EMatchStatus::Success, { span.m_Start, span.m_Start } };
		default: return { EMatchStatus::Failure, result.m_Span };
		}
	}

	SpaceMatcher::SpaceMatcher(std::unique_ptr<IMatcher>&& matcher, bool forced, SpaceDirectionFlags direction, ESpaceMethod method)
	    : m_Matcher(std::move(matcher)), m_Forced(forced), m_Direction(direction), m_Method(method) {}

	MatchResult SpaceMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		SourceSpan subSpan { span };
		SourceSpan totalSpan { span.m_Start, span.m_Start };

		// Match spaces behind
		if (m_Direction.contains(ESpaceDirection::Left))
		{
			switch (m_Method)
			{
			case ESpaceMethod::Normal:
			{
				auto result = matchNormalSpaces(state, scopedState, subSpan);
				switch (result.m_Status)
				{
				case EMatchStatus::Success:
					subSpan.m_Start = totalSpan.m_End = result.m_Span.m_End;
					break;
				case EMatchStatus::Skip: break;
				case EMatchStatus::Failure: [[fallthrough]];
				default: return { EMatchStatus::Failure, totalSpan };
				}
				break;
			}
			case ESpaceMethod::Whitespace:
			{
				auto result = matchWhitespaceSpaces(state, scopedState, subSpan);
				switch (result.m_Status)
				{
				case EMatchStatus::Success:
					subSpan.m_Start = totalSpan.m_End = result.m_Span.m_End;
					break;
				case EMatchStatus::Skip: break;
				case EMatchStatus::Failure: [[fallthrough]];
				default: return { EMatchStatus::Failure, totalSpan };
				}
				break;
			}
			}
		}

		{
			auto result = m_Matcher->match(state, scopedState, subSpan);
			switch (result.m_Status)
			{
			case EMatchStatus::Success:
				subSpan.m_Start = totalSpan.m_End = result.m_Span.m_End;
				break;
			case EMatchStatus::Skip: break;
			case EMatchStatus::Failure: [[fallthrough]];
			default: return { EMatchStatus::Failure, totalSpan };
			}
		}

		// Match spaces ahead
		if (m_Direction.contains(ESpaceDirection::Right))
		{
			switch (m_Method)
			{
			case ESpaceMethod::Normal:
			{
				auto result = matchNormalSpaces(state, scopedState, subSpan);
				switch (result.m_Status)
				{
				case EMatchStatus::Success:
					subSpan.m_Start = totalSpan.m_End = result.m_Span.m_End;
					break;
				case EMatchStatus::Skip: break;
				case EMatchStatus::Failure: [[fallthrough]];
				default: return { EMatchStatus::Failure, totalSpan };
				}
				break;
			}
			case ESpaceMethod::Whitespace:
			{
				auto result = matchWhitespaceSpaces(state, scopedState, subSpan);
				switch (result.m_Status)
				{
				case EMatchStatus::Success:
					subSpan.m_Start = totalSpan.m_End = result.m_Span.m_End;
					break;
				case EMatchStatus::Skip: break;
				case EMatchStatus::Failure: [[fallthrough]];
				default: return { EMatchStatus::Failure, totalSpan };
				}
				break;
			}
			}
		}

		return { EMatchStatus::Success, totalSpan };
	}

	MatchResult SpaceMatcher::matchNormalSpaces(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		std::size_t i = 0;

		auto begin = state.m_SourceSpan.begin(state.m_Source);
		auto itr   = span.begin(state.m_Source);
		auto end   = span.end(state.m_Source);

		if (m_Forced)
		{
			--itr;
			if (itr < begin)
				++itr;
		}

		while (itr != end)
		{
			switch (*itr)
			{
			case '\t': [[fallthrough]];
			case ' ':
				++itr;
				++i;
				continue;
			}

			if (m_Forced && i == 0)
			{
				++itr;
				continue;
			}
			break;
		}

		if (m_Forced && i == 0)
		{
			scopedState.addMessage(fmt::format("Expected space or tab but got '{}'", itr != end ? std::string { *itr } : "EOF"), span.m_Start, SourceSpan { span.m_Start, span.m_Start }, state.m_CurrentRule);
		}

		return { EMatchStatus::Success, { span.m_Start, itr } };
	}

	MatchResult SpaceMatcher::matchWhitespaceSpaces(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		std::size_t i = 0;

		auto begin = state.m_SourceSpan.begin(state.m_Source);
		auto itr   = span.begin(state.m_Source);
		auto end   = span.end(state.m_Source);

		if (m_Forced)
		{
			--itr;
			if (itr < begin)
				++itr;
		}

		while (itr != end)
		{
			switch (*itr)
			{
			case '\t': [[fallthrough]];
			case '\n': [[fallthrough]];
			case '\v': [[fallthrough]];
			case '\f': [[fallthrough]];
			case '\r': [[fallthrough]];
			case ' ':
				++itr;
				++i;
				continue;
			}

			if (m_Forced && i == 0)
			{
				++itr;
				continue;
			}
			break;
		}

		if (m_Forced && i == 0)
		{
			scopedState.addMessage(fmt::format("Expected space, tab, vertical tab, form feed, carriage return or line feed but got '{}'", itr != end ? std::string { *itr } : "EOF"), span.m_Start, SourceSpan { span.m_Start, span.m_Start }, state.m_CurrentRule);
		}

		return { EMatchStatus::Success, { span.m_Start, itr } };
	}

	NamedGroupMatcher::NamedGroupMatcher(const std::string& name, std::unique_ptr<IMatcher>&& matcher)
	    : m_Name(name), m_Matcher(std::move(matcher)) {}
	NamedGroupMatcher::NamedGroupMatcher(std::string&& name, std::unique_ptr<IMatcher>&& matcher)
	    : m_Name(std::move(name)), m_Matcher(std::move(matcher)) {}

	MatchResult NamedGroupMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		auto result = m_Matcher->match(state, scopedState, span);
		if (result.m_Status == EMatchStatus::Success)
			state.setGroupedValue(m_Name, result.m_Span);
		return result;
	}

	NamedGroupReferenceMatcher::NamedGroupReferenceMatcher(const std::string& name)
	    : m_Name(name) {}

	NamedGroupReferenceMatcher::NamedGroupReferenceMatcher(std::string&& name)
	    : m_Name(std::move(name)) {}

	MatchResult NamedGroupReferenceMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		auto groupedSpan = state.getGroupedValue(m_Name);

		auto itr = span.begin(state.m_Source);
		auto end = span.end(state.m_Source);

		auto textItr = groupedSpan.begin(state.m_Source);
		auto textEnd = groupedSpan.end(state.m_Source);

		while (textItr != textEnd)
		{
			if (itr == end)
			{
				scopedState.addMessage(fmt::format("Expected '{}', but got 'EOF'", SourceSpan { textItr, textEnd }.getSpan(state.m_Source)), itr, SourceSpan { span.m_Start, itr }, state.m_CurrentRule);
				return { EMatchStatus::Failure, { span.m_Start, itr } };
			}

			if (*itr != *textItr)
			{
				scopedState.addMessage(fmt::format("Expected '{}', but got '{}'", *textItr, *itr), itr, SourceSpan { span.m_Start, itr }, state.m_CurrentRule);
				return { EMatchStatus::Failure, { span.m_Start, itr } };
			}

			++itr;
			++textItr;
		}
		return { EMatchStatus::Success, { span.m_Start, itr } };
	}

	ReferenceMatcher::ReferenceMatcher(const std::string& name)
	    : m_Name(name) {}

	ReferenceMatcher::ReferenceMatcher(std::string&& name)
	    : m_Name(std::move(name)) {}

	MatchResult ReferenceMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		if (!m_Rule)
		{
			m_Rule = state.m_Lex->getLexer()->getRule(m_Name);
			if (!m_Rule)
			{
				scopedState.addMessage(fmt::format("Expected non existent rule '{}'", m_Name), span.m_Start, SourceSpan { span.m_Start, span.m_Start }, state.m_CurrentRule);
				return { EMatchStatus::Failure, { span.m_Start, span.m_Start } };
			}
		}

		return m_Rule->match(state, scopedState, span);
	}

	TextMatcher::TextMatcher(const std::string& text)
	    : m_Text(text) {}

	TextMatcher::TextMatcher(std::string&& text)
	    : m_Text(std::move(text)) {}

	MatchResult TextMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		auto itr = span.begin(state.m_Source);
		auto end = span.end(state.m_Source);

		auto textItr = m_Text.begin();
		auto textEnd = m_Text.end();

		while (textItr != textEnd)
		{
			if (itr == end)
			{
				scopedState.addMessage(fmt::format("Expected '{}', but got 'EOF'", std::string_view { textItr.operator->(), static_cast<std::size_t>(textEnd - textItr) }), itr, SourceSpan { span.m_Start, itr }, state.m_CurrentRule);
				return { EMatchStatus::Failure, { span.m_Start, itr } };
			}

			if (*itr != *textItr)
			{
				scopedState.addMessage(fmt::format("Expected '{}', but got '{}'", *textItr, *itr), itr, SourceSpan { span.m_Start, itr }, state.m_CurrentRule);
				return { EMatchStatus::Failure, { span.m_Start, itr } };
			}

			++itr;
			++textItr;
		}
		return { EMatchStatus::Success, { span.m_Start, itr } };
	}

	RegexMatcher::RegexMatcher(const std::string& regex)
	    : m_Regex(regex, std::regex_constants::ECMAScript | std::regex_constants::optimize) {}
	RegexMatcher::RegexMatcher(std::string&& regex)
	    : m_Regex(std::move(regex), std::regex_constants::ECMAScript | std::regex_constants::optimize) {}

	MatchResult RegexMatcher::match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)
	{
		std::match_results<SourceIterator> results;
		if (std::regex_search(span.begin(state.m_Source), span.end(state.m_Source), results, m_Regex, std::regex_constants::match_continuous))
			return { EMatchStatus::Success, { results[0].first, results[0].second } };
		scopedState.addMessage("Expected regex to succeed, but failed. Sadly I don't get regex error messages, maybe in the future ;)", span.m_Start, SourceSpan { span.m_Start, span.m_Start }, state.m_CurrentRule);
		return { EMatchStatus::Failure, { span.m_Start, span.m_Start } };
	}
} // namespace CommonLexer