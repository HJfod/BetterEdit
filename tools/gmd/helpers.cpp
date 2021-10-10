#include "helpers.hpp"
#include <regex>

using namespace gmd;

std::string gmd::sanitizeString(std::string const& _text, bool _actually_do_it) {
    if (_actually_do_it) {
        std::string s = _text;
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c){ return std::tolower(c); });
        s = std::regex_replace(s, std::regex("^ +| +$|( ) +"), "$1");
        return s;
    }
    return _text;
}

std::string gmd::decodeCompression(std::string const& _text, std::string const& _comp_args) {
    // decode compression if such has been applied
    if (!(_comp_args == "" || _comp_args == "none")) {
        std::vector<uint8_t> decr = convert_vs(_text);
        std::string comps = _comp_args;

        while (comps.size() > 0) {
            std::string com;
            if (comps.find(";") == std::string::npos) {
                com = comps;
                comps = "";
            } else {
                com = comps.substr(0, comps.find_first_of(";"));
                comps = comps.substr(comps.find_first_of(";") + 1);
            }

            if (com == "") continue;

            int key = 0;
            // check if key has been defined
            if (com.find("-") != std::string::npos) {
                key = std::stoi(com.substr(com.find_last_of("-") + 1));
                com = com.substr(0, com.find_first_of("-"));
            }

            switch (hash(com.c_str())) {
                case hash("gzip"): {
                    decr = decoder::GZipX(decr);
                } break;

                case hash("base64"): {
                    decr = decoder::Base64X(decr);
                } break;

                case hash("xor"): {
                    decr = decoder::XORX(decr, key);
                } break;
            }
        }

        return convert_vs(decr);
    }

    return _text;
}

std::string gmd::applyCompression(std::string const& _text, std::string const& _comp_args) {
    if (!(_comp_args == "" || _comp_args == "none")) {
        std::vector<uint8_t> decr = convert_vs(_text);
        std::string comps = _comp_args;

        while (comps.size() > 0) {
            std::string com;
            if (comps.find(";") == std::string::npos) {
                com = comps;
                comps = "";
            } else {
                com = comps.substr(comps.find_last_of(";") + 1);
                comps = comps.substr(0, comps.find_last_of(";"));
            }

            if (com == "") continue;

            int key = 0;
            // check if key has been defined
            if (com.find("-") != std::string::npos) {
                key = std::stoi(com.substr(com.find_last_of("-") + 1));
                com = com.substr(0, com.find_first_of("-"));
            }

            switch (hash(com.c_str())) {
                case hash("gzip"): {
                    decr = encoder::GZip(decr);
                } break;

                case hash("base64"): {
                    decr = encoder::Base64(decr);
                } break;

                case hash("xor"): {
                    decr = encoder::XOR(decr, key);
                } break;
            }
        }

        return convert_vs(decr);
    }

    return _text;
}

std::string gmd::fixPlist(std::string const& _text) {
    return "<plist version=\"1.0\" gjver=\"2.0\"><dict><k>lvl</k>" + _text + "</dict></plist>";
}

