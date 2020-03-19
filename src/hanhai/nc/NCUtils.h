/*
 * NCUtils.h
 * ���빤����
 *  Created on: 2017��11��29��
 *      Author: Administrator
 */
#pragma once
#include "inet/applications/udpapp/hanhai/nc/GF.h"
#include <vector>
using std::vector;

//C++�� û��byte�ؼ���
typedef unsigned char byte;
typedef vector<vector<byte>> vbArray;
#define random(x) (rand()%x)   // [0,x) ���ұ����������������
class NCUtils {
private:
    static GF gf;
    NCUtils() = delete;
public:
    //���������
    static vbArray multiply(vbArray matrix1, vbArray matrix2);

    // ����������� 0-256
    // �������Ƚ�С  ʹ��vector�Ϸ���  �������ֶ��ͷſռ�
    static vbArray generateRandMatrix(int row, int col);

    static int generateRandNumber(int limit);

    //����
    static int getRank(vbArray matrix);

    // �ٱ��� ����һ������
    static vbArray reencode(vbArray matrix);

private:
    // ���������
    static byte** multiply(byte** matrix1, int row1, int col1, byte** matrix2,
                int row2, int col2);
    //����
    static int getRank(byte** matrix, int nRow, int nCol);
    //ֻ�Է�������
    static byte** inverse(byte** matrix, int nK);

    static byte** vbArray2newArray(vbArray matrix);
    static vbArray newArray2vbArray(byte** matrix,int row, int col);
    static void releaseNewArray(byte** matrix, int row);
};

