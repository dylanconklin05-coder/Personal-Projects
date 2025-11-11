#include <queue>
#include "parserInterp.h"
#include "lex.h"
#include <set>
#include <algorithm>

map<string, bool> defVar; // checking to see if variable is initialized
map<string, ValType> varTypes; //comparing 2 types
map<string, Value> valMap;
set<string> declVar;

static bool missedOperand = false;
static bool missedExprInAssign = false;
static bool insideIfBlock = false;
static bool exprError;
string procName; //procname check

namespace Parser
{
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line)
	{
		if (pushed_back)
		{
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem& t)
	{
		if (pushed_back)
		{
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

// Prog ::= PROCEDURE ProcName IS ProcBody
bool Prog(istream& in, int& line)
{

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != PROCEDURE)
	{

		ParseError(line, "Incorrect compilation file.");

		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != IDENT)
	{

		ParseError(line, "Missing Procedure Name.");
		return false;
	}

	procName = tok.GetLexeme(); //  proc name
	declVar.insert(procName);

	tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != IS)
	{
		ParseError(line, "Missing IS keyword.");
		return false;
	}
	if (!ProcBody(in, line))
	{

		ParseError(line, "Incorrect Procedure Definition.");
		return false;
	}
	else
	{
		cout << endl;
		cout << "(DONE)" << endl;
	}
	return true;
}

//ProcBody ::= DeclPart BEGIN StmtList END ProcName ;
bool ProcBody(istream& in, int& line)
{

	if (!DeclPart(in, line))
	{
		ParseError(line, "Incorrect procedure body.");
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != BEGIN) {
		ParseError(line, "Missing BEGIN.");
		return false;
	}

	if (!StmtList(in, line))
	{
		ParseError(line, "Incorrect Proedure Body.");
		return false;
	}

	tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != END)
	{
		ParseError(line, "Missing END.");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != IDENT)
	{
		ParseError(line, "Missing Procedure Name.");
		return false;
	}

	//check if proc names are the same
	if (tok.GetLexeme() != procName)
	{
		ParseError(line, "Procedure name mismatch in closing end identifier.");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != SEMICOL)
	{
		line = line - 1;
		ParseError(line, "Missing semicolon at end of statement.");
		return false;
	}
	return true;
}

//DeclPar./parsert ::= DeclStmt { DeclStmt }
bool DeclPart(istream& in, int& line)
{
	bool status = false;
	LexItem tok;

	status = DeclStmt(in, line);
	if (status)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok == BEGIN)
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else
		{
			Parser::PushBackToken(tok);
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

// DeclStmt ::= IDENT {, IDENT } : Type [(Range)] [:= Expr] ;
bool DeclStmt(istream& in, int& line)
{
	bool isComma = true;
	LexItem tok = Parser::GetNextToken(in, line);
	vector<string> currentDecls;

	if (tok.GetToken() != IDENT)
	{
		ParseError(line, "Incorrect Declaration Statement Syntax.");
		return false;
	}

	if (defVar.find(tok.GetLexeme()) != defVar.end())
	{
		ParseError(line, "Variable Redefinition");
		ParseError(line, "Incorrect identifiers list in Declaration Statement.");
		return false;
	}

	defVar[tok.GetLexeme()] = false;
	declVar.insert(tok.GetLexeme());
	currentDecls.push_back(tok.GetLexeme());

	while (isComma)
	{
		tok = Parser::GetNextToken(in, line);

		if (tok.GetToken() == COMMA)
		{
			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() != IDENT)
			{
				ParseError(line, "Missing variable name after comma.");
				return false;
			}

			if (defVar.find(tok.GetLexeme()) != defVar.end())
			{
				ParseError(line, "Variable Redefinition");
				ParseError(line, "Incorrect identifiers list in Declaration Statement.");
				return false;
			}

			defVar[tok.GetLexeme()] = false;
			declVar.insert(tok.GetLexeme());
			currentDecls.push_back(tok.GetLexeme());
		}
		else if (tok == IDENT)
		{
			ParseError(line, "Missing comma in declaration statement.");
			ParseError(line, "Incorrect identifiers list in Declaration Statement.");
			return false;
		}
		else if (tok.GetToken() == BOOL || tok.GetToken() == INT || tok.GetToken() == FLOAT || tok.GetToken() == CHAR || tok.GetToken() == STRING)
		{
			ParseError(line, "Invalid name for an Identifier:\n(" + tok.GetLexeme() + ")");
			ParseError(line, "Incorrect identifiers list in Declaration Statement.");
			return false;
		}
		else
		{
			isComma = false;
		}
	}

	if (tok.GetToken() != COLON)
	{
		ParseError(line, "Missing colon in declaration statement.");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	Token declaredType = tok.GetToken();

	if (declaredType != INT && declaredType != FLOAT && declaredType != BOOL &&
		declaredType != STRING && declaredType != CHAR)
	{
		ParseError(line, "Incorrect Declaration Type.");
		return false;
	}

	// Map Token to ValType
	ValType valType;
	if (declaredType == INT)
	{
		valType = VINT;
	}
	else if (declaredType == BOOL)
	{
		valType = VBOOL;
	}
	else if (declaredType == CHAR)
	{
		valType = VCHAR;
	}
	else if (declaredType == FLOAT)
	{
		valType = VREAL;
	}
	else if (declaredType == STRING)
	{
		valType = VSTRING;
	}
	else
	{
		valType = VERR;
	}

	// Always register the type
	for (const auto& name : currentDecls)
	{
		varTypes[name] = valType;
	}

	// Optional Range (only allowed if not STRING)
	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == LPAREN)
	{
		if (declaredType == STRING)
		{
			ParseError(line, "Length specifier not allowed for STRING type.");
			return false;
		}

		Parser::PushBackToken(tok);
		Value retVal1, retVal2;
		if (!Range(in, line, retVal1, retVal2))
		{
			ParseError(line, "Invalid Range in declaration.");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(tok);
	}

	// Optional assignment operator 
	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ASSOP)
	{
		Value retVal;
		if (!Expr(in, line, retVal))
		{
			ParseError(line, "Invalid initialization expression.");
			return false;
		}

		// Only initialized if := used
		for (const auto& name : currentDecls)
		{
			defVar[name] = true;
			valMap[name] = retVal;
		}

	}
	else
	{
		Parser::PushBackToken(tok);
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != SEMICOL)
	{
		line = line - 1;
		ParseError(line, "Missing semicolon at end of statement.");
		return false;
	}

	return true;
}

// Type ::= INTEGER | FLOAT | BOOLEAN | STRING | CHARACTER
bool Type(istream& in, int& line) 
{

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != INT && tok.GetToken() != FLOAT && tok.GetToken() != BOOL && tok.GetToken() != STRING && tok.GetToken() != CHAR) {
		Parser::PushBackToken(tok);
		return false;
	}
	return true;
}

//StmtList ::= Stmt { Stmt }
bool StmtList(istream& in, int& line)
{
	bool status;
	LexItem tok;
	status = Stmt(in, line);

	tok = Parser::GetNextToken(in, line);
	while (status && (tok != END && tok != ELSIF && tok != ELSE))
	{
		Parser::PushBackToken(tok);
		status = Stmt(in, line);
		tok = Parser::GetNextToken(in, line);
	}
	if (!status)
	{
		ParseError(line, "Syntactic error in statement list.");
		return false;
	}
	Parser::PushBackToken(tok); //push back the END tok
	return true;
}//End of StmtList

// Stmt ::= AssignStmt | PrintStmts | GetStmt | IfStmt
bool Stmt(istream& in, int& line)
{

	LexItem tok = Parser::GetNextToken(in, line);


	if (tok.GetToken() == IDENT)
	{
		Parser::PushBackToken(tok);
		return AssignStmt(in, line);
	}

	else if (tok.GetToken() == PUT || tok.GetToken() == PUTLN)
	{
		Parser::PushBackToken(tok);

		if (!PrintStmts(in, line))
		{
			ParseError(line, "Invalid print statement.");
			return false;
		}

		return true;
	}
	else if (tok.GetToken() == GET)
	{
		Parser::PushBackToken(tok);

		return GetStmt(in, line);
	}
	else
	{
		Parser::PushBackToken(tok);
		return IfStmt(in, line);
	}
	return true;
}

// PrintStmts ::= (PutLine | Put) ( Expr) ;
bool PrintStmts(istream& in, int& line)
{
	LexItem printTok = Parser::GetNextToken(in, line);
	if (printTok.GetToken() != PUT && printTok.GetToken() != PUTLN)
	{
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != LPAREN)
	{
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	Value retVal;
	if (!Expr(in, line, retVal))
	{
		return false;
	}

	// Output final form data
	if (printTok.GetToken() == PUTLN)
	{
		cout << retVal << endl;
	}
	else
	{
		cout << retVal;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != RPAREN)
	{
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != SEMICOL)
	{
		line = line - 1;
		ParseError(line, "Missing semicolon at end of statement");
		return false;
	}

	return true;
}

// GetStmt := Get (Var) ;
bool GetStmt(istream& in, int& line)
{
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != GET)
		return false;

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != LPAREN)
	{
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	LexItem idtok;
	if (!Var(in, line, idtok))
	{
		return false;
	}

	string varName = idtok.GetLexeme();
	transform(varName.begin(), varName.end(), varName.begin(), ::tolower);

	if (!varTypes.count(varName))
	{
		ParseError(line, "Undeclared variable in Get");
		return false;
	}

	ValType vtype = varTypes[varName];

	// Read the value from input
	if (vtype == VREAL)
	{
		double fval;
		cin >> fval;
		valMap[varName] = Value(fval);
	}
	else if (vtype == VINT)
	{
		int ival;
		cin >> ival;
		valMap[varName] = Value(ival);
	}
	else if (vtype == VSTRING)
	{
		string sval;
		cin >> sval;
		valMap[varName] = Value(sval);
	}

	// Mark initialized
	defVar[varName] = true;

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != RPAREN)
	{
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != SEMICOL)
	{
		line -= 1;
		ParseError(line, "Missing semicolon at end of statement");
		return false;
	}

	return true;
}
// IfStmt ::= IF Expr THEN StmtList { ELSIF Expr THEN StmtList } [ ELSE StmtList ] END IF ;
bool IfStmt(istream& in, int& line)
{
	bool matched = false;

	if (insideIfBlock)
	{
		ParseError(line, "Nested IF statements are not allowed.");
		return false;
	}

	insideIfBlock = true;

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != IF)
	{
		insideIfBlock = false;
		return false;
	}

	Value retVal;
	if (!Expr(in, line, retVal))
	{
		insideIfBlock = false;
		ParseError(line, "Invalid If statement.");
		return false;
	}
	if (retVal.GetType() != VBOOL)
	{
		insideIfBlock = false;
		ParseError(line, "Invalid expression type for an If condition");
		ParseError(line, "Invalid If statement.");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != THEN)
	{
		insideIfBlock = false;
		ParseError(line, "Missing THEN in If Statement.");
		ParseError(line, "Invalid If statement.");
		return false;
	}

	// Only if true
	if (retVal.GetBool())
	{
		matched = true;
		if (!StmtList(in, line))
		{
			insideIfBlock = false;
			ParseError(line, "Missing Statement for If-Stmt Then-clause");
			ParseError(line, "Invalid If statement.");
			return false;
		}
	}
	else
	{
		// Parse until the next ELSE, ELSIF, or END
		while (true)
		{
			LexItem temp = Parser::GetNextToken(in, line);
			Token t = temp.GetToken();
			if (t == ELSE || t == ELSIF || t == END || t == DONE || t == ERR)
			{
				Parser::PushBackToken(temp);
				break;
			}
		}
	}

	// ELSIF chain
	tok = Parser::GetNextToken(in, line);
	while (tok.GetToken() == ELSIF)
	{
		if (!Expr(in, line, retVal))
		{
			insideIfBlock = false;
			ParseError(line, "Invalid If statement.");
			return false;
		}
		if (retVal.GetType() != VBOOL)
		{
			insideIfBlock = false;
			ParseError(line, "Invalid expression type for an Elsif condition");
			ParseError(line, "Invalid If statement.");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() != THEN)
		{
			insideIfBlock = false;
			ParseError(line, "Missing THEN after ELSIF clause.");
			ParseError(line, "Invalid If statement.");
			return false;
		}

		if (!matched && retVal.GetBool())
		{
			matched = true;
			if (!StmtList(in, line))
			{
				insideIfBlock = false;
				ParseError(line, "Missing Statement for ELSEIF clause.");
				ParseError(line, "Invalid If statement.");
				return false;
			}
		}
		else
		{
			while (true)
			{
				LexItem temp = Parser::GetNextToken(in, line);
				Token t = temp.GetToken();
				if (t == ELSE || t == ELSIF || t == END || t == DONE || t == ERR)
				{
					Parser::PushBackToken(temp);
					break;
				}
			}
		}

		tok = Parser::GetNextToken(in, line);
	}

	// ELSE
	if (tok.GetToken() == ELSE)
	{
		if (!matched)
		{
			if (!StmtList(in, line))
			{
				insideIfBlock = false;
				ParseError(line, "Missing Statement for ELSE clause.");
				ParseError(line, "Invalid If statement.");
				return false;
			}
		}
		else
		{
			while (true)
			{
				LexItem temp = Parser::GetNextToken(in, line);
				Token t = temp.GetToken();
				if (t == END || t == DONE || t == ERR)
				{
					Parser::PushBackToken(temp);
					break;
				}
			}
		}

		tok = Parser::GetNextToken(in, line);
	}

	// END IF ;
	if (tok.GetToken() != END)
	{
		insideIfBlock = false;
		ParseError(line, "Missing closing END IF for If-statement.");
		ParseError(line, "Invalid If statement.");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != IF)
	{
		insideIfBlock = false;
		ParseError(line, "Missing IF after END in If statement.");
		ParseError(line, "Invalid If statement.");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != SEMICOL)
	{
		insideIfBlock = false;
		ParseError(line, "Missing semicolon after END IF");
		return false;
	}

	insideIfBlock = false;
	return true;
}

// AssignStmt ::= Var := Expr ;
bool AssignStmt(istream& in, int& line)
{
	missedExprInAssign = false;
	exprError = false;

	LexItem idtok;
	if (!Var(in, line, idtok))
	{
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != ASSOP)
	{
		ParseError(line, "Missing Assignment Operator");
		ParseError(line, "Invalid assignment statement.");
		return false;
	}

	Value retVal;
	if (!Expr(in, line, retVal))
	{
		if (missedOperand && !missedExprInAssign)
		{
			ParseError(line, "Missing operand after operator");
			ParseError(line, "Missing Expression in Assignment Statement");
		}

		ParseError(line, "Invalid assignment statement.");
		return false;
	}


	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != SEMICOL)
	{
		line = line - 1;
		ParseError(line, "Missing semicolon  at end of statement");
		return false;
	}

	//do the varType after completeing the entire line check because if you do it in the middle it wont read the entire line before throwwing an error
	const auto& varName = idtok.GetLexeme();
	if (!varTypes.count(varName))
	{
		return true;
	}

	ValType expected = varTypes[varName];//lookup for declared type of variable
	ValType actual = retVal.GetType();// actual type of expression

	if (expected != actual)
	{
		ParseError(line, "Illegal Expression type for the assigned variable");
		ParseError(line, "Invalid assignment statement.");
		return false;
	}

	defVar[varName] = true;
	valMap[varName] = retVal;
	return true;
}

bool Var(istream& in, int& line, LexItem& idtok)
{
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != IDENT && defVar.find(tok.GetLexeme()) != defVar.end())
	{
		return false;
	}
	idtok = tok; //look at
	return true;
}

bool Expr(istream& in, int& line, Value& retVal)
{
	missedOperand = false;
	if (!Relation(in, line, retVal))
	{
		missedOperand = true;
		return false;
	}

	LexItem tok;
	while (true)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == AND || tok.GetToken() == OR)
		{
			if (!Relation(in, line, retVal))
			{
				missedOperand = true;
				return false;
			}
		}
		else if (tok == IDENT || tok == ICONST || tok == FCONST || tok == SCONST || tok == BCONST || tok == CCONST || tok == LPAREN)
		{
			missedExprInAssign = true;
			ParseError(line, "Illegal expression for an assignment statement");
			return false;
		}
		else
		{
			//if tok is not AND or OR u push back the tok 
			Parser::PushBackToken(tok);
			break;
		}
	}
	return true;
}

//Relation ::= SimpleExpr [ ( = | /= | < | <= | > | >= )  SimpleExpr ]
bool Relation(istream& in, int& line, Value& retVal)
{
	// Parse the first expression
	if (!SimpleExpr(in, line, retVal))
	{
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == EQ || tok.GetToken() == NEQ ||
		tok.GetToken() == LTHAN || tok.GetToken() == LTE ||
		tok.GetToken() == GTHAN || tok.GetToken() == GTE)
	{
		Value rhs;
		if (!SimpleExpr(in, line, rhs))
		{
			ParseError(line, "Incorrect Operand");
			return false;
		}

		ValType left = retVal.GetType();
		ValType right = rhs.GetType();


		if (left != right)
		{
			ParseError(line, "Illegal operand type for comparison.");
			return false;
		}

		switch (left)
		{
		case VINT: // compare 2 ints
		{
			int l = retVal.GetInt();
			int r = rhs.GetInt();
			if (tok.GetToken() == EQ)      retVal = Value(l == r);
			else if (tok.GetToken() == NEQ) retVal = Value(l != r);
			else if (tok.GetToken() == LTHAN) retVal = Value(l < r);
			else if (tok.GetToken() == LTE)   retVal = Value(l <= r);
			else if (tok.GetToken() == GTHAN) retVal = Value(l > r);
			else if (tok.GetToken() == GTE)   retVal = Value(l >= r);
			break;
		}
		case VREAL: //compare 2 floats
		{
			double l = retVal.GetReal();
			double r = rhs.GetReal();
			if (tok.GetToken() == EQ)      retVal = Value(l == r);
			else if (tok.GetToken() == NEQ) retVal = Value(l != r);
			else if (tok.GetToken() == LTHAN) retVal = Value(l < r);
			else if (tok.GetToken() == LTE)   retVal = Value(l <= r);
			else if (tok.GetToken() == GTHAN) retVal = Value(l > r);
			else if (tok.GetToken() == GTE)   retVal = Value(l >= r);
			break;
		}
		case VSTRING: // compare strings
		{
			string l = retVal.GetString();
			string r = rhs.GetString();
			if (tok.GetToken() == EQ)      retVal = Value(l == r);
			else if (tok.GetToken() == NEQ) retVal = Value(l != r);
			else
			{
				ParseError(line, "Invalid comparison operator for strings.");
				return false;
			}
			break;
		}
		default:
			ParseError(line, "Unsupported operand types in comparison.");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(tok);  // No relational operator
	}

	return true;
}

//SimpleExpr ::= STerm { ( + | - | & ) STerm }
bool SimpleExpr(istream& in, int& line, Value& retVal)
{
	LexItem tok;

	// Get the first STerm (left operand)
	if (!STerm(in, line, retVal))
	{
		return false;
	}

	while (true)
	{
		tok = Parser::GetNextToken(in, line);

		if (tok.GetToken() == PLUS || tok.GetToken() == MINUS || tok.GetToken() == CONCAT)
		{
			Value rhs;
			if (!STerm(in, line, rhs))
			{
				return false;
			}

			ValType leftType = retVal.GetType();
			ValType rightType = rhs.GetType();

			// Disallow mixed operations
			if (leftType != rightType)
			{
				ParseError(line, "Illegal operand type for the operation.");
				ParseError(line, "Missing Expression in Assignment Statement");
				exprError = true;
				return false;
			}

			// valid operations
			if (tok.GetToken() == PLUS)
			{
				if (leftType == VINT)
					retVal = Value(retVal.GetInt() + rhs.GetInt());
				else if (leftType == VREAL)
					retVal = Value(retVal.GetReal() + rhs.GetReal());
				else
				{
					ParseError(line, "Illegal operand type for addition.");
					return false;
				}
			}
			else if (tok.GetToken() == MINUS)
			{
				if (leftType == VINT)
					retVal = Value(retVal.GetInt() - rhs.GetInt());
				else if (leftType == VREAL)
					retVal = Value(retVal.GetReal() - rhs.GetReal());
				else
				{
					ParseError(line, "Illegal operand type for subtraction.");
					return false;
				}
			}
			else if (tok.GetToken() == CONCAT)
			{
				if (leftType == VSTRING)
					retVal = Value(retVal.GetString() + rhs.GetString());
				else
				{
					ParseError(line, "Illegal operand type for concatenation.");
					return false;
				}
			}
		}
		else
		{
			Parser::PushBackToken(tok);
			break;
		}
	}

	return true;
}

//STerm ::= [ ( + | - ) ] Term
bool STerm(istream& in, int& line, Value& retVal)
{
	LexItem tok = Parser::GetNextToken(in, line);
	int sign = 1;
	bool signUsed = false;

	if (tok.GetToken() == MINUS)
	{
		signUsed = true;
		sign = -1;
	}
	else if (tok.GetToken() == PLUS)
	{
		signUsed = true;
		sign = 1;
	}
	else
	{
		Parser::PushBackToken(tok);
	}

	if (!Term(in, line, sign, retVal))
	{
		return false;
	}

	if ((signUsed == true) && (retVal.GetType() != VINT && retVal.GetType() != VBOOL && retVal.GetType() != VREAL)) // only legal types are VBool and Vint
	{
		ParseError(line, "Illegal Operand type for sign operator");
		return false;
	}
	return true;
}

//Term ::= Factor { ( * | / | MOD ) Factor }
bool Term(istream& in, int& line, int sign, Value& retVal)
{
	if (!Factor(in, line, sign, retVal))
	{
		return false;
	}

	LexItem tok;
	while (true)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == MULT || tok.GetToken() == DIV || tok.GetToken() == MOD)
		{
			Value rhs; // store right hand side value while keeping retval for the left hand side
			if (!Factor(in, line, sign, rhs))
			{
				return false;
			}

			ValType leftType = retVal.GetType();
			ValType rightType = rhs.GetType();

			if ((leftType != VINT && leftType != VREAL) || (rightType != VINT && rightType != VREAL)) // comparing left and right hand side for correct variable types
			{
				ParseError(line, "Illegal operand type for the operation.");
				ParseError(line, "Missing Expression in Assignment Statement");
				exprError = true;
				return false;
			}

			// This is where val.cpp would come into play but it is not needed you can do the math in this file
			if (tok.GetToken() == MULT) // must do the auctual math so the error can pass if its not valid
			{
				if (leftType == VREAL || rightType == VREAL)
				{
					retVal = Value(retVal.GetReal() * rhs.GetReal());
				}
				else
				{
					retVal = Value(retVal.GetInt() * rhs.GetInt());
				}
			}
			else if (tok.GetToken() == DIV)
			{
				if (rhs.GetReal() == 0.0)
				{
					ParseError(line, "Cant divide by 0");
					return false;
				}

				if (leftType == VREAL || rightType == VREAL)
				{
					retVal = Value(retVal.GetReal() / rhs.GetReal());
				}
				else
				{
					retVal = Value(retVal.GetInt() / rhs.GetInt());
				}
			}
			else if (tok.GetToken() == MOD)
			{
				if (leftType != VINT || rightType != VINT || rhs.GetInt() == 0)
				{
					ParseError(line, "Illegal or zero divisor in MOD operation.");
					return false;
				}
				retVal = Value(retVal.GetInt() % rhs.GetInt());

			}

		}
		else
		{
			Parser::PushBackToken(tok);
			break;
		}
	}
	return true;
}


//Factor ::= Primary [** [(+ | -)] Primary ] | NOT Primary
bool Factor(istream& in, int& line, int sign, Value& retVal)
{
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == NOT)
	{
		if (!Factor(in, line, sign, retVal)) //check factor agin because it could be a double not or a primary 
		{
			ParseError(line, "Invalid NOT operand.");
			return false;
		}

		if (retVal.GetType() != VBOOL)
		{
			ParseError(line, "NOT operator requires a boolean operand.");
			return false;
		}

		retVal = Value(!retVal.GetBool());
		return true;
	}

	Parser::PushBackToken(tok);
	if (!Primary(in, line, sign, retVal))
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == EXP)
	{
		// Optional + or - sign before exponent
		int expSign = 1;
		LexItem signTok = Parser::GetNextToken(in, line);
		if (signTok.GetToken() == PLUS)
		{
			expSign = 1;
		}
		else if (signTok.GetToken() == MINUS)
		{
			expSign = -1;
		}
		else
		{
			Parser::PushBackToken(signTok);
		}


		Value exponent;
		if (!Primary(in, line, expSign, exponent))
		{
			ParseError(line, "Invalid exponent expression.");
			return false;
		}
		ValType baseType = retVal.GetType();
		ValType expType = exponent.GetType();

		if ((baseType != VINT && baseType != VREAL) ||
			(expType != VINT && expType != VREAL))
		{
			ParseError(line, "Invalid operand types for exponentiation.");
			return false;
		}


		double base = (baseType == VREAL) ? retVal.GetReal() : retVal.GetInt();
		double expo = (expType == VREAL) ? exponent.GetReal() : exponent.GetInt();
		retVal = Value(pow(base, expo));  // always returns float (VREAL)

		return true;
	}
	else
	{
		Parser::PushBackToken(tok);
	}

	return true;
}

// Primary ::= Name | ICONST | FCONST | SCONST | BCONST | CCONST | (Expr)
bool Primary(istream& in, int& line, int sign, Value& retVal)
{
	LexItem tok = Parser::GetNextToken(in, line);
	// keywords not valid
	if (tok.GetToken() == IF || tok.GetToken() == THEN || tok.GetToken() == ELSE || tok.GetToken() == ELSIF || tok.GetToken() == END)
	{
		Parser::PushBackToken(tok);
		ParseError(line, "Invalid Expression");
		ParseError(line, "Incorrect operand");
		ParseError(line, "Missing if statement condition.");
		ParseError(line, "Invalid If statement");
		return false;
	}

	if (tok.GetToken() == ICONST)
	{
		retVal = Value(stoi(tok.GetLexeme()) * sign);
		return true;
	}
	else if (tok.GetToken() == FCONST)
	{
		retVal = Value(stod(tok.GetLexeme()) * sign);
		return true;
	}
	else if (tok.GetToken() == SCONST)
	{
		retVal = Value(tok.GetLexeme());
		return true;
	}
	else if (tok.GetToken() == BCONST)
	{
		string val = tok.GetLexeme();
		retVal = Value(val == "true" || val == "TRUE");
		return true;
	}
	else if (tok.GetToken() == CCONST)
	{
		retVal = Value(tok.GetLexeme()[0]);
		return true;
	}

	if (tok == IDENT)
	{
		Parser::PushBackToken(tok); //so name can do the entire parse
		if (!Name(in, line, sign, retVal)) //simplex and complex varable names
		{
			ParseError(line, "Incorrect operand");
			return false;
		}
		return true;
	}

	if (tok.GetToken() == LPAREN) // check optional Expr
	{
		if (!Expr(in, line, retVal))
		{
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() != RPAREN)
		{
			ParseError(line, "Missing Right Parenthesis");
			return false;
		}
		return true;
	}
	return false;
}

//Name ::= IDENT [ ( Range ) ]
bool Name(istream& in, int& line, int sign, Value& retVal)
{
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != IDENT)
	{
		ParseError(line, "Missing variable name");
		return false;
	}

	string varName = tok.GetLexeme();
	transform(varName.begin(), varName.end(), varName.begin(), ::tolower); // make names not case sensitive (Y_1, y_1)

	// declared
	if (defVar.find(varName) == defVar.end())
	{
		ParseError(line, "Using Undefined Variable");
		ParseError(line, "Invalid reference to a variable.");
		return false;
	}

	// initialized
	if (!defVar[varName])
	{
		ParseError(line, "Invalid use of an unintialized variable.");
		return false;
	}

	ValType vt = varTypes[varName];

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == LPAREN)
	{
		Value idx1, idx2;
		bool isRange = false;

		if (!SimpleExpr(in, line, idx1))
		{
			ParseError(line, "Invalid expression inside parentheses.");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == DOT)
		{
			// Handle Range like 3..6
			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() != DOT)
			{
				ParseError(line, "Missing second '.' in range.");
				return false;
			}

			if (!SimpleExpr(in, line, idx2))
			{
				ParseError(line, "Invalid upper bound in range.");
				return false;
			}
			isRange = true;
			tok = Parser::GetNextToken(in, line);
		}
		else
		{
			Parser::PushBackToken(tok);
			tok = Parser::GetNextToken(in, line);
		}

		if (tok.GetToken() != RPAREN)
		{
			ParseError(line, "Missing closing parenthesis.");
			return false;
		}

		// string indexing or substringing
		if (varTypes[varName] != VSTRING)
		{
			ParseError(line, "Indexing or range operation on non-string variable.");
			return false;
		}

		string full = valMap[varName].GetString();

		if (!isRange)
		{
			int pos = idx1.GetInt();
			if (pos < 1 || pos > full.length())
			{
				ParseError(line, "Index out of bounds.");
				return false;
			}
			retVal = Value(full[pos]);
		}
		else
		{
			int start = idx1.GetInt();
			int end = idx2.GetInt();
			if (start < 1 || end > full.length() || start > end)
			{
				ParseError(line, "Invalid substring range.");
				return false;
			}
			retVal = Value(full.substr(start, end - start + 1));  //substring
		}
	}
	else
	{
		// No indexing or range, use value directly
		Parser::PushBackToken(tok);
		switch (vt)
		{
		case VINT:
		{
			retVal = Value(valMap[varName].GetInt() * sign); break;
		}
		case VREAL:
		{
			retVal = Value(valMap[varName].GetReal() * sign); break;
		}
		case VBOOL:
		{
			retVal = valMap[varName]; break;
		}
		case VCHAR:
		{
			retVal = valMap[varName]; break;
		}
		case VSTRING:
		{
			retVal = valMap[varName]; break;
		}
		default:
		{
			retVal = Value(); break;
		}
		
	}

	return true;
}
bool Range(istream& in, int& line, Value& retVal1, Value& retVal2)
{
	if (!SimpleExpr(in, line, retVal1))
	{
		ParseError(line, "Invalid lower bound of Range.");
		return false;

	}

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == DOT)
	{

		tok = Parser::GetNextToken(in, line);

		if (tok.GetToken() != DOT)
		{
			ParseError(line, "Missing second '.' in Range.");
			return false;
		}
		if (!SimpleExpr(in, line, retVal2))
		{
			ParseError(line, "Invalid upper bound of Range.");
			return false;
		}
		return true;

	}
	else // might have to check this
	{

		Parser::PushBackToken(tok);
		return true;
	}
}