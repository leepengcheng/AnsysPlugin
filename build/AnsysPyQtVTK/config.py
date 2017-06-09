#coding:utf-8
'''
配置文件
'''
#默认容差
TOL1 = 0.001
TOL2 = 0.999
#颜色
RED = (1, 0, 0)
GREEN = (0, 1, 0)
BLUE = (0, 0, 1)
YELLOW = (1, 1, 0)
BLACK = (0, 0, 0)
WHITE = (1, 1, 1)


#输出结果类型
RESULTS = {'Ux':0, 'Uy':1, 'Uz':2,'Usum':-1, 'Sx':(0,0), 'Sy':(1,1), 'Sz':(2,2), 'Sxy':(0,1), 'Sxz':(0,2), 'Syz':(1,2), 'Seqv':-1}

########单元拓扑关系################
#单元面拓扑关系
eorder = {}
eorder['solid6'] = ((0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5),
                    (2, 3, 7, 6), (3, 0, 4, 7))
eorder['solid5'] = ((0, 1, 2), (4, 5, 6), (0, 1, 5, 4), (1, 2, 6, 5), (2, 0, 4,
                                                                       6))
eorder['solid4'] = ((0, 1, 2), (0, 1, 4), (0, 2, 4), (1, 2, 4))
eorder['shell4'] = ((0, 1, 2, 3), )
eorder['shell3'] = ((0, 1, 2), )
eorder['beam2'] = ((0, 1), )

#单元边拓扑关系
lorder = {}
lorder['solid6'] = ((0, 1), (1, 2), (2, 3), (3, 0), (0, 4), (1, 5), (2, 6),
                    (3, 7), (7, 4), (6, 7), (5, 6), (4, 5))
lorder['solid4'] = ((0, 1), (1, 2), (2, 0), (0, 4), (1, 4), (2, 4))
lorder['shell4'] = ((0, 1), (1, 2), (2, 3), (3, 0))
lorder['shell3'] = ((0, 1), (1, 2), (2, 0))
lorder['beam2'] = ((0, 1))


    