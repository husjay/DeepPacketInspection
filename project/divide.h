#ifndef DIVIDE_H
#define DIVIDE_H


class divide
{

    public:
        divide();
        virtual ~divide();

        //��������ʽ���з���
        void droup();

        //���ݷ���������������ʽ����
        void outputGroupResult();

    protected:
    private:
        //���㵱ǰ������ʽ��K������������
        void countCurrentM(int i, int currentM[]);

        //�ҵ���С������Ե�������ʽ
        int findMinCurrentM(int *currentM);

};
