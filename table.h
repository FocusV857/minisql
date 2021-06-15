#ifndef TABLE_H
#define TABLE_H
#include <string>
#include <vector>
#include "tuple.h"
using namespace std;


class Table {
private:
    string title_;  //����
    vector<Tuple> tuple_;  //������е�Ԫ��
    Index index_;  //���������Ϣ
public:
    Attribute attr_;  //����
    //���캯��
    Table() {};
    Table(string title, Attribute attr);
    Table(const Table& table_in);

    int addTuple(Tuple tuple_in); // ����Ԫ�棬�ж�tuple_in �ĸ�ʽ�Ƿ����table�����Ը�ʽ
    int setIndex(short index, string index_name);  //��������������Ҫ����������Attribute�ı�ţ��Լ����������֣��ɹ�����1ʧ�ܷ���0
    int dropIndex(string index_name);  //ɾ�����������뽨�������������֣��ɹ�����1ʧ�ܷ���0
    string getTitle();
    Attribute getAttr();
    vector <Tuple>& getTuple();//����Ԫ��
    Index getIndex();
    short gethasKey();//�����������ڵ�����λ��
    void showTable(); //��ӡ�����Ժ�Ԫ��
    void showTable(int limit);//��ʾtable�Ĳ�������
};

#endif