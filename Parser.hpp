#ifndef PARSER_HPP
#define PARSER_HPP

#include "Token.hpp"
#include "AST.hpp"
#include <vector>
#include <memory>
#include <stdexcept>

namespace Paper5 {

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    Token peek() {
        if (current >= tokens.size()) return Token(TokenType::END_OF_FILE, "", 0, 0);
        return tokens[current];
    }
    
    Token previous() {
        if (current == 0) return Token(TokenType::END_OF_FILE, "", 0, 0);
        return tokens[current - 1];
    }
    
    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }
    
    bool isEventKeyword(TokenType type) {
	    return (type >= TokenType::KEYWORD_ON_CLICK && type <= TokenType::KEYWORD_ON_DROP);
	}
    
    bool isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
    
    bool check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }
    
    bool match(TokenType type) {
        if (check(type)) { advance(); return true; }
        return false;
    }
    
    Token consume(TokenType type, const std::string& errorMsg) {
	    if (check(type)) return advance();
	    throw std::runtime_error(errorMsg + " at line " + std::to_string(peek().line));
	}

    bool isPropertyToken(TokenType type) {
        return (type >= TokenType::PROP_PARENT && type <= TokenType::PROP_REPEAT);
    }

    // ===== Expression Parsing =====
    
    ExpressionPtr parseExpression() { return parseAssignment(); }
    
    ExpressionPtr parseAssignment() {
        ExpressionPtr expr = parseLogicalOr();
        if (match(TokenType::OPERATOR_ASSIGN)) {
            ExpressionPtr right = parseAssignment();
            if (dynamic_cast<IdentifierExpr*>(expr.get()) ||
                dynamic_cast<PropertyAccessExpr*>(expr.get()) ||
                dynamic_cast<ArrayAccessExpr*>(expr.get()))
                return std::make_unique<BinaryExpr>(std::move(expr), TokenType::OPERATOR_ASSIGN, std::move(right));
            throw std::runtime_error("Invalid assignment target at line " + std::to_string(peek().line));
        }
        return expr;
    }
    
    ExpressionPtr parseLogicalOr() {
        ExpressionPtr expr = parseLogicalAnd();
        while (match(TokenType::OPERATOR_OR)) {
            TokenType op = previous().type;
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, parseLogicalAnd());
        }
        return expr;
    }
    
    ExpressionPtr parseLogicalAnd() {
        ExpressionPtr expr = parseEquality();
        while (match(TokenType::OPERATOR_AND)) {
            TokenType op = previous().type;
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, parseEquality());
        }
        return expr;
    }
    
    ExpressionPtr parseEquality() {
        ExpressionPtr expr = parseComparison();
        while (match(TokenType::OPERATOR_EQ) || match(TokenType::OPERATOR_NEQ)) {
            TokenType op = previous().type;
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, parseComparison());
        }
        return expr;
    }
    
    ExpressionPtr parseComparison() {
        ExpressionPtr expr = parseTerm();
        while (match(TokenType::OPERATOR_LT) || match(TokenType::OPERATOR_GT) ||
               match(TokenType::OPERATOR_LTE) || match(TokenType::OPERATOR_GTE)) {
            TokenType op = previous().type;
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, parseTerm());
        }
        return expr;
    }
    
    ExpressionPtr parseTerm() {
        ExpressionPtr expr = parseFactor();
        while (match(TokenType::OPERATOR_PLUS) || match(TokenType::OPERATOR_MINUS)) {
            TokenType op = previous().type;
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, parseFactor());
        }
        return expr;
    }
    
    ExpressionPtr parseFactor() {
        ExpressionPtr expr = parseUnary();
        while (match(TokenType::OPERATOR_MUL) || match(TokenType::OPERATOR_DIV) || match(TokenType::OPERATOR_MOD)) {
            TokenType op = previous().type;
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, parseUnary());
        }
        return expr;
    }
    
    ExpressionPtr parseUnary() {
        if (match(TokenType::OPERATOR_NOT) || match(TokenType::OPERATOR_MINUS)) {
            TokenType op = previous().type;
            return std::make_unique<UnaryExpr>(op, parseUnary());
        }
        return parsePostfix();
    }
    
    ExpressionPtr parsePostfix() {
        ExpressionPtr expr = parsePrimary();
        while (true) {
            if (match(TokenType::SEP_DOT)) {
                std::string propName;
                if (isPropertyToken(peek().type)) {
                    propName = peek().lexeme;
                    advance();
                } else {
                    Token t = consume(TokenType::IDENTIFIER, "Expected property name");
                    propName = t.lexeme;
                }
                std::string objName = dynamic_cast<IdentifierExpr*>(expr.get()) ? dynamic_cast<IdentifierExpr*>(expr.get())->name : "";
                expr = std::make_unique<PropertyAccessExpr>(objName, propName);
            } else if (match(TokenType::SEP_LBRACKET)) {
                ExpressionPtr index = parseExpression();
                consume(TokenType::SEP_RBRACKET, "Expected ']'");
                expr = std::make_unique<ArrayAccessExpr>(std::move(expr), std::move(index));
            } else if (match(TokenType::SEP_LPAREN)) {
                std::string funcName = dynamic_cast<IdentifierExpr*>(expr.get()) ? dynamic_cast<IdentifierExpr*>(expr.get())->name : "";
                auto call = std::make_unique<CallExpr>(funcName);
                if (!check(TokenType::SEP_RPAREN)) {
                    do { call->arguments.push_back(parseExpression()); } while (match(TokenType::SEP_COMMA));
                }
                consume(TokenType::SEP_RPAREN, "Expected ')'");
                expr = std::move(call);
            } else break;
        }
        return expr;
    }
    
    ExpressionPtr parsePrimary() {
        if (match(TokenType::LIT_INTEGER)) return std::make_unique<LiteralExpr>(previous().type, previous().intValue);
        if (match(TokenType::LIT_FLOAT)) return std::make_unique<LiteralExpr>(previous().type, previous().floatValue);
        if (match(TokenType::LIT_STRING)) return std::make_unique<LiteralExpr>(previous().type, previous().stringValue);
        if (match(TokenType::LIT_BOOL)) return std::make_unique<LiteralExpr>(previous().type, previous().boolValue);
        if (match(TokenType::LIT_COLOR)) return std::make_unique<LiteralExpr>(previous().type, previous().stringValue);
        if (match(TokenType::IDENTIFIER) || isComponentKeyword(peek().type) || isPropertyToken(peek().type)
		    || isEventKeyword(peek().type) || isTypeKeyword(peek().type)) {
		    if (check(TokenType::IDENTIFIER) || isComponentKeyword(peek().type) || isPropertyToken(peek().type)) {
		        advance();
		    } else {
		        advance();
		    }
		    return std::make_unique<IdentifierExpr>(previous().lexeme);
		}
        if (match(TokenType::SEP_LPAREN)) {
            ExpressionPtr expr = parseExpression();
            consume(TokenType::SEP_RPAREN, "Expected ')'");
            return expr;
        }
        if (match(TokenType::SEP_LBRACKET)) {
            auto arr = std::make_unique<ArrayExpr>();
            if (!check(TokenType::SEP_RBRACKET)) {
                do { arr->elements.push_back(parseExpression()); } while (match(TokenType::SEP_COMMA));
            }
            consume(TokenType::SEP_RBRACKET, "Expected ']'");
            return arr;
        }
        throw std::runtime_error("Expected expression at line " + std::to_string(peek().line));
    }
    
    // ===== Statement Parsing =====
    
    std::unique_ptr<VarDeclStmt> parseVarDecl() {
        TokenType varType = peek().type;
        consume(varType, "Expected type keyword");
        Token nameToken = consume(TokenType::IDENTIFIER, "Expected variable name");
        consume(TokenType::OPERATOR_ASSIGN, "Expected '='");
        ExpressionPtr init = parseExpression();
        consume(TokenType::SEP_SEMICOLON, "Expected ';'");
        return std::make_unique<VarDeclStmt>(varType, nameToken.lexeme, std::move(init));
    }
    
    PropertyStmt parseProperty() {
        // Name is already a PROP_xxx token
        Token nameToken = advance();
        consume(TokenType::OPERATOR_ASSIGN, "Expected '='");
        ExpressionPtr value = parseExpression();
        match(TokenType::SEP_SEMICOLON); // semicolon may be optional in some contexts
        return PropertyStmt(nameToken.lexeme, std::move(value));
    }
    
    std::unique_ptr<Component> parseComponent() {
        TokenType compType = peek().type;
        advance();
        
        Token nameToken;
		if (check(TokenType::IDENTIFIER) || isComponentKeyword(peek().type) || isPropertyToken(peek().type)) {
		    nameToken = advance();
		} else {
		    nameToken = consume(TokenType::IDENTIFIER, "Expected component name");
		}
        
        auto component = std::make_unique<Component>(compType, nameToken.lexeme);
        // Separator is self-closing
		if (compType == TokenType::KEYWORD_SEPARATOR) {
		    match(TokenType::SEP_SEMICOLON);
		    return component;
		}
        consume(TokenType::SEP_LBRACE, "Expected '{'");
        while (!check(TokenType::SEP_RBRACE) && !isAtEnd()) {
            if (isPropertyToken(peek().type)) {
                component->properties.push_back(parseProperty());
            } else if (isComponentKeyword(peek().type)) {
                component->children.push_back(parseComponent());
            } else {
                advance();
            }
        }
        consume(TokenType::SEP_RBRACE, "Expected '}'");
        return component;
    }
    
    std::unique_ptr<PaperDef> parsePaper() {
        consume(TokenType::KEYWORD_PAPER, "Expected '<paper>'");
        Token nameToken = consume(TokenType::IDENTIFIER, "Expected paper name");
        auto paper = std::make_unique<PaperDef>(nameToken.lexeme);
        consume(TokenType::SEP_LBRACE, "Expected '{'");
        while (!check(TokenType::SEP_RBRACE) && !isAtEnd()) {
            if (isPropertyToken(peek().type)) {
                paper->properties.push_back(parseProperty());
            } else if (isComponentKeyword(peek().type)) {
                paper->children.push_back(parseComponent());
            } else {
                advance();
            }
        }
        consume(TokenType::SEP_RBRACE, "Expected '}'");
        return paper;
    }
    
    std::vector<StatementPtr> parseBlock() {
        std::vector<StatementPtr> statements;
        consume(TokenType::SEP_LBRACE, "Expected '{'");
        while (!check(TokenType::SEP_RBRACE) && !isAtEnd()) {
            statements.push_back(parseStatement());
        }
        consume(TokenType::SEP_RBRACE, "Expected '}'");
        return statements;
    }
    
    StatementPtr parseStatement() {
        if (isTypeKeyword(peek().type)) return parseVarDecl();
        
        // Assignment statement: identifiery = expression ;
		if (check(TokenType::IDENTIFIER)) {
		    size_t saved = current;
		    advance();
		    if (match(TokenType::OPERATOR_ASSIGN)) {
		        ExpressionPtr right = parseExpression();
		        Token nameTok = tokens[saved];
		        auto left = std::make_unique<IdentifierExpr>(nameTok.lexeme);
		        auto assign = std::make_unique<BinaryExpr>(std::move(left), TokenType::OPERATOR_ASSIGN, std::move(right));
		        consume(TokenType::SEP_SEMICOLON, "Expected ';'");
		        return std::make_unique<ExprStmt>(std::move(assign));
		    }
		    current = saved;
		}
        
        if (match(TokenType::KEYWORD_IF)) {
            consume(TokenType::SEP_LPAREN, "Expected '('");
            auto ifStmt = std::make_unique<IfStmt>(parseExpression());
            consume(TokenType::SEP_RPAREN, "Expected ')'");
            ifStmt->thenBranch = parseBlock();
            if (match(TokenType::KEYWORD_ELSE)) {
                if (match(TokenType::KEYWORD_IF)) {
                    consume(TokenType::SEP_LPAREN, "Expected '('");
                    auto elseIf = std::make_unique<IfStmt>(parseExpression());
                    consume(TokenType::SEP_RPAREN, "Expected ')'");
                    elseIf->thenBranch = parseBlock();
                    ifStmt->elseBranch.push_back(std::move(elseIf));
                } else {
                    ifStmt->elseBranch = parseBlock();
                }
            }
            return ifStmt;
        }
        if (match(TokenType::KEYWORD_WHILE)) {
            consume(TokenType::SEP_LPAREN, "Expected '('");
            auto whileStmt = std::make_unique<WhileStmt>(parseExpression());
            consume(TokenType::SEP_RPAREN, "Expected ')'");
            whileStmt->body = parseBlock();
            return whileStmt;
        }
        if (match(TokenType::KEYWORD_FOR)) {
            consume(TokenType::SEP_LPAREN, "Expected '('");
            auto forStmt = std::make_unique<ForStmt>();
            if (!check(TokenType::SEP_SEMICOLON)) {
                if (isTypeKeyword(peek().type)) forStmt->init = parseVarDecl();
                else { forStmt->init = std::make_unique<ExprStmt>(parseExpression()); consume(TokenType::SEP_SEMICOLON, "Expected ';'"); }
            } else consume(TokenType::SEP_SEMICOLON, "Expected ';'");
            if (!check(TokenType::SEP_SEMICOLON)) forStmt->condition = parseExpression();
            consume(TokenType::SEP_SEMICOLON, "Expected ';'");
            if (!check(TokenType::SEP_RPAREN)) forStmt->step = std::make_unique<ExprStmt>(parseExpression());
            consume(TokenType::SEP_RPAREN, "Expected ')'");
            forStmt->body = parseBlock();
            return forStmt;
        }
        if (match(TokenType::KEYWORD_FOREACH)) {
            consume(TokenType::SEP_LPAREN, "Expected '('");
            Token varToken = consume(TokenType::IDENTIFIER, "Expected variable name");
            consume(TokenType::KEYWORD_IN, "Expected 'in'");
            ExpressionPtr arr = parseExpression();
            consume(TokenType::SEP_RPAREN, "Expected ')'");
            auto foreachStmt = std::make_unique<ForeachStmt>(varToken.lexeme, std::move(arr));
            foreachStmt->body = parseBlock();
            return foreachStmt;
        }
        if (match(TokenType::KEYWORD_RETURN)) {
            if (check(TokenType::SEP_SEMICOLON)) { consume(TokenType::SEP_SEMICOLON, "Expected ';'"); return std::make_unique<ReturnStmt>(); }
            auto retStmt = std::make_unique<ReturnStmt>(parseExpression());
            consume(TokenType::SEP_SEMICOLON, "Expected ';'");
            return retStmt;
        }
        if (match(TokenType::KEYWORD_BREAK)) { consume(TokenType::SEP_SEMICOLON, "Expected ';'"); return std::make_unique<BreakStmt>(); }
        if (match(TokenType::KEYWORD_CONTINUE)) { consume(TokenType::SEP_SEMICOLON, "Expected ';'"); return std::make_unique<ContinueStmt>(); }
        ExpressionPtr expr = parseExpression();
		consume(TokenType::SEP_SEMICOLON, "Expected ';'");
		return std::make_unique<ExprStmt>(std::move(expr));
        
    }
    
    // ===== Script Parsing =====
    
    std::unique_ptr<Script> parseScript() {
        Token nameToken = consume(TokenType::IDENTIFIER, "Expected component name");
        consume(TokenType::SEP_DOT, "Expected '.'");
        consume(TokenType::KEYWORD_SCRIPT, "Expected 'script'");
        auto script = std::make_unique<Script>(nameToken.lexeme);
        consume(TokenType::SEP_LBRACE, "Expected '{'");
        while (!check(TokenType::SEP_RBRACE) && !isAtEnd()) {
            if (check(TokenType::KEYWORD_CODE)) {
                advance();
                Token funcToken = consume(TokenType::IDENTIFIER, "Expected function name");
                auto func = std::make_unique<FunctionDef>(funcToken.lexeme);
                if (match(TokenType::SEP_LPAREN)) {
                    if (!check(TokenType::SEP_RPAREN)) {
                        do {
                            Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
                            TokenType paramType = TokenType::KEYWORD_VALUE;
                            if (match(TokenType::SEP_COLON) && isTypeKeyword(peek().type)) {
                                paramType = peek().type; advance();
                            }
                            func->parameters.push_back({paramName.lexeme, paramType});
                        } while (match(TokenType::SEP_COMMA));
                    }
                    consume(TokenType::SEP_RPAREN, "Expected ')'");
                }
                func->body = parseBlock();
                script->functions.push_back(std::move(func));
            }
            else if (check(TokenType::IDENTIFIER) || isComponentKeyword(peek().type) || isPropertyToken(peek().type)) {
    			Token compToken = advance();
                if (match(TokenType::SEP_DOT)) {
                    Token eventToken;
					if (isEventKeyword(peek().type)) {
					    eventToken = advance();
					} else {
					    eventToken = consume(TokenType::IDENTIFIER, "Expected event name");
					}
                    consume(TokenType::SEP_LPAREN, "Expected '('");
                    auto handler = std::make_unique<EventHandler>(compToken.lexeme, eventToken.lexeme);
                    if (!check(TokenType::SEP_RPAREN)) {
                        do { 
							Token paramToken;
							if (check(TokenType::IDENTIFIER) || isPropertyToken(peek().type) || isComponentKeyword(peek().type)) {
							    paramToken = advance();
							} else {
							    paramToken = consume(TokenType::IDENTIFIER, "Expected parameter");
							}
							handler->parameters.push_back(paramToken.lexeme);
						 } while (match(TokenType::SEP_COMMA));
                    }
                    consume(TokenType::SEP_RPAREN, "Expected ')'");
                    handler->body = parseBlock();
                    script->eventHandlers.push_back(std::move(handler));
                }
            }
            else { advance(); }
        }
        consume(TokenType::SEP_RBRACE, "Expected '}'");
        return script;
    }
    
public:
    Parser(const std::vector<Token>& tokenList) : tokens(tokenList), current(0) {}
    
    std::unique_ptr<AST> parse() {
        auto ast = std::make_unique<AST>();
        while (!isAtEnd()) {
            if (check(TokenType::KEYWORD_PAPER)) ast->paper = parsePaper();
            else if (isTypeKeyword(peek().type)) ast->globalVars.push_back(parseVarDecl());
            else if (check(TokenType::IDENTIFIER)) {
                size_t saved = current;
                advance();
                if (check(TokenType::SEP_DOT)) { current = saved; ast->scripts.push_back(parseScript()); }
                else current = saved + 1;
            }
            else advance();
        }
        return ast;
    }
};

} // namespace Paper5

#endif // PARSER_HPP