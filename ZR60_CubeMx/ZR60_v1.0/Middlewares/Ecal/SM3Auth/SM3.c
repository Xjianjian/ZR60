/******************************************************
文件名：	SM3.c

描述：		
Data			Vasion			author
2018/1/13		V1.0			liujian
*******************************************************/

/*******************************************************
description： include the header file
*******************************************************/
#include "SM3Auth/SM3.h"


//循环左移
unsigned int ROTL(unsigned int x, int n) {
    unsigned int x1 = (x & 0xFFFFFFFF) << n;
    unsigned int x2 = (x >> (32 - n));
    return x1 | x2;
}

// 初始IV
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
 * 鑾峰彇SM3
 * @param input_m 输入
 * @param input_m_len 输入长度
 * @param output_m 输出
 */
void sm3(unsigned char *input_m, unsigned int input_m_len, unsigned char *output_m) 
{
    unsigned char *m = input_m; // 信息m
    unsigned int m_len = input_m_len; // 信息m的长度
    unsigned char *new_m; // 填充后的信息m

	/* 填充 */
    // 假设消息m 的长度为l 比特」
    // 首先将比特?添加到消息的末尾，再添加k 个?，k是满 足l + 1 + k ?48mod512 的最小的非负整数」
    // 然后再添加一临位比特串，该比特串是长度l的二逥?表示。填充后的消息m??比特长度为512的倍数」
     unsigned long l = m_len * 8;
    int k = 448 - (l + 1) % 512;
    if (k < 0) 
	{
        k = k + 512;
    }
    unsigned char _m[64 * 64];
    new_m = _m;
 
    int index = 0;
    for (; index < m_len; index++) 
	{
        new_m[index] = m[index];
    }
    new_m[index] = 0x80;
    index++;
    int _k = k - 7;
    int i;
    while (_k > 0) 
	{
        new_m[index] = 0x00;
        index++;
        _k -= 8;
    }
    for ( i = 0; i < 8; i++, index++) 
	{
        new_m[index] = (l >> ((7 - i) * 8) & 0xff);
    }
    /* 杩唬鍘嬬缉 */
    // 灏嗗～鍏呭悗鐨勬秷鎭痬鈥叉寜512姣旂壒杩涜鍒嗙粍:m鈥= B(0)B(1) 路 路 路 B(n鈭)
    // 鍏朵腑n=(l+k+65)/512銆
    // 瀵筸鈥叉寜涓嬪垪鏂瑰紡杩唬:
    // FOR i=0 TO n-1
    // V(i+1) =CF(V(i),B(i))
    // ENDFOR
    // 鍏朵腑CF鏄帇缂╁嚱鏁帮紝V (0)涓56姣旂壒鍒濆鍊糏V锛孊(i)涓哄～鍏呭悗鐨勬秷鎭垎缁勶紝杩唬鍘嬬缉鐨勭粨鏋涓篤 (n)銆
    int n = (l + k + 65) / 512;
    unsigned char *vi = IV;
    unsigned char vi1[32];
    unsigned char b[64];
    int ii;
    for (i = 0; i < n; i++) 
	{
        for (ii = 0; ii < 64; ii++) 
		{
            b[ii] = new_m[ii + (i * 64)];
        }
        CF(vi, b, vi1);
        vi = vi1;
    }
    for ( i = 0; i < 32; i++) 
	{
        output_m[i] = vi1[i];
    }
}
