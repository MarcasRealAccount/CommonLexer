#pragma once

#include "Message.h"
#include "SourceSpan.h"

#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace CommonLexer
{
	struct Lex;
	class IRule;
	struct Node;

	struct MatcherState
	{
	public:
		MatcherState(Lex* lex, ISource* source, SourceSpan sourceSpan) : m_Lex(lex), m_Source(source), m_SourceSpan(sourceSpan), m_CurrentRule(nullptr) {}

		void setCurrentRule(IRule* rule, SourcePoint begin);

		void setGroupedValue(const std::string& name, SourceSpan span);
		void setGroupedValue(std::string&& name, SourceSpan span);

		[[nodiscard]] SourceSpan getGroupedValue(const std::string& name) const;

	public:
		std::unordered_map<std::string, SourceSpan> m_GroupedValues;

		Lex* m_Lex;

		ISource*   m_Source;
		SourceSpan m_SourceSpan;

		IRule*      m_CurrentRule;
		SourcePoint m_RuleBegin;
	};

	struct MatcherScopedState
	{
	public:
		explicit MatcherScopedState(Node* parentNode) : m_ParentNode(parentNode) {}

		template <class... Ts>
		void addMessage(Ts&&... args)
		{
			m_Messages.emplace_back(std::forward<Ts>(args)...);
		}

		void addMessages(const MatcherScopedState& state);
		void addMessages(MatcherScopedState&& state);
		void addMessages(const std::vector<Message>& messages);
		void addMessages(std::vector<Message>&& messages);

	public:
		std::vector<Message> m_Messages;

		Node* m_ParentNode;
	};

	enum class EMatchStatus
	{
		Success,
		Skip,
		Failure
	};

	struct MatchResult
	{
	public:
		EMatchStatus m_Status;
		SourceSpan   m_Span;
	};

	class IMatcher
	{
	public:
		virtual ~IMatcher() = default;

		virtual MatchResult match(MatcherState& state, MatcherScopedState& scopedState, SourceSpan span) = 0;
	};

	template <class T>
	concept Matcher = std::is_base_of_v<IMatcher, T>;
} // namespace CommonLexer