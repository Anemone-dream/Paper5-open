#ifndef LEXER_HPP
#define LEXER_HPP

#include "Token.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

namespace Paper5 {

class Lexer {
private:
    std::string source;
    size_t pos;
    int line;
    int col;
    std::unordered_map<std::string, TokenType> kw;

    void initKW() {
        kw["value"] = TokenType::KEYWORD_VALUE;
        kw["int"] = TokenType::KEYWORD_INT;
        kw["float"] = TokenType::KEYWORD_FLOAT;
        kw["string"] = TokenType::KEYWORD_STRING;
        kw["bool"] = TokenType::KEYWORD_BOOL;
        kw["color"] = TokenType::KEYWORD_COLOR;
        kw["position"] = TokenType::KEYWORD_POSITION;
        kw["size"] = TokenType::KEYWORD_SIZE;
        kw["rect"] = TokenType::KEYWORD_RECT;
        kw["arr"] = TokenType::KEYWORD_ARR;
        kw["code"] = TokenType::KEYWORD_CODE;
        kw["paper"] = TokenType::KEYWORD_PAPER;
        kw["script"] = TokenType::KEYWORD_SCRIPT;
        kw["return"] = TokenType::KEYWORD_RETURN;
        kw["if"] = TokenType::KEYWORD_IF;
        kw["else"] = TokenType::KEYWORD_ELSE;
        kw["while"] = TokenType::KEYWORD_WHILE;
        kw["for"] = TokenType::KEYWORD_FOR;
        kw["foreach"] = TokenType::KEYWORD_FOREACH;
        kw["in"] = TokenType::KEYWORD_IN;
        kw["break"] = TokenType::KEYWORD_BREAK;
        kw["continue"] = TokenType::KEYWORD_CONTINUE;
        kw["null"] = TokenType::KEYWORD_NULL;
        kw["panel"] = TokenType::KEYWORD_PANEL;
        kw["group"] = TokenType::KEYWORD_GROUP;
        kw["tab"] = TokenType::KEYWORD_TAB;
        kw["splitter"] = TokenType::KEYWORD_SPLITTER;
        kw["scroll"] = TokenType::KEYWORD_SCROLL;
        kw["button"] = TokenType::KEYWORD_BUTTON;
        kw["entry"] = TokenType::KEYWORD_ENTRY;
        kw["textarea"] = TokenType::KEYWORD_TEXTAREA;
        kw["checkbox"] = TokenType::KEYWORD_CHECKBOX;
        kw["radio"] = TokenType::KEYWORD_RADIO;
        kw["combobox"] = TokenType::KEYWORD_COMBOBOX;
        kw["listbox"] = TokenType::KEYWORD_LISTBOX;
        kw["slider"] = TokenType::KEYWORD_SLIDER;
        kw["spin"] = TokenType::KEYWORD_SPIN;
        kw["datepicker"] = TokenType::KEYWORD_DATEPICKER;
        kw["colorpicker"] = TokenType::KEYWORD_COLORPICKER;
        kw["text"] = TokenType::KEYWORD_TEXT;
        kw["picture"] = TokenType::KEYWORD_PICTURE;
        kw["canvas"] = TokenType::KEYWORD_CANVAS;
        kw["progress"] = TokenType::KEYWORD_PROGRESS;
        kw["status"] = TokenType::KEYWORD_STATUS;
        kw["tooltip"] = TokenType::KEYWORD_TOOLTIP;
        kw["icon"] = TokenType::KEYWORD_ICON;
        kw["menu"] = TokenType::KEYWORD_MENU;
        kw["menuitem"] = TokenType::KEYWORD_MENUITEM;
        kw["submenu"] = TokenType::KEYWORD_SUBMENU;
        kw["separator"] = TokenType::KEYWORD_SEPARATOR;
        kw["popupmenu"] = TokenType::KEYWORD_POPUPMENU;
        kw["messagebox"] = TokenType::KEYWORD_MESSAGEBOX;
        kw["filedialog"] = TokenType::KEYWORD_FILEDIALOG;
        kw["colordialog"] = TokenType::KEYWORD_COLORDIALOG;
        kw["fontdialog"] = TokenType::KEYWORD_FONTDIALOG;
        kw["inputbox"] = TokenType::KEYWORD_INPUTBOX;
        kw["timer"] = TokenType::KEYWORD_TIMER;
        kw["task"] = TokenType::KEYWORD_TASK;
        kw["clipboard"] = TokenType::KEYWORD_CLIPBOARD;
        kw["registry"] = TokenType::KEYWORD_REGISTRY;
        kw["ini"] = TokenType::KEYWORD_INI;
    }

    char ch() { return pos < source.size() ? source[pos] : '\0'; }
    char chNext() { return pos + 1 < source.size() ? source[pos + 1] : '\0'; }
    void next() { if (pos < source.size()) { if (source[pos] == '\n') { line++; col = 1; } else col++; pos++; } }
    void skipWS() { while (ch() == ' ' || ch() == '\t' || ch() == '\r' || ch() == '\n') next(); }

    std::string readTagName() {
        next(); // skip '<'
        std::string s;
        while (isalnum(ch()) || ch() == '_' || ch() == '[' || ch() == ']') {
            if (ch() == '[') { s += ch(); next(); while (ch() != ']' && ch() != '\0') { s += ch(); next(); } if (ch() == ']') { s += ch(); next(); } }
            else { s += ch(); next(); }
        }
        if (ch() == '>') next();
        return s;
    }

    TokenType tagType(const std::string& t) {
        auto it = kw.find(t);
        if (it != kw.end()) return it->second;
        if (t == "true" || t == "false") return TokenType::LIT_BOOL;
        return TokenType::UNKNOWN;
    }

    std::string readIdent() {
        std::string s;
        while (isalnum(ch()) || ch() == '_') { s += ch(); next(); }
        return s;
    }

    bool isNumChar(char c) { return isdigit(c) || c == '.'; }

public:
    Lexer(const std::string& src) : source(src), pos(0), line(1), col(1) { initKW(); }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (pos < source.size()) {
            skipWS();
            if (pos >= source.size()) break;

            int sl = line, sc = col;
            char c = ch();

            // Comment
            if (c == '/' && chNext() == '/') { while (ch() != '\n' && ch() != '\0') next(); continue; }
            if (c == '/' && chNext() == '*') { next(); next(); while (!(ch() == '*' && chNext() == '/') && ch() != '\0') next(); if (ch() == '*') { next(); next(); } continue; }

            // Tag <...>
            if (c == '<') {
                std::string tn = readTagName();
                if (tn == "true") { Token tok(TokenType::LIT_BOOL, "<true>", sl, sc); tok.boolValue = true; tokens.push_back(tok); continue; }
                if (tn == "false") { Token tok(TokenType::LIT_BOOL, "<false>", sl, sc); tok.boolValue = false; tokens.push_back(tok); continue; }
                TokenType tt = tagType(tn);
                tokens.push_back(Token(tt == TokenType::UNKNOWN ? TokenType::UNKNOWN : tt, "<" + tn + ">", sl, sc));
                continue;
            }

            // Identifier / keyword
            if (isalpha(c) || c == '_') {
                std::string id = readIdent();
                // Check property keywords
                if (id == "parent") tokens.push_back(Token(TokenType::PROP_PARENT, id, sl, sc));
                else if (id == "x") tokens.push_back(Token(TokenType::PROP_X, id, sl, sc));
                else if (id == "y") tokens.push_back(Token(TokenType::PROP_Y, id, sl, sc));
                else if (id == "width") tokens.push_back(Token(TokenType::PROP_WIDTH, id, sl, sc));
                else if (id == "height") tokens.push_back(Token(TokenType::PROP_HEIGHT, id, sl, sc));
                else if (id == "text") tokens.push_back(Token(TokenType::PROP_TEXT, id, sl, sc));
                else if (id == "color") tokens.push_back(Token(TokenType::PROP_COLOR, id, sl, sc));
                else if (id == "background") tokens.push_back(Token(TokenType::PROP_BACKGROUND, id, sl, sc));
                else if (id == "font") tokens.push_back(Token(TokenType::PROP_FONT, id, sl, sc));
                else if (id == "visible") tokens.push_back(Token(TokenType::PROP_VISIBLE, id, sl, sc));
                else if (id == "enabled") tokens.push_back(Token(TokenType::PROP_ENABLED, id, sl, sc));
                else if (id == "title") tokens.push_back(Token(TokenType::PROP_TITLE, id, sl, sc));
                else if (id == "icon") tokens.push_back(Token(TokenType::PROP_ICON, id, sl, sc));
                else if (id == "resizable") tokens.push_back(Token(TokenType::PROP_RESIZABLE, id, sl, sc));
                else if (id == "src") tokens.push_back(Token(TokenType::PROP_SRC, id, sl, sc));
                else if (id == "value") tokens.push_back(Token(TokenType::PROP_VALUE, id, sl, sc));
                else if (id == "min") tokens.push_back(Token(TokenType::PROP_MIN, id, sl, sc));
                else if (id == "max") tokens.push_back(Token(TokenType::PROP_MAX, id, sl, sc));
                else if (id == "items") tokens.push_back(Token(TokenType::PROP_ITEMS, id, sl, sc));
                else if (id == "checked") tokens.push_back(Token(TokenType::PROP_CHECKED, id, sl, sc));
                else if (id == "placeholder") tokens.push_back(Token(TokenType::PROP_PLACEHOLDER, id, sl, sc));
                else if (id == "interval") tokens.push_back(Token(TokenType::PROP_INTERVAL, id, sl, sc));
                else if (id == "onClick") tokens.push_back(Token(TokenType::KEYWORD_ON_CLICK, id, sl, sc));
                else if (id == "onMouseDown") tokens.push_back(Token(TokenType::KEYWORD_ON_MOUSEDOWN, id, sl, sc));
                else if (id == "onMouseMove") tokens.push_back(Token(TokenType::KEYWORD_ON_MOUSEMOVE, id, sl, sc));
                else if (id == "onMouseUp") tokens.push_back(Token(TokenType::KEYWORD_ON_MOUSEUP, id, sl, sc));
                else if (id == "onKeyDown") tokens.push_back(Token(TokenType::KEYWORD_ON_KEYDOWN, id, sl, sc));
                else if (id == "onChange") tokens.push_back(Token(TokenType::KEYWORD_ON_CHANGE, id, sl, sc));
                else if (id == "onSelect") tokens.push_back(Token(TokenType::KEYWORD_ON_SELECT, id, sl, sc));
                else if (id == "onCheck") tokens.push_back(Token(TokenType::KEYWORD_ON_CHECK, id, sl, sc));
                else if (id == "onPaint") tokens.push_back(Token(TokenType::KEYWORD_ON_PAINT, id, sl, sc));
                else if (id == "onResize") tokens.push_back(Token(TokenType::KEYWORD_ON_RESIZE, id, sl, sc));
                else if (id == "onClose") tokens.push_back(Token(TokenType::KEYWORD_ON_CLOSE, id, sl, sc));
                else if (id == "onLoad") tokens.push_back(Token(TokenType::KEYWORD_ON_LOAD, id, sl, sc));
                else if (id == "onTimer") tokens.push_back(Token(TokenType::KEYWORD_ON_TIMER, id, sl, sc));
                else if (id == "onDrop") tokens.push_back(Token(TokenType::KEYWORD_ON_DROP, id, sl, sc));
                else if (id == "onFocus") tokens.push_back(Token(TokenType::KEYWORD_ON_FOCUS, id, sl, sc));
                else if (id == "onBlur") tokens.push_back(Token(TokenType::KEYWORD_ON_BLUR, id, sl, sc));
                else if (id == "onSlide") tokens.push_back(Token(TokenType::KEYWORD_ON_SLIDE, id, sl, sc));
                else if (id == "onDblClick") tokens.push_back(Token(TokenType::KEYWORD_ON_DBLCLICK, id, sl, sc));
                else if (id == "onMouseEnter") tokens.push_back(Token(TokenType::KEYWORD_ON_MOUSEENTER, id, sl, sc));
                else if (id == "onMouseLeave") tokens.push_back(Token(TokenType::KEYWORD_ON_MOUSELEAVE, id, sl, sc));
                else if (id == "onMouseWheel") tokens.push_back(Token(TokenType::KEYWORD_ON_MOUSEWHEEL, id, sl, sc));
                else if (id == "onKeyUp") tokens.push_back(Token(TokenType::KEYWORD_ON_KEYUP, id, sl, sc));
                else if (id == "onKeyPress") tokens.push_back(Token(TokenType::KEYWORD_ON_KEYPRESS, id, sl, sc));
                else if (id == "onMinimize") tokens.push_back(Token(TokenType::KEYWORD_ON_MINIMIZE, id, sl, sc));
                else if (id == "onMaximize") tokens.push_back(Token(TokenType::KEYWORD_ON_MAXIMIZE, id, sl, sc));
                else if (id == "onRestore") tokens.push_back(Token(TokenType::KEYWORD_ON_RESTORE, id, sl, sc));
                else if (id == "style") tokens.push_back(Token(TokenType::PROP_STYLE, id, sl, sc));
                else if (id == "tag") tokens.push_back(Token(TokenType::PROP_TAG, id, sl, sc));
                else if (id == "border") tokens.push_back(Token(TokenType::PROP_BORDER, id, sl, sc));
                else if (id == "tooltip") tokens.push_back(Token(TokenType::PROP_TOOLTIP, id, sl, sc));
                else if (id == "cursor") tokens.push_back(Token(TokenType::PROP_CURSOR, id, sl, sc));
                else if (id == "anchor") tokens.push_back(Token(TokenType::PROP_ANCHOR, id, sl, sc));
                else if (id == "dock") tokens.push_back(Token(TokenType::PROP_DOCK, id, sl, sc));
                else if (id == "margin") tokens.push_back(Token(TokenType::PROP_MARGIN, id, sl, sc));
                else if (id == "padding") tokens.push_back(Token(TokenType::PROP_PADDING, id, sl, sc));
                else if (id == "maximize") tokens.push_back(Token(TokenType::PROP_MAXIMIZE, id, sl, sc));
                else if (id == "minimize") tokens.push_back(Token(TokenType::PROP_MINIMIZE, id, sl, sc));
                else if (id == "topmost") tokens.push_back(Token(TokenType::PROP_TOPMOST, id, sl, sc));
                else if (id == "opacity") tokens.push_back(Token(TokenType::PROP_OPACITY, id, sl, sc));
                else if (id == "default") tokens.push_back(Token(TokenType::PROP_DEFAULT, id, sl, sc));
                else if (id == "image") tokens.push_back(Token(TokenType::PROP_IMAGE, id, sl, sc));
                else if (id == "password") tokens.push_back(Token(TokenType::PROP_PASSWORD, id, sl, sc));
                else if (id == "readonly") tokens.push_back(Token(TokenType::PROP_READONLY, id, sl, sc));
                else if (id == "maxlength") tokens.push_back(Token(TokenType::PROP_MAXLENGTH, id, sl, sc));
                else if (id == "multiline") tokens.push_back(Token(TokenType::PROP_MULTILINE, id, sl, sc));
                else if (id == "selectedIndex") tokens.push_back(Token(TokenType::PROP_SELECTEDINDEX, id, sl, sc));
                else if (id == "selectedItem") tokens.push_back(Token(TokenType::PROP_SELECTEDITEM, id, sl, sc));
                else if (id == "step") tokens.push_back(Token(TokenType::PROP_STEP, id, sl, sc));
                else if (id == "marquee") tokens.push_back(Token(TokenType::PROP_MARQUEE, id, sl, sc));
                else if (id == "stretch") tokens.push_back(Token(TokenType::PROP_STRETCH, id, sl, sc));
                else if (id == "fit") tokens.push_back(Token(TokenType::PROP_FIT, id, sl, sc));
                else if (id == "shortcut") tokens.push_back(Token(TokenType::PROP_SHORTCUT, id, sl, sc));
                else if (id == "repeat") tokens.push_back(Token(TokenType::PROP_REPEAT, id, sl, sc));
                else {
                    auto it = kw.find(id);
                    if (it != kw.end()) {
                        Token tok(it->second, id, sl, sc);
                        tokens.push_back(tok);
                    } else {
                        tokens.push_back(Token(TokenType::IDENTIFIER, id, sl, sc));
                    }
                }
                continue;
            }

            // Number
            if (isdigit(c)) {
                std::string num; bool isF = false;
                while (isdigit(ch()) || ch() == '.') { if (ch() == '.') isF = true; num += ch(); next(); }
                Token tok(isF ? TokenType::LIT_FLOAT : TokenType::LIT_INTEGER, num, sl, sc);
                if (isF) tok.floatValue = std::stod(num); else tok.intValue = std::stoi(num);
                tokens.push_back(tok);
                continue;
            }

            // String
            if (c == '"' || c == '\'') {
                char q = c; next();
                std::string s;
                while (ch() != q && ch() != '\0') {
                    if (ch() == '\\') { next(); char e = ch(); next(); if (e == 'n') s += '\n'; else if (e == 't') s += '\t'; else if (e == 'r') s += '\r'; else s += e; }
                    else { s += ch(); next(); }
                }
                if (ch() == q) next();
                Token tok(TokenType::LIT_STRING, "\"" + s + "\"", sl, sc);
                tok.stringValue = s;
                tokens.push_back(tok);
                continue;
            }

            // Color
            if (c == '#') {
                next();
                std::string hex;
                while (isxdigit(ch())) { hex += ch(); next(); }
                Token tok(TokenType::LIT_COLOR, "#" + hex, sl, sc);
                tok.stringValue = "#" + hex;
                tokens.push_back(tok);
                continue;
            }

            // Two-char operators
            if (c == '=' && chNext() == '=') { next(); next(); tokens.push_back(Token(TokenType::OPERATOR_EQ, "==", sl, sc)); continue; }
            if (c == '!' && chNext() == '=') { next(); next(); tokens.push_back(Token(TokenType::OPERATOR_NEQ, "!=", sl, sc)); continue; }
            if (c == '<' && chNext() == '=') { next(); next(); tokens.push_back(Token(TokenType::OPERATOR_LTE, "<=", sl, sc)); continue; }
            if (c == '>' && chNext() == '=') { next(); next(); tokens.push_back(Token(TokenType::OPERATOR_GTE, ">=", sl, sc)); continue; }
            if (c == '&' && chNext() == '&') { next(); next(); tokens.push_back(Token(TokenType::OPERATOR_AND, "&&", sl, sc)); continue; }
            if (c == '|' && chNext() == '|') { next(); next(); tokens.push_back(Token(TokenType::OPERATOR_OR, "||", sl, sc)); continue; }

            // Single char
            next();
            switch (c) {
                case '{': tokens.push_back(Token(TokenType::SEP_LBRACE, "{", sl, sc)); break;
                case '}': tokens.push_back(Token(TokenType::SEP_RBRACE, "}", sl, sc)); break;
                case '(': tokens.push_back(Token(TokenType::SEP_LPAREN, "(", sl, sc)); break;
                case ')': tokens.push_back(Token(TokenType::SEP_RPAREN, ")", sl, sc)); break;
                case '[': tokens.push_back(Token(TokenType::SEP_LBRACKET, "[", sl, sc)); break;
                case ']': tokens.push_back(Token(TokenType::SEP_RBRACKET, "]", sl, sc)); break;
                case ';': tokens.push_back(Token(TokenType::SEP_SEMICOLON, ";", sl, sc)); break;
                case ',': tokens.push_back(Token(TokenType::SEP_COMMA, ",", sl, sc)); break;
                case '.': tokens.push_back(Token(TokenType::SEP_DOT, ".", sl, sc)); break;
                case ':': tokens.push_back(Token(TokenType::SEP_COLON, ":", sl, sc)); break;
                case '=': tokens.push_back(Token(TokenType::OPERATOR_ASSIGN, "=", sl, sc)); break;
                case '+': tokens.push_back(Token(TokenType::OPERATOR_PLUS, "+", sl, sc)); break;
                case '-': tokens.push_back(Token(TokenType::OPERATOR_MINUS, "-", sl, sc)); break;
                case '*': tokens.push_back(Token(TokenType::OPERATOR_MUL, "*", sl, sc)); break;
                case '/': tokens.push_back(Token(TokenType::OPERATOR_DIV, "/", sl, sc)); break;
                case '%': tokens.push_back(Token(TokenType::OPERATOR_MOD, "%", sl, sc)); break;
                case '!': tokens.push_back(Token(TokenType::OPERATOR_NOT, "!", sl, sc)); break;
                case '<': tokens.push_back(Token(TokenType::OPERATOR_LT, "<", sl, sc)); break;
                case '>': tokens.push_back(Token(TokenType::OPERATOR_GT, ">", sl, sc)); break;
                default: tokens.push_back(Token(TokenType::UNKNOWN, std::string(1, c), sl, sc)); break;
            }
        }
        tokens.push_back(Token(TokenType::END_OF_FILE, "", line, col));
        return tokens;
    }
};

} // namespace Paper5

#endif // LEXER_HPP