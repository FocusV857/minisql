#ifndef TUPLE_H
#define TUPLE_H
#include <vector>
#include <iostream>
using namespace std;

//����where���ж� �ֱ�ΪС�ڣ�С�ڵ��ڣ����ڣ����ڵ��ڣ����ڣ�������
typedef enum {
    LESS,
    LESS_OR_EQUAL,
    EQUAL,
    GREATER_OR_EQUAL,
    GREATER,
    NOT_EQUAL
} WHERE;

//һ��struct�������һ����Ϣ�����ͺ�ֵ
//��һ��struntǶ��һ��unionʵ�ֶ������͵�ת��
//type�����ͣ�-1��int,0:float,1-255:string(��ֵΪ�ַ����ĳ���+1)
struct Data {
    int type;
    int datai;
    float dataf;
    std::string datas;
};

//Where���һ���ж����
struct Where {
    Data data; //����
    WHERE relation_character;   //��ϵ
};

struct Index {
    int num;  //index������
    short location[10];  //��i��index�����ڵ�location[i]��attribute��
    string indexname[10];  //ÿ��index������
};

struct Attribute {
    int num;  //���table��������
    string name[32];  //���ÿ�����Ե�����
    string index_name[32];
    short type[32];  //���ÿ�����Ե����ͣ�-1��int,0:float,1~255:string�ĳ���+1
    bool unique[32];  //�ж�ÿ�������Ƿ�unique����Ϊtrue
    short primary_key;  //�ж��Ƿ��������,-1Ϊ�����ڣ�������Ϊ����������λ��
    bool has_index[32]; //�ж��Ƿ��������
};

class Tuple {
private:
    vector<Data> data_;  //�洢Ԫ�����ÿ�����ݵ���Ϣ
public:
    Tuple();
    Tuple(const Tuple& tuple_in);  //����Ԫ��
    void addData(Data data_in);  //����Ԫ��
    vector<Data> getData() const;  //��������
    int getSize();  //����Ԫ�����������
    bool isDeleted();
    void setDeleted();
    void showTuple();  //��ʾԪ���е���������
};

//�������ݳ���
template <typename T>
int getDataLength(T data) {
    stringstream stream;
    stream << data;
    return stream.str().length();
}

//����a,b��ϵ���ж�
template <typename T>
bool isSatisfied(T a, T b, WHERE relation) {
    switch (relation) {
    case LESS: {
        if (a < b)
            return true;
        else
            return false;
    }; break;
    case LESS_OR_EQUAL: {
        if (a <= b)
            return true;
        else
            return false;
    }; break;
    case EQUAL: {
        if (a == b)
            return true;
        else
            return false;
    }; break;
    case GREATER_OR_EQUAL: {
        if (a >= b)
            return true;
        else
            return false;
    }; break;
    case GREATER: {
        if (a > b)
            return true;
        else
            return false;
    }; break;
    case NOT_EQUAL: {
        if (a != b)
            return true;
        else
            return false;
    }; break;
    }
}

//������ת��Ϊ�ַ���
template <typename T>
void copyString(char* p, int& offset, T data) {
    stringstream stream;
    stream << data;
    string s1 = stream.str();
    for (int i = 0; i < s1.length(); i++, offset++)
        p[offset] = s1[i];
}
#endif
