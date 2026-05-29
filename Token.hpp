#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <vector>
#include <iostream>

namespace Paper5 {

enum class TokenType {
    // Type keywords
    KEYWORD_VALUE,
    KEYWORD_INT,
    KEYWORD_FLOAT,
    KEYWORD_STRING,
    KEYWORD_BOOL,
    KEYWORD_COLOR,
    KEYWORD_POSITION,
    KEYWORD_SIZE,
    KEYWORD_RECT,
    KEYWORD_ARR,
    KEYWORD_CODE,
    KEYWORD_PAPER,
    KEYWORD_SCRIPT,
    
    // Component keywords - containers
    KEYWORD_PANEL,
    KEYWORD_GROUP,
    KEYWORD_TAB,
    KEYWORD_SPLITTER,
    KEYWORD_SCROLL,
    
    // Component keywords - interactive
    KEYWORD_BUTTON,
    KEYWORD_ENTRY,
    KEYWORD_TEXTAREA,
    KEYWORD_CHECKBOX,
    KEYWORD_RADIO,
    
    // Component keywords - selection
    KEYWORD_COMBOBOX,
    KEYWORD_LISTBOX,
    KEYWORD_SLIDER,
    KEYWORD_SPIN,
    KEYWORD_DATEPICKER,
    KEYWORD_COLORPICKER,
    
    // Component keywords - display
    KEYWORD_TEXT,
    KEYWORD_PICTURE,
    KEYWORD_CANVAS,
    KEYWORD_PROGRESS,
    KEYWORD_STATUS,
    KEYWORD_TOOLTIP,
    KEYWORD_ICON,
    
    // Component keywords - menu
    KEYWORD_MENU,
    KEYWORD_MENUITEM,
    KEYWORD_SUBMENU,
    KEYWORD_SEPARATOR,
    KEYWORD_POPUPMENU,
    
    // Component keywords - dialog
    KEYWORD_MESSAGEBOX,
    KEYWORD_FILEDIALOG,
    KEYWORD_COLORDIALOG,
    KEYWORD_FONTDIALOG,
    KEYWORD_INPUTBOX,
    
    // Component keywords - other
    KEYWORD_TIMER,
    KEYWORD_TASK,
    KEYWORD_CLIPBOARD,
    KEYWORD_REGISTRY,
    KEYWORD_INI,
    
    // Event keywords
    KEYWORD_ON_CLICK,
    KEYWORD_ON_DBLCLICK,
    KEYWORD_ON_MOUSEDOWN,
    KEYWORD_ON_MOUSEUP,
    KEYWORD_ON_MOUSEMOVE,
    KEYWORD_ON_MOUSEENTER,
    KEYWORD_ON_MOUSELEAVE,
    KEYWORD_ON_MOUSEWHEEL,
    KEYWORD_ON_KEYDOWN,
    KEYWORD_ON_KEYUP,
    KEYWORD_ON_KEYPRESS,
    KEYWORD_ON_CHANGE,
    KEYWORD_ON_SELECT,
    KEYWORD_ON_CHECK,
    KEYWORD_ON_SLIDE,
    KEYWORD_ON_LOAD,
    KEYWORD_ON_CLOSE,
    KEYWORD_ON_RESIZE,
    KEYWORD_ON_MOVE,
    KEYWORD_ON_FOCUS,
    KEYWORD_ON_BLUR,
    KEYWORD_ON_MINIMIZE,
    KEYWORD_ON_MAXIMIZE,
    KEYWORD_ON_RESTORE,
    KEYWORD_ON_PAINT,
    KEYWORD_ON_TIMER,
    KEYWORD_ON_DROP,
    
    // Property keywords - common
    PROP_PARENT,
    PROP_X, PROP_Y,
    PROP_WIDTH, PROP_HEIGHT,
    PROP_TEXT,
    PROP_COLOR,
    PROP_BACKGROUND,
    PROP_FONT,
    PROP_VISIBLE,
    PROP_ENABLED,
    PROP_TAG,
    PROP_BORDER,
    PROP_TOOLTIP,
    PROP_CURSOR,
    PROP_ANCHOR,
    PROP_DOCK,
    PROP_MARGIN,
    PROP_PADDING,
    
    // Property keywords - window
    PROP_TITLE,
    PROP_ICON,
    PROP_RESIZABLE,
    PROP_MAXIMIZE,
    PROP_MINIMIZE,
    PROP_TOPMOST,
    PROP_OPACITY,
    PROP_STYLE,
    
    // Property keywords - button
    PROP_DEFAULT,
    PROP_IMAGE,
    
    // Property keywords - input
    PROP_PLACEHOLDER,
    PROP_PASSWORD,
    PROP_READONLY,
    PROP_MAXLENGTH,
    PROP_MULTILINE,
    
    // Property keywords - selection
    PROP_ITEMS,
    PROP_SELECTEDINDEX,
    PROP_SELECTEDITEM,
    PROP_MIN, PROP_MAX,
    PROP_VALUE,
    PROP_STEP,
    
    // Property keywords - progress
    PROP_MARQUEE,
    
    // Property keywords - image
    PROP_SRC,
    PROP_STRETCH,
    PROP_FIT,
    
    // Property keywords - menu
    PROP_SHORTCUT,
    PROP_CHECKED,
    
    // Property keywords - timer
    PROP_INTERVAL,
    PROP_REPEAT,
    
    // Literals
    LIT_INTEGER,
    LIT_FLOAT,
    LIT_STRING,
    LIT_BOOL,
    LIT_COLOR,
    LIT_POSITION,
    LIT_SIZE,
    LIT_RECT,
    
    IDENTIFIER,
    
    // Operators
    OPERATOR_ASSIGN,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MUL,
    OPERATOR_DIV,
    OPERATOR_MOD,
    OPERATOR_EQ,
    OPERATOR_NEQ,
    OPERATOR_LT,
    OPERATOR_GT,
    OPERATOR_LTE,
    OPERATOR_GTE,
    OPERATOR_AND,
    OPERATOR_OR,
    OPERATOR_NOT,
    
    // Separators
    SEP_LBRACE,
    SEP_RBRACE,
    SEP_LPAREN,
    SEP_RPAREN,
    SEP_LBRACKET,
    SEP_RBRACKET,
    SEP_SEMICOLON,
    SEP_COMMA,
    SEP_DOT,
    SEP_COLON,
    
    // Statement keywords
    KEYWORD_RETURN,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_FOREACH,
    KEYWORD_IN,
    KEYWORD_BREAK,
    KEYWORD_CONTINUE,
    KEYWORD_TRUE,
    KEYWORD_FALSE,
    KEYWORD_NULL,
    
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    int intValue;
    double floatValue;
    std::string stringValue;
    bool boolValue;
    
    Token() : type(TokenType::UNKNOWN), line(0), column(0), 
              intValue(0), floatValue(0.0), boolValue(false) {}
    
    Token(TokenType t, const std::string& lex, int l, int c) 
        : type(t), lexeme(lex), line(l), column(c), 
          intValue(0), floatValue(0.0), boolValue(false) {}
    
    std::string toString() const {
        return "Token(" + std::to_string(static_cast<int>(type)) + 
               ", '" + lexeme + "', line " + std::to_string(line) + ")";
    }
};

inline std::string tokenTypeToString(TokenType type) {
    switch(type) {
        case TokenType::KEYWORD_VALUE: return "VALUE";
        case TokenType::KEYWORD_INT: return "INT";
        case TokenType::KEYWORD_FLOAT: return "FLOAT";
        case TokenType::KEYWORD_STRING: return "STRING";
        case TokenType::KEYWORD_BOOL: return "BOOL";
        case TokenType::KEYWORD_COLOR: return "COLOR";
        case TokenType::KEYWORD_POSITION: return "POSITION";
        case TokenType::KEYWORD_SIZE: return "SIZE";
        case TokenType::KEYWORD_RECT: return "RECT";
        case TokenType::KEYWORD_ARR: return "ARR";
        case TokenType::KEYWORD_CODE: return "CODE";
        case TokenType::KEYWORD_PAPER: return "PAPER";
        case TokenType::KEYWORD_SCRIPT: return "SCRIPT";
        case TokenType::KEYWORD_PANEL: return "PANEL";
        case TokenType::KEYWORD_GROUP: return "GROUP";
        case TokenType::KEYWORD_TAB: return "TAB";
        case TokenType::KEYWORD_SPLITTER: return "SPLITTER";
        case TokenType::KEYWORD_SCROLL: return "SCROLL";
        case TokenType::KEYWORD_BUTTON: return "BUTTON";
        case TokenType::KEYWORD_ENTRY: return "ENTRY";
        case TokenType::KEYWORD_TEXTAREA: return "TEXTAREA";
        case TokenType::KEYWORD_CHECKBOX: return "CHECKBOX";
        case TokenType::KEYWORD_RADIO: return "RADIO";
        case TokenType::KEYWORD_COMBOBOX: return "COMBOBOX";
        case TokenType::KEYWORD_LISTBOX: return "LISTBOX";
        case TokenType::KEYWORD_SLIDER: return "SLIDER";
        case TokenType::KEYWORD_SPIN: return "SPIN";
        case TokenType::KEYWORD_DATEPICKER: return "DATEPICKER";
        case TokenType::KEYWORD_COLORPICKER: return "COLORPICKER";
        case TokenType::KEYWORD_TEXT: return "TEXT";
        case TokenType::KEYWORD_PICTURE: return "PICTURE";
        case TokenType::KEYWORD_CANVAS: return "CANVAS";
        case TokenType::KEYWORD_PROGRESS: return "PROGRESS";
        case TokenType::KEYWORD_STATUS: return "STATUS";
        case TokenType::KEYWORD_TOOLTIP: return "TOOLTIP";
        case TokenType::KEYWORD_ICON: return "ICON";
        case TokenType::KEYWORD_MENU: return "MENU";
        case TokenType::KEYWORD_MENUITEM: return "MENUITEM";
        case TokenType::KEYWORD_SUBMENU: return "SUBMENU";
        case TokenType::KEYWORD_SEPARATOR: return "SEPARATOR";
        case TokenType::KEYWORD_POPUPMENU: return "POPUPMENU";
        case TokenType::KEYWORD_MESSAGEBOX: return "MESSAGEBOX";
        case TokenType::KEYWORD_FILEDIALOG: return "FILEDIALOG";
        case TokenType::KEYWORD_COLORDIALOG: return "COLORDIALOG";
        case TokenType::KEYWORD_FONTDIALOG: return "FONTDIALOG";
        case TokenType::KEYWORD_INPUTBOX: return "INPUTBOX";
        case TokenType::KEYWORD_TIMER: return "TIMER";
        case TokenType::KEYWORD_TASK: return "TASK";
        case TokenType::KEYWORD_CLIPBOARD: return "CLIPBOARD";
        case TokenType::KEYWORD_REGISTRY: return "REGISTRY";
        case TokenType::KEYWORD_INI: return "INI";
        case TokenType::KEYWORD_ON_CLICK: return "ON_CLICK";
        case TokenType::KEYWORD_ON_DBLCLICK: return "ON_DBLCLICK";
        case TokenType::KEYWORD_ON_MOUSEDOWN: return "ON_MOUSEDOWN";
        case TokenType::KEYWORD_ON_MOUSEUP: return "ON_MOUSEUP";
        case TokenType::KEYWORD_ON_MOUSEMOVE: return "ON_MOUSEMOVE";
        case TokenType::KEYWORD_ON_MOUSEENTER: return "ON_MOUSEENTER";
        case TokenType::KEYWORD_ON_MOUSELEAVE: return "ON_MOUSELEAVE";
        case TokenType::KEYWORD_ON_MOUSEWHEEL: return "ON_MOUSEWHEEL";
        case TokenType::KEYWORD_ON_KEYDOWN: return "ON_KEYDOWN";
        case TokenType::KEYWORD_ON_KEYUP: return "ON_KEYUP";
        case TokenType::KEYWORD_ON_KEYPRESS: return "ON_KEYPRESS";
        case TokenType::KEYWORD_ON_CHANGE: return "ON_CHANGE";
        case TokenType::KEYWORD_ON_SELECT: return "ON_SELECT";
        case TokenType::KEYWORD_ON_CHECK: return "ON_CHECK";
        case TokenType::KEYWORD_ON_SLIDE: return "ON_SLIDE";
        case TokenType::KEYWORD_ON_LOAD: return "ON_LOAD";
        case TokenType::KEYWORD_ON_CLOSE: return "ON_CLOSE";
        case TokenType::KEYWORD_ON_RESIZE: return "ON_RESIZE";
        case TokenType::KEYWORD_ON_MOVE: return "ON_MOVE";
        case TokenType::KEYWORD_ON_FOCUS: return "ON_FOCUS";
        case TokenType::KEYWORD_ON_BLUR: return "ON_BLUR";
        case TokenType::KEYWORD_ON_MINIMIZE: return "ON_MINIMIZE";
        case TokenType::KEYWORD_ON_MAXIMIZE: return "ON_MAXIMIZE";
        case TokenType::KEYWORD_ON_RESTORE: return "ON_RESTORE";
        case TokenType::KEYWORD_ON_PAINT: return "ON_PAINT";
        case TokenType::KEYWORD_ON_TIMER: return "ON_TIMER";
        case TokenType::KEYWORD_ON_DROP: return "ON_DROP";
        case TokenType::PROP_PARENT: return "PROP_PARENT";
        case TokenType::PROP_X: return "PROP_X";
        case TokenType::PROP_Y: return "PROP_Y";
        case TokenType::PROP_WIDTH: return "PROP_WIDTH";
        case TokenType::PROP_HEIGHT: return "PROP_HEIGHT";
        case TokenType::PROP_TEXT: return "PROP_TEXT";
        case TokenType::PROP_COLOR: return "PROP_COLOR";
        case TokenType::PROP_BACKGROUND: return "PROP_BACKGROUND";
        case TokenType::PROP_FONT: return "PROP_FONT";
        case TokenType::PROP_VISIBLE: return "PROP_VISIBLE";
        case TokenType::PROP_ENABLED: return "PROP_ENABLED";
        case TokenType::PROP_TAG: return "PROP_TAG";
        case TokenType::PROP_BORDER: return "PROP_BORDER";
        case TokenType::PROP_TOOLTIP: return "PROP_TOOLTIP";
        case TokenType::PROP_CURSOR: return "PROP_CURSOR";
        case TokenType::PROP_ANCHOR: return "PROP_ANCHOR";
        case TokenType::PROP_DOCK: return "PROP_DOCK";
        case TokenType::PROP_MARGIN: return "PROP_MARGIN";
        case TokenType::PROP_PADDING: return "PROP_PADDING";
        case TokenType::PROP_TITLE: return "PROP_TITLE";
        case TokenType::PROP_ICON: return "PROP_ICON";
        case TokenType::PROP_RESIZABLE: return "PROP_RESIZABLE";
        case TokenType::PROP_MAXIMIZE: return "PROP_MAXIMIZE";
        case TokenType::PROP_MINIMIZE: return "PROP_MINIMIZE";
        case TokenType::PROP_TOPMOST: return "PROP_TOPMOST";
        case TokenType::PROP_OPACITY: return "PROP_OPACITY";
        case TokenType::PROP_STYLE: return "PROP_STYLE";
        case TokenType::PROP_DEFAULT: return "PROP_DEFAULT";
        case TokenType::PROP_IMAGE: return "PROP_IMAGE";
        case TokenType::PROP_PLACEHOLDER: return "PROP_PLACEHOLDER";
        case TokenType::PROP_PASSWORD: return "PROP_PASSWORD";
        case TokenType::PROP_READONLY: return "PROP_READONLY";
        case TokenType::PROP_MAXLENGTH: return "PROP_MAXLENGTH";
        case TokenType::PROP_MULTILINE: return "PROP_MULTILINE";
        case TokenType::PROP_ITEMS: return "PROP_ITEMS";
        case TokenType::PROP_SELECTEDINDEX: return "PROP_SELECTEDINDEX";
        case TokenType::PROP_SELECTEDITEM: return "PROP_SELECTEDITEM";
        case TokenType::PROP_MIN: return "PROP_MIN";
        case TokenType::PROP_MAX: return "PROP_MAX";
        case TokenType::PROP_VALUE: return "PROP_VALUE";
        case TokenType::PROP_STEP: return "PROP_STEP";
        case TokenType::PROP_MARQUEE: return "PROP_MARQUEE";
        case TokenType::PROP_SRC: return "PROP_SRC";
        case TokenType::PROP_STRETCH: return "PROP_STRETCH";
        case TokenType::PROP_FIT: return "PROP_FIT";
        case TokenType::PROP_SHORTCUT: return "PROP_SHORTCUT";
        case TokenType::PROP_CHECKED: return "PROP_CHECKED";
        case TokenType::PROP_INTERVAL: return "PROP_INTERVAL";
        case TokenType::PROP_REPEAT: return "PROP_REPEAT";
        case TokenType::LIT_INTEGER: return "INTEGER";
        case TokenType::LIT_FLOAT: return "FLOAT";
        case TokenType::LIT_STRING: return "STRING";
        case TokenType::LIT_BOOL: return "BOOL";
        case TokenType::LIT_COLOR: return "COLOR";
        case TokenType::LIT_POSITION: return "POSITION";
        case TokenType::LIT_SIZE: return "SIZE";
        case TokenType::LIT_RECT: return "RECT";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::OPERATOR_ASSIGN: return "ASSIGN";
        case TokenType::OPERATOR_PLUS: return "PLUS";
        case TokenType::OPERATOR_MINUS: return "MINUS";
        case TokenType::OPERATOR_MUL: return "MUL";
        case TokenType::OPERATOR_DIV: return "DIV";
        case TokenType::OPERATOR_MOD: return "MOD";
        case TokenType::OPERATOR_EQ: return "EQ";
        case TokenType::OPERATOR_NEQ: return "NEQ";
        case TokenType::OPERATOR_LT: return "LT";
        case TokenType::OPERATOR_GT: return "GT";
        case TokenType::OPERATOR_LTE: return "LTE";
        case TokenType::OPERATOR_GTE: return "GTE";
        case TokenType::OPERATOR_AND: return "AND";
        case TokenType::OPERATOR_OR: return "OR";
        case TokenType::OPERATOR_NOT: return "NOT";
        case TokenType::SEP_LBRACE: return "LBRACE";
        case TokenType::SEP_RBRACE: return "RBRACE";
        case TokenType::SEP_LPAREN: return "LPAREN";
        case TokenType::SEP_RPAREN: return "RPAREN";
        case TokenType::SEP_LBRACKET: return "LBRACKET";
        case TokenType::SEP_RBRACKET: return "RBRACKET";
        case TokenType::SEP_SEMICOLON: return "SEMICOLON";
        case TokenType::SEP_COMMA: return "COMMA";
        case TokenType::SEP_DOT: return "DOT";
        case TokenType::SEP_COLON: return "COLON";
        case TokenType::KEYWORD_RETURN: return "RETURN";
        case TokenType::KEYWORD_IF: return "IF";
        case TokenType::KEYWORD_ELSE: return "ELSE";
        case TokenType::KEYWORD_WHILE: return "WHILE";
        case TokenType::KEYWORD_FOR: return "FOR";
        case TokenType::KEYWORD_FOREACH: return "FOREACH";
        case TokenType::KEYWORD_IN: return "IN";
        case TokenType::KEYWORD_BREAK: return "BREAK";
        case TokenType::KEYWORD_CONTINUE: return "CONTINUE";
        case TokenType::KEYWORD_TRUE: return "TRUE";
        case TokenType::KEYWORD_FALSE: return "FALSE";
        case TokenType::KEYWORD_NULL: return "NULL";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}

inline bool isTypeKeyword(TokenType type) {
    return type == TokenType::KEYWORD_VALUE ||
           type == TokenType::KEYWORD_INT ||
           type == TokenType::KEYWORD_FLOAT ||
           type == TokenType::KEYWORD_STRING ||
           type == TokenType::KEYWORD_BOOL ||
           type == TokenType::KEYWORD_COLOR ||
           type == TokenType::KEYWORD_POSITION ||
           type == TokenType::KEYWORD_SIZE ||
           type == TokenType::KEYWORD_RECT ||
           type == TokenType::KEYWORD_ARR;
}

inline bool isComponentKeyword(TokenType type) {
    return (type >= TokenType::KEYWORD_PANEL && type <= TokenType::KEYWORD_INI) ||
           type == TokenType::KEYWORD_PAPER;
}

} // namespace Paper5

#endif // TOKEN_HPP