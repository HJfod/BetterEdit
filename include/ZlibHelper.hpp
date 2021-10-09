#ifndef _GDCRYPTO_ZLIBHELPER_HPP
#define _GDCRYPTO_ZLIBHELPER_HPP

#include <zlib.h>
#include <vector>

namespace gdcrypto::zlib
{
	class DeflateStream
	{
		z_stream m_Stream;
		std::vector<uint8_t> const& m_Buffer;
		bool m_HasMoreData;
	public:
		DeflateStream(std::vector<uint8_t> const& buffer);
		~DeflateStream();

		void closeStream();

		bool hasMoreData() const { return m_HasMoreData; }

		bool getChunk(std::vector<uint8_t>& chunk);
	};

	class InflateStream
	{
		z_stream m_Stream;
		std::vector<uint8_t> const& m_Buffer;
		bool m_HasMoreData;
	public:
		InflateStream(std::vector<uint8_t> const& buffer);
		~InflateStream();

		void closeStream();

		bool hasMoreData() const { return m_HasMoreData; }

		bool getChunk(std::vector<uint8_t>& chunk);
	};

	std::vector<uint8_t> deflateBuffer(std::vector<uint8_t> const& buffer);
	std::vector<uint8_t> inflateBuffer(std::vector<uint8_t> const& buffer);
}

#endif /* _GDCRYPTO_ZLIBHELPER_HPP */