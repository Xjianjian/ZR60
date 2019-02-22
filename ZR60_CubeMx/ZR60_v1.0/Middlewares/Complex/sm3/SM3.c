/******************************************************
�ļ�����	SM3.c

������		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./sm3/SM3.h"
#include "Include.h"

// 循环左移
unsigned int ROTL(unsigned int x, int n) {
    unsigned int x1 = (x & 0xFFFFFFFF) << n;
    unsigned int x2 = (x >> (32 - n));
    return x1 | x2;
}

// 初始�
 unsigned char IV[32] = {
        0x73, 0x80, 0x16, 0x6f,
        0x49, 0x14, 0xb2, 0xb9,
        0x17, 0x24, 0x42, 0xd7,
        0xda, 0x8a, 0x06, 0x00,
        0xa9, 0x6f, 0x30, 0xbc,
        0x16, 0x31, 0x38, 0xaa,
        0xe3, 0x8d, 0xee, 0x4d,
        0xb0, 0xfb, 0x0e, 0x4e
};

// 常量
unsigned int T(int j) {
    return j < 16 ? 0x79cc4519 : 0x7a879d8a;
}

// 布尔函数
unsigned int FF(unsigned int x, unsigned int y, unsigned int z, int j) {
    return j < 16 ? (x ^ y ^ z) : ((x & y) | (x & z) | (y & z));
}

unsigned int GG(unsigned int x, unsigned int y, unsigned int z, int j) {
    return j < 16 ? (x ^ y ^ z) : ((x & y) | (~x & z));
}

// 置换函数
unsigned int P0(unsigned int x) {
    return x ^ ROTL(x, 9) ^ ROTL(x, 17);
}

unsigned int P1(unsigned int x) {
    return x ^ ROTL(x, 15) ^ ROTL(x, 23);
}

// 压缩函数
void CF(unsigned char *vi, unsigned char *bi, unsigned char *output_v) {
    int w[68];
    int _w[64];
    int j;
		    int A, B, C, D, E, F, G, H;
	    int SS1, SS2, TT1, TT2;
    int i;
	
    // a)
    for ( j = 0; j <= 15; j++) {
        w[j] = bi[j * 4 + 0] << 24 | bi[j * 4 + 1] << 16 | bi[j * 4 + 2] << 8 | bi[j * 4 + 3];
    }
    // b)
    for ( j = 16; j <= 67; j++) {
        w[j] = P1(w[j - 16] ^ w[j - 9] ^ ROTL(w[j - 3], 15)) ^ ROTL(w[j - 13], 7) ^ w[j - 6];
    }
    // c)
    for ( j = 0; j <= 63; j++) {
        _w[j] = w[j] ^ w[j + 4];
    }


    A = vi[0] << 24 | vi[1] << 16 | vi[2] << 8 | vi[3];
    B = vi[4] << 24 | vi[5] << 16 | vi[6] << 8 | vi[7];
    C = vi[8] << 24 | vi[9] << 16 | vi[10] << 8 | vi[11];
    D = vi[12] << 24 | vi[13] << 16 | vi[14] << 8 | vi[15];
    E = vi[16] << 24 | vi[17] << 16 | vi[18] << 8 | vi[19];
    F = vi[20] << 24 | vi[21] << 16 | vi[22] << 8 | vi[23];
    G = vi[24] << 24 | vi[25] << 16 | vi[26] << 8 | vi[27];
    H = vi[28] << 24 | vi[29] << 16 | vi[30] << 8 | vi[31];


    for ( j = 0; j <= 63; j++) 
	{
        SS1 = ROTL((ROTL(A, 12) + E + ROTL(T(j), j)), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = FF(A, B, C, j) + D + SS2 + _w[j];
        TT2 = GG(E, F, G, j) + H + SS1 + w[j];
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = P0(TT2);
    }
    unsigned char ABCDEFGH[32] = {
            (A >> 24) & 0xff, (A >> 16) & 0xff, (A >> 8) & 0xff, A & 0xff,
            (B >> 24) & 0xff, (B >> 16) & 0xff, (B >> 8) & 0xff, B & 0xff,
            (C >> 24) & 0xff, (C >> 16) & 0xff, (C >> 8) & 0xff, C & 0xff,
            (D >> 24) & 0xff, (D >> 16) & 0xff, (D >> 8) & 0xff, D & 0xff,
            (E >> 24) & 0xff, (E >> 16) & 0xff, (E >> 8) & 0xff, E & 0xff,
            (F >> 24) & 0xff, (F >> 16) & 0xff, (F >> 8) & 0xff, F & 0xff,
            (G >> 24) & 0xff, (G >> 16) & 0xff, (G >> 8) & 0xff, G & 0xff,
            (H >> 24) & 0xff, (H >> 16) & 0xff, (H >> 8) & 0xff, H & 0xff
    };

    for ( i = 0; i < 32; i++) {
        output_v[i] = ABCDEFGH[i] ^ vi[i];
    }
}

/**
 * 获取SM3
 * @param input_m 输入信息
 * @param input_m_len 输入信息长度
 * @param output_m 输出信息
 */
void sm3(unsigned char *input_m, unsigned int input_m_len, unsigned char *output_m) {
    unsigned char *m = input_m; // 消息m
    unsigned int m_len = input_m_len; // 消息m 的长�注意单位是字�
    unsigned char *new_m; // 填充后的消息m�

    /* 填充 */
    // 假设消息m 的长度为l 比特�
    // 首先将比特�”添加到消息的末尾，再添加k 个�”，k是满 足l + 1 + k �448mod512 的最小的非负整数�
    // 然后再添加一�4位比特串，该比特串是长度l的二�制表示。填充后的消息m�的比特长度为512的倍数�
    unsigned long l = m_len * 8;
    int k = 448 - (l + 1) % 512;
    if (k < 0) 
	{
        k = k + 512;
    }
    unsigned char _m[64 * 64];
    new_m = _m;
    //复制m到output_m
    int index = 0;
    for (; index < m_len; index++) 
	{
        new_m[index] = m[index];
    }
    new_m[index] = 0x80;
    index++;
    int _k = k - 7;
    int i;
    while (_k > 0) {
        new_m[index] = 0x00;
        index++;
        _k -= 8;
    }
    for ( i = 0; i < 8; i++, index++) {
        new_m[index] = (l >> ((7 - i) * 8) & 0xff);
    }
    /* 迭代压缩 */
    // 将填充后的消息m′按512比特进行分组:m�= B(0)B(1) · · · B(n�)
    // 其中n=(l+k+65)/512�
    // 对m′按下列方式迭代:
    // FOR i=0 TO n-1
    // V(i+1) =CF(V(i),B(i))
    // ENDFOR
    // 其中CF是压缩函数，V (0)�56比特初始值IV，B(i)为填充后的消息分组，迭代压缩的结�为V (n)�
    int n = (l + k + 65) / 512;
    unsigned char *vi = IV;
    unsigned char vi1[32];
    unsigned char b[64];
    int ii;
    for (i = 0; i < n; i++) {
        for (ii = 0; ii < 64; ii++) {
            b[ii] = new_m[ii + (i * 64)];
        }
        CF(vi, b, vi1);
        vi = vi1;
    }
    for ( i = 0; i < 32; i++) {
        output_m[i] = vi1[i];
    }
}

int get_sm3_pass(unsigned long now_time,unsigned  char *community_id,unsigned char *build_num,unsigned char *cell_num) 
{									
	unsigned char d = 3;
    unsigned char index; 
	uint64_t sum_i = 0;
	unsigned char p0[32] = {0};
	unsigned long is[8] = {0};
	uint64_t i = 0U;
	unsigned long _d = 10;
	unsigned char j;
	unsigned int p;
    unsigned char s[32] = 
	{
            (now_time & 0xff), ((now_time >> 8) & 0xff), ((now_time >> 16) & 0xff), ((now_time >> 24) & 0xff), 0, 0, 0, 0,
            community_id[0], community_id[1], community_id[2], community_id[3], community_id[4], community_id[5],
            community_id[6], community_id[7], //小区id
            build_num[0], build_num[1], build_num[2], build_num[3], build_num[4], build_num[5], build_num[6],
            build_num[7], //楼栋编号
            cell_num[0], cell_num[1], cell_num[2], cell_num[3], cell_num[4], cell_num[5], cell_num[6], cell_num[7]
    }; //单元编号
	
    sm3(s, 32, p0);
	
    for(index = 0; index < 8; index++)
	{
        is[index] = p0[index * 4 + 0] << 24 | p0[index * 4 + 1] << 16 | p0[index * 4 + 2] << 8 | p0[index * 4 + 3];
    }
    
    for(index = 0; index < 8; index++)
	{
        sum_i = sum_i + is[index];
    }
		
    i = (sum_i % 0xffffffff);

    for (j = 1; j < d; j++) 
	{
        _d = _d * 10;
    }
   
    //p = abs(i % _d);
	p = (unsigned int)(i % _d);
    //USART_PRINTF_D("create passwd is: %d\n",p);
    return p;
}
