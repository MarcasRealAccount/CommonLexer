#pragma once

#include "Flags.h"
#include "Matcher.h"
#include "Tuple.h"

#include <memory>
#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace CommonLexer
{
	//---------
	// Depth 2
	//---------

	class CombinationMatcher final : public IMatcher
	{
	public:
		template <Matcher... Matchers>
		CombinationMatcher(Matchers&&... matchers);
		template <Matcher... Matchers>
		CombinationMatcher(Tuple<Matchers...>&& matchers);
		CombinationMatcher(std::vector<std::unique_ptr<IMatcher>>&& matchers);
		CombinationMatcher(CombinationMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::vector<std::unique_ptr<IMatcher>> m_Matchers;
	};

	class OrMatcher final : public IMatcher
	{
	public:
		template <Matcher... Matchers>
		OrMatcher(Matchers&&... matchers);
		template <Matcher... Matchers>
		OrMatcher(Tuple<Matchers...>&& matchers);
		OrMatcher(std::vector<std::unique_ptr<IMatcher>>&& matchers);
		OrMatcher(OrMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::vector<std::unique_ptr<IMatcher>> m_Matchers;
	};

	//---------
	// Depth 3
	//---------

	class RangeMatcher final : public IMatcher
	{
	public:
		template <Matcher Matcher>
		RangeMatcher(Matcher&& matcher, std::size_t lowerBounds = 0, std::size_t upperBounds = ~0ULL);
		RangeMatcher(std::unique_ptr<IMatcher>&& matcher, std::size_t lowerBounds = 0, std::size_t upperBounds = ~0ULL);
		RangeMatcher(RangeMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::unique_ptr<IMatcher> m_Matcher;

		std::size_t m_LowerBounds;
		std::size_t m_UpperBounds;
	};

	class OptionalMatcher final : public IMatcher
	{
	public:
		template <Matcher Matcher>
		OptionalMatcher(Matcher&& matcher);
		OptionalMatcher(std::unique_ptr<IMatcher>&& matcher);
		OptionalMatcher(OptionalMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::unique_ptr<IMatcher> m_Matcher;
	};

	class NegativeMatcher final : public IMatcher
	{
	public:
		template <Matcher Matcher>
		NegativeMatcher(Matcher&& matcher);
		NegativeMatcher(std::unique_ptr<IMatcher>&& matcher);
		NegativeMatcher(NegativeMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::unique_ptr<IMatcher> m_Matcher;
	};

	//---------
	// Depth 4
	//---------

	using SpaceDirectionFlags = Flags<std::uint8_t>;

	namespace ESpaceDirection
	{
		static constexpr SpaceDirectionFlags None  = 0;
		static constexpr SpaceDirectionFlags Right = 1;
		static constexpr SpaceDirectionFlags Left  = 2;
		static constexpr SpaceDirectionFlags Both  = Left | Right;
	} // namespace ESpaceDirection

	enum class ESpaceMethod
	{
		Normal,
		Whitespace
	};

	class SpaceMatcher final : public IMatcher
	{
	public:
		template <Matcher Matcher>
		SpaceMatcher(Matcher&& matcher, bool forced = false, SpaceDirectionFlags direction = ESpaceDirection::Right, ESpaceMethod method = ESpaceMethod::Normal);
		SpaceMatcher(std::unique_ptr<IMatcher>&& matcher, bool forced = false, SpaceDirectionFlags direction = ESpaceDirection::Right, ESpaceMethod method = ESpaceMethod::Normal);
		SpaceMatcher(SpaceMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

		MatchResult matchNormalSpaces(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span);
		MatchResult matchWhitespaceSpaces(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span);

	private:
		std::unique_ptr<IMatcher> m_Matcher;

		bool                m_Forced;
		SpaceDirectionFlags m_Direction;
		ESpaceMethod        m_Method;
	};

	//---------
	// Depth 5
	//---------

	class NamedGroupMatcher final : public IMatcher
	{
	public:
		template <Matcher Matcher>
		NamedGroupMatcher(const std::string& name, Matcher&& matcher);
		template <Matcher Matcher>
		NamedGroupMatcher(std::string&& name, Matcher&& matcher);
		NamedGroupMatcher(const std::string& name, std::unique_ptr<IMatcher>&& matcher);
		NamedGroupMatcher(std::string&& name, std::unique_ptr<IMatcher>&& matcher);
		NamedGroupMatcher(NamedGroupMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::string m_Name;

		std::unique_ptr<IMatcher> m_Matcher;
	};

	class NamedGroupReferenceMatcher final : public IMatcher
	{
	public:
		NamedGroupReferenceMatcher(const std::string& name);
		NamedGroupReferenceMatcher(std::string&& name);
		NamedGroupReferenceMatcher(NamedGroupReferenceMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::string m_Name;
	};

	class ReferenceMatcher final : public IMatcher
	{
	public:
		ReferenceMatcher(const std::string& name);
		ReferenceMatcher(std::string&& name);
		ReferenceMatcher(ReferenceMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::string m_Name;

		IRule* m_Rule;
	};

	class TextMatcher final : public IMatcher
	{
	public:
		TextMatcher(const std::string& text);
		TextMatcher(std::string&& text);
		TextMatcher(TextMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::string m_Text;
	};

	class RegexMatcher final : public IMatcher
	{
	public:
		RegexMatcher(const std::string& regex);
		RegexMatcher(std::string&& regex);
		RegexMatcher(RegexMatcher&& move) noexcept;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) override;

	private:
		std::regex m_Regex;
	};

	//--------------------------
	// Template Implementations
	//--------------------------

	template <Matcher... Matchers>
	CombinationMatcher::CombinationMatcher(Matchers&&... matchers)
	    : m_Matchers(Details::make_unique_ptrs<IMatcher>(Tuple { std::move(matchers)... }))
	{
	}

	template <Matcher... Matchers>
	CombinationMatcher::CombinationMatcher(Tuple<Matchers...>&& matchers)
	    : m_Matchers(Details::make_unique_ptrs<IMatcher>(std::move(matchers)))
	{
	}

	template <Matcher... Matchers>
	OrMatcher::OrMatcher(Matchers&&... matchers)
	    : m_Matchers(Details::make_unique_ptrs<IMatcher>(Tuple { std::move(matchers)... }))
	{
	}

	template <Matcher... Matchers>
	OrMatcher::OrMatcher(Tuple<Matchers...>&& matchers)
	    : m_Matchers(Details::make_unique_ptrs<IMatcher>(std::move(matchers)))
	{
	}

	template <Matcher Matcher>
	RangeMatcher::RangeMatcher(Matcher&& matcher, std::size_t lowerBounds, std::size_t upperBounds)
	    : m_Matcher(std::make_unique<Matcher>(std::move(matcher))), m_LowerBounds(lowerBounds), m_UpperBounds(upperBounds)
	{
	}

	template <Matcher Matcher>
	OptionalMatcher::OptionalMatcher(Matcher&& matcher)
	    : m_Matcher(std::make_unique<Matcher>(std::move(matcher)))
	{
	}

	template <Matcher Matcher>
	NegativeMatcher::NegativeMatcher(Matcher&& matcher)
	    : m_Matcher(std::make_unique<Matcher>(std::move(matcher)))
	{
	}

	template <Matcher Matcher>
	SpaceMatcher::SpaceMatcher(Matcher&& matcher, bool forced, SpaceDirectionFlags direction, ESpaceMethod method)
	    : m_Matcher(std::make_unique<Matcher>(std::move(matcher))), m_Forced(forced), m_Direction(direction), m_Method(method)
	{
	}

	template <Matcher Matcher>
	NamedGroupMatcher::NamedGroupMatcher(const std::string& name, Matcher&& matcher)
	    : m_Name(name), m_Matcher(std::make_unique<Matcher>(std::move(matcher)))
	{
	}

	template <Matcher Matcher>
	NamedGroupMatcher::NamedGroupMatcher(std::string&& name, Matcher&& matcher)
	    : m_Name(std::move(name)), m_Matcher(std::make_unique<Matcher>(std::move(matcher)))
	{
	}
} // namespace CommonLexer