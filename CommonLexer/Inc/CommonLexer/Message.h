#pragma once

#include "SourceSpan.h"

#include <string>
#include <utility>

namespace CommonLexer
{
	enum class EMessageSeverity
	{
		Warning,
		Error
	};

	struct Message
	{
	public:
		Message(const std::string& message, SourcePoint point, SourceSpan span, std::uint32_t ruleID, EMessageSeverity severity = EMessageSeverity::Error) : m_Message(message), m_Point(point), m_Span(span), m_RuleID(ruleID), m_Severity(severity) {}
		Message(std::string&& message, SourcePoint point, SourceSpan span, std::uint32_t ruleID, EMessageSeverity severity = EMessageSeverity::Error) : m_Message(std::move(message)), m_Point(point), m_Span(span), m_RuleID(ruleID), m_Severity(severity) {}

		[[nodiscard]] auto& getMessage() const { return m_Message; }
		[[nodiscard]] auto  getPoint() const { return m_Point; }
		[[nodiscard]] auto  getSpan() const { return m_Span; }
		[[nodiscard]] auto  getRuleID() const { return m_RuleID; }
		[[nodiscard]] auto  getSeverity() const { return m_Severity; }

	private:
		std::string      m_Message;
		SourcePoint      m_Point;
		SourceSpan       m_Span;
		std::uint32_t    m_RuleID;
		EMessageSeverity m_Severity;
	};
} // namespace CommonLexer