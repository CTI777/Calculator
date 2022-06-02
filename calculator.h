#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <atlstr.h>
#include<string>

void log(CString head, CString s, bool end);

template <typename Type>
CString ToCString(Type value)
{
	return std::to_string(value).data();
};

struct exprOperator {
	WCHAR ch;
	int pos;
	void Init() {
		ch = 0;
		pos = -1;
	}
	void Set(WCHAR nch, int npos) {
		ch = nch;
		pos = npos;
	}
};

struct exprVar {
	WCHAR name[10];
	float val;
};

class Calculator {
public:
	Calculator();
	~Calculator();

	void removeBadChars(CString &expr);
	int checkExpression(CString expr, CString &error, bool removeBadCh=false);

	int setExpression(CString e);
	CString getExpression();
	void setVariables(exprVar *v, int n);

	CString getError();

	int getNextOperator(CString expr, int start, exprOperator *op);
	int getValue(CString expr, int start, int end, float *val);

	int calculate(float *result);

private:
	int calculate(CString expr, float *result);
	CString expr, error;

	exprVar *vars;
	int n_vars;

};