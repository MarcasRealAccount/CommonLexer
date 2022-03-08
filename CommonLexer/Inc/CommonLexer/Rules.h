#pragma once

#include "Rule.h"

#include <functional>
#include <memory>
#include <utility>

namespace CommonLexer
{
	class MatcherRule final : public IRule
	{
	public:
		template <Matcher Matcher>
		MatcherRule(const std::string& name, Matcher&& matcher, bool createNode = true);
		template <Matcher Matcher>
		MatcherRule(std::string&& name, Matcher&& matcher, bool createNode = true);
		MatcherRule(const std::string& name, std::unique_ptr<IMatcher>&& matcher, bool createNode = true);
		MatcherRule(std::string&& name, std::unique_ptr<IMatcher>&& matcher, bool createNode = true);
		MatcherRule(MatcherRule&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::unique_ptr<IMatcher> m_Matcher;

		bool m_CreateNode;
	};

	class CallbackRule final : public IRule
	{
	public:
		using Callback = std::function<MatchResult(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span)>;

	public:
		CallbackRule(const std::string& name, Callback&& callback);
		CallbackRule(std::string&& name, Callback&& callback);
		CallbackRule(CallbackRule&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		Callback m_Callback;
	};

	//--------------------------
	// Template Implementations
	//--------------------------

	template <Matcher Matcher>
	MatcherRule::MatcherRule(const std::string& name, Matcher&& matcher, bool createNode)
	    : IRule(name), m_Matcher(std::make_unique<Matcher>(std::move(matcher))), m_CreateNode(createNode)
	{
	}

	template <Matcher Matcher>
	MatcherRule::MatcherRule(std::string&& name, Matcher&& matcher, bool createNode)
	    : IRule(std::move(name)), m_Matcher(std::make_unique<Matcher>(std::move(matcher))), m_CreateNode(createNode)
	{
	}
} // namespace CommonLexer