#pragma once

#include <cstddef>

#include <string>
#include <vector>

namespace CommonLexer
{
	class ISource;

	struct SourcePoint
	{
	public:
		SourcePoint(std::size_t index = 0) : m_Index(index) {}

		operator std::size_t() const { return m_Index; }

		[[nodiscard]] std::size_t getLine(ISource* source) const;
		[[nodiscard]] std::size_t getColumn(ISource* source) const;

		SourcePoint& operator++()
		{
			++m_Index;
			return *this;
		}
		SourcePoint operator++(int)
		{
			auto copy = *this;
			++m_Index;
			return copy;
		}
		SourcePoint& operator--()
		{
			--m_Index;
			return *this;
		}
		SourcePoint operator--(int)
		{
			auto copy = *this;
			--m_Index;
			return copy;
		}

		SourcePoint& operator+=(std::size_t count)
		{
			m_Index += count;
			return *this;
		}
		SourcePoint& operator-=(std::size_t count)
		{
			m_Index -= count;
			return *this;
		}

		[[nodiscard]] SourcePoint operator+(std::size_t count)
		{
			auto copy = *this;
			copy += count;
			return copy;
		}
		[[nodiscard]] SourcePoint operator-(std::size_t count)
		{
			auto copy = *this;
			copy -= count;
			return copy;
		}

	public:
		std::size_t m_Index;
	};

	class SourceIterator
	{
	public:
		using value_type        = char;
		using difference_type   = std::size_t;
		using reference         = char&;
		using pointer           = char*;
		using iterator_category = std::bidirectional_iterator_tag;

		SourceIterator() : m_Source(nullptr), m_CacheOffset(0) {}
		SourceIterator(ISource* source, SourcePoint point) : m_Source(source), m_Point(point), m_CacheOffset(0) { recache(); }

		[[nodiscard]] operator SourcePoint() const { return m_Point; }

		[[nodiscard]] char operator*() const
		{
			if (m_Cache.empty() || m_CacheOffset >= m_Cache.size())
				return '\0';
			return m_Cache[m_CacheOffset];
		}

		SourceIterator& operator++()
		{
			++m_Point;
			++m_CacheOffset;
			recache();
			return *this;
		}
		SourceIterator operator++(int)
		{
			auto copy = *this;
			++m_Point;
			++m_CacheOffset;
			recache();
			return copy;
		}
		SourceIterator& operator--()
		{
			--m_Point;
			--m_CacheOffset;
			recache();
			return *this;
		}
		SourceIterator operator--(int)
		{
			auto copy = *this;
			--m_Point;
			--m_CacheOffset;
			recache();
			return copy;
		}

		SourceIterator& operator+=(std::size_t count)
		{
			m_Point += count;
			m_CacheOffset += count;
			recache();
			return *this;
		}
		SourceIterator& operator-=(std::size_t count)
		{
			m_Point -= count;
			m_CacheOffset -= count;
			recache();
			return *this;
		}

		[[nodiscard]] SourceIterator operator+(std::size_t count)
		{
			auto copy = *this;
			copy += count;
			return copy;
		}
		[[nodiscard]] SourceIterator operator-(std::size_t count)
		{
			auto copy = *this;
			copy -= count;
			return copy;
		}

		[[nodiscard]] bool operator==(const SourceIterator& other) const { return m_Point == other.m_Point; }
		[[nodiscard]] bool operator!=(const SourceIterator& other) const { return m_Point != other.m_Point; }
		[[nodiscard]] bool operator<(const SourceIterator& other) const { return m_Point < other.m_Point; }
		[[nodiscard]] bool operator<=(const SourceIterator& other) const { return m_Point <= other.m_Point; }
		[[nodiscard]] bool operator>(const SourceIterator& other) const { return m_Point > other.m_Point; }
		[[nodiscard]] bool operator>=(const SourceIterator& other) const { return m_Point >= other.m_Point; }

	private:
		void recache();

	private:
		ISource*    m_Source;
		SourcePoint m_Point;
		std::string m_Cache;
		std::size_t m_CacheOffset;
	};

	struct SourceSpan
	{
	public:
		SourceSpan(SourcePoint start, SourcePoint end) : m_Start(start), m_End(end) {}

		[[nodiscard]] SourceIterator           begin(ISource* source);
		[[nodiscard]] SourceIterator           end(ISource* source);
		[[nodiscard]] std::string              getSpan(ISource* source);
		[[nodiscard]] std::vector<std::string> getLines(ISource* source);

		[[nodiscard]] std::size_t length() const { return m_End - m_Start; }

	public:
		SourcePoint m_Start, m_End;
	};
} // namespace CommonLexer