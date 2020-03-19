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

//  ʹ�õ�λ�����ʼ��ϵ������
//  �����ǵ�λ����  ���Ҿ�����ȳ�ʼ��ΪK ��ֻ���Ǹ��ڵ�ž��г�ʼ������ �����ж�����������������
void AbstractRunMode::initCoefMatrix() {
    for (int i = 0; i < K; i++) {
        //�����ֽ����� ȫΪ0
        vector<byte> coef(K, 0);
        //��i������Ϊ1
        coef[i] = 1;
        //���ֽ�����Ž�����ϵ���� �ͳ��˵�λ����
        coefMatrix.push_back(coef);
    }
}

vector<byte> AbstractRunMode::hasUsefulData(vector<byte>& itsCoefBytes) {
    // ����ֵ
    vector<byte> usefulCoef;
    // ת��Ϊ������ʽ
    int nRow = itsCoefBytes.size() / K;
    vbArray temp_coef(coefMatrix);//coefMatrix�ĸ���
    int row = coefMatrix.size();//coefMatrix��һ����ά���� size() ��ȡ�������� ����������

    // �ж���
    for (int i = 0; i < nRow; i++) {
        vector<byte> coef(K);
        for (int j = 0; j < K; j++) {
            //��������ϵ������ ��ÿһ�зֱ���� coef������
            coef[j] = itsCoefBytes[i * K + j];
        }
        //��coef ����coefMatrix�ĸ����� ����
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
//���ݵı��滹�ǿ��Ըı�һ�»��ɵ�ǰ���ݵ����Ƿ�������
bool AbstractRunMode::saveData(vector<byte>& coef) {
    int row = coefMatrix.size();//���� Ҳ�������ڵ���
    vbArray temp_coef(coefMatrix);
    temp_coef.push_back(coef);
    int rank = NCUtils::getRank(temp_coef);
    if (rank > row) {
        coefMatrix.push_back(coef);//�������˾ͽ����ݷ�������
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
    int row = coefMatrix.size();//���� Ҳ�������ڵ���
    //return rank1;
    vbArray temp_coef(coefMatrix);
    temp_coef.push_back(coef);
    int rank = NCUtils::getRank(temp_coef);
    if (rank > row) {
         coefMatrix.push_back(coef);//�������˾ͽ����ݷ�������

         if(rank==10)
         {
             //����һ���������rank�ﵽK Ҳ����10 ��ʱ��Ϳ��� �����ٱ��������
             reencodeMatrix.clear();
             reencodeMatrix=NCUtils::reencode(coefMatrix);
             //��� �ǿ��Ե�
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

//��ȡϵ������  ����ǵ�λ����--ÿ�λ�ȡ�������ĵ�λ��--���ҷ���һά������ ---lyl
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
    //���ٱ����Ķ�ά����ת��Ϊһά����
    for (auto& v : reencodeMatrix) {
        for (auto& i : v) {
            coefs.push_back(i);
        }
    }
    return coefs;
}

} /* namespace inet */

