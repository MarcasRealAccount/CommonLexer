#pragma once

#include "Lex.h"
#include "Rule.h"

#include <memory>
#include <random>
#include <unordered_map>
#include <utility>

namespace CommonLexer
{
	class ISource;

	class Lexer
	{
	public:
		Lexer();

		Lex lexSource(ISource* source);
		Lex lexSource(ISource* source, SourceSpan span);

		template <Rule Rule>
		void registerRule(Rule&& rule);
		void registerRule(std::unique_ptr<IRule>&& rule);
		void setMainRule(const std::string& rule);

		[[nodiscard]] std::uint32_t getRuleID(const std::string& rule) const;
		[[nodiscard]] IRule*        getRule(std::uint32_t ruleID);
		[[nodiscard]] IRule*        getRule(const std::string& rule) { return getRule(getRuleID(rule)); }

	private:
		std::uint32_t newRuleID();

	private:
		std::mt19937 m_RNG;

		std::string m_MainRule;

		std::unordered_map<std::uint32_t, std::unique_ptr<IRule>> m_Rules;
	};

	//--------------------------
	// Template Implementations
	//--------------------------

	template <Rule Rule>
	void Lexer::registerRule(Rule&& rule)
	{
		registerRule(std::make_unique<Rule>(std::move(rule)));
	}
} // namespace CommonLexer