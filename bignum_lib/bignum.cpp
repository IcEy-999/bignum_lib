#include "bignum.h"


bignum::bignum() {
	memset(U64, 0, Bit_Wide / 64 * 2 * 8 + 8);
	U64_Len = 1;
}

VOID bignum::clear() {
	memset(U64, 0, Bit_Wide / 64 * 2 * 8 + 8);
	U64_Len = 1;
}

VOID bignum::out() {
	printf("0x");
	for (int i = U64_Len - 1; i >= 0; i--) {
		if (i == U64_Len - 1)
			printf("%llX", U64[i]);
		else
			printf("%p", (void*)U64[i]);
	}
}

VOID bignum::set_len() {
	int i = Bit_Wide / 64 - 1;
	for (; U64[i] == 0 && i>0; i--);
	U64_Len = i + 1;
}

BOOLEAN bignum::set(const char* number) {
	
	int len = strlen(number);
	PUCHAR a = new UCHAR[len+1]();
	memcpy(a, number, len);
	//18446744073709551615
	if (len > Bit_Wide / 64 * 8 * 2 || len < 3)
		return FALSE;
	PBYTE offset = (PBYTE)&U64[0];
	if (a[0] != '0') {
		return FALSE;
	}
	else if (a[1] != 'x' && a[1] != 'X') {
		return FALSE;
	}
	for (int i = 2; i < len; i++)
	{
		if (a[i] >= 0x30 && a[i] <= 0x39)
			a[i] -= 0x30;
		else if (a[i] == 'a' || a[i] == 'A')
			a[i] = 0xa;
		else if (a[i] == 'b' || a[i] == 'B')
			a[i] = 0xb;
		else if (a[i] == 'c' || a[i] == 'C')
			a[i] = 0xc;
		else if (a[i] == 'd' || a[i] == 'D')
			a[i] = 0xd;
		else if (a[i] == 'e' || a[i] == 'E')
			a[i] = 0xe;
		else if (a[i] == 'f' || a[i] == 'F')
			a[i] = 0xf;
		else
			return 0;
	}
	for (int i = 2; i < len; i++)
	{
		if (((i - 2) % 2) == 1) {
			offset[(i - 2) / 2] += a[len - 1 - (i - 2)] * 0x10;
		}
		else {
			offset[(i - 2) / 2] += a[len - 1 - (i - 2)];
		}
	}
	U64_Len = len / 16;
	if (len % 16 > 0)
		U64_Len++;
	return TRUE;
}

UCHAR bignum_cmp(bignum a, bignum b) {
	/*
	cmp a, b
	return 1:a > b;
	return 0:a < b;
	return 2:a = b;
	*/
	if (a.U64_Len > b.U64_Len) {
		return 1;
	}
	else if (a.U64_Len < b.U64_Len) {
		return 0;
	}
	else {
		for (int i = a.U64_Len - 1; i >= 0; i--) {
			if (a.U64[i] > b.U64[i]) {
				return 1;
			}
			else if (a.U64[i] < b.U64[i]) {
				return 0;
			}

		}
	}
	return 2;//a == b

}

bignum bignum_ls1(bignum a) {
	//a >> 1
	PUCHAR s = new UCHAR[a.U64_Len];
	for (int i = a.U64_Len; i > 0; i--) {
		s[i - 1] = (UCHAR)((a.U64[i - 1] & (1ULL << 63)) >> 63);
		a.U64[i - 1] = a.U64[i - 1] << 1;
		if (s[i - 1] != 0x0) {
			a.U64[i] |= 1;
		}
	}
	if (a.U64[a.U64_Len] != 0)
		a.U64_Len++;
	return a;
}

bignum bignum_rs1(bignum a) {
	//a >> 1
	PUCHAR s = new UCHAR[a.U64_Len];
	for (int i = 0; i < a.U64_Len; i++) {
		s[i] = (UCHAR)(a.U64[i + 1] & 1);
		a.U64[i] = a.U64[i] >> 1;
		if (s[i] != 0)
			a.U64[i] |= (1ULL << 63);
	}
	if (a.U64[a.U64_Len - 1] == 0 && a.U64_Len > 1)
		a.U64_Len--;
	return a;
}

bignum bignum_div_u1(bignum a, bignum b) {
	// a/b 的工具 ， 返回 1000....0
	bignum ls;
	UCHAR flag = 0;
	if (0 == bignum_cmp(a, b)) {
		a.clear();
		return a;//出错
	}

	//ULONG64 对齐
	int x64a = 0;
	int xh = 0;
	for (xh = b.U64_Len; xh < a.U64_Len; xh++) {
		for (int j = b.U64_Len; j > 0; j--) {
			b.U64[j] = b.U64[j - 1];
		}
		b.U64[0] = 0;
		b.U64_Len++;
		x64a++;
	}


	int shift = 0, xb = 0, xb1 = 0, xb2 = 0;
	PUCHAR p = NULL;
	p = (PUCHAR)&a.U64[a.U64_Len];
	p--;//指向a.U64[a.U64_Len-1]的最高字节
	while (*p == 0x0) {
		p--;
		xb1 = xb1 + 2;
	}
	if ((*p & 0xf0) == 0) {
		xb1++;
	}

	p = (PUCHAR)&b.U64[b.U64_Len];
	p--;//指向a.U64[a.U64_Len-1]的最高字节
	while (*p == 0x0) {
		p--;
		xb2 = xb2 + 2;
	}
	if ((*p & 0xf0) == 0) {
		xb2++;
	}
	if (xb2 >= xb1) {
		xb = xb2 - xb1;
	}
	else {
		xb = xb1 - xb2;
	}

	ls.U64[x64a] = 1;
	ls.U64_Len = x64a + 1;
	flag = bignum_cmp(a, b);
	if (xb != 0) {
		if (flag == 1) {
			//a>b
			b = bignum_ls(b, (ULONG64)xb * 4);
			flag = bignum_cmp(a, b);
			if (flag == 0) {
				//完全对齐后，a<b
				ls = bignum_ls(ls, ((ULONG64)xb - 1) * 4);
			}
			else {
				//完全对齐后 a>b or a=b
				ls = bignum_ls(ls, (ULONG64)xb * 4);
			}
		}
		else {
			//b>a
			b = bignum_rs(b, (ULONG64)xb * 4);
			flag = bignum_cmp(a, b);
			if (flag == 0) {
				//完全对齐后，a<b
				ls = bignum_rs(ls, ((ULONG64)xb + 1) * 4);
			}
			else {
				//完全对齐后 a>b or a=b
				ls = bignum_rs(ls, (ULONG64)xb * 4);
			}
		}
	}
	else {
		if (flag == 0) {
			//a<b
			ls = bignum_rs(ls, 4);
		}
	}

	return ls;
}

bignum bignum_ls(bignum a, ULONG64 n) {
	//a << n
	for (ULONG64 i = 0; i < n; i++)
		a = bignum_ls1(a);
	return a;
}

bignum bignum_rs(bignum a, ULONG64 n) {
	//a >> n
	for (ULONG64 i = 0; i < n; i++)
		a = bignum_rs1(a);
	return a;
}

bignum bignum_add(bignum a, bignum b) {
	bignum ls;
	ULONG64 add = 0;
	ULONG32 Number_of_cycles = 0;
	if (a.U64_Len > b.U64_Len)
		Number_of_cycles = a.U64_Len;
	else
		Number_of_cycles = b.U64_Len;

	for (ULONG32 i = 0; i < Number_of_cycles; i++) {
		add = a.U64[i] + b.U64[i];
		if (add < a.U64[i]) {//溢出
			ls.U64[i + 1] += 1;
		}
		ls.U64[i] += add;
		if (ls.U64[i] < add) {//溢出
			ls.U64[i + 1] += 1;
		}
	}
	if (ls.U64[Number_of_cycles] != 0)
		ls.U64_Len = Number_of_cycles + 1;
	else
		ls.U64_Len = Number_of_cycles;
	return ls;
}

bignum bignum_sub(bignum a, bignum b) {
	//a>b
	bignum ls;
	ULONG64 sub = 0;
	ULONG32 Number_of_cycles = 0;
	if (0 == bignum_cmp(a, b))
		return ls;//a<b无法减

	if (a.U64_Len > b.U64_Len)
		Number_of_cycles = a.U64_Len;
	else
		Number_of_cycles = b.U64_Len;
	for (ULONG32 i = 0; i < Number_of_cycles; i++) {
		sub = a.U64[i] - b.U64[i];
		if (sub > a.U64[i]) {//溢出
			ls.U64[i + 1] -= 1;
		}
		ls.U64[i] += sub;
		ls.set_len();
	}
	return ls;
}

bignum bignum_imul(bignum a, bignum b) {
	// a * b , a在上，b在下
	ULONG32  Number_of_cycles = 0;
	if (a.U64_Len > b.U64_Len)
		Number_of_cycles = a.U64_Len;
	else
		Number_of_cycles = b.U64_Len;

	bignum* layer = new bignum[((ULONG64)Number_of_cycles + 1) * 2 * 2]();
	//memset(layer, 0, sizeof(bignum) * (Number_of_cycles + 1) * 2 * 2);

	//ULONG32 layer_site[Bit_Wide / 32 *2] = {0};
	PULONG32 layer_site = new ULONG32[(ULONG64)Number_of_cycles * 4]();

	for (int i = 0; i < Number_of_cycles * 2; i = i++) {
		layer_site[2 * i] = i + 1;
		layer_site[2 * i + 1] = i;
	}
	bignum out;
	ULONG64 mul = 0, a1 = 0, b1 = 0;
	ULONG32 c = 0, o = 0;//进位数 原位数
	for (int i = 0; i < Number_of_cycles * 2; i++) {
		//printf("a");
		for (int j = 0; j < Number_of_cycles * 2; j++) {
			a1 = a.U32[j]; b1 = b.U32[i];
			mul = b1 * a1;
			o = (ULONG32)(mul & 0xffffffff); c = (ULONG32)(mul >> 32);
			layer[2 * i].U32[layer_site[2 * i]] = c;
			layer[2 * i + 1].U32[layer_site[2 * i + 1]] = o;//here!!
			if (c != 0)
				layer[2 * i].U64_Len = layer_site[2 * i] / 2 + 1;
			if (o != 0)
				layer[2 * i + 1].U64_Len = layer_site[2 * i + 1] / 2 + 1;

			layer_site[2 * i] += 1;
			layer_site[2 * i + 1] += 1;
		}
	}
	for (int i = 0; i < Number_of_cycles * 2 * 2 + 1; i++) {
		out = bignum_add(out, layer[i]);
	}
	if (out.U64[Bit_Wide / 64] != 0)
		out.clear();
	delete[](layer);
	delete[](layer_site);
	return out;
}

bignum bignum_div(bignum dividend, bignum divisor) {
	bignum ls, quotient, remainder = dividend;
	bignum quotient_ls, quotient_num;
	int j = 0;
	while (bignum_cmp(remainder, divisor) == 1) {
		ls = bignum_div_u1(remainder, divisor);
		quotient_ls = ls;
		ls = bignum_imul(ls, divisor);
		for (j = 0; j < 16; j++) {
			remainder = bignum_sub(remainder, ls);
			if (bignum_cmp(remainder, ls) == 0) {
				j++;
				break;
			}
		}
		quotient_num.U32[0] = j;
		quotient_ls = bignum_imul(quotient_num, quotient_ls);
		quotient = bignum_add(quotient, quotient_ls);
	}
	return quotient;
}

bignum bignum_mod(bignum dividend, bignum divisor) {
	bignum ls, quotient, remainder = dividend;
	bignum quotient_ls, quotient_num;
	ULONG32 j = 0;
	while (bignum_cmp(remainder, divisor) == 1) {
		ls = bignum_div_u1(remainder, divisor);
		quotient_ls = ls;
		ls = bignum_imul(ls, divisor);
		for (j = 0; j < 16; j++) {
			remainder = bignum_sub(remainder, ls);
			if (bignum_cmp(remainder, ls) == 0) {
				j++;
				break;
			}
		}
		quotient_num.U32[0] = j;
		quotient_ls = bignum_imul(quotient_num, quotient_ls);
		quotient = bignum_add(quotient, quotient_ls);
	}
	return remainder;
}

bignum bignum_and(bignum a, bignum b) {
	//a & b
	ULONG32 Number_of_cycles = 0;
	if (a.U64_Len > b.U64_Len)
		Number_of_cycles = a.U64_Len;
	else
		Number_of_cycles = b.U64_Len;
	for (ULONG32 i = 0; i < Number_of_cycles; i++) {
		a.U64[i] &= b.U64[i];
	}
	a.set_len();
	return a;
}

bignum bignum_or(bignum a, bignum b) {
	//a | b
	ULONG32 Number_of_cycles = 0;
	if (a.U64_Len > b.U64_Len)
		Number_of_cycles = a.U64_Len;
	else
		Number_of_cycles = b.U64_Len;
	for (ULONG32 i = 0; i < Number_of_cycles; i++) {
		a.U64[i] |= b.U64[i];
	}
	a.set_len();
	return a;
}

bignum bignum_xor(bignum a, bignum b) {
	//a ^ b
	ULONG32 Number_of_cycles = 0;
	if (a.U64_Len > b.U64_Len)
		Number_of_cycles = a.U64_Len;
	else
		Number_of_cycles = b.U64_Len;
	for (ULONG32 i = 0; i < Number_of_cycles; i++) {
		a.U64[i] ^= b.U64[i];
	}
	a.set_len();
	return a;
}

bignum bignum_not(bignum a) {
	//~a
	for (ULONG32 i = 0; i < a.U64_Len; i++) {
		a.U64[i] = ~a.U64[i];
	}
	a.set_len();
	return a;
}

bignum bignum_pow(bignum a, bignum b) {
	// pow(a,b) a的b次方
	bignum ls,zero,one;
	one.U64[0] = 1;
	ls = one;
	while (bignum_cmp(b, zero) != 2) {
		ls = bignum_imul(ls, a);
		b = bignum_sub(b, one);
	}
	return ls;
}