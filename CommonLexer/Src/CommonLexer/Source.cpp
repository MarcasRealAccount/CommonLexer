#include "CommonLexer/Source.h"

namespace CommonLexer
{
	StringSource::StringSource(const std::string& str)
	    : m_Str(str)
	{
		setupLineToIndex();
	}

	StringSource::StringSource(std::string&& str)
	    : m_Str(std::move(str))
	{
		setupLineToIndex();
	}

	std::size_t StringSource::getSize()
	{
		return m_Str.size();
	}

	std::size_t StringSource::getNumLines()
	{
		return m_LineToIndex.size();
	}

	std::size_t StringSource::getIndexFromLine(std::size_t line)
	{
		return line > 0 && line <= m_LineToIndex.size() ? m_LineToIndex[line - 1] : ~0ULL;
	}

	std::size_t StringSource::getLineFromIndex(std::size_t index)
	{
		for (std::size_t i = m_LineToIndex.size(); i > 0; --i)
			if (m_LineToIndex[i - 1] <= index)
				return i;
		return ~0ULL;
	}

	std::size_t StringSource::getColumnFromIndex(std::size_t index)
	{
		std::size_t line = getLineFromIndex(index);
		if (line == ~0ULL)
			return 1;
		std::size_t lineStart = getIndexFromLine(line);
		if (lineStart == ~0ULL)
			return 1;
		return index - lineStart + 1;
	}

	std::string StringSource::getSpan(std::size_t index, std::size_t length)
	{
		if (index >= m_Str.size())
			return {};

		if ((index + length) >= m_Str.size())
			length = m_Str.size() - index;

		return m_Str.substr(index, length);
	}

	std::string StringSource::getLine(std::size_t line)
	{
		std::size_t lineStart = getIndexFromLine(line);
		if (lineStart == ~0ULL)
			return {};
		std::size_t lineEnd = getIndexFromLine(line + 1);
		if (lineStart == ~0ULL)
			return {};
		return m_Str.substr(lineStart, lineEnd - lineStart - 1);
	}

	std::vector<std::string> StringSource::getLines(std::size_t startLine, std::size_t lines)
	{
		std::vector<std::string> lns;
		lns.reserve(lines);
		for (std::size_t line = startLine, end = startLine + lines; line != end; ++line)
			lns.push_back(getLine(line));
		return lns;
	}

	void StringSource::setupLineToIndex()
	{
		m_LineToIndex.emplace_back(0);
		for (std::size_t i = 0; i < m_Str.size(); ++i)
			if (m_Str[i] == '\n')
				m_LineToIndex.emplace_back(i + 1);
	}
} // namespace CommonLexer