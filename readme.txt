Linux-L7 filter正则表达式转换成标准正则表达式
运行express中的ExpressChange，可以将Linux-L7 filter正则表达式转换成分组算法可以处理的标准正则表达式

分组算法
运行project中的divide可以完成正则表达式的分组

GRELS算法的实现
1.运行newproject中的main，main运行后得到正则表达式对应的M矩阵，DR矩阵，和转移边信息矩阵，分别存放在m1，m2和dfile文件之中
2.运行newregex 中Divide，Divide 运行得到分组结果
3.运行newregex 中GroupFile，GroupFile运行后根据分组结果产生分组文件
