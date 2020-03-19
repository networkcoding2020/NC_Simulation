#include "inet/applications/udpapp/hanhai/nc/GF.h"
// 不写static会报错
static GFType prim_poly[13] = {
	/*	0 */0x00000000,
	/*  1 */0x00000001,
	/*  2 */0x00000007,
	/*  3 */0x0000000b,
	/*  4 */0x00000013,
	/*  5 */0x00000025,
	/*  6 */0x00000043,
	/*  7 */0x00000089,
	/*  8 */0x00000187,
	/*  9 */0x00000211,
	/* 10 */0x00000409,
	/* 11 */0x00000805,
	/* 12 */0x00001053 };

GF::GF() {
	//初始化有限域
	init(8, 0x00000187);
}
GF::~GF() {
	//释放有限域
	uninit();
}

void GF::init(unsigned int m, unsigned int prim) // GF(2^m), primitive polymonial
{
	int i = 0, j = 0;

	if (m > 12)	// the field size is supported from GF(2^1) to GF(2^12).
		return;

	gFieldSize = 1 << m;

	if (0 == prim)
		prim = prim_poly[m];

	table_alpha = new GFType[gFieldSize];
	table_index = new GFType[gFieldSize];
	table_mul = new GFType*[gFieldSize];
	table_div = new GFType*[gFieldSize];

	for (i = 0; i < gFieldSize; i++) {
		table_mul[i] = new GFType[gFieldSize];
		table_div[i] = new GFType[gFieldSize];
	}

	table_alpha[0] = 1;
	table_index[0] = -1;

	for (i = 1; i < gFieldSize; i++) {
		table_alpha[i] = table_alpha[i - 1] << 1;
		if (table_alpha[i] >= gFieldSize) {
			table_alpha[i] ^= prim;
		}

		table_index[table_alpha[i]] = i;
	}

	table_index[1] = 0;

	// create the tables of mul and div
	for (i = 0; i < gFieldSize; i++) {
		for (j = 0; j < gFieldSize; j++) {
			table_mul[i][j] = gfmul(i, j);
			table_div[i][j] = gfdiv(i, j);

		}
	}

}
void GF::uninit() {

	delete table_alpha;
	delete table_index;

	for (int i = 0; i < gFieldSize; i++) {
		delete[] table_mul[i];
		delete[] table_div[i];
	}

	delete[] table_mul;
	delete[] table_div;

	// 置空释放的指针   防止再次访问查找到错误值
	table_alpha = nullptr;
	table_index = nullptr;

	table_mul = nullptr;
	table_div = nullptr;
}

GFType GF::exp(GFType a, GFType n) {
	if (a == 0 && n == 0) {
		return 1;
	}
	if (a == 0 && n != 0) {
		return 0;
	}
	return table_alpha[table_index[a] * n % (gFieldSize - 1)];
}

GFType GF::gfmul(GFType a, GFType b) {
	if (0 == a || 0 == b)
		return 0;

	return table_alpha[(table_index[a] + table_index[b]) % (gFieldSize - 1)];
}

GFType GF::gfdiv(GFType a, GFType b) {
	if (0 == a || 0 == b)
		return 0;

	return table_alpha[(table_index[a] - table_index[b] + (gFieldSize - 1))
		% (gFieldSize - 1)];
}
