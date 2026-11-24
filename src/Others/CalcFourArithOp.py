class IdxAndVal:
	i: int
	v: int

def main():
	while True:
		s = input(" \n式(空なら終了)? ") + "\n"
		if s == "\n": return
		r = IdxAndVal
		r.v = r.i = 0
		if scanExpr(s, r) and s[r.i] == "\n":
			print("計算した結果  :", r.v)
		else:
			print("入力内容が不正:", " " * r.i + "^")

def scanExpr(s, r):
	c = s[r.i]
	if c == "-": r.i += 1
	if not scanTerm(s, r): return False
	v = r.v
	if c == "-": v = - v
	while s[r.i] == "+" or s[r.i] == "-":
		op = s[r.i]
		r.i += 1
		if not scanTerm(s, r): return False
		if op == "+":
			v += r.v
		else:
			v -= r.v
	r.v = v
	return True

def scanTerm(s, r):
	if not scanFactor(s, r): return False
	v = r.v
	while s[r.i] == "*" or s[r.i] == "/":
		op = s[r.i]
		r.i += 1
		iPrev = r.i
		if not scanFactor(s, r): return False
		if op == "*":
			v *= r.v
		else:
			if r.v == 0: r.i = iPrev; return False
			v //= r.v
	r.v = v
	return True

def scanFactor(s, r):
	if s[r.i] == "(":
		r.i += 1
		if not scanExpr(s, r): return False
		if s[r.i] != ")": return False
		r.i += 1
	else:
		if not scanNumber(s, r): return False
	return True

def scanNumber(s, r):
	if not s[r.i].isdecimal(): return False
	r.v = int(s[r.i])
	r.i += 1
	while s[r.i].isdecimal():
		r.v = r.v * 10 + int(s[r.i])
		r.i += 1
	return True

main()
