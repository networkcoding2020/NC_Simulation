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

#ifndef INET_APPLICATIONS_UDPAPP_NCFS_RUNMODE_ABSTRACTRUNMODE_H_
#define INET_APPLICATIONS_UDPAPP_NCFS_RUNMODE_ABSTRACTRUNMODE_H_

#include "inet/applications/udpapp/hanhai/nc/NCUtils.h"
#include <vector>
#include <string>

using std::string;
using std::vector;
typedef unsigned char byte;

namespace inet {

class AbstractRunMode {
protected:
    // 判断是不是数据源
    bool isSourceNode;
    int K;
    //typedef vector<vector<byte>> vbArray; 这是一个二维向量
    vbArray coefMatrix;
    vbArray reencodeMatrix;
public:

    AbstractRunMode(bool isSourceNode, int K) {
        // TODO Auto-generated constructor stub
        this->isSourceNode = isSourceNode;
        this->K = K;
        if(isSourceNode){
            initCoefMatrix();
        }
    }
    virtual ~AbstractRunMode() {
        // TODO Auto-generated constructor stub
    }

private:
    void initCoefMatrix();
public:
    // 判断对方是否有对自己有用的数据
    // itsCoefBytes对方的编码系数   一维数组形式
    // 若是有，则返回有用的系数
    // 没有的话， 返回一个空vector
    vector<byte> hasUsefulData(vector<byte>& itsCoefBytes);

    // 存储对方发过来的数据
    bool saveData(vector<byte>& coef);
    int  checkData(vector<byte>& coef,bool flag);

    //生成随机数、
    int randNumber(int limit);

    // 返回一维矩阵
    vector<byte> getCoefMatrix_1();
    vector<byte> getCoefMatrix_lyl(int num);
    vector<byte> getCoefMatrix_recode();
    vbArray getCoefMatrix_2(){
        return coefMatrix;
    }

    int totalDataNum(){
        return K;
    }
    int currentDataNum(){
        return coefMatrix.size();
    }

    bool isBootNode(){return this->isSourceNode;}

    bool haveAllData(){
        return K == coefMatrix.size();
    }

    // itsNeedCoef 对方需要的编码系数数据
    virtual vector<byte> getData(vector<byte>& itsNeedCoef) = 0;
    virtual string getRunModeString() = 0;
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_UDPAPP_NCFS_RUNMODE_ABSTRACTRUNMODE_H_ */
