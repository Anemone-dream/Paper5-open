#ifndef CODEGENERATOR_HPP
#define CODEGENERATOR_HPP

#include "AST.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <functional>
#include <windows.h>
#include <commctrl.h>

namespace Paper5 {

class CodeGenerator {
private:
    std::string output;
    int indentLevel;
    int controlIdCounter;
    std::map<std::string, int> controlIds;
    std::string currentPaperName;
    
    void indent() { for (int i = 0; i < indentLevel; i++) output += "    "; }
    void addLine(const std::string& line) { indent(); output += line + "\n"; }
    void addBlankLine() { output += "\n"; }
    
    int getControlId(const std::string& name) {
        auto it = controlIds.find(name);
        if (it != controlIds.end()) return it->second;
        int id = ++controlIdCounter;
        controlIds[name] = id;
        return id;
    }
    
    std::string escapeString(const std::string& str) {
        std::string r;
        for (char c : str) {
            if (c == '"') r += "\\\"";
            else if (c == '\\') r += "\\\\";
            else if (c == '\n') r += "\\n";
            else if (c == '\r') r += "\\r";
            else if (c == '\t') r += "\\t";
            else r += c;
        }
        return r;
    }
    
    std::string getWinClass(TokenType type) {
        switch(type) {
            case TokenType::KEYWORD_BUTTON: return "BUTTON";
            case TokenType::KEYWORD_ENTRY: return "EDIT";
            case TokenType::KEYWORD_TEXTAREA: return "EDIT";
            case TokenType::KEYWORD_TEXT: return "STATIC";
            case TokenType::KEYWORD_CHECKBOX: return "BUTTON";
            case TokenType::KEYWORD_RADIO: return "BUTTON";
            case TokenType::KEYWORD_LISTBOX: return "LISTBOX";
            case TokenType::KEYWORD_COMBOBOX: return "COMBOBOX";
            case TokenType::KEYWORD_PANEL: return "STATIC";
            case TokenType::KEYWORD_GROUP: return "BUTTON";
            case TokenType::KEYWORD_PICTURE: return "STATIC";
            case TokenType::KEYWORD_CANVAS: return "STATIC";
            case TokenType::KEYWORD_PROGRESS: return "msctls_progress32";
            case TokenType::KEYWORD_SLIDER: return "msctls_trackbar32";
            case TokenType::KEYWORD_SPIN: return "msctls_updown32";
            case TokenType::KEYWORD_STATUS: return "msctls_statusbar32";
            case TokenType::KEYWORD_TAB: return "SysTabControl32";
            default: return "STATIC";
        }
    }
    
    DWORD getWinStyle(TokenType type) {
        DWORD style = WS_CHILD | WS_VISIBLE;
        switch(type) {
            case TokenType::KEYWORD_BUTTON: return style | BS_PUSHBUTTON;
            case TokenType::KEYWORD_ENTRY: return style | ES_LEFT | ES_AUTOHSCROLL;
            case TokenType::KEYWORD_TEXTAREA: return style | ES_LEFT | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL;
            case TokenType::KEYWORD_TEXT: return style | SS_LEFT;
            case TokenType::KEYWORD_CHECKBOX: return style | BS_AUTOCHECKBOX;
            case TokenType::KEYWORD_RADIO: return style | BS_AUTORADIOBUTTON;
            case TokenType::KEYWORD_LISTBOX: return style | LBS_STANDARD | LBS_NOTIFY | WS_VSCROLL;
            case TokenType::KEYWORD_COMBOBOX: return style | CBS_DROPDOWNLIST | WS_VSCROLL;
            case TokenType::KEYWORD_PANEL: return style | SS_SUNKEN;
            case TokenType::KEYWORD_GROUP: return style | BS_GROUPBOX;
            case TokenType::KEYWORD_PICTURE: return style | SS_BITMAP;
            case TokenType::KEYWORD_CANVAS: return style | SS_OWNERDRAW;
            case TokenType::KEYWORD_PROGRESS: return style | PBS_SMOOTH;
            case TokenType::KEYWORD_SLIDER: return style | TBS_HORZ | TBS_NOTICKS;
            case TokenType::KEYWORD_SPIN: return style | UDS_ALIGNRIGHT | UDS_AUTOBUDDY;
            case TokenType::KEYWORD_STATUS: return style | SBARS_SIZEGRIP;
            case TokenType::KEYWORD_TAB: return style | TCS_TABS;
            default: return style;
        }
    }
    
    DWORD getWinStyleEx(TokenType type) {
        switch(type) {
            case TokenType::KEYWORD_TEXTAREA: return WS_EX_CLIENTEDGE;
            case TokenType::KEYWORD_ENTRY: return WS_EX_CLIENTEDGE;
            case TokenType::KEYWORD_LISTBOX: return WS_EX_CLIENTEDGE;
            case TokenType::KEYWORD_COMBOBOX: return WS_EX_CLIENTEDGE;
            default: return 0;
        }
    }
    
    int getPropInt(const Component& comp, const std::string& name, int def) {
        for (auto& p : comp.properties) {
            if (p.name == name && p.value)
                if (auto* lit = dynamic_cast<LiteralExpr*>(p.value.get())) return lit->intValue;
        }
        return def;
    }
    
    int getPropInt(const PaperDef& paper, const std::string& name, int def) {
        for (auto& p : paper.properties) {
            if (p.name == name && p.value)
                if (auto* lit = dynamic_cast<LiteralExpr*>(p.value.get())) return lit->intValue;
        }
        return def;
    }
    
    std::string getPropStr(const Component& comp, const std::string& name, const std::string& def) {
        for (auto& p : comp.properties) {
            if (p.name == name && p.value)
                if (auto* lit = dynamic_cast<LiteralExpr*>(p.value.get())) return lit->stringValue;
        }
        return def;
    }
    
    std::string getPropStr(const PaperDef& paper, const std::string& name, const std::string& def) {
        for (auto& p : paper.properties) {
            if (p.name == name && p.value)
                if (auto* lit = dynamic_cast<LiteralExpr*>(p.value.get())) return lit->stringValue;
        }
        return def;
    }
    
    void generateComponentCreation(const Component& comp, const std::string& parentHwnd) {
        if (comp.type == TokenType::KEYWORD_MENU || comp.type == TokenType::KEYWORD_MENUITEM ||
            comp.type == TokenType::KEYWORD_SUBMENU || comp.type == TokenType::KEYWORD_SEPARATOR ||
            comp.type == TokenType::KEYWORD_POPUPMENU) return;
            
        std::string name = comp.name;
        int x = getPropInt(comp, "x", 0);
        int y = getPropInt(comp, "y", 0);
        int w = getPropInt(comp, "width", 100);
        int h = getPropInt(comp, "height", 24);
        std::string text = getPropStr(comp, "text", "");
        std::string winClass = getWinClass(comp.type);
        DWORD style = getWinStyle(comp.type);
        DWORD styleEx = getWinStyleEx(comp.type);
        int id = getControlId(name);
        
        addLine("h" + name + " = CreateWindowEx(");
        addLine("    " + std::to_string(styleEx) + ", \"" + winClass + "\", \"" + escapeString(text) + "\",");
        addLine("    " + std::to_string(style) + ", " +
                std::to_string(x) + ", " + std::to_string(y) + ", " +
                std::to_string(w) + ", " + std::to_string(h) + ",");
        addLine("    " + parentHwnd + ", (HMENU)" + std::to_string(id) + ", hInst, NULL);");
        
        std::string font = getPropStr(comp, "font", "");
        if (!font.empty()) {
            addLine("{ HFONT hf = CreateFont(16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,\"Consolas\"); SendMessage(h" + name + ",WM_SETFONT,(WPARAM)hf,TRUE); }");
        }
        addBlankLine();
        for (auto& child : comp.children) generateComponentCreation(*child, "h" + name);
    }
    
    std::string findEditorName(const AST& ast) {
        if (!ast.paper) return "";
        std::function<std::string(const std::unique_ptr<Component>&)> find;
        find = [&](const std::unique_ptr<Component>& comp) -> std::string {
            if (comp->type == TokenType::KEYWORD_TEXTAREA) return comp->name;
            for (auto& c : comp->children) { std::string r = find(c); if (!r.empty()) return r; }
            return "";
        };
        for (auto& c : ast.paper->children) { std::string r = find(c); if (!r.empty()) return r; }
        return "";
    }
    
    void generateMenuAction(const std::string& menuName, const std::string& editorName, std::string& cases) {
        int id = getControlId(menuName);
        
        if (menuName == "menuNew") {
            cases += "            case " + std::to_string(id) + ": {\n";
            cases += "                currentFile = \"\";\n";
            cases += "                api_setControlText(h" + editorName + ", \"\");\n";
            cases += "                api_setWindowTitle(hMainWnd, \"Paper5 IDE - Untitled\");\n";
            cases += "                api_setControlText(hstatusBar, \"New file\");\n";
            cases += "                break;\n            }\n";
        }
        else if (menuName == "menuOpen") {
            cases += "            case " + std::to_string(id) + ": {\n";
            cases += "                std::string path = api_openFileDialog(\"Open File\", \"Paper5 Files\\0*.paper\\0All Files\\0*.*\\0\");\n";
            cases += "                if (!path.empty()) {\n";
            cases += "                    std::string content = api_readFile(path);\n";
            cases += "                    api_setControlText(h" + editorName + ", content);\n";
            cases += "                    currentFile = path;\n";
            cases += "                    api_setWindowTitle(hMainWnd, (\"Paper5 IDE - \" + path).c_str());\n";
            cases += "                    api_setControlText(hstatusBar, (\"Opened: \" + path).c_str());\n";
            cases += "                }\n";
            cases += "                break;\n            }\n";
        }
        else if (menuName == "menuSave") {
            cases += "            case " + std::to_string(id) + ": {\n";
            cases += "                if (currentFile.empty()) {\n";
            cases += "                    currentFile = api_saveFileDialog(\"Save File\", \"Paper5 Files\\0*.paper\\0All Files\\0*.*\\0\");\n";
            cases += "                }\n";
            cases += "                if (!currentFile.empty()) {\n";
            cases += "                    api_writeFile(currentFile, api_getControlText(h" + editorName + "));\n";
            cases += "                    api_setWindowTitle(hMainWnd, (\"Paper5 IDE - \" + currentFile).c_str());\n";
            cases += "                    api_setControlText(hstatusBar, (\"Saved: \" + currentFile).c_str());\n";
            cases += "                }\n";
            cases += "                break;\n            }\n";
        }
        else if (menuName == "menuSaveAs") {
            cases += "            case " + std::to_string(id) + ": {\n";
            cases += "                std::string path = api_saveFileDialog(\"Save As\", \"Paper5 Files\\0*.paper\\0All Files\\0*.*\\0\");\n";
            cases += "                if (!path.empty()) {\n";
            cases += "                    api_writeFile(path, api_getControlText(h" + editorName + "));\n";
            cases += "                    currentFile = path;\n";
            cases += "                    api_setWindowTitle(hMainWnd, (\"Paper5 IDE - \" + path).c_str());\n";
            cases += "                    api_setControlText(hstatusBar, (\"Saved: \" + path).c_str());\n";
            cases += "                }\n";
            cases += "                break;\n            }\n";
        }
        else if (menuName == "menuExit") {
            cases += "            case " + std::to_string(id) + ":\n";
            cases += "                PostQuitMessage(0);\n";
            cases += "                break;\n";
        }
        else if (menuName == "menuUndo") {
            cases += "            case " + std::to_string(id) + ":\n";
            cases += "                api_editorUndo(h" + editorName + ");\n";
            cases += "                break;\n";
        }
        else if (menuName == "menuRedo") {
            cases += "            case " + std::to_string(id) + ":\n";
            cases += "                api_editorRedo(h" + editorName + ");\n";
            cases += "                break;\n";
        }
        else if (menuName == "menuCut") {
            cases += "            case " + std::to_string(id) + ":\n";
            cases += "                api_editorCut(h" + editorName + ");\n";
            cases += "                break;\n";
        }
        else if (menuName == "menuCopy") {
            cases += "            case " + std::to_string(id) + ":\n";
            cases += "                api_editorCopy(h" + editorName + ");\n";
            cases += "                break;\n";
        }
        else if (menuName == "menuPaste") {
            cases += "            case " + std::to_string(id) + ":\n";
            cases += "                api_editorPaste(h" + editorName + ");\n";
            cases += "                break;\n";
        }
        else if (menuName == "menuSelectAll") {
            cases += "            case " + std::to_string(id) + ":\n";
            cases += "                api_editorSelectAll(h" + editorName + ");\n";
            cases += "                break;\n";
        }
        else if (menuName == "menuFind") {
            cases += "            case " + std::to_string(id) + ": {\n";
            cases += "                std::string findWhat = api_getControlText(h" + editorName + ");\n";
            cases += "                // Simple find: select all for now\n";
            cases += "                api_editorSelectAll(h" + editorName + ");\n";
            cases += "                api_setControlText(hstatusBar, \"Find: Use Ctrl+F to search\");\n";
            cases += "                break;\n            }\n";
        }
        else if (menuName == "menuCompile" || menuName == "menuCompileRun") {
            cases += "            case " + std::to_string(id) + ": {\n";
            cases += "                if (currentFile.empty()) {\n";
            cases += "                    currentFile = api_saveFileDialog(\"Save Before Compile\", \"Paper5 Files\\0*.paper\\0All Files\\0*.*\\0\");\n";
            cases += "                    if (!currentFile.empty()) api_writeFile(currentFile, api_getControlText(h" + editorName + "));\n";
            cases += "                } else {\n";
            cases += "                    api_writeFile(currentFile, api_getControlText(h" + editorName + "));\n";
            cases += "                }\n";
            cases += "                if (!currentFile.empty()) {\n";
            cases += "                    api_setControlText(hstatusBar, \"Compiling...\");\n";
            cases += "                    std::string cmd = \"paper5c \\\"\" + currentFile + \"\\\"\";\n";
            if (menuName == "menuCompileRun") cases += "                    cmd += \" --run\";\n";
            cases += "                    api_run(\"paper5c.exe\");\n";
            cases += "                    // Use system to run paper5c with arguments\n";
            cases += "                    std::string fullCmd = \"Paper5_1.1.exe \\\"\" + currentFile + \"\\\"\";\n";
            if (menuName == "menuCompileRun") cases += "                    fullCmd += \" --run\";\n";
            cases += "                    system(fullCmd.c_str());\n";
            cases += "                    api_setControlText(hstatusBar, \"Compilation complete\");\n";
            cases += "                }\n";
            cases += "                break;\n            }\n";
        }
        else if (menuName == "menuRunApp") {
            cases += "            case " + std::to_string(id) + ": {\n";
            cases += "                if (currentFile.empty()) {\n";
            cases += "                    currentFile = api_saveFileDialog(\"Save Before Run\", \"Paper5 Files\\0*.paper\\0All Files\\0*.*\\0\");\n";
            cases += "                    if (!currentFile.empty()) api_writeFile(currentFile, api_getControlText(h" + editorName + "));\n";
            cases += "                }\n";
            cases += "                if (!currentFile.empty()) {\n";
            cases += "                    // Replace .paper with .exe\n";
            cases += "                    std::string exe = currentFile;\n";
            cases += "                    size_t dot = exe.find_last_of('.');\n";
            cases += "                    if (dot != std::string::npos) exe = exe.substr(0, dot) + \".exe\";\n";
            cases += "                    api_run(exe);\n";
            cases += "                    api_setControlText(hstatusBar, (\"Running: \" + exe).c_str());\n";
            cases += "                }\n";
            cases += "                break;\n            }\n";
        }
        else if (menuName == "menuTokens") {
            cases += "            case " + std::to_string(id) + ": {\n";
            cases += "                if (!currentFile.empty()) {\n";
            cases += "                    api_writeFile(currentFile, api_getControlText(h" + editorName + "));\n";
            cases += "                    system((\"paper5c \\\"\" + currentFile + \"\\\" --dump-tokens\").c_str());\n";
            cases += "                }\n";
            cases += "                break;\n            }\n";
        }
        else if (menuName == "menuAST") {
            cases += "            case " + std::to_string(id) + ": {\n";
            cases += "                if (!currentFile.empty()) {\n";
            cases += "                    api_writeFile(currentFile, api_getControlText(h" + editorName + "));\n";
            cases += "                    system((\"paper5c \\\"\" + currentFile + \"\\\" --dump-ast\").c_str());\n";
            cases += "                }\n";
            cases += "                break;\n            }\n";
        }
        else if (menuName == "menuAbout") {
            cases += "            case " + std::to_string(id) + ":\n";
            cases += "                api_alert(\"Paper5 IDE v1.1\\n\\nBuilt with Paper5 Language\\nWin32 ANSI Application\");\n";
            cases += "                break;\n";
        }
        else {
            // Default for unhandled menu items
            std::string text = menuName;
            cases += "            case " + std::to_string(id) + ":\n";
            cases += "                api_alert(\"" + escapeString(text) + " clicked!\");\n";
            cases += "                break;\n";
        }
    }
    
    void collectMenuActions(const AST& ast, std::string& cases, const std::string& editorName) {
        if (!ast.paper) return;
        for (auto& child : ast.paper->children) {
            if (child->type == TokenType::KEYWORD_MENU) {
                for (auto& menuItem : child->children) {
                    if (menuItem->type == TokenType::KEYWORD_MENUITEM) {
                        for (auto& subItem : menuItem->children) {
                            if (subItem->type == TokenType::KEYWORD_MENUITEM) {
                                generateMenuAction(subItem->name, editorName, cases);
                            }
                        }
                    }
                }
            }
        }
    }
    
public:
    CodeGenerator() : indentLevel(0), controlIdCounter(1000) {}
    
    void generate(const AST& ast, const std::string& filename) {
        output.clear();
        indentLevel = 0;
        controlIdCounter = 1000;
        controlIds.clear();
        currentPaperName = ast.paper ? ast.paper->name : "MainWindow";
        std::string editorName = findEditorName(ast);
        
        // Headers
        addLine("// Generated by Paper5 Compiler v1.1");
        addLine("#define WIN32_LEAN_AND_MEAN");
        addLine("#include <windows.h>");
        addLine("#include <commctrl.h>");
        addLine("#include <commdlg.h>");
        addLine("#include <string>");
        addLine("#include <shellapi.h>");
        addLine("#include <cstdlib>");
        addLine("#include <ctime>");
        addLine("#include <cmath>");
        addLine("#include <cstdio>");
        addLine("#include <cstring>");
        addLine("#include <fstream>");
        addLine("#include <sstream>");
        addLine("#include <vector>");
        addBlankLine();
        addLine("#pragma comment(lib, \"comctl32.lib\")");
        addLine("#pragma comment(lib, \"comdlg32.lib\")");
        addBlankLine();
        
        addLine("#ifndef EM_REDO");
		addLine("#define EM_REDO (WM_USER + 84)");
		addLine("#endif");
		addBlankLine();
        
        // Inline RuntimeAPI
        addLine("// Runtime API inline implementations");
        addLine("namespace Runtime {");
        addLine("const double PI = 3.141592653589793;");
        addLine("const double DEG2RAD = PI / 180.0;");
        addLine("const double RAD2DEG = 180.0 / PI;");
        addLine("inline int api_abs(int x) { return x < 0 ? -x : x; }");
        addLine("inline int api_min(int a, int b) { return a < b ? a : b; }");
        addLine("inline int api_max(int a, int b) { return a > b ? a : b; }");
        addLine("inline int api_clamp(int x, int lo, int hi) { return x < lo ? lo : x > hi ? hi : x; }");
        addLine("inline double api_sqrt(double x) { return sqrt(x); }");
        addLine("inline double api_pow(double b, double e) { return pow(b, e); }");
        addLine("inline int api_floor(double x) { return (int)floor(x); }");
        addLine("inline int api_ceil(double x) { return (int)ceil(x); }");
        addLine("inline int api_round(double x) { return (int)(x + 0.5); }");
        addLine("inline double api_sin(double d) { return sin(d * DEG2RAD); }");
        addLine("inline double api_cos(double d) { return cos(d * DEG2RAD); }");
        addLine("inline int api_random(int min, int max) { return min + rand() % (max - min + 1); }");
        addLine("inline void api_sleep(int ms) { Sleep(ms); }");
        addLine("inline void api_exit(int c) { exit(c); }");
        addLine("inline void api_alert(const std::string& m) { MessageBoxA(NULL, m.c_str(), \"Paper5\", MB_OK); }");
        addLine("inline bool api_confirm(const std::string& m) { return MessageBoxA(NULL, m.c_str(), \"Paper5\", MB_YESNO) == IDYES; }");
        addLine("inline std::string api_readFile(const std::string& p) { std::ifstream f(p, std::ios::binary); if(!f.is_open()) return \"\"; std::stringstream ss; ss << f.rdbuf(); return ss.str(); }");
        addLine("inline bool api_writeFile(const std::string& p, const std::string& c) { std::ofstream f(p, std::ios::binary); if(!f.is_open()) return false; f << c; return true; }");
        addLine("inline bool api_fileExists(const std::string& p) { return GetFileAttributesA(p.c_str()) != INVALID_FILE_ATTRIBUTES; }");
        addLine("inline std::string api_getEnv(const std::string& n) { char* v = getenv(n.c_str()); return v ? v : \"\"; }");
        addLine("inline DWORD api_getTick() { return GetTickCount(); }");
        addLine("inline int api_run(const std::string& p) { return (int)(INT_PTR)ShellExecuteA(NULL,\"open\",p.c_str(),NULL,NULL,SW_SHOWNORMAL) > 32 ? 0 : -1; }");
        addLine("inline void api_setWindowTitle(HWND h, const std::string& t) { SetWindowTextA(h, t.c_str()); }");
        addLine("inline void api_setControlText(HWND h, const std::string& t) { SetWindowTextA(h, t.c_str()); }");
        addLine("inline std::string api_getControlText(HWND h) { char b[65536]; GetWindowTextA(h,b,sizeof(b)); return b; }");
        addLine("inline std::string api_openFileDialog(const std::string& t, const std::string& f) { OPENFILENAMEA o={0}; char b[MAX_PATH*2]={0}; o.lStructSize=sizeof(o); o.hwndOwner=GetActiveWindow(); o.lpstrFilter=f.c_str(); o.lpstrFile=b; o.nMaxFile=sizeof(b); o.lpstrTitle=t.c_str(); o.Flags=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY; if(GetOpenFileNameA(&o)) return b; return \"\"; }");
        addLine("inline std::string api_saveFileDialog(const std::string& t, const std::string& f) { OPENFILENAMEA o={0}; char b[MAX_PATH*2]={0}; strncpy(b,\"Untitled.paper\",sizeof(b)-1); o.lStructSize=sizeof(o); o.hwndOwner=GetActiveWindow(); o.lpstrFilter=f.c_str(); o.lpstrFile=b; o.nMaxFile=sizeof(b); o.lpstrTitle=t.c_str(); o.lpstrDefExt=\"paper\"; o.Flags=OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY; if(GetSaveFileNameA(&o)) return b; return \"\"; }");
        addLine("inline void api_editorUndo(HWND h) { SendMessageA(h, EM_UNDO, 0, 0); }");
        addLine("inline void api_editorRedo(HWND h) { SendMessageA(h, EM_REDO, 0, 0); }");
        addLine("inline void api_editorCut(HWND h) { SendMessageA(h, WM_CUT, 0, 0); }");
        addLine("inline void api_editorCopy(HWND h) { SendMessageA(h, WM_COPY, 0, 0); }");
        addLine("inline void api_editorPaste(HWND h) { SendMessageA(h, WM_PASTE, 0, 0); }");
        addLine("inline void api_editorSelectAll(HWND h) { SendMessageA(h, EM_SETSEL, 0, -1); }");
        addLine("}");
        addLine("using namespace Runtime;");
        addBlankLine();
        
        // Forward declarations
        addLine("LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);");
        addLine("void CreateMenus(HWND hWnd);");
        addLine("void CreateControls(HWND hWnd);");
        addBlankLine();
        
        // Global variables
        addLine("HINSTANCE hInst;");
        addLine("HWND hMainWnd;");
        addLine("std::string currentFile;");
        
        if (ast.paper) {
            std::function<void(const std::unique_ptr<Component>&)> declareHwnds;
            declareHwnds = [&](const std::unique_ptr<Component>& comp) {
                if (comp->type != TokenType::KEYWORD_MENU && comp->type != TokenType::KEYWORD_MENUITEM &&
                    comp->type != TokenType::KEYWORD_SUBMENU && comp->type != TokenType::KEYWORD_SEPARATOR &&
                    comp->type != TokenType::KEYWORD_POPUPMENU) {
                    addLine("HWND h" + comp->name + " = NULL;");
                    getControlId(comp->name);
                }
                for (auto& c : comp->children) declareHwnds(c);
            };
            for (auto& child : ast.paper->children) declareHwnds(child);
        }
        addBlankLine();
        
        // Menu action IDs
        addLine("enum { IDM_FIRST = 1001 };");
        addBlankLine();
        
        // Collect menu actions
        std::string menuActions;
        collectMenuActions(ast, menuActions, editorName);
        
        // WndProc
        addLine("LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {");
        indentLevel++;
        addLine("switch (msg) {");
        addLine("    case WM_CREATE:");
        addLine("        CreateMenus(hWnd);");
        addLine("        CreateControls(hWnd);");
        addLine("        return 0;");
        addLine("    case WM_COMMAND:");
        addLine("        switch (LOWORD(wParam)) {");
        if (!menuActions.empty()) output += menuActions;
        addLine("            default: break;");
        addLine("        }");
        addLine("        break;");
        addLine("    case WM_SIZE:");
		if (!editorName.empty())
		    addLine("        if (h" + editorName + ") SetWindowPos(h" + editorName + ", NULL, 0, 0, LOWORD(lParam), HIWORD(lParam) - 20, SWP_NOZORDER);");
		if (controlIds.find("statusBar") != controlIds.end())
		    addLine("        if (hstatusBar) SendMessageA(hstatusBar, WM_SIZE, 0, 0);");
		addLine("        break;");
        addLine("    case WM_DESTROY:");
        addLine("        PostQuitMessage(0);");
        addLine("        return 0;");
        addLine("}");
        addLine("return DefWindowProc(hWnd, msg, wParam, lParam);");
        indentLevel--;
        addLine("}");
        addBlankLine();
        
        // CreateMenus
        addLine("void CreateMenus(HWND hWnd) {");
        indentLevel++;
        if (ast.paper) {
            int menuId = 2000;
            for (auto& child : ast.paper->children) {
                if (child->type == TokenType::KEYWORD_MENU) {
                    addLine("HMENU hMenuBar = CreateMenu();");
                    for (auto& menuItem : child->children) {
                        if (menuItem->type == TokenType::KEYWORD_MENUITEM) {
                            std::string text = getPropStr(*menuItem, "text", menuItem->name);
                            int subId = ++menuId;
                            addLine("{ HMENU hSub" + std::to_string(subId) + " = CreatePopupMenu();");
                            for (auto& subItem : menuItem->children) {
                                if (subItem->type == TokenType::KEYWORD_SEPARATOR) {
                                    addLine("AppendMenu(hSub" + std::to_string(subId) + ", MF_SEPARATOR, 0, NULL);");
                                } else if (subItem->type == TokenType::KEYWORD_MENUITEM) {
                                    std::string sText = getPropStr(*subItem, "text", subItem->name);
                                    int sId = getControlId(subItem->name);
                                    addLine("AppendMenu(hSub" + std::to_string(subId) + ", MF_STRING, " + std::to_string(sId) + ", \"" + escapeString(sText) + "\");");
                                }
                            }
                            addLine("AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hSub" + std::to_string(subId) + ", \"" + escapeString(text) + "\"); }");
                        }
                    }
                    addLine("SetMenu(hWnd, hMenuBar);");
                }
            }
        }
        indentLevel--;
        addLine("}");
        addBlankLine();
        
        // CreateControls
        addLine("void CreateControls(HWND hWnd) {");
        indentLevel++;
        addLine("hMainWnd = hWnd;");
        addLine("INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_STANDARD_CLASSES | ICC_BAR_CLASSES };");
        addLine("InitCommonControlsEx(&icex);");
        if (ast.paper) {
            for (auto& child : ast.paper->children) {
                if (child->type != TokenType::KEYWORD_MENU && child->type != TokenType::KEYWORD_MENUITEM &&
                    child->type != TokenType::KEYWORD_SUBMENU && child->type != TokenType::KEYWORD_SEPARATOR) {
                    generateComponentCreation(*child, "hWnd");
                }
            }
        }
        indentLevel--;
        addLine("}");
        addBlankLine();
        
        // Window properties
        std::string winTitle = "Paper5 Application";
        int winW = 800, winH = 600;
        if (ast.paper) {
            winTitle = getPropStr(*ast.paper, "title", winTitle);
            winW = getPropInt(*ast.paper, "width", winW);
            winH = getPropInt(*ast.paper, "height", winH);
        }
        
        // WinMain
        addLine("int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {");
        indentLevel++;
        addLine("hInst = hInstance;");
        addLine("srand((unsigned int)time(NULL));");
        addLine("WNDCLASSEX wc = { sizeof(WNDCLASSEX) };");
        addLine("wc.lpfnWndProc = WndProc;");
        addLine("wc.hInstance = hInstance;");
        addLine("wc.hCursor = LoadCursor(NULL, IDC_ARROW);");
        addLine("wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);");
        addLine("wc.lpszClassName = \"" + currentPaperName + "Class\";");
        addLine("RegisterClassEx(&wc);");
        addLine("hMainWnd = CreateWindowEx(0, \"" + currentPaperName + "Class\", \"" + escapeString(winTitle) + "\", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, " + std::to_string(winW) + ", " + std::to_string(winH) + ", NULL, NULL, hInstance, NULL);");
        addLine("if (!hMainWnd) return 1;");
        addLine("ShowWindow(hMainWnd, nCmdShow);");
        addLine("UpdateWindow(hMainWnd);");
        addLine("MSG m;");
        addLine("while (GetMessage(&m, NULL, 0, 0)) { TranslateMessage(&m); DispatchMessage(&m); }");
        addLine("return (int)m.wParam;");
        indentLevel--;
        addLine("}");
        
        std::ofstream outFile(filename);
        if (outFile.is_open()) { outFile << output; outFile.close(); std::cout << "Generated: " << filename << std::endl; }
        else std::cerr << "Error: Cannot write to " << filename << std::endl;
    }
};

} // namespace Paper5

#endif // CODEGENERATOR_HPP