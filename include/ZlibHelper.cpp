#include "ZlibHelper.hpp"

#include <stdexcept>

using namespace gdcrypto;
using namespace gdcrypto::zlib;

//Constants

static auto constexpr CHUNK_SIZE = 32768u;

static auto constexpr Z_GZIP_HEADER = 0x10;
static auto constexpr Z_AUTO_HEADER = 0x20;

//DeflateStream

DeflateStream::DeflateStream(std::vector<uint8_t> const& buffer)
    : m_Stream({}),
    m_Buffer(buffer),
    m_HasMoreData(true)
{
    m_Stream.zalloc = nullptr;
    m_Stream.zfree = nullptr;
    m_Stream.opaque = nullptr;
    m_Stream.next_in = const_cast<uint8_t*>(m_Buffer.data());
    m_Stream.avail_in = 0u;

    if (deflateInit2(
        &m_Stream,
        Z_DEFAULT_COMPRESSION,
        Z_DEFLATED,
        MAX_WBITS | Z_GZIP_HEADER,
        8,
        Z_DEFAULT_STRATEGY) != Z_OK)
        throw std::runtime_error("deflate error");
}

DeflateStream::~DeflateStream()
{
    // code moved to closeStream()
}

void DeflateStream::closeStream()
{
    if (deflateEnd(&m_Stream) != Z_OK)
        throw std::runtime_error("deflate error");
}

bool DeflateStream::getChunk(std::vector<uint8_t>& chunk)
{
    int flush = Z_NO_FLUSH;

    if (!m_HasMoreData)
        return false;

    if (chunk.max_size() != CHUNK_SIZE)
        chunk.resize(CHUNK_SIZE);

    m_Stream.avail_in = CHUNK_SIZE;
    m_Stream.next_out = chunk.data();
    m_Stream.avail_out = CHUNK_SIZE;

    //Check for EOF
    if ((m_Stream.total_in + CHUNK_SIZE) > m_Buffer.size())
    {
        m_Stream.avail_in = m_Buffer.size() - m_Stream.total_in;
        flush = Z_FINISH;
    }

    auto state = deflate(&m_Stream, flush);

    if (state == Z_STREAM_END)
    {
        m_HasMoreData = false;
        state = Z_OK;
    }

    if (state == Z_OK)
    {
        auto dataLeft = CHUNK_SIZE - m_Stream.avail_out;
        chunk.resize(dataLeft);
        return true;
    }

    return false;
}

//InflateStream

InflateStream::InflateStream(std::vector<uint8_t> const& buffer)
	: m_Stream({}),
    m_Buffer(buffer),
    m_HasMoreData(true)
{
	m_Stream.zalloc = nullptr;
	m_Stream.zfree = nullptr;
	m_Stream.opaque = nullptr;
	m_Stream.next_in = const_cast<uint8_t*>(m_Buffer.data());
	m_Stream.avail_in = m_Buffer.size();

    if (inflateInit2(
        &m_Stream,
        MAX_WBITS | Z_AUTO_HEADER) != Z_OK)
        throw std::runtime_error("zlib inflate init error");
}

InflateStream::~InflateStream()
{
    // code moved to closeStream()
}

void InflateStream::closeStream()
{
    if (inflateEnd(&m_Stream) != Z_OK)
        throw std::runtime_error("Failed to cleanup the decompressor");
}

bool InflateStream::getChunk(std::vector<uint8_t>& chunk)
{
    if (!m_HasMoreData)
        return false;

    if (chunk.max_size() != CHUNK_SIZE)
        chunk.resize(CHUNK_SIZE);

    m_Stream.next_out = chunk.data();
    m_Stream.avail_out = CHUNK_SIZE;

    auto state = inflate(&m_Stream, Z_NO_FLUSH);

    if (state == Z_STREAM_END)
    {
        m_HasMoreData = false;
        state = Z_OK;
    }

    if (state == Z_OK)
    {
        auto dataLeft = CHUNK_SIZE - m_Stream.avail_out;
        chunk.resize(dataLeft);
        return true;
    }

    return false;
}

//Wrappers

std::vector<uint8_t> zlib::deflateBuffer(std::vector<uint8_t> const& buffer)
{
    DeflateStream stream(buffer);
    std::vector<uint8_t> out, chunk;

    while (stream.hasMoreData())
    {
        if (!stream.getChunk(chunk))
        {
            out.clear();
            break;
        }

        out.insert(
            out.end(),
            chunk.begin(),
            chunk.end());
    }

    stream.closeStream();

    return out;
}

std::vector<uint8_t> zlib::inflateBuffer(std::vector<uint8_t> const& buffer)
{
    InflateStream stream(buffer);
    std::vector<uint8_t> out, chunk;

    while (stream.hasMoreData())
    {
        if (!stream.getChunk(chunk))
        {
            out.clear();
            break;
        }

        out.insert(
            out.end(),
            chunk.begin(),
            chunk.end());
    }

    stream.closeStream();

    return out;
}