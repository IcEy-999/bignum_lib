# bignum_lib

C++ 大数运算库，支持超过 64bit 的 无符号整数 运算。

#### 已经实现：

加法、减法、乘法、乘方、除法、除余、左移、右移、与运算、或运算、异或运算、取反

#### class:

```c++
class bignum {
public:
	union
	{
		ULONG64 U64[Bit_Wide / 64 * 2 + 1];
		ULONG32 U32[Bit_Wide / 32 * 2 + 2];
	};
	ULONG32 U64_Len;//used ULONG64 number
	bignum();
	//set number = 0
	VOID clear();
	//print number (hexadecimal)
	VOID out();
	VOID set_len();
	//set number,Only hexadecimal is supported "0x???"
	BOOLEAN set(const char* number);
};
```



#### derived function

```c++
/*
	cmp a, b
	return 1:a > b;
	return 0:a < b;
	return 2:a = b;
*/
UCHAR bignum_cmp(bignum a, bignum b);

//a << n
bignum bignum_ls(bignum a, ULONG64 n);

//a >> n
bignum bignum_rs(bignum a, ULONG64 n);

//a + b
bignum bignum_add(bignum a, bignum b);

//a - b
bignum bignum_sub(bignum a, bignum b);

//a * b
bignum bignum_imul(bignum a, bignum b);

//a / b
bignum bignum_div(bignum a, bignum b);

//a % b
bignum bignum_mod(bignum a, bignum b);

//pow(a,b)
bignum bignum_pow(bignum a, bignum b);

//a & b
bignum bignum_and(bignum a, bignum b);

//a | b
bignum bignum_or(bignum a, bignum b);

//a ^ b
bignum bignum_xor(bignum a, bignum b);

//~a
bignum bignum_not(bignum a);
```



## demo:

###### code:

```c++
#include"bignum.h"
#pragma comment(lib,"bignum_lib.lib")

int main() {
	bignum a, b, c;
	a.set("0xfffffffffffffffffffffffffff");
	b.set("0xabcd123456789abcc");
	c = bignum_imul(a, b);
	c.out();
	printf("\n");
	system("pause");
}
```

###### out:

```c++
0xABCD123456789ABCBFFFFFFFFFF5432EDCBA98765434
```

