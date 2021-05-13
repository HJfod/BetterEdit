#include <Windows.h>
#include <iostream>
#include <string>
#include <WinUser.h>
#include <WtsApi32.h>
#include <tlhelp32.h>
#include <Shlwapi.h>
#include <fstream>
#include <direct.h>
#include <process.h>
#include <filesystem>

const char* AppName = "ModLdr Installer";
const char* DataFile = ".gmdpath";
DWORD GD_PID = 0;

namespace methods {
    std::string replace(std::string const& original, std::string const& from, std::string const& to) {
        std::string results;
        std::string::const_iterator end = original.end();
        std::string::const_iterator current = original.begin();
        std::string::const_iterator next = std::search( current, end, from.begin(), from.end() );
        while ( next != end ) {
            results.append( current, next );
            results.append( to );
            current = next + from.size();
            next = std::search( current, end, from.begin(), from.end() );
        }
        results.append( current, next );
        return results;
    }

    void fsave (std::string _path, std::string _cont) {
        std::ofstream file;
        file.open(_path);
        file << _cont;
        file.close();
    }

    bool fcopy(std::string from, std::string to) {
        if (!std::filesystem::exists(from))
            return false;
        std::ifstream src(from, std::ios::binary);
        std::ofstream dst(to,   std::ios::binary);
        dst << src.rdbuf();

        return true;
    }

    bool proc_running(const char* _proc, DWORD* _pid = NULL) {
        WTS_PROCESS_INFO* pWPIs = NULL;
        DWORD dwProcCount = 0;
        bool found = false;
        if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
            for (DWORD i = 0; i < dwProcCount; i++)
                if (strcmp((LPSTR)pWPIs[i].pProcessName, _proc) == 0) {
                    found = true;
                    if (_pid != NULL)
                        *_pid = pWPIs[i].ProcessId;
                }

        if (pWPIs) {
            WTSFreeMemory(pWPIs);
            pWPIs = NULL;
        }

        return found;
    }

    std::string fread (std::string _path) {
        std::ifstream in(_path, std::ios::in | std::ios::binary);
        if (in) {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize((size_t)in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return(contents);
        } throw(errno);
    }

    std::string workdir() {
        char buff[FILENAME_MAX];
        _getcwd(buff, FILENAME_MAX);
        std::string current_working_dir(buff);
        return current_working_dir;
    }
}

// #define NOMSGBOX

#define INJECT_SUCCESS 0x3F
#define INJECT_TARGET_OPEN_FAIL 0x30
#define INJECT_TARGET_MALLOC_FAIL 0x31
#define INJECT_TARGET_CANT_WRITE 0x32
#define INJECT_TARGET_CANT_CREATE_THREAD 0x33
#define INJECT_ERROR_UNKNOWN 0x34

bool findGDPath(std::string* _res) {
    char value[255];
    DWORD BufferSize = 8192;

    LSTATUS key = RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Valve\\Steam", "InstallPath", RRF_RT_ANY, NULL, (PVOID)&value, &BufferSize);

    if (key != ERROR_SUCCESS) {
        return false;
    } else {
        std::string steamPath = value;
        std::string testPath = steamPath + std::string("\\steamapps\\common\\Geometry Dash\\GeometryDash.exe");

        if (std::filesystem::exists(testPath))
            if (std::filesystem::is_regular_file(testPath)) {
                *_res = testPath;
                return true;
            }

        // if we're here, then that means steamPath did not contain GD

        std::string line;
        std::string configPath = steamPath + "\\config\\config.vdf";

        std::ifstream f (configPath);

        while (std::getline(f, line)) {
            if (line.find("BaseInstallFolder_") != std::string::npos) {
                std::string val = line.substr(0, line.find_last_of("\""));
                val = val.substr(val.find_last_of("\"") + 1);

                testPath = val + "\\steamapps\\common\\Geometry Dash\\GeometryDash.exe";

                testPath = methods::replace(testPath, "\\\\", "\\");

                if (std::filesystem::exists(testPath))
                    if (std::filesystem::is_regular_file(testPath)) {
                        *_res = testPath;
                        f.close();
                        return true;
                    }
            }
        }

        return false;
    }

    return false;
}

enum err {
    PROG_SUCCESS = 0,
    FILE_NOT_FOUND,
    GD_PATH_NOT_VALID,
    INJECT_ERROR,
    CANT_OPEN_GD
};

// these are the files the runner moves
// the first one in this array is the one
// that gets injected

// by default files are moved to the 
// Geometry Dash folder

// add a | and then a folder name to move the
// file in a different folder relative to
// the GD folder e.g. resources

const char* req_files[1] = {
    "BetterEdit.dll"
};

int throwErr(std::string _msg, int _err) {
    std::cout << " Failed" << std::endl;

    #ifndef NOMSGBOX
        MessageBoxA(NULL, _msg.c_str(), AppName, MB_OK);
    #endif

    exit(_err);
}

int InjectDLL(const int &pid, const std::string &DLL_Path) {
    // adapted from https://github.com/saeedirha/DLL-Injector
    
    long dll_size = DLL_Path.length() + 1;
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (hProc == NULL)
        return INJECT_TARGET_OPEN_FAIL;

    LPVOID MyAlloc = VirtualAllocEx(hProc, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (MyAlloc == NULL)
        return INJECT_TARGET_MALLOC_FAIL;

    int IsWriteOK = WriteProcessMemory(hProc , MyAlloc, DLL_Path.c_str() , dll_size, 0);
    if (IsWriteOK == 0)
        return INJECT_TARGET_CANT_WRITE;

    DWORD dWord;
    LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("kernel32"), "LoadLibraryA");
    HANDLE ThreadReturn = CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, MyAlloc, 0, &dWord);
    if (ThreadReturn == NULL)
        return INJECT_TARGET_CANT_CREATE_THREAD;

    if ((hProc != NULL) && (MyAlloc != NULL) && (IsWriteOK != ERROR_INVALID_HANDLE) && (ThreadReturn != NULL))
        return INJECT_SUCCESS;

    return INJECT_ERROR_UNKNOWN;
}

int main(int argc, char* argv[]) {
    
    std::cout << "MsgBox: " <<
    #ifdef NOMSGBOX
        0
    #else
        1
    #endif
    << std::endl;
    std::cout << "PID: " << _getpid() << std::endl;

    std::string GDDataPath = "";

    if (std::filesystem::exists(DataFile))
        GDDataPath = methods::fread(DataFile);
    else {
        std::cout << "Detecting GD path...";

        if (findGDPath(&GDDataPath)) {
            std::cout << " Success" << std::endl;

            std::cout << "Detected path: " << GDDataPath << std::endl;
        } else {
            std::cout << " Failed" << std::endl;
            
            std::cout << "Type the path to GeometryDash.exe" << std::endl;
            getline(std::cin, GDDataPath);
        }
    }

    ////////////////////////////

    std::cout << "Checking data path...";

    bool gddpcs = false;
    if (GDDataPath.ends_with("GeometryDash.exe"))
        if (std::filesystem::exists(GDDataPath))
            gddpcs = true;
    if (!gddpcs)
        throwErr("GeometryDash.exe path does not appear to be correct.", err::GD_PATH_NOT_VALID);

    GDDataPath = methods::replace(GDDataPath, "/", "\\");
    methods::fsave(DataFile, GDDataPath);

    std::cout << " Success" << std::endl;

    ////////////////////////////

    std::cout << "Checking required files...";

    for (std::string f : req_files) {
        if (!std::filesystem::exists(f.find("|") != std::string::npos ? f.substr(0, f.find_first_of("|")) : f))
            throwErr("Unable to load: " + f + " not found! (Did you forget to unzip?)", err::FILE_NOT_FOUND);
    }
    
    std::cout << " Success" << std::endl;
    
    ////////////////////////////

    std::cout << "Checking GD status...";

    if (!methods::proc_running("GeometryDash.exe", &GD_PID))
        throwErr("GD isn't running!", err::FILE_NOT_FOUND);

    std::cout << " Success (GD PID: " << GD_PID << ")" << std::endl;
    
    ////////////////////////////

    std::cout << "Moving files...";

    for (int i = 1; i < (sizeof(req_files)/sizeof(*req_files)); i++) {
        std::string act = req_files[i];
        if (std::string(req_files[i]).find("|") != std::string::npos)
            act = act.substr(act.find_first_of("|") + 1) + "\\" + act.substr(0, act.find_first_of("|"));
        
        std::string nMHpath = GDDataPath.substr(0, GDDataPath.find_last_of("\\") + 1) + act;
        if (!std::filesystem::exists(nMHpath))
            if (!methods::fcopy(act == req_files[i] ? act : act.substr(act.find_first_of("\\") + 1), nMHpath))
                throwErr("There was an error copying files (METH_COPY_FROM_DOESNT_EXIST, probably)", err::FILE_NOT_FOUND);
    }

    std::cout << " Success" << std::endl;
    
    ////////////////////////////

    std::cout << "Injecting DLL...";

    int dll_suc = InjectDLL(GD_PID, methods::workdir() + "\\" + req_files[0]);
    if (dll_suc != INJECT_SUCCESS)
        throwErr("Unable to inject DLL! (Error code: " + std::to_string(dll_suc) + ")", err::INJECT_ERROR);
    
    std::cout << " Success" << std::endl;
    
    ////////////////////////////
    
    #ifndef NOMSGBOX
        MessageBoxA(NULL, "Succesfully loaded! :)", AppName, MB_OK);
    #endif

    return PROG_SUCCESS;
}

