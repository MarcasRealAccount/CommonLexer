#include "CommonLexer/SourceSpan.h"
#include "CommonLexer/Source.h"

namespace CommonLexer
{
	std::size_t SourcePoint::getLine(ISource* source) const
	{
		return source->getLineFromIndex(m_Index);
	}

	std::size_t SourcePoint::getColumn(ISource* source) const
	{
		return source->getColumnFromIndex(m_Index);
	}

	void SourceIterator::recache()
	{
		if (m_Source && m_CacheOffset >= m_Cache.size())
		{
			std::size_t index = m_Point.m_Index < 128 ? 0 : m_Point.m_Index - 128;
			m_Cache           = m_Source->getSpan(index, 256);
			m_CacheOffset     = m_Point.m_Index < 128 ? m_Point.m_Index : 128;
		}
	}

	SourceIterator SourceSpan::begin(ISource* source)
	{
		return { source, m_Start };
	}

	SourceIterator SourceSpan::end(ISource* source)
	{
		return { source, m_End };
	}

	std::string SourceSpan::getSpan(ISource* source)
	{
		return source->getSpan(*this);
	}

	std::vector<std::string> SourceSpan::getLines(ISource* source)
	{
		return source->getLines(*this);
	}
} // namespace CommonLexer