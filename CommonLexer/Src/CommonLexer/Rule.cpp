#include "CommonLexer/Rule.h"

namespace CommonLexer
{
	IRule::IRule(const std::string& name)
	    : m_Name(name) {}

	IRule::IRule(std::string&& name)
	    : m_Name(std::move(name)) {}
} // namespace CommonLexer