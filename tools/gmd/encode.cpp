#include "gmd.hpp"
#include "../../include/ZlibHelper.hpp"

std::vector<uint8_t> gmd::decoder::XORX(const std::vector<uint8_t> & _data, int _key) {
    std::vector<uint8_t> copy(_data);

    for (unsigned int ix = 0; ix < copy.size(); ix++)
        copy[ix] ^= _key;

    return copy;
}

std::vector<uint8_t> gmd::decoder::Base64X(const std::vector<uint8_t> & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);
    return b64.decode(_data);
}

std::vector<uint8_t> gmd::decoder::GZipX(const std::vector<uint8_t> & _data) {
    return gdcrypto::zlib::inflateBuffer(_data);
}


std::string gmd::decoder::GZip(const std::string & _data) {
    std::vector<uint8_t> data = gmd::decoder::Convert(_data);

    return gmd::decoder::Convert(gmd::decoder::GZipX(data));
}

std::string gmd::decoder::Base64(const std::string & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);

    std::vector<uint8_t> data = b64.decode(_data);

    return gmd::decoder::Convert(data);
}

std::string gmd::decoder::XOR(const std::string & _data, int _key) {
    std::vector<uint8_t> data = gmd::decoder::Convert(_data);

    for (uint8_t b : data)
        b ^= _key;

    return gmd::decoder::Convert(data);
}


std::vector<uint8_t> gmd::encoder::GZip(const std::vector<uint8_t> & _data) {
    return gdcrypto::zlib::deflateBuffer(_data);
}

std::vector<uint8_t> gmd::encoder::Base64(const std::vector<uint8_t> & _data) {
    gdcrypto::base64::Base64 b64(gdcrypto::base64::URL_SAFE_DICT);

    std::string data = b64.encode(_data);

    return gmd::decoder::Convert(data);
}

std::vector<uint8_t> gmd::encoder::XOR(const std::vector<uint8_t> & _data, int _key) {
    return gmd::decoder::XORX(_data, _key);
}

