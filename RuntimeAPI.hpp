#ifndef RUNTIMEAPI_HPP
#define RUNTIMEAPI_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>

namespace Paper5 {
namespace Runtime {

// ===== Math Constants =====
const double PI = 3.141592653589793;
const double E = 2.718281828459045;
const double DEG2RAD = PI / 180.0;
const double RAD2DEG = 180.0 / PI;

// ===== Math Functions =====
inline int api_abs(int x) { return std::abs(x); }
inline double api_fabs(double x) { return std::fabs(x); }
inline int api_min(int a, int b) { return a < b ? a : b; }
inline double api_fmin(double a, double b) { return a < b ? a : b; }
inline int api_max(int a, int b) { return a > b ? a : b; }
inline double api_fmax(double a, double b) { return a > b ? a : b; }
inline int api_clamp(int x, int lo, int hi) { return x < lo ? lo : x > hi ? hi : x; }
inline double api_fclamp(double x, double lo, double hi) { return x < lo ? lo : x > hi ? hi : x; }
inline double api_pow(double base, double exp) { return std::pow(base, exp); }
inline double api_sqrt(double x) { return std::sqrt(x); }
inline double api_cbrt(double x) { return std::cbrt(x); }
inline int api_floor(double x) { return (int)std::floor(x); }
inline int api_ceil(double x) { return (int)std::ceil(x); }
inline int api_round(double x) { return (int)std::round(x); }
inline int api_trunc(double x) { return (int)std::trunc(x); }
inline double api_sin(double deg) { return std::sin(deg * DEG2RAD); }
inline double api_cos(double deg) { return std::cos(deg * DEG2RAD); }
inline double api_tan(double deg) { return std::tan(deg * DEG2RAD); }
inline double api_asin(double x) { return std::asin(x) * RAD2DEG; }
inline double api_acos(double x) { return std::acos(x) * RAD2DEG; }
inline double api_atan(double x) { return std::atan(x) * RAD2DEG; }
inline double api_atan2(double y, double x) { return std::atan2(y, x) * RAD2DEG; }
inline double api_sinh(double x) { return std::sinh(x); }
inline double api_cosh(double x) { return std::cosh(x); }
inline double api_tanh(double x) { return std::tanh(x); }
inline double api_hypot(double x, double y) { return std::hypot(x, y); }
inline double api_fmod(double x, double y) { return std::fmod(x, y); }
inline double api_log(double x) { return std::log(x); }
inline double api_log10(double x) { return std::log10(x); }
inline double api_exp(double x) { return std::exp(x); }
inline int api_random(int min, int max) { return min + (rand() % (max - min + 1)); }
inline double api_randomFloat(double min, double max) { return min + ((double)rand() / RAND_MAX) * (max - min); }

// ===== String Functions =====
inline std::string api_intToStr(int v) { return std::to_string(v); }
inline std::string api_floatToStr(double v) { return std::to_string(v); }
inline int api_strToInt(const std::string& s) { try { return std::stoi(s); } catch(...) { return 0; } }
inline double api_strToFloat(const std::string& s) { try { return std::stod(s); } catch(...) { return 0.0; } }
inline int api_strLength(const std::string& s) { return (int)s.length(); }
inline std::string api_strUpper(std::string s) { for (auto& c : s) c = toupper(c); return s; }
inline std::string api_strLower(std::string s) { for (auto& c : s) c = tolower(c); return s; }
inline std::string api_strSubstr(const std::string& s, int pos, int len) {
    if (pos < 0 || pos >= (int)s.length()) return "";
    return s.substr(pos, len);
}
inline int api_strFind(const std::string& s, const std::string& sub) {
    size_t p = s.find(sub);
    return p == std::string::npos ? -1 : (int)p;
}
inline std::string api_strReplace(std::string s, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.length(), to);
        pos += to.length();
    }
    return s;
}
inline std::vector<std::string> api_strSplit(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) result.push_back(item);
    return result;
}
inline std::string api_strJoin(const std::vector<std::string>& parts, const std::string& delim) {
    std::string r;
    for (size_t i = 0; i < parts.size(); i++) { if (i > 0) r += delim; r += parts[i]; }
    return r;
}
inline std::string api_strTrim(const std::string& s) {
    size_t f = s.find_first_not_of(" \t\r\n");
    if (f == std::string::npos) return "";
    size_t l = s.find_last_not_of(" \t\r\n");
    return s.substr(f, l - f + 1);
}

// ===== Console =====
inline void api_print(const std::string& s) {
    static bool alloc = false;
    if (!alloc) { AllocConsole(); freopen("CONOUT$", "w", stdout); alloc = true; }
    std::cout << s << std::endl;
}

// ===== Dialogs (ANSI) =====
inline void api_alert(const std::string& msg) { MessageBoxA(NULL, msg.c_str(), "Paper5", MB_OK | MB_ICONINFORMATION); }
inline bool api_confirm(const std::string& msg) { return MessageBoxA(NULL, msg.c_str(), "Paper5", MB_YESNO | MB_ICONQUESTION) == IDYES; }

// ===== File Dialogs (ANSI) =====
inline std::string api_openFileDialog(const std::string& title, const std::string& filter) {
    OPENFILENAMEA ofn = {0};
    char buf[MAX_PATH * 2] = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFilter = filter.empty() ? "Paper5 Files\0*.paper\0All Files\0*.*\0" : filter.c_str();
    ofn.lpstrFile = buf;
    ofn.nMaxFile = sizeof(buf);
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    if (GetOpenFileNameA(&ofn)) return buf;
    return "";
}

inline std::string api_saveFileDialog(const std::string& title, const std::string& filter) {
    OPENFILENAMEA ofn = {0};
    char buf[MAX_PATH * 2] = {0};
    strncpy(buf, "Untitled.paper", sizeof(buf)-1);
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFilter = filter.empty() ? "Paper5 Files\0*.paper\0All Files\0*.*\0" : filter.c_str();
    ofn.lpstrFile = buf;
    ofn.nMaxFile = sizeof(buf);
    ofn.lpstrTitle = title.c_str();
    ofn.lpstrDefExt = "paper";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    if (GetSaveFileNameA(&ofn)) return buf;
    return "";
}

// ===== File Operations =====
inline std::string api_readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) return "";
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
inline bool api_writeFile(const std::string& path, const std::string& content) {
    std::ofstream f(path, std::ios::binary);
    if (!f.is_open()) return false;
    f << content;
    return true;
}
inline bool api_fileExists(const std::string& path) {
    DWORD attr = GetFileAttributesA(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}
inline bool api_deleteFile(const std::string& path) { return DeleteFileA(path.c_str()) != 0; }

// ===== Clipboard =====
inline std::string api_getClipboard() {
    if (!OpenClipboard(NULL)) return "";
    HANDLE h = GetClipboardData(CF_TEXT);
    if (!h) { CloseClipboard(); return ""; }
    char* data = (char*)GlobalLock(h);
    std::string r(data ? data : "");
    GlobalUnlock(h); CloseClipboard();
    return r;
}
inline bool api_setClipboard(const std::string& text) {
    if (!OpenClipboard(NULL)) return false;
    EmptyClipboard();
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, text.length() + 1);
    if (!h) { CloseClipboard(); return false; }
    memcpy(GlobalLock(h), text.c_str(), text.length() + 1);
    GlobalUnlock(h);
    SetClipboardData(CF_TEXT, h);
    CloseClipboard();
    return true;
}

// ===== Program Control =====
inline void api_sleep(int ms) { Sleep(ms); }
inline void api_exit(int code) { exit(code); }
inline int api_run(const std::string& path) {
    return (int)ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL) > 32 ? 0 : -1;
}

// ===== System =====
inline std::string api_getEnv(const std::string& name) { char* v = getenv(name.c_str()); return v ? v : ""; }
inline bool api_setEnv(const std::string& name, const std::string& value) { return SetEnvironmentVariableA(name.c_str(), value.c_str()) != 0; }
inline DWORD api_getTick() { return GetTickCount(); }
inline std::string api_getDate() { SYSTEMTIME st; GetLocalTime(&st); char b[32]; sprintf(b, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay); return b; }
inline std::string api_getTime() { SYSTEMTIME st; GetLocalTime(&st); char b[32]; sprintf(b, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond); return b; }

// ===== Audio =====
inline bool api_playSound(const std::string& path) { return PlaySoundA(path.c_str(), NULL, SND_FILENAME | SND_ASYNC) != 0; }
inline bool api_beep(int freq, int dur) { return Beep(freq, dur) != 0; }

// ===== Window/Control Helpers =====
inline void api_setWindowTitle(HWND hWnd, const std::string& title) { SetWindowTextA(hWnd, title.c_str()); }
inline void api_setControlText(HWND hWnd, const std::string& text) { SetWindowTextA(hWnd, text.c_str()); }
inline std::string api_getControlText(HWND hWnd) { char b[65536]; GetWindowTextA(hWnd, b, sizeof(b)); return b; }
inline void api_resizeControl(HWND hWnd, int x, int y, int w, int h) { SetWindowPos(hWnd, NULL, x, y, w, h, SWP_NOZORDER); }

// ===== Text Editor Operations =====
inline void api_editorUndo(HWND hEdit) { SendMessageA(hEdit, EM_UNDO, 0, 0); }
inline void api_editorRedo(HWND hEdit) { SendMessageA(hEdit, EM_REDO, 0, 0); }
inline void api_editorCut(HWND hEdit) { SendMessageA(hEdit, WM_CUT, 0, 0); }
inline void api_editorCopy(HWND hEdit) { SendMessageA(hEdit, WM_COPY, 0, 0); }
inline void api_editorPaste(HWND hEdit) { SendMessageA(hEdit, WM_PASTE, 0, 0); }
inline void api_editorSelectAll(HWND hEdit) { SendMessageA(hEdit, EM_SETSEL, 0, -1); }
inline std::string api_editorGetSelectedText(HWND hEdit) {
    DWORD start, end;
    SendMessageA(hEdit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
    if (start == end) return "";
    std::string text = api_getControlText(hEdit);
    return text.substr(start, end - start);
}
inline void api_editorInsertText(HWND hEdit, const std::string& text) {
    SendMessageA(hEdit, EM_REPLACESEL, TRUE, (LPARAM)text.c_str());
}

} // namespace Runtime
} // namespace Paper5

#endif // RUNTIMEAPI_HPP