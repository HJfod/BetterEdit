#include "gmd.hpp"
#include <zipper.h>
#include <unzipper.h>
#include "../../include/json.hpp"
#include "helpers.hpp"
#include "../../include/ZlibHelper.hpp"

using namespace gmd;
using namespace zipper;
namespace fs = std::filesystem;

constexpr const char* GmdTypeToString(GmdType type) {
    switch (type) {
        case kGmdTypeGmd:   return "gmd";
        case kGmdTypeGmd2:  return "gmd2";
        case kGmdTypeLvl:   return "lvl";
        default:            return "gmd";
    }
}

byte_array gmd::convert_vs(std::string const& data) {
    return byte_array(data.begin(), data.end());
}

std::string gmd::convert_vs(byte_array const& data) {
    return std::string(data.begin(), data.end());
}

GmdFile::GmdFile(GJGameLevel* lvl) {
    this->m_pLevel = lvl;
    this->m_sFileName = lvl->m_sLevelName;
}

GmdFile::GmdFile(std::string const& pathStr) {
    auto path = fs::path(pathStr);
    if (fs::is_directory(path)) {
        this->m_sPath = pathStr;
    } else {
        this->m_sPath = path.parent_path().string();
    }
    this->m_sFileName = path.stem().string();
    this->m_sFullPath = pathStr;

    switch (hash(path.extension().string().c_str())) {
        case "gmd"_h:   this->m_eFormat = kGmdTypeGmd; break;
        case "gmd2"_h:  this->m_eFormat = kGmdTypeGmd2; break;
        case "lvl"_h:   this->m_eFormat = kGmdTypeLvl; break;
    }
}

void GmdFile::setFileName(std::string const& name) {
    this->m_sFileName = name;
}

void GmdFile::setExportFlags(int flags) {
    this->m_nFlags = flags;
}

void GmdFile::setExportFlag(ExportFlag flag) {
    this->m_nFlags |= flag;
}

void GmdFile::setType(GmdType type) {
    this->m_eFormat = type;
}

void GmdFile::removeNullbytesFromString(std::string & str) {
    for (auto i = 0u; i < str.size(); i++) {
        if (!str[i]) str[i] = 32;
    }
}

Result<GJGameLevel*> GmdFile::parseLevel() {
    if (!fs::exists(this->m_sFullPath)) {
        return Result<GJGameLevel*>::err(
            "File does not exist! (Likely reason is that the path or "
            "filename contains <co>unrecognized</c> characters; <cy>Move</c> "
            "the file to a different location and try again)"
        );
    }

    switch (this->m_eFormat) {
        case kGmdTypeGmd2: {
            Unzipper zip (this->m_sFullPath);

            std::vector<uint8_t> metaBuffer;
            std::vector<uint8_t> dataBuffer;

            zip.extractEntryToMemory("level.meta", metaBuffer);
            zip.extractEntryToMemory("level.data", dataBuffer);

            if (!metaBuffer.size())
                return Result<GJGameLevel*>::err("Unable to read level metadata!");

            if (!dataBuffer.size())
                return Result<GJGameLevel*>::err("Unable to read level data!");
            
            std::string metadata = convert_vs(metaBuffer);
            std::string data = convert_vs(dataBuffer);

            nlohmann::json metaj;
            try {
                metaj = nlohmann::json::parse(metadata);
            } catch (...) {
                zip.close();
                return Result<GJGameLevel*>::err("Unable to parse metadata!");
            }

            try {
                std::string songfile = metaj["song-file"];

                if (songfile.size()) {
                    if (this->m_nFlags & kfExportFlag_IncludeSong) {
                        if (!zip.extractEntryToMemory(songfile, this->m_vSongData))
                            goto skip_song_file;

                        std::string targetPath;
                        if (metaj["song-is-custom"]) {
                            try {
                                targetPath = MusicDownloadManager::pathForSong(
                                    std::stoi(songfile.substr(0, songfile.find_first_of(".")))
                                );
                            } catch (...) {
                                goto skip_song_file;
                            }
                        } else {
                            targetPath = "Resources/" + songfile;
                        }

                        this->m_sSongPath = targetPath;
                    }
                }
            } catch (...) {}
        skip_song_file:

            zip.close();
            
            std::string compr = metaj["compression"];

            compr = sanitizeString(compr);

            data = decodeCompression(data, compr);

            if (!data.size())
                return Result<GJGameLevel*>::err("Unable to decode compression!");

            auto dict = new DS_Dictionary;
            dict->loadRootSubDictFromString(data);

            return Result<GJGameLevel*>::res(
                GJGameLevel::createWithCoder(dict)
            );
        } break;

        case kGmdTypeGmd: {
            std::string data = readFileString(this->m_sFullPath);

            if (!data.size())
                return Result<GJGameLevel*>::err("File is empty");

            auto dict = new DS_Dictionary;
            dict->loadRootSubDictFromString(data);

            return Result<GJGameLevel*>::res(
                GJGameLevel::createWithCoder(dict)
            );
        } break;

        case kGmdTypeLvl: {
            auto datac = readFileString(this->m_sFullPath);

            if (!datac.length())
                return Result<GJGameLevel*>::err("File is empty");
            
            auto data = gmd::decoder::GZip(datac);
            data = "<d>" + data + "</d>";

            auto dict = new DS_Dictionary;
            dict->loadRootSubDictFromString(data);

            return Result<GJGameLevel*>::res(
                GJGameLevel::createWithCoder(dict)
            );
        } break;

        default:
            return Result<GJGameLevel*>::err("Invalid Format");
    }

    return Result<GJGameLevel*>::err("Unknown Error");
}

Result<std::string> GmdFile::createString() {
    if (!this->m_pLevel) {
        return Result<std::string>::err("Level is nullptr... somehow");
    }
    
    auto dict = new DS_Dictionary;
    this->m_pLevel->encodeWithCoder(dict);
    std::stringstream res;
    dict->doc.save(res);
    delete dict;

    return Result<std::string>::res(res.str());
}

Result<> GmdFile::saveFile() {
    auto str = this->createString();

    fs::path path = this->m_sPath;
    path /= this->m_sFileName;
    path.replace_extension(GmdTypeToString(this->m_eFormat));
    this->m_sFullPath = path.string();

    if (str) {
        switch (this->m_eFormat) {
            case kGmdTypeGmd2: {
                nlohmann::json metajson = nlohmann::json::object({
                    { "compression", "none" }
                });

                if (this->m_sSongPath.size()) {
                    metajson["song-file"] = fs::path(this->m_sSongPath).filename().string();
                    metajson["song-is-custom"] = this->m_pLevel->m_nSongID;
                }

                std::string metadata = metajson.dump();

                if (fs::exists(this->m_sFullPath))
                    if (!fs::remove(this->m_sFullPath))
                        return Result<>::err("Unable to write file!");

                Zipper zip (this->m_sFullPath);

                std::istringstream dataStream (str.data);
                std::istringstream metaStream (metadata);

                zip.add(dataStream, "level.data");
                zip.add(metaStream, "level.meta");

                if (this->m_sSongPath.size())
                    zip.add(this->m_sSongPath);

                zip.close();

                return true;
            } break;
            
            case kGmdTypeGmd: {
                return writeFileString(this->m_sFullPath, str.data);
            } break;

            case kGmdTypeLvl: {
                std::string data = str.data.substr(3, str.data.length() - 7);

                return writeFileBinary(
                    this->m_sFullPath,
                    gdcrypto::zlib::deflateBuffer(
                        convert_vs(data)
                    )
                );
            } break;
        }
    } else {
        return str;
    }

    return Result<>::res();
}
