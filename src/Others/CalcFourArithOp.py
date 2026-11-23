class IdxAndVal:
	i: int
	v: float

def main():
	while True:
		s = input("式(空文字列入力なら終了)? ") + "\r"
		if s == "\r": return
		r = IdxAndVal
		r.v = r.i = 0
		if scanExpr(s, r) and s[r.i] == "\r":
			print("答え:", r.v)
		else:
			print("エラー:", s[r.i:-1], "以降が不正")

def scanExpr(s, r):
	c = s[r.i]
	if c == "-": r.i += 1
	if not scanTerm(s, r): return False
	v = r.v
	if c == "-": v = - v
	while s[r.i] == "+" or s[r.i] == "-":
		op = s[r.i]
		r.i += 1
		if not scanTerm(s,r): break
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
		if not scanFactor(s,r): break
		if op == "*":
			v *= r.v
		else:
			if r.v == 0: return False
			v /= r.v
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
