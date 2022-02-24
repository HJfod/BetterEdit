#include "gmd.hpp"
#include "../../include/ZlibHelper.hpp"
#include "../../include/Base64.hpp"

using namespace gmd;

std::vector<uint8_t> decoder::XORX(const std::vector<uint8_t> & _data, int _key) {
    std::vector<uint8_t> copy(_data);

    for (unsigned int ix = 0; ix < copy.size(); ix++)
        copy[ix] ^= _key;

    return copy;
}

std::vector<uint8_t> decoder::Base64X(const std::vector<uint8_t> & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);
    return b64.decode(_data);
}

std::vector<uint8_t> decoder::GZipX(const std::vector<uint8_t> & _data) {
    return gdcrypto::zlib::inflateBuffer(_data);
}


std::string decoder::GZip(const std::string & _data) {
    std::vector<uint8_t> data = convert_vs(_data);

    return convert_vs(decoder::GZipX(data));
}

std::string decoder::Base64(const std::string & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);

    std::vector<uint8_t> data = b64.decode(_data);

    return convert_vs(data);
}

std::string decoder::XOR(const std::string & _data, int _key) {
    std::vector<uint8_t> data = convert_vs(_data);

    for (uint8_t b : data)
        b ^= _key;

    return convert_vs(data);
}


std::vector<uint8_t> encoder::GZip(const std::vector<uint8_t> & _data) {
    return gdcrypto::zlib::deflateBuffer(_data);
}

std::vector<uint8_t> encoder::Base64(const std::vector<uint8_t> & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);

    std::string data = b64.encode(_data);

    return convert_vs(data);
}

std::vector<uint8_t> encoder::XOR(const std::vector<uint8_t> & _data, int _key) {
    return decoder::XORX(_data, _key);
}

