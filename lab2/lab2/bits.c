/* 
 * CS:APP Data Lab 
 * 
 * <김인성, cs20200136>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  int temp1 = x & y;
  int temp2 = (~x) & (~y);
  return (~temp1) & (~temp2);
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
  int leftover = ((0xFF << (m << 3)) | (0xFF << (n << 3))) ^ (~0);
  int nx = (x >> (n << 3)) & 0xFF;
  int mx = (x >> (m << 3)) & 0xFF;
  return (leftover & x) | (nx << (m << 3)) | (mx << (n << 3)); 
}
/* 
 * rotateLeft - Rotate x to the left by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateLeft(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateLeft(int x, int n) {
  // x<<n | ??? 임은 쉽게 알 수 있고, ???에 -는 사용 불가
  // ??? = x>>(32-n) 하면 날라간 부분 가능 -> signed int이기에 ~이용
  // signed int들은 무언가가 더 필요 -> 뒤에 부분 + & ~(-1 << n) -> 0...01...1
  return (x << n) | ((x >> (32 + (~n + 1))) & ~((~0) << n));
}
/*
 * leftBitCount - returns count of number of consective 1's in
 *     left-hand (most significant) end of word.
 *   Examples: leftBitCount(-1) = 32, leftBitCount(0xFFF0F0F0) = 12
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Rating: 4
 */
int leftBitCount(int x) {
  //왼쪽부터 연속한 1의 개수 구하기
  //max op를 넘기지 않기 위해 왼쪽에서부터 32, 16, 8, ... bit 순서로 체크하기
  int check = x;
  int lbc; //우리가 리턴할 값
  int lbc2;
  //왼쪽에서부터 32 bit 체크
  int constant = !(~check);
  //왼쪽에서부터 16 bit 체크
  lbc = !(~(check >> 16)) << 4; //만약에 앞자리가 다 0이면 16자리 건너뜀
  check = check << lbc;
  //왼쪽에서부터 8 bit 체크 -> lbc 개수 줄이기가 중요
  lbc2 = !(~(check >> 24)) << 3; // 16 + 8
  check = check << lbc2;
  lbc = lbc | lbc2;
  // 4 bit
  lbc2 = !(~(check >> 28)) << 2; // 16 + 8 + 4
  check = check << lbc2;
  lbc = lbc | lbc2;
  // 2 bit
  lbc2 = !(~(check >> 30)) << 1; // 16 + 8 + 4 + 2
  check = check << lbc2;
  lbc = lbc | lbc2;
  // 1 bit 체크
  lbc = lbc ^ (0x01 & (check >> 31));
  return lbc + constant;
}
/* 
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) {
  int sign = x >> 31; // x가 양수면 0000...0, 음수면 1111...1임 생각
  return sign ^ (sign + x);
}
/* 
 * TMax - return maximum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmax(void) {
  return ((~0) >> 31) ^ (1 << 31); //111...111 ^ 011...111 생각
}
/* 
 * fitsShort - return 1 if x can be represented as a 
 *   16-bit, two's complement integer.
 *   Examples: fitsShort(33000) = 0, fitsShort(-32768) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int fitsShort(int x) {
  int cost = (x << 16) >> 16; // << 이후 >> 이후 같은가? 확인
  return !(x ^ cost);
}
/* 
 * rempwr2 - Compute x%(2^n), for 0 <= n <= 30
 *   Negative arguments should yield negative remainders
 *   Examples: rempwr2(15,2) = 3, rempwr2(-35,3) = -3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int rempwr2(int x, int n) {
  //2^n으로 나누어준다는 뜻 -> 양수라면 뒤의 n자리를 제외하면 의미 없음
  //음수라면 나머지도 음수 -> 원래 계산에서 2^n 빼주면 됨
  //먼저 0...01...1 의 형태를 만들어주기 (연속된 n개의 1)
  int cost = ~(~0 << n);
  //음수일 경우엔 2^n을 빼줘야하는데, 음수임을 체크하고, 나누어지는지 체크
  //하려면 0x01 혹은 0x00형태의 결과값을 left shift, right shift 시켜서 같은지 체크
  //x >> 0x1F 생각 -> x가 양수면 0, 음수면 1
  int ifnegative = (((!!(x & cost) << 0x1F) >> 0x1F) & (x >> 0x1F)) & ((~0) << n);
  return (x & cost) + ifnegative; //ifnegative는 음수일때만 적용
}
/* 
 * sign - return 1 if positive, 0 if zero, and -1 if negative
 *  Examples: sign(130) = 1
 *            sign(-23) = -1
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 10
 *  Rating: 2
 */
int sign(int x) {
  //!(x>>31) 하면 음수일 때 0 나옴 -> 이거만 고치면 됨 -> !!x이용
  return ((x >> 31) | (!!x));
}
/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
  return !(x >> 31);
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
  //case1 : x와 y가 부호가 같으면 크기 비교만 하면 되고
  int case1 = ((x + ~y) >> 31) & (!((x >> 31) ^ (y >> 31)));
  //case2 : x와 y가 부호가 다르면 하나만 리버스해서 계산
  int case2 = (x >> 31) & (!(y >> 31));
  return !(case1 | case2);
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  //x >= 2^n 인 n의 최댓값을 찾고 + 1 하기 (MSB 생각)
  int case1, case2, case3, case4, case5;
  //x가 음수라면 양수로 바꾸고 시작해야 함.
  x = (~(x >> 31) & x) | ((x >> 31) & ~x);
  //여러 가지 방법이 있겠지만 가장 쉬운 건 이진 탐색이라고 생각
  case1 = !!(x >> (1 << 4)) << 4;
  x = x >> case1;
  case2 = !!(x >> (1 << 3)) << 3;
  x = x >> case2;
  case3 = !!(x >> (1 << 2)) << 2;
  x = x >> case3;
  case4 = !!(x >> (1 << 1)) << 1;
  x = x >> case4;
  case5 = !!(x >> (1));
  x = x >> case5;
  return 1 + case1 + case2 + case3 + case4 + case5 + x;
}
/* 
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_abs(unsigned uf) {
  //32 bit에서 float는 sign 1bit, exp 8bit, frac 23bit 메모 M*2^E
  //먼저 sign은 0이고 exp, frac 은 모두 1인수 생각 0x7FFFFFFF -> float 양수중가장큰수
  unsigned abs = uf & 0x7FFFFFFF;
  //exp = 111...1이고 frac != 000...0 이면 Not-a-Number이므로 이를 고려해야함
  if(abs > 0x7F800000) return uf;
  return abs;
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
  int f2i;
  unsigned sign = uf >> 31;
  unsigned exp = 0xFF & (uf >> 23);
  unsigned frac = uf & 0x7FFFFF;
  int E = exp - 127; // Bias : 127, normalize라면 E = exp - Bias임
  //exp = 111...1이라면 infinity 이거나 Not-a-Number 이거나 overflow일때
  if(exp == 0x7F800000 || E >= 32) return 0x80000000u;
  //E가 -1~1이면 각각 -0, +0으로 0 리턴
  if(E < 0 || exp == 0) return 0;
  if(E >= 23) f2i = (frac | 0x800000) << (E - 23);
  else f2i = (frac | 0x800000) >> (23 - E);
  if(sign == 1) return -f2i;
  else return f2i;
}
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
  unsigned sign = uf >> 31;
  unsigned exp = 0xFF & (uf >> 23);
  unsigned frac = uf & 0x7FFFFF;
  int constant = (frac & 0x03) == 0x03;
  //argument = NaN -> return argument : exp, frac = 0 or exp = 111...1
  if(exp == 0xFF || (exp == 0 && frac == 0)) return uf;
  //frac 마지막 두 자리 생각
  // 00 -> 0 일때 >> 1
  // 01 -> 0 일 때 round to even 적용하여 >> 1
  // 10 -> 1 일때 >> 1
  // 11 -> 1 일 때가 문제! round to even 적용하여 >> 1 하고 + 1까지 해야함
  if(exp == 0) { //when denormalized
    frac = (frac >> 1) + constant;
  } else if(exp == 1) {
    //normalized에서 denormalized로
    exp--;
    frac = (((uf & 0x7FFFFFFF) >> 1) + constant) & 0x7FFFFF;
  } else { //when normalized
    exp--;
  }
  return (sign << 31) | (exp << 23) | frac;
}
