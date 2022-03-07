#pragma once

#include "Matcher.h"

#include <type_traits>

namespace CommonLexer
{
	class IRule : public IMatcher
	{
	public:
		IRule(const std::string& name);
		IRule(std::string&& name);

		[[nodiscard]] auto& getName() const { return m_Name; }
		[[nodiscard]] auto  getID() { return m_ID; }

		void setID(std::uint32_t id) { m_ID = id; }

	private:
		std::string   m_Name;
		std::uint32_t m_ID;
	};

	template <class T>
	concept Rule = std::is_base_of_v<IRule, T>;
} // namespace CommonLexer