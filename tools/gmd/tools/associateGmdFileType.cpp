#include "associateGmdFileType.hpp"
#include "../logic/gmd.hpp"
#include "../layers/goToHook.hpp"
#include "../layers/ImportListView.hpp"
#include "passGmdFile.hpp"

// https://www.cplusplus.com/forum/windows/26987/
bool associateFileType(std::string const& program, std::string const& desc, std::string const& ext) {
    HKEY hkey;

    auto app = "C:\\"_s + program + " %1";
    auto path = ext + "\\shell\\OpenWithGeometryDash\\command\\";

    if (
        RegCreateKeyExA(
            HKEY_CLASSES_ROOT,
            ext.c_str(),
            0, nullptr, 0,
            KEY_ALL_ACCESS,
            nullptr, &hkey, nullptr
        ) != ERROR_SUCCESS
    ) {
        std::cout << "fucked on first RegCreateKeyExA\n";
        return false;
    }
    RegSetValueExA(
        hkey, "", 0,
        REG_SZ,
        as<const BYTE*>(desc.c_str()),
        desc.size()
    );
    RegCloseKey(hkey);

    auto stat = RegCreateKeyExA(
            HKEY_CLASSES_ROOT,
            path.c_str(),
            0, nullptr, 0,
            KEY_ALL_ACCESS,
            nullptr, &hkey, nullptr
        );
    if (
        stat != ERROR_SUCCESS
    ) {
        std::cout << "fucked on second RegCreateKeyExA: " << stat << "\n";
        std::cout << path << "\n";
        return false;
    }
    RegSetValueExA(
        hkey, "", 0,
        REG_SZ,
        as<const BYTE*>(app.c_str()),
        app.size()
    );

    RegCloseKey(hkey);

    return true;
}

std::vector<std::string> tokenize(std::string const& str) {
    std::vector<std::string> res = { "" };

    bool insideQuotes = false;
    for (auto c : str) {
        switch (c) {
            case '"':
                insideQuotes = !insideQuotes;
                break;

            case ' ':
                if (!insideQuotes) {
                    res.push_back("");
                    break;
                }
                
            default:
                res[res.size() - 1] += c;
        }
    }

    return res;
}

void handleCli(std::string const& args) {
    auto anotherInstance = anotherInstanceIsOpen();
    if (anotherInstance) {
        BetterEdit::log()
            << kDebugTypeGeneric
            << "Found Another Active Instance, Redirecting Input to It"
            << log_end();
    }

    auto importList = CCArray::create(); 

    auto ix = 0;
    BetterEdit::log() << kDebugTypeLoading << "Importing levels" << log_end();
    for (auto token : tokenize(args)) {
        if (gmd::isLevelFileName(token)) {
            if (anotherInstance) {
                importList->addObject(CCString::create(token));
            } else {
                auto res = gmd::GmdFile(token).parseLevel();
                if (res) {
                    BetterEdit::log() << kDebugTypeLoading << "Added " << token << " to import list" << log_end();
                    importList->addObject(new ImportObject(res.data, token));
                } else {
                    BetterEdit::log() << kDebugTypeMinorError
                        << "Unable to Import " << token << ": "
                        << res.error << log_end();
                }
            }
        }
        ix++;
    }

    if (importList->count()) {
        if (anotherInstance) {
            COPYDATASTRUCT data;
            data.dwData = MSG_GD_IMPORT_LEVEL;
            data.cbData = sizeof(importList);
            data.lpData = importList;
            SendMessage(
                anotherInstance,
                WM_COPYDATA,
                as<WPARAM>(nullptr),
                as<LPARAM>(&data)
            );
        } else {
            goToImportLayer(importList);
        }
    }

    if (anotherInstance) {
        // force quit
        // exit(0);
    }
}

bool associateGmdFileTypes() {
    char gdName[MAX_PATH];
    GetModuleFileNameA(
        nullptr, gdName, MAX_PATH
    );

    SetConsoleTitleA("GD Debug Console");

    BetterEdit::log() << kDebugTypeLoading << "Handling Command Line Parameters" << log_end();

    handleCli(GetCommandLineA());

    if (!associateFileType(
        gdName, "Geometry Dash Level", ".gmd"
    )) return false;

    if (!associateFileType(
        gdName, "Geometry Dash Level (Zipped)", ".gmd2"
    )) return false;

    if (!associateFileType(
        gdName, "Geometry Dash Level (Compressed)", ".lvl"
    )) return false;

    return true;
}
