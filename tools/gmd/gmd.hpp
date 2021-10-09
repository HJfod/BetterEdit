#pragma once

#include "../../BetterEdit.hpp"

namespace gmd {
    using byte_array = std::vector<uint8_t>;

    enum GmdType {
        kGmdTypeGmd,
        kGmdTypeGmd2,
        kGmdTypeLvl,
    };

    constexpr const char* GmdTypeToString(GmdType);
    
    const enum ExportFlag {
        kfExportFlag_None           = 0b0000000,
        kfExportFlag_IncludeSong    = 0b0000001,
    };

    byte_array  convert_vs(std::string const& string);
    std::string convert_vs(byte_array  const& vector);

    namespace encoder {
        std::vector<uint8_t> XOR(const std::vector<uint8_t> & data, int key);
        std::vector<uint8_t> Base64(const std::vector<uint8_t> & data);
        std::vector<uint8_t> GZip(const std::vector<uint8_t> & data);
    }

    namespace decoder {
        std::string XOR(const std::string & data, int key);
        std::string Base64(const std::string & data);
        std::string GZip(const std::string & data);

        std::vector<uint8_t> XORX(const std::vector<uint8_t> & data, int key);
        std::vector<uint8_t> Base64X(const std::vector<uint8_t> & data);
        std::vector<uint8_t> GZipX(const std::vector<uint8_t> & data);
    };

    template<typename T = char>
    struct Result {
        bool success;
        T data;
        const char* error = nullptr;

        template<typename R>
        inline Result(Result<R> const& other) {
            this->success = other.success;
            this->error = other.error;
        }

        inline Result(bool suc) {
            this->success = suc;
        }

        inline operator int() {
            return this->success;
        }

        inline static Result res() {
            auto res = Result;
            res.success = true;
            return res;
        }

        inline static Result res(T d) {
            auto res = Result;
            res.data = d;
            res.success = true;
            return res;
        }

        inline static Result err(const char* e) {
            auto res = Result;
            res.error = e;
            res.success = false;
            return res;
        }
    };

    class GmdFile {
        protected:
            GJGameLevel* m_pLevel = nullptr;
            std::string m_sPath;
            std::string m_sFullPath;
            std::string m_sFileName;
            int m_nFlags = kfExportFlag_None;
            GmdType m_eFormat = kGmdTypeGmd;
            std::string m_sSongPath;
            byte_array m_vSongData;

            void saveFileFormat();

        public:
            GmdFile(GJGameLevel*);
            GmdFile(std::string const& path);

            Result<GJGameLevel*> parseLevel();
            Result<std::string> createString();
            Result<> saveFile();

            void setFileName(std::string const&);
            void setType(GmdType);
            void setExportFlags(int);
            void setExportFlag(ExportFlag);

            static void removeNullbytesFromString(std::string &);
    };
}
