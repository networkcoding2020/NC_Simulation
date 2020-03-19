/*
 * NCUtils.h
 * 编码工具类
 *  Created on: 2017年11月29日
 *      Author: Administrator
 */
#pragma once
#include "inet/applications/udpapp/hanhai/nc/GF.h"
#include <vector>
using std::vector;

//C++中 没有byte关键字
typedef unsigned char byte;
typedef vector<vector<byte>> vbArray;
#define random(x) (rand()%x)   // [0,x) 左开右闭区间上生成随机数
class NCUtils {
private:
    static GF gf;
    NCUtils() = delete;
public:
    //两矩阵相乘
    static vbArray multiply(vbArray matrix1, vbArray matrix2);

    // 生成随机数据 0-256
    // 数据量比较小  使用vector较方便  不用再手动释放空间
    static vbArray generateRandMatrix(int row, int col);

    static int generateRandNumber(int limit);

    //求秩
    static int getRank(vbArray matrix);

    // 再编码 产生一个数据
    static vbArray reencode(vbArray matrix);

private:
    // 两矩阵相乘
    static byte** multiply(byte** matrix1, int row1, int col1, byte** matrix2,
                int row2, int col2);
    //求秩
    static int getRank(byte** matrix, int nRow, int nCol);
    //只对方阵求逆
    static byte** inverse(byte** matrix, int nK);

    static byte** vbArray2newArray(vbArray matrix);
    static vbArray newArray2vbArray(byte** matrix,int row, int col);
    static void releaseNewArray(byte** matrix, int row);
};

