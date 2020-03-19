#pragma once

typedef unsigned int GFType;

class GF {
private:
	int gFieldSize;
	GFType* table_alpha;
	GFType* table_index;
	GFType** table_mul;
	GFType** table_div;
public:
	GF();
	~GF();

	// 加减乘除   四个内联函数
	GFType add(GFType a, GFType b) {
		return a ^ b;
	}
	GFType sub(GFType a, GFType b) {
		return a ^ b;
	}
	GFType mul(GFType a, GFType b) {
		return table_mul[a][b];
	}
	GFType div(GFType a, GFType b) {
		return table_div[a][b];
	}
	// 求指数
	GFType exp(GFType a, GFType n);
private:
	void init(unsigned int m, unsigned int prim);
	void uninit();

	GFType gfmul(GFType a, GFType b);
	GFType gfdiv(GFType a, GFType b);
};


