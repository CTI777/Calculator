#include "calculator.h"

using namespace std;

void log(CString head, CString s, bool end) {
	if (head.GetLength() > 0) {
		std::cout << CStringA(head).GetString();
	}

	if (s.GetLength() > 0) {
		std::cout << CStringA(s).GetString();
	}

	if (end) {
		std::cout << "\n";
	}
}


//#################################
//Calculator class
//#################################

Calculator::Calculator() {
	expr.Empty();
	vars = 0;
	n_vars = 0;
}

Calculator::~Calculator() {
	expr.Empty();
	error.Empty();

	if (vars) delete vars;
	vars = 0;
	n_vars = 0;
}

int Calculator::setExpression(CString e) {
	expr.Append(e);

	removeBadChars(expr);
	error.Empty();

	int r;
	r = checkExpression(expr, error);
	if (r) {
		return 1;
	}
	return 0;
}

CString Calculator::getExpression() {
	return expr;
}

void Calculator::setVariables(exprVar *v, int n) {
	if (vars) delete vars;
	n_vars = 0;
	if (n > 0 && v) {
		n_vars = n;
		vars = new exprVar[n_vars + 1];
		for (int i = 0; i < n_vars; i++) {
			_tcscpy(vars[i].name, v[i].name);
			vars[i].val = v[i].val;
		}
	}
}


CString Calculator::getError() {
	return error;
}


void Calculator::removeBadChars(CString &expr) {

	if (expr.GetLength() < 1) return;
	
	CString result;
	WCHAR ch;
	bool isBad;

	for (int i = 0; i < expr.GetLength(); i++) {
		ch = expr.GetAt(i);

		isBad = (ch <= 32);
		if (!isBad) result.AppendChar(ch);
	}
	expr = result;
}

int Calculator::checkExpression(CString expr, CString &error,bool removeBadCh) {
	if (removeBadCh) removeBadChars(expr);

	if (expr.GetLength() < 1) return 0;

	error = "";
	WCHAR ch;
	bool isOp = false, isBracketStart = false;
	int bracket = 0;

	for (int i = 0; i < expr.GetLength(); i++) {
		ch = expr.GetAt(i);

		if (i == 0 || isBracketStart) {
			if (ch == '+' || ch == '*' || ch == '/') {
				error = "Not allowed: operator on the first pos";
				if (isBracketStart) {
					error += " after bracket";
				}
				return 1;
			}
		}

		if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
			if (i == expr.GetLength() - 1) {
				error = "Not allowed: operator on the last pos";
				return 2;
			}

			if (isOp) {
				error = "Not allowed: operator on the wrong pos " + i;
				return 3;
			}

			isOp = true;
		}
		else
			isOp = false;

		//Check Brackets
		if (ch == '(') {
			isBracketStart = true;
			bracket++;
		}
		else
			isBracketStart = false;

		if (ch == ')') bracket--;
	}

	if (bracket != 0) {
		error = "Wrong brackets";
		return 4;
	}

	return 0;
}


int Calculator::getNextOperator(CString expr, int start, exprOperator *op) {

	if (!op) return 1;
	op->Init();

	if (expr.GetLength() < 1) return 2;

	if (start < 0) start = 0;
	WCHAR ch;
	int bracket = 0;

	for (int i = start; i < expr.GetLength(); i++) {
		ch = expr.GetAt(i);

		if (ch == '(' || ch == ')') {
			if (ch == '(') bracket++; else bracket--;
		}
		else {
			//Check operators only outside bracket
			if (bracket == 0) {
				if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
					op->Set(ch, i);
					return 0;
				}
			}
		}
	}
	return 3;
}

int Calculator::getValue(CString expr, int start, int end, float *val) {
	*val = 0;
	if (expr.GetLength() < 1) return 0;
	if (end > expr.GetLength()) return 0;

	if (start < 0) start = 0;

	if (end - start < 1) return 0;

	WCHAR *pEnd;
	CString s = expr.Mid(start, (end - start));

	//Check that this is not expression in bracket
	if (s.GetAt(0) == '(' && s.GetAt(s.GetLength() - 1) == ')') {
		return calculate(s.Mid(1, s.GetLength() - 2), val);
	}

	*val = wcstof(s.GetBuffer(s.GetLength()), &pEnd);

	if (*pEnd != '\0')
	{	// Error in parsing
		return 1;
	}
	return 0;
}

int Calculator::calculate(CString expr, float *result) {

	if (expr.GetLength() < 1) return 1;

	(*result) = 0;

	float par = 0, tPar;
	exprOperator op, nextOp, tOp;
	int res;
	bool isEnd = false;

	op.Set('+', -1);

	for (;;) {
		if (isEnd) break;
		if (op.pos >= expr.GetLength() - 1) break;

		res = getNextOperator(expr, op.pos + 1, &nextOp);
		if (res) {
			res = getValue(expr, op.pos + 1, expr.GetLength(), &par);
			if (res) return 2; //Error

			switch (op.ch) {
			case '+': (*result) += par; break;
			case '-': (*result) -= par; break;
			case '*': (*result) *= par; break;
			case '/': (*result) /= par; break;
			}
			break;
		}

		switch (op.ch) {
		case '+':;
		case '-':;
			if (nextOp.ch == '+' || nextOp.ch == '-') {
				if (nextOp.pos == 0) {
					par = 0;
				}
				else {
					res = getValue(expr, op.pos + 1, nextOp.pos, &par);
					if (res) return 2; //Error
				}

				if (op.ch == '+') (*result) += par; else  (*result) -= par;

				op.Set(nextOp.ch, nextOp.pos);
				break;
			}
			if (nextOp.ch == '*' || nextOp.ch == '/') {

				res = getValue(expr, op.pos + 1, nextOp.pos, &par);
				if (res) return 2; //Error

				//Check if next operator is + or - 
				res = getNextOperator(expr, nextOp.pos + 1, &tOp);
				if (res || (tOp.ch != '+' && tOp.ch != '-')) { // End of string, or +, - not fond
					isEnd = true;
					res = calculate(expr.Mid(nextOp.pos + 1, expr.GetLength() - nextOp.pos - 1), &tPar);
				}
				else {
					// '+' || '-') {
					res = calculate(expr.Mid(nextOp.pos + 1, tOp.pos - nextOp.pos - 1), &tPar);
				}
				if (res) return res;

				if (nextOp.ch == '*') par *= tPar;
				else {
					if (tPar == 0.) {
						error = "Devide by 0";
						return 10; //Devide by 0
					}
					par /= tPar;
				}

				if (op.ch == '+') (*result) += par; else  (*result) -= par;

				if (!isEnd) op.Set(tOp.ch, tOp.pos);
				break;
			}

			break;
		case '*':;
		case '/':;
			res = getValue(expr, op.pos + 1, nextOp.pos, &par);
			if (res) return 2; //Error

			if (op.ch == '*') (*result) *= par; else  (*result) /= par;

			op.Set(nextOp.ch, nextOp.pos);
			break;
		}
	}

	log("   calculate: ", expr, false);
	log(" => ", ToCString(*result), true);

	return 0;
}


int Calculator::calculate(float *result) {
	if (n_vars > 0 && vars != 0) {
		CString name, val;
		for (int i = 0; i < n_vars; i++) {
			name.Format(_T("%c%s"), '%',vars[i].name);
			val = ToCString(vars[i].val);
			expr.Replace(name, val);
		}
		log("   after replace: ", expr, true);
	}

	return calculate(expr, result);
}

