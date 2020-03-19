//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "AbstractRunMode.h"
#include "omnetpp/clog.h"

namespace inet {

//  使用单位矩阵初始化系数矩阵
//  这里是单位矩阵  而且矩阵的秩初始化为K 且只有是根节点才具有初始化矩阵 可以判断它具有完整的数据
void AbstractRunMode::initCoefMatrix() {
    for (int i = 0; i < K; i++) {
        //生成字节数组 全为0
        vector<byte> coef(K, 0);
        //第i个设置为1
        coef[i] = 1;
        //将字节数组放进编码系数中 就成了单位矩阵
        coefMatrix.push_back(coef);
    }
}

vector<byte> AbstractRunMode::hasUsefulData(vector<byte>& itsCoefBytes) {
    // 返回值
    vector<byte> usefulCoef;
    // 转化为矩阵形式
    int nRow = itsCoefBytes.size() / K;
    vbArray temp_coef(coefMatrix);//coefMatrix的副本
    int row = coefMatrix.size();//coefMatrix是一个二维矩阵 size() 获取的是行数 既是它的秩

    // 判断秩
    for (int i = 0; i < nRow; i++) {
        vector<byte> coef(K);
        for (int j = 0; j < K; j++) {
            //将到来的系数矩阵 的每一行分别放入 coef数组中
            coef[j] = itsCoefBytes[i * K + j];
        }
        //将coef 放入coefMatrix的副本中 求秩
        temp_coef.push_back(coef);
        int rank = NCUtils::getRank(temp_coef);
        if (rank > row) {

            usefulCoef = coef;
            break;
        }
        temp_coef.pop_back();
    }

    return usefulCoef;
}
//数据的保存还是可以改变一下换成当前数据的秩是否增加了
bool AbstractRunMode::saveData(vector<byte>& coef) {
    int row = coefMatrix.size();//行数 也就是现在的秩
    vbArray temp_coef(coefMatrix);
    temp_coef.push_back(coef);
    int rank = NCUtils::getRank(temp_coef);
    if (rank > row) {
        coefMatrix.push_back(coef);//秩增加了就将数据放入里面
        return true;
    } else {
        return false;
    }
}
int  AbstractRunMode::checkData(vector<byte>& coef,bool flag){
    //coefMatrix.push_back(coef);
    //int rank1 = NCUtils::getRank(coefMatrix);
    if(flag==true)
    {
        coefMatrix.clear();
    }
    int row = coefMatrix.size();//行数 也就是现在的秩
    //return rank1;
    vbArray temp_coef(coefMatrix);
    temp_coef.push_back(coef);
    int rank = NCUtils::getRank(temp_coef);
    if (rank > row) {
         coefMatrix.push_back(coef);//秩增加了就将数据放入里面

         if(rank==10)
         {
             //这里一旦矩阵的秩rank达到K 也就是10 的时候就可以 进行再编码操作了
             reencodeMatrix.clear();
             reencodeMatrix=NCUtils::reencode(coefMatrix);
             //清空 是可以的
             coefMatrix.clear();
         }
         return rank;
    } else {
         return row;
    }
}

int  AbstractRunMode::randNumber(int limit)
{
    int res = NCUtils::generateRandNumber(limit);
    return res;
}

//获取系数矩阵  这个是单位矩阵--每次获取到完整的单位阵--并且放入一维矩阵中 ---lyl
vector<byte> AbstractRunMode::getCoefMatrix_1() {
    vector<byte> coefs;
    for (auto& v : coefMatrix) {
        for (auto& i : v) {
            coefs.push_back(i);
        }
    }
    return coefs;
}

vector<byte> AbstractRunMode::getCoefMatrix_lyl(int num) {
    vector<byte> coefs;
    vbArray randomMatrix;
    randomMatrix=NCUtils::generateRandMatrix(num, 10);
    for (auto& v : randomMatrix) {
        for (auto& i : v) {
            coefs.push_back(i);
        }
    }
    return coefs;
}

vector<byte> AbstractRunMode::getCoefMatrix_recode() {
    vector<byte> coefs;
    //将再编码后的二维矩阵转化为一维矩阵
    for (auto& v : reencodeMatrix) {
        for (auto& i : v) {
            coefs.push_back(i);
        }
    }
    return coefs;
}

} /* namespace inet */

