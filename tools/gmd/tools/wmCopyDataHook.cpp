#include "../../../BetterEdit.hpp"
#include "passGmdFile.hpp"
#include "../layers/goToHook.hpp"
#include <unzipper.h>

using namespace zipper;

namespace gmd {
    bool isLevelFileName(std::string const& fname);
}

GDMAKE_HOOK("libcocos2d.dll" + 0x113090)
LRESULT __stdcall CCEGLView_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COPYDATA) {
        auto data = as<PCOPYDATASTRUCT>(lParam);

        BetterEdit::log() << "Received Data From Another Instance" << log_end();

        if (data->dwData == MSG_GD_IMPORT_LEVEL) {
            auto str = as<const char*>(data->lpData);

            auto lvls = stringSplit(str, GD_IMPORT_SEPARATOR);

            BetterEdit::log()
                << "Received " << lvls.size()
                << " Levels, Adding to Import List" << log_end();
            
            addLevelsToImportLayerFromVector(lvls);

            SetForegroundWindow(getGDHWND());
            BringWindowToTop(getGDHWND());
            SetFocus(getGDHWND());
            SetActiveWindow(getGDHWND());
        }

        return TRUE;
    }

    if (msg == WM_DROPFILES) {
        std::vector<std::string> files;
        auto hDropInfo = as<HDROP>(wParam);
        char sItem[MAX_PATH];
        for(int i = 0; DragQueryFileA(hDropInfo, i, as<LPSTR>(sItem), sizeof(sItem)); i++) {
            if (GetFileAttributesA(sItem) & FILE_ATTRIBUTE_DIRECTORY) {
                for (auto const& file : std::filesystem::directory_iterator(sItem)) {
                    if (gmd::isLevelFileName(file.path().string())) {
                        files.push_back(file.path().string());
                    }
                }
            } else {
                if (gmd::isLevelFileName(sItem)) {
                    files.push_back(sItem);
                } else if (std::string(sItem).ends_with(".zip")) {
                    Unzipper zip (sItem);
                    for (auto const& entry : zip.entries()) {
                        if (gmd::isLevelFileName(entry.name)) {
                            auto targetDir = std::filesystem::current_path() / "be_temp_unzip";
                            zip.extractEntry(entry.name, targetDir.string());
                            files.push_back((targetDir / entry.name).string());
                        }
                    }
                    zip.close();
                }
            }

        }
        DragFinish(hDropInfo);

        addLevelsToImportLayerFromVector(files);

        return TRUE;
    }

    return GDMAKE_ORIG(hwnd, msg, wParam, lParam);
}
