// ExprParser.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
#include "calculator.h"

int main()
{
	CString expr("-10 + %a * (15 + 22 / 11 /%c + 6 -%b )/12+11");
	exprVar *vars = new exprVar[3];
		_tcscpy(vars[0].name,_T("a")) ;
		vars[0].val = 10;

		_tcscpy(vars[1].name, _T("b"));
		vars[1].val = 20;

		_tcscpy(vars[2].name, _T("c"));
		vars[2].val = 0.5;
		

	Calculator calc;
	int r;
	float result;

	log("Start: ", expr, true);

	r = calc.setExpression(expr);
	if (r) { //Error in the Expression
		log("setExpression: ", calc.getExpression(), false);
		log(" => ", ToCString(r), false);
		
		if (calc.getError().GetLength() > 0) {
			log(" => error: ", calc.getError(), false);
		}
		log("", "", true);

		if (vars) delete vars;
		return 1;
	}

	calc.setVariables(vars,3);

	if (vars) delete vars;

	r = calc.calculate(&result);
	
	log("calculate: ", calc.getExpression(), false);
	log(" => ", ToCString(r), false);
	
	if (r==0) {
		log(" => ", ToCString(result), true);
	}
	else {
		if (calc.getError().GetLength() > 0) {
			log(" => error: ", calc.getError(), false);
		}
		log("", "", true);
	}
}
