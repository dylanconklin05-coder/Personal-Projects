
#include <queue>
#include "parser.h"
#include "lex.h"
#include <set>
#include "val.h"

map<string, bool> defVar;

set<string> declVar;

static bool missedOperand = false;
static bool missedExprInAssign = false;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if (pushed_back) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem& t) {
		if (pushed_back) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

//ProcName ::= IDENT
// bool ProcName(istream& in, int& line){
// 	LexItem token =Parser::GetNextToken(in, line);
// 	if(token.GetToken() != IDENT){
// 		ParseError(line,"Invalid Procedure Name.");
// 		return false;
// 	}
//     return true;
// }

// Prog ::= PROCEDURE ProcName IS ProcBody
bool Prog(istream& in, int& line) {

	LexItem token = Parser::GetNextToken(in, line);

	if (token.GetToken() != PROCEDURE) {

		ParseError(line, "Incorrect compilation file.");

		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != IDENT) {

		ParseError(line, "Missing Procedure Name.");
		return false;
	}

	string procName = token.GetLexeme();
	declVar.insert(procName);

	token = Parser::GetNextToken(in, line);

	if (token.GetToken() != IS) {
		ParseError(line, "Missing IS keyword.");
		return false;
	}
	if (!ProcBody(in, line)) {

		ParseError(line, "Incorrect Procedure Definition.");
		return false;
	}
	else {
		cout << "Declared Variables:" << endl;

		for (auto it = declVar.begin(); it != declVar.end(); ++it) {
			cout << *it;
			if (next(it) != declVar.end())
				cout << ", ";
		}
		cout << "\n" << endl;
		cout << "(DONE)" << endl;
	}
	return true;
}


//ProcBody ::= DeclPart BEGIN StmtList END ProcName ;
bool ProcBody(istream& in, int& line) {
	//DeclPart
	if (!DeclPart(in, line)) {
		ParseError(line, "Incorrect procedure body.");
		return false;
	}

	//BEGIN
	LexItem token = Parser::GetNextToken(in, line);
	if (token.GetToken() != BEGIN) {
		ParseError(line, "Missing BEGIN.");
		return false;
	}

	//StmtList
	if (!StmtList(in, line)) {
		ParseError(line, "Incorrect Proedure Body.");
		return false;
	}

	token = Parser::GetNextToken(in, line);

	if (token.GetToken() != END) {
		ParseError(line, "Missing END.");
		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != IDENT) {
		ParseError(line, "Missing Procedure Name.");
		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != SEMICOL) {
		line = line - 1;
		ParseError(line, "Missing semicolon at end of statement.");
		return false;
	}
	return true;
}

//DeclPar./parsert ::= DeclStmt { DeclStmt }
bool DeclPart(istream& in, int& line) {
	bool status = false;
	LexItem token;
	//cout << "in DeclPart" << endl;

	status = DeclStmt(in, line);
	if (status)
	{
		token = Parser::GetNextToken(in, line);
		if (token == BEGIN)
		{
			Parser::PushBackToken(token);
			return true;
		}
		else
		{
			Parser::PushBackToken(token);
			status = DeclPart(in, line);
			return status;
		}
	}
	else
	{

		ParseError(line, "Non-recognizable Declaration Part.");
		return false;
	}
	return true;
}//end of DeclPart function


//DeclStmt ::= IDENT {, IDENT } : [CONSTANT] Type [(Range)] [:= Expr] ;
bool DeclStmt(istream& in, int& line) {

	bool isComma = true;
	LexItem token = Parser::GetNextToken(in, line);

	if (token.GetToken() != IDENT) {
		ParseError(line, "Incorrect Declaration Statement Syntax.");
		return false;
	}
	if (defVar.find(token.GetLexeme()) != defVar.end()) {
		ParseError(line, "Variable Redefinition");
		ParseError(line, "Incorrect identifiers list in Declaration Statement.");
		return false;
	}
	defVar[token.GetLexeme()] = false;
	declVar.insert(token.GetLexeme());

	while (isComma) {
		token = Parser::GetNextToken(in, line);
		//Comma is present 
		if (token.GetToken() == COMMA) {
			token = Parser::GetNextToken(in, line);

			if (token.GetToken() != IDENT) {
				ParseError(line, "Missing variable name after comma.");
				return false;
			}

			if (defVar.find(token.GetLexeme()) != defVar.end()) {
				ParseError(line, "Variable Redefinition");
				ParseError(line, "Incorrect identifiers list in Declaration Statement.");
				return false;
			}

			defVar[token.GetLexeme()] = false;
			declVar.insert(token.GetLexeme());

		}

		else if (token == IDENT) {
			ParseError(line, "Missing comma in declaration statement.");
			ParseError(line, "Incorrect identifiers list in Declaration Statement.");
			return false;
		}

		else if (token.GetToken() == BOOL || token.GetToken() == INT || token.GetToken() == FLOAT || token.GetToken() == CHAR || token.GetToken() == STRING) {
			ParseError(line, "Invalid name for an Identifier:\n(" + token.GetLexeme() + ")");
			ParseError(line, "Incorrect identifiers list in Declaration Statement.");
			return false;
		}
		else {
			isComma = false;
		}
	}

	//Move on

	if (token.GetToken() != COLON) {
		ParseError(line, "Missing colon in declaration statement.");
		return false;
	}


	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != CONST) {

		Parser::PushBackToken(token);

	}

	if (!Type(in, line)) {
		ParseError(line, "Incorrect Declaration Type.");
		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() == LPAREN) {

		Parser::PushBackToken(token);
		//if it is LPAREN and nothing is wrong with it push back

		if (!Range(in, line)) {
			ParseError(line, "Invalid Range in declaration.");
			return false;
		}
		// token = Parser::GetNextToken(in, line);
		// if(token !=RPAREN){
		// 	ParseError(line, "Missing Right Parenthesis");
		// 	return false;
		// }
	}
	else {
		Parser::PushBackToken(token);
	}
	//if it is LPAREN and nothing is wrong with it



	token = Parser::GetNextToken(in, line);
	if (token.GetToken() == ASSOP) {
		if (!Expr(in, line)) {
			ParseError(line, "Invalid initialization expression.");
			return false;
		}
		for (auto& entry : defVar) {
			entry.second = true;
		}

	}
	else {
		Parser::PushBackToken(token);
	}
	//if nothing is wrong and it is sctually just ASSOP
	token = Parser::GetNextToken(in, line);

	if (token.GetToken() != SEMICOL) {
		//since it went to next line while getting token
		line = line - 1;
		ParseError(line, "Missing semicolon at end of statement.");
		return false;
	}
	return true;

}

// Type ::= INTEGER | FLOAT | BOOLEAN | STRING | CHARACTER
bool Type(istream& in, int& line) {

	LexItem token = Parser::GetNextToken(in, line);

	if (token.GetToken() != INT && token.GetToken() != FLOAT && token.GetToken() != BOOL && token.GetToken() != STRING && token.GetToken() != CHAR) {
		Parser::PushBackToken(token);
		return false;
	}
	return true;
}

//StmtList ::= Stmt { Stmt }
bool StmtList(istream& in, int& line)
{
	bool status;
	LexItem token;
	//cout << "in StmtList" << endl;
	status = Stmt(in, line);

	token = Parser::GetNextToken(in, line);
	while (status && (token != END && token != ELSIF && token != ELSE))
	{
		Parser::PushBackToken(token);
		status = Stmt(in, line);
		token = Parser::GetNextToken(in, line);
	}
	if (!status)
	{
		ParseError(line, "Syntactic error in statement list.");
		return false;
	}
	Parser::PushBackToken(token); //push back the END token
	return true;
}//End of StmtList


// Stmt ::= AssignStmt | PrintStmts | GetStmt | IfStmt
bool Stmt(istream& in, int& line) {

	//You want to read token and process it by calling respective function
	LexItem token = Parser::GetNextToken(in, line);


	if (token.GetToken() == IDENT) {

		//You want to get token and check for each case
		Parser::PushBackToken(token);
		return AssignStmt(in, line);
	}

	else if (token.GetToken() == PUT || token.GetToken() == PUTLN) {
		//You want to get token and check for each case
		Parser::PushBackToken(token);

		//THIS IS A TEST CASE
		if (!PrintStmts(in, line)) {
			ParseError(line, "Invalid print statement.");
			return false;
		}

		return true;
	}
	else if (token.GetToken() == GET) {
		//You want to get token and check for each case
		Parser::PushBackToken(token);

		return GetStmt(in, line);
	}
	else {
		//You want to get token and check for each case
		Parser::PushBackToken(token);
		return IfStmt(in, line);
	}
	return true;
}


// PrintStmts ::= (PutLine | Put) ( Expr) ;
bool PrintStmts(istream& in, int& line) {
	LexItem token = Parser::GetNextToken(in, line);
	if (token.GetToken() != PUT && token.GetToken() != PUTLN) {

		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != LPAREN) {
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	if (!Expr(in, line)) {
		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != RPAREN) {
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != SEMICOL) {
		line = line - 1;
		ParseError(line, "Missing semicolon at end of statement");
		return false;
	}
	return true;
}


// GetStmt := Get (Var) ;
bool GetStmt(istream& in, int& line) {

	LexItem token = Parser::GetNextToken(in, line);
	if (token.GetToken() != GET) {

		return false;
	}


	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != LPAREN) {
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	if (!Var(in, line)) {

		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != RPAREN) {
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}


	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != SEMICOL) {
		line = line - 1;
		ParseError(line, "Missing semicolon at end of statement");
		return false;
	}
	return true;
}

// IfStmt ::= IF Expr THEN StmtList { ELSIF Expr THEN StmtList } [ ELSE StmtList ] END IF ;
bool IfStmt(istream& in, int& line) {


	LexItem token = Parser::GetNextToken(in, line);
	if (token.GetToken() != IF) {
		return false;
	}

	int prevErrors = ErrCount();
	if (!Expr(in, line)) {
		if (ErrCount() == prevErrors) {
			ParseError(line, "Invalid Expression");
			ParseError(line, "Incorrect operand");
			ParseError(line, "Missing if statement condition");
		}
		ParseError(line, "Invalid If statement.");
		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != THEN) {
		ParseError(line, "Missing THEN in If Statement.");
		ParseError(line, "Invalid If statement.");
		return false;
	}

	if (!StmtList(in, line)) {
		ParseError(line, "Missing Statement for If-Stmt Then-clause");
		ParseError(line, "Invalid If statement.");
		return false;
	}

	token = Parser::GetNextToken(in, line);
	while (token.GetToken() == ELSIF) {
		if (!Expr(in, line)) {
			if (ErrCount() == prevErrors) {
				ParseError(line, "Invalid Expression");
				ParseError(line, "Incorrect operand");
				ParseError(line, "Missing if statement condition");
			}
			ParseError(line, "Invalid If statement.");
			return false;
		}

		token = Parser::GetNextToken(in, line);
		if (token.GetToken() != THEN) {
			ParseError(line, "Missing THEN after ELSIF clause.");
			ParseError(line, "Invalid If statement.");
			return false;
		}

		if (!StmtList(in, line)) {
			ParseError(line, "Missing Statement for ELSEIF clause.");
			ParseError(line, "Invalid If statement.");
			return false;
		}
		token = Parser::GetNextToken(in, line);
	}

	if (token.GetToken() == ELSE) {
		if (!StmtList(in, line)) {
			ParseError(line, "Missing Statement for ELSE clause.");
			ParseError(line, "Invalid If statement.");
			return false;
		}
		token = Parser::GetNextToken(in, line);

	}
	if (token.GetToken() != END) {
		ParseError(line, "Missing closing END IF for If-statement.");
		ParseError(line, "Invalid If statement.");
		return false;
	}

	token = Parser::GetNextToken(in, line);

	if (token.GetToken() != IF) {
		ParseError(line, "Missing IF after END in If statement.");
		ParseError(line, "Invalid If statement.");
		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != SEMICOL) {

		ParseError(line, "Missing semicolon after END IF");
		return false;
	}
	return true;
}

// AssignStmt ::= Var := Expr ;
bool AssignStmt(istream& in, int& line) {
	if (!Var(in, line)) {

		return false;
	}

	LexItem token = Parser::GetNextToken(in, line);
	if (token.GetToken() != ASSOP) {

		ParseError(line, "Missing Assignment Operator");
		ParseError(line, "Invalid assignment statement.");
		return false;
	}

	if (!Expr(in, line)) {
		if (missedOperand && !missedExprInAssign) {
			ParseError(line, "Missing operand after operator");
			ParseError(line, "Missing Expression in Assignment Statement");
		}

		ParseError(line, "Invalid assignment statement.");
		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() != SEMICOL) {
		line = line - 1;
		ParseError(line, "Missing semicolon  at end of statement");
		return false;
	}
	return true;
}

// Var ::= IDENT
bool Var(istream& in, int& line) {
	LexItem token = Parser::GetNextToken(in, line);
	if (token.GetToken() != IDENT && defVar.find(token.GetLexeme()) != defVar.end()) {

		return false;
	}
	return true;
}

// Expr ::= Relation {(AND | OR) Relation }
bool Expr(istream& in, int& line) {

	missedOperand = false;
	if (!Relation(in, line)) {
		missedOperand = true;
		return false;
	}

	LexItem token;

	while (true) {
		token = Parser::GetNextToken(in, line);
		if (token.GetToken() == AND || token.GetToken() == OR) {
			//While there is a relation
			if (!Relation(in, line)) {

				missedOperand = true;
				return false;
			}
		}


		else if (token == IDENT || token == ICONST || token == FCONST || token == SCONST || token == BCONST || token == CCONST || token == LPAREN) {
			missedExprInAssign = true;
			ParseError(line, "Illegal expression for an assignment statement");
			return false;
		}
		else {
			//if token is not AND or OR u push back the token 
			Parser::PushBackToken(token);
			break;
		}
	}
	return true;
}

// Relation ::= SimpleExpr [ ( = | /= | < | <= | > | >= )  SimpleExpr ]
bool Relation(istream& in, int& line) {

	if (!SimpleExpr(in, line)) {
		return false;
	}

	LexItem token = Parser::GetNextToken(in, line);

	//if token is =, /=, <, <=, >, >= u continue to check next token
	if (token.GetToken() == EQ || token.GetToken() == NEQ || token.GetToken() == LTHAN || token.GetToken() == LTE || token.GetToken() == GTHAN || token.GetToken() == GTE) {

		if (!SimpleExpr(in, line)) {
			ParseError(line, "Incorrect operand");
			return false;
		}

		LexItem nexttoken = Parser::GetNextToken(in, line);
		if (nexttoken.GetToken() == EQ || nexttoken.GetToken() == NEQ || nexttoken.GetToken() == LTHAN || nexttoken.GetToken() == LTE || nexttoken.GetToken() == GTHAN || nexttoken.GetToken() == GTE) {
			ParseError(line, "Invalid Expression");
			ParseError(line, "Incorrect operand.");
			ParseError(line, "Missing if statement condition.");
			return false;
		}
		else 
		{
			Parser::PushBackToken(nexttoken);
		}
	}
	else 
	{

		Parser::PushBackToken(token);
	}
	return true;
}



// SimpleExpr ::= STerm { ( + | - | & ) STerm }
bool SimpleExpr(istream& in, int& line) {

	if (!STerm(in, line)) {

		return false;
	}

	LexItem token;
	while (true) {
		token = Parser::GetNextToken(in, line);

		if (token.GetToken() == PLUS || token.GetToken() == MINUS || token.GetToken() == CONCAT) {
			if (!STerm(in, line)) {
				return false;
			}
		}
		else {
			Parser::PushBackToken(token);
			break;
		}

	}
	return true;
}
// STerm ::= [ ( + | - ) ] Term
bool STerm(istream& in, int& line) {
	LexItem token = Parser::GetNextToken(in, line);
	int sign = 1;

	if (token.GetToken() == PLUS) {
		sign = 1;
	}
	else if (token.GetToken() == MINUS) {
		sign = -1;
	}
	else {
		Parser::PushBackToken(token);
	}

	if (!Term(in, line, sign)) {

		return false;
	}
	return true;
}

// Term ::= Factor { ( * | / | MOD ) Factor }
bool Term(istream& in, int& line, int sign) {

	if (!Factor(in, line, sign)) {

		return false;
	}

	LexItem token;
	while (true) {
		token = Parser::GetNextToken(in, line);

		if (token.GetToken() == MULT || token.GetToken() == DIV || token.GetToken() == MOD) {
			if (!Factor(in, line, sign)) {
				return false;
			}
		}
		else {
			Parser::PushBackToken(token);
			break;
		}
	}
	return true;
}
// Factor ::= Primary [** [(+ | -)] Primary ] | NOT Primary
bool Factor(istream& in, int& line, int sign)
{

	LexItem token = Parser::GetNextToken(in, line);
	if (token == NOT) 
	{
		return Primary(in, line, sign);
	}

	Parser::PushBackToken(token);
	if (!Primary(in, line, sign)) 
	{

		return false;
	}
	token = Parser::GetNextToken(in, line);
	//first check for the case without the optional cases

	if (token.GetToken() == EXP) 
	{
		int optionalSign = 0;
		token = Parser::GetNextToken(in, line);
		if (token.GetToken() == PLUS)
		{
			optionalSign = 1;
		}
		else if (token.GetToken() == MINUS) 
		{
			optionalSign = -1;
		}
		else 
		{

			Parser::PushBackToken(token);

			if (!Primary(in, line, optionalSign))
			{
				return false;
			}
		}

	}
	else {
		Parser::PushBackToken(token);
	}

	return true;

}

// Primary ::= Name | ICONST | FCONST | SCONST | BCONST | CCONST | (Expr)
bool Primary(istream& in, int& line, int sign) {

	LexItem token = Parser::GetNextToken(in, line);
	if (token.GetToken() == IF || token.GetToken() == THEN || token.GetToken() == ELSE || token.GetToken() == ELSIF || token.GetToken() == END) {
		Parser::PushBackToken(token);
		ParseError(line, "Invalid Expression");
		ParseError(line, "Incorrect operand");
		ParseError(line, "Missing if statement condition.");
		ParseError(line, "Invalid If statement");
		return false;
	}

	if (token.GetToken() == ICONST || token.GetToken() == FCONST || token.GetToken() == SCONST || token.GetToken() == BCONST || token.GetToken() == CCONST) {
		return true;
	}

	if (token == IDENT) {
		Parser::PushBackToken(token);

		if (!Name(in, line)) {
			ParseError(line, "Incorrect operand");
			return false;
		}
		return true;
	}
	//if token is not any of these, check for (Expr)
	if (token.GetToken() == LPAREN) {
		if (!Expr(in, line)) {

			return false;
		}
		token = Parser::GetNextToken(in, line);
		if (token != RPAREN) {
			ParseError(line, "Missing Right Parenthesis");
			return false;
		}
		return true;
	}

	return false;
}
// Name ::= IDENT [ ( Range ) ]
bool Name(istream& in, int& line) {
	LexItem token = Parser::GetNextToken(in, line);
	if (token.GetToken() != IDENT) {
		ParseError(line, "Missing variable name.");
		return false;
	}
	if (defVar.find(token.GetLexeme()) == defVar.end()) {
		ParseError(line, "Using Undefined Variable");
		ParseError(line, "Invalid reference to a variable.");
		return false;
	}

	token = Parser::GetNextToken(in, line);
	if (token.GetToken() == LPAREN) {
		if (!Range(in, line)) {
			return false;
		}
		token = Parser::GetNextToken(in, line);
		if (token.GetToken() != RPAREN) {
			ParseError(line, "Missing Right Parenthesis after Range.");
			return false;
		}
		return true;
	}
	else {
		Parser::PushBackToken(token);
	}
	return true;
}

// Range ::= SimpleExpr [. . SimpleExpr ]
bool Range(istream& in, int& line) {

	if (!SimpleExpr(in, line)) {
		ParseError(line, "Invalid lower bound of Range.");
		return false;

	}

	LexItem token = Parser::GetNextToken(in, line);

	if (token.GetToken() == DOT) {

		token = Parser::GetNextToken(in, line);

		if (token.GetToken() != DOT) {
			ParseError(line, "Missing second '.' in Range.");
			return false;
		}
		if (!SimpleExpr(in, line)) {
			ParseError(line, "Invalid upper bound of Range.");
			return false;
		}
		return true;

	}
	else {

		Parser::PushBackToken(token);
		return true;
	}
}
