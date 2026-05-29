#ifndef AST_HPP
#define AST_HPP

#include "Token.hpp"
#include <vector>
#include <memory>
#include <string>
#include <utility>

namespace Paper5 {

struct Expression;
struct Statement;
struct Component;
struct Script;

using ExpressionPtr = std::unique_ptr<Expression>;
using StatementPtr = std::unique_ptr<Statement>;

// ===== Expressions =====

struct Expression {
    virtual ~Expression() = default;
};

struct LiteralExpr : Expression {
    TokenType literalType;
    
    int intValue = 0;
    double floatValue = 0.0;
    std::string stringValue;
    bool boolValue = false;
    std::vector<ExpressionPtr> elements; // for position, size, rect
    
    LiteralExpr(TokenType t, int val) : literalType(t), intValue(val) {}
    LiteralExpr(TokenType t, double val) : literalType(t), floatValue(val) {}
    LiteralExpr(TokenType t, const std::string& val) : literalType(t), stringValue(val) {}
    LiteralExpr(TokenType t, bool val) : literalType(t), boolValue(val) {}
    LiteralExpr(TokenType t) : literalType(t) {}
};

struct IdentifierExpr : Expression {
    std::string name;
    IdentifierExpr(const std::string& n) : name(n) {}
};

struct PropertyAccessExpr : Expression {
    std::string object;
    std::string property;
    PropertyAccessExpr(const std::string& obj, const std::string& prop)
        : object(obj), property(prop) {}
};

struct ArrayAccessExpr : Expression {
    ExpressionPtr array;
    ExpressionPtr index;
    ArrayAccessExpr(ExpressionPtr arr, ExpressionPtr idx)
        : array(std::move(arr)), index(std::move(idx)) {}
};

struct BinaryExpr : Expression {
    ExpressionPtr left;
    TokenType op;
    ExpressionPtr right;
    BinaryExpr(ExpressionPtr l, TokenType o, ExpressionPtr r) 
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

struct UnaryExpr : Expression {
    TokenType op;
    ExpressionPtr operand;
    UnaryExpr(TokenType o, ExpressionPtr opnd) : op(o), operand(std::move(opnd)) {}
};

struct ArrayExpr : Expression {
    std::vector<ExpressionPtr> elements;
    TokenType elementType = TokenType::UNKNOWN;
    ArrayExpr() = default;
};

struct CallExpr : Expression {
    std::string callee;
    std::vector<ExpressionPtr> arguments;
    CallExpr(const std::string& name) : callee(name) {}
};

// ===== Statements =====

struct Statement {
    virtual ~Statement() = default;
};

struct VarDeclStmt : Statement {
    TokenType varType;
    std::string name;
    ExpressionPtr initializer;
    VarDeclStmt(TokenType type, const std::string& n, ExpressionPtr init)
        : varType(type), name(n), initializer(std::move(init)) {}
};

struct PropertyStmt : Statement {
    std::string name;
    ExpressionPtr value;
    PropertyStmt(const std::string& n, ExpressionPtr v)
        : name(n), value(std::move(v)) {}
};

struct ExprStmt : Statement {
    ExpressionPtr expr;
    ExprStmt(ExpressionPtr e) : expr(std::move(e)) {}
};

struct IfStmt : Statement {
    ExpressionPtr condition;
    std::vector<StatementPtr> thenBranch;
    std::vector<StatementPtr> elseBranch;
    IfStmt(ExpressionPtr cond) : condition(std::move(cond)) {}
};

struct WhileStmt : Statement {
    ExpressionPtr condition;
    std::vector<StatementPtr> body;
    WhileStmt(ExpressionPtr cond) : condition(std::move(cond)) {}
};

struct ForStmt : Statement {
    StatementPtr init;
    ExpressionPtr condition;
    StatementPtr step;
    std::vector<StatementPtr> body;
    ForStmt() = default;
};

struct ForeachStmt : Statement {
    std::string itemVar;
    ExpressionPtr array;
    std::vector<StatementPtr> body;
    ForeachStmt(const std::string& var, ExpressionPtr arr)
        : itemVar(var), array(std::move(arr)) {}
};

struct ReturnStmt : Statement {
    ExpressionPtr value;
    ReturnStmt() = default;
    ReturnStmt(ExpressionPtr v) : value(std::move(v)) {}
};

struct BreakStmt : Statement {};
struct ContinueStmt : Statement {};

// ===== Components and Windows =====

struct Component {
    TokenType type;
    std::string name;
    std::vector<PropertyStmt> properties;
    std::vector<std::unique_ptr<Component>> children;
    Component(TokenType t, const std::string& n) : type(t), name(n) {}
};

struct PaperDef {
    std::string name;
    std::vector<PropertyStmt> properties;
    std::vector<std::unique_ptr<Component>> children;
    PaperDef(const std::string& n) : name(n) {}
};

// ===== Scripts =====

struct EventHandler {
    std::string componentName;
    std::string eventName;
    std::vector<std::string> parameters;
    std::vector<StatementPtr> body;
    EventHandler(const std::string& comp, const std::string& event)
        : componentName(comp), eventName(event) {}
};

struct FunctionDef {
    std::string name;
    std::vector<std::pair<std::string, TokenType>> parameters;
    std::vector<StatementPtr> body;
    FunctionDef(const std::string& n) : name(n) {}
};

struct Script {
    std::string ownerName;
    std::vector<std::unique_ptr<FunctionDef>> functions;
    std::vector<std::unique_ptr<EventHandler>> eventHandlers;
    Script(const std::string& owner) : ownerName(owner) {}
};

// ===== AST Root =====

struct AST {
    std::vector<std::unique_ptr<VarDeclStmt>> globalVars;
    std::unique_ptr<PaperDef> paper;
    std::vector<std::unique_ptr<Script>> scripts;
    AST() = default;
};

} // namespace Paper5

#endif // AST_HPP