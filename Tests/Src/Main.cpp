#include <CommonCLI/Colors.h>
#include <CommonCLI/Core.h>
#include <CommonCLI/KeyValue/KVHandler.h>
#include <CommonLexer/Lex.h>
#include <CommonLexer/Lexer.h>
#include <CommonLexer/Node.h>
#include <CommonLexer/Source.h>
#include <CommonLexer/SourceSpan.h>
#include <GrammarLexer/GrammarLexer.h>

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <utility>

std::size_t UTF8Codepoints(const std::string& str)
{
	auto itr = str.begin();
	auto end = str.end();

	std::size_t count = 0;
	while (itr != end)
	{
		std::uint8_t c = *itr;
		if (c < 0b1000'0000U)
			++itr;
		else if (c < 0b1110'0000U)
			itr += 2;
		else if (c < 0b1111'0000U)
			itr += 3;
		else if (c < 0b1111'1000U)
			itr += 4;
		++count;
	}
	return count;
}

std::string EscapeString(const std::string& str)
{
	return std::regex_replace(str, std::regex { "\"" }, "\\\"");
}

void PrintMessage(const CommonLexer::Message& message, CommonLexer::ISource* source)
{
	std::ostringstream str;
	switch (message.getSeverity())
	{
	case CommonLexer::EMessageSeverity::Warning:
		str << CommonCLI::Colors::Warn << "CommonLexer Warning ";
		break;
	case CommonLexer::EMessageSeverity::Error:
		str << CommonCLI::Colors::Error << "CommonLexer Error ";
		break;
	}
	auto point       = message.getPoint();
	auto pointLine   = point.getLine(source);
	auto pointColumn = point.getColumn(source);
	str << '(' << pointLine << ':' << pointColumn << "): ";
	str << message.getMessage() << ANSI::GraphicsForegroundDefault << '\n';

	auto span        = message.getSpan();
	auto lines       = source->getLines(span);
	auto beginLine   = span.m_Start.getLine(source);
	auto beginColumn = span.m_Start.getColumn(source);
	auto endLine     = span.m_End.getLine(source);
	auto endColumn   = span.m_End.getColumn(source);
	for (std::size_t i = 0; i < lines.size(); ++i)
	{
		std::string ln   = std::to_string(beginLine + i) + ": ";
		auto&       line = lines[i];
		if (line.empty())
			continue;
		str << ln << line << '\n'
		    << std::string(ln.size(), ' ');
		str << CommonCLI::Colors::Note;

		if (i == 0)
		{
			str << std::string(beginColumn - 1, ' ');

			if (beginLine + i == pointLine)
			{
				str << std::string(pointColumn - beginColumn, '~') << '^';

				if (endLine == beginLine)
					str << std::string(endColumn - pointColumn, '~');
				else
					str << std::string(line.size() - pointColumn, '~');
			}
			else
			{
				str << std::string(line.size() - beginColumn, '~');
			}
		}
		else if (i == lines.size() - 1)
		{
			if (beginLine + i == pointLine)
				str << std::string(pointColumn - 1, '~') << '^' << std::string(endColumn - pointColumn, '~');
		}
		else
		{
			str << std::string(line.size(), '~');
		}
		str << ANSI::GraphicsForegroundDefault << '\n';
	}

	std::cout << str.str();
}

void PrintLexNode(const CommonLexer::Node& node, std::vector<std::vector<std::string>>& lines, std::vector<bool>& layers, bool end = true)
{
	{
		std::vector<std::string> line;
		std::ostringstream       str;

		for (auto layer : layers)
		{
			if (layer)
				str << "\xE2\x94\x82 ";
			else
				str << "  ";
		}

		if (end)
			str << "\xE2\x94\x94\xE2\x94\x80";
		else
			str << "\xE2\x94\x9C\xE2\x94\x80";

		layers.push_back(!end);

		auto span = node.getSpan();
		str << node.getLex().getLexer()->getRule(node.getRule())->getName();
		line.push_back(str.str());
		str = {};

		auto beginLine   = span.m_Start.getLine(node.getLex().getSource());
		auto beginColumn = span.m_Start.getColumn(node.getLex().getSource());
		auto endLine     = span.m_End.getLine(node.getLex().getSource());
		auto endColumn   = span.m_End.getColumn(node.getLex().getSource());
		str << '(' << beginLine << ':' << beginColumn << " -> " << endLine << ':' << endColumn << ')';
		line.push_back(str.str());
		str = {};

		if (beginLine == endLine)
		{
			std::string s = node.getLex().getSource()->getSpan(span);
			if (s.find_first_of('\n') >= s.size())
			{
				str << "= \"" << EscapeString(s) << '"';
				line.push_back(str.str());
			}
		}
		lines.push_back(std::move(line));
	}

	auto& children = node.getChildren();
	for (std::size_t i = 0; i < children.size(); ++i)
		PrintLexNode(children[i], lines, layers, i >= children.size() - 1);

	layers.pop_back();
}

void PrintLex(const CommonLexer::Lex& lex)
{
	std::vector<std::vector<std::string>> lines;
	std::vector<bool>                     layers;
	PrintLexNode(lex.getRoot(), lines, layers);

	std::vector<std::size_t> sizes;
	for (auto& line : lines)
	{
		if ((line.size() - 1) > sizes.size())
			sizes.resize(line.size() - 1, 0);

		for (std::size_t i = 0; i < line.size() - 1; ++i)
		{
			auto&       column     = line[i];
			std::size_t codepoints = UTF8Codepoints(column);
			if (codepoints > sizes[i])
				sizes[i] = codepoints;
		}
	}

	std::ostringstream str;
	for (auto& line : lines)
	{
		for (std::size_t i = 0; i < line.size(); ++i)
		{
			auto& column = line[i];
			str << column;
			if (i < line.size() - 1)
				str << std::string(sizes[i] - UTF8Codepoints(column) + 1, ' ');
		}
		str << '\n';
	}
	std::cout << str.str();
}

int main(int argc, char** argv)
{
	using namespace CommonCLI::KeyValue;
	Handler handler("CommonLexerTests", "CommonLexer Tester", { 1, 0, 0, "", "alpha" });

	auto  result   = handler.handle(argc, const_cast<const char**>(argv));
	auto& messages = result.getMessages();
	bool  exit     = false;
	for (auto& message : messages)
	{
		if (message.isError())
		{
			std::cerr << message.getStr() << '\n';
			exit = true;
		}
		else
		{
			std::cout << message.getStr() << '\n';
		}
	}
	if (exit)
		return 1;

	std::ifstream stream { "CommonLexer.grammar", std::ios::ate };
	std::string   str;
	if (stream)
	{
		str.resize(stream.tellg());
		stream.seekg(0);
		stream.read(str.data(), str.size());
		stream.close();
	}
	CommonLexer::StringSource source { std::move(str) };

	GrammarLexer::GrammarLexer lexer;

	auto start = std::chrono::high_resolution_clock::now();
	auto lex   = lexer.lexSource(&source);
	auto end   = std::chrono::high_resolution_clock::now();
	if (!lex.getMessages().empty())
		for (auto& message : lex.getMessages())
			PrintMessage(message, &source);

	PrintLex(lex);

	std::cout << fmt::format("\nTime: {}\n", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start));
}