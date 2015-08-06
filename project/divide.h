#ifndef DIVIDE_H
#define DIVIDE_H


class divide
{

    public:
        divide();
        virtual ~divide();

        //对正则表达式进行分组
        void droup();

        //根据分组结果，将正则表达式分组
        void outputGroupResult();

    protected:
    private:
        //计算当前正则表达式和K个分组的相关性
        void countCurrentM(int i, int currentM[]);

        //找到最小的相关性的正则表达式
        int findMinCurrentM(int *currentM);

};
