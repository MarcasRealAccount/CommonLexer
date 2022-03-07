#pragma once

#include "SourceSpan.h"

#include <cstddef>

#include <string>
#include <vector>

namespace CommonLexer
{
	class ISource
	{
	public:
		virtual ~ISource() = default;

		[[nodiscard]] virtual std::size_t getSize()                             = 0;
		[[nodiscard]] virtual std::size_t getNumLines()                         = 0;
		[[nodiscard]] virtual std::size_t getIndexFromLine(std::size_t line)    = 0;
		[[nodiscard]] virtual std::size_t getLineFromIndex(std::size_t index)   = 0;
		[[nodiscard]] virtual std::size_t getColumnFromIndex(std::size_t index) = 0;

		[[nodiscard]] SourceSpan getCompleteSpan() { return { 0, getSize() }; }

		[[nodiscard]] virtual std::string getSpan(std::size_t index, std::size_t length) = 0;
		[[nodiscard]] std::string         getSpan(SourceSpan span) { return getSpan(span.m_Start, span.length()); }

		[[nodiscard]] virtual std::string getLine(std::size_t line) = 0;
		[[nodiscard]] std::string         getLine(SourcePoint point) { return getLine(getLineFromIndex(point)); }

		[[nodiscard]] virtual std::vector<std::string> getLines(std::size_t startLine, std::size_t lines) = 0;
		[[nodiscard]] std::vector<std::string>         getLines(SourcePoint start, SourcePoint end)
		{
			auto startLine = getLineFromIndex(start);
			auto endLine   = getLineFromIndex(end);
			return getLines(startLine, endLine - startLine);
		}
		[[nodiscard]] std::vector<std::string> getLines(SourceSpan span) { return getLines(span.m_Start, span.m_End); }
	};

	class StringSource : public ISource
	{
	public:
		explicit StringSource(const std::string& str);
		explicit StringSource(std::string&& str);

		[[nodiscard]] virtual std::size_t getSize() override;
		[[nodiscard]] virtual std::size_t getNumLines() override;
		[[nodiscard]] virtual std::size_t getIndexFromLine(std::size_t line) override;
		[[nodiscard]] virtual std::size_t getLineFromIndex(std::size_t index) override;
		[[nodiscard]] virtual std::size_t getColumnFromIndex(std::size_t index) override;

		[[nodiscard]] virtual std::string getSpan(std::size_t index, std::size_t length) override;

		[[nodiscard]] virtual std::string getLine(std::size_t line) override;

		[[nodiscard]] virtual std::vector<std::string> getLines(std::size_t startLine, std::size_t lines) override;

	private:
		void setupLineToIndex();

	private:
		std::string              m_Str;
		std::vector<std::size_t> m_LineToIndex;
	};
} // namespace CommonLexer