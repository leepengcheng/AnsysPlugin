#coding:utf-8
import vtk
import numpy as np
from domain import *
from config import *
from util import *
from collections import Counter, OrderedDict


class Section(object):
    """截面"""

    def __init__(self, nodes, elements, results):
        """
        初始化数据
        """
        super(Section, self).__init__()
        self.nodeset = nodes
        self.elements = elements
        self.results = results
        self.actors = {}


    def createSectionActor(self,p1,p2,p3,showLine=False,display="Seqv",bindNum=10):
        '''显示模型的切面
        '''
        #输入p1 p2 p3如果是节点号,则转换为节点坐标值
        p1, p2, p3 = getNodesNDXYZ(self,p1,p2,p3)
        self.matrix = getTMatrix(p1, p2, p3)
        #获得截面的点坐标 截面单元拓扑关系 结果值
        self.sec_points, self.sec_polys = self.getModelSectionData()
        points = vtk.vtkPoints()  #插入的节点
        cells = vtk.vtkCellArray()  #插入的节点组合
        scalars = vtk.vtkDoubleArray()
        n = 0
        for cell in self.sec_polys:
            poly = []
            for node in cell:
                points.InsertPoint(n, node)  #插入节点
                scalars.InsertTuple1(
                    n, self.sec_points[tuple(node)][display])  #插入结果值
                poly.append(n)
                n += 1
            cells.InsertNextCell(len(poly), poly)
        min_val, max_val = scalars.GetValueRange()  #获取节点的最大最小值
        sectionData = vtk.vtkPolyData()
        sectionData.SetPoints(points)
        sectionData.SetPolys(cells)
        sectionMapper = vtk.vtkPolyDataMapper()
        sectionMapper.SetScalarModeToUseCellData()  ##设置向量数据的显示模式
        sectionMapper.SetScalarRange(0, 8)  #设置显示的数值的上下限的值

        # sectionData.GetPointData().SetScalars(scalars) #设置节点结果值
        colorTable = getColorTable(bindNum)  #获得颜色对应的颜色值

        #显示结果云图
        # if display in RESULTS.keys():

        ## Bindedfilter
        # bandedFilter = vtk.vtkBandedPolyDataContourFilter()
        # bandedFilter.GenerateValues(bindNum, min_val,max_val)
        # bandedFilter.SetInputData(sectionData)
        # sectionMapper.SetInputConnection(bandedFilter.GetOutputPort())
        ##############Bindedfilter################################

        # 截面Actor
        sectionActor = vtk.vtkActor()
        sectionActor.SetMapper(sectionMapper)
        sectionActor.GetProperty().SetColor((0, 0, 1))  #设置截面颜色
        if showLine:
            sectionActor.GetProperty().SetRepresentationToWireframe()  #显示线模型
        sectionMapper.SetInputData(sectionData)
        return sectionActor

    def createScalarBarActor(self, colorTable, title=""):
        '''
        创建颜色条 
        '''
        # 添加颜色条
        scalarBarActor = vtk.vtkScalarBarActor()
        scalarBarActor.SetWidth(0.8)  #颜色条的宽度
        scalarBarActor.SetHeight(0.1)  #颜色条的高度
        # scalarBarActor.SetMaximumWidthInPixels(800)
        # scalarBarActor.SetMaximumHeightInPixels(600)
        scalarBarActor.SetPosition(0.1, 0.05)  #颜色条的位置
        scalarBarActor.SetLabelFormat("%.2f")  #颜色条数值格式
        scalarBarActor.SetOrientationToHorizontal()
        # scalarBarActor.SetOrientationToVertical() #颜色条数值方位
        scalarBarActor.SetLookupTable(colorTable)  ##颜色条颜色映射表
        scalarBarActor.SetTitle(title)  ##颜色条标题栏
        return scalarBarActor

    def createSectionCellActors(self, color=GREEN):
        '''
        创建截面 单元面Actors 用于选中和交互
        '''
        cellActors = []
        for poly in self.sec_polys:
            points = vtk.vtkPoints()
            polygon = vtk.vtkPolygon()
            for pt in poly:
                points.InsertNextPoint(*pt)
            npts = len(poly)
            polygon.GetPointIds().SetNumberOfIds(npts)
            for x in xrange(npts):
                polygon.GetPointIds().SetId(x, x)
            polygons = vtk.vtkCellArray()
            polygons.InsertNextCell(polygon)

            polygonPolyData = vtk.vtkPolyData()
            polygonPolyData.SetPoints(points)
            polygonPolyData.SetPolys(polygons)
            mapper = vtk.vtkPolyDataMapper()
            mapper.SetInputData(polygonPolyData)
            actor = vtk.vtkActor()
            actor.SetMapper(mapper)
            actor.GetProperty().SetOpacity(0.2)  #透明度
            actor.GetProperty().SetColor(*color)  #绿色
            cellActors.append(actor)
        return cellActors

    def createSectionNodeActors(self, radius=0.5, color=YELLOW):
        '''
        创建截面节点actors 用于选中和交互
        '''
        nodeActors = []
        for pt in self.sec_points.keys():
            source = vtk.vtkSphereSource()
            source.SetRadius(radius)
            source.SetCenter(*pt)
            source.SetPhiResolution(11)
            source.SetThetaResolution(21)
            mapper = vtk.vtkPolyDataMapper()
            mapper.SetInputConnection(source.GetOutputPort())
            actor = vtk.vtkActor()
            actor.SetMapper(mapper)
            actor.GetProperty().SetColor(*color)
            nodeActors.append(actor)
        return nodeActors

    def getElementClipSortedPoints(self, sec_wpts):
        '''
        求单元与截面交点的排序列表
        '''
        #转换为截面上的坐标
        sec_lpts = [getNodeTFXYZ(pt,self.matrix) for pt in sec_wpts]
        pts_enu = list(enumerate(sec_lpts))  #加上标签
        pts_enu.sort(key=lambda x: x[1][1])  #对节点按照Y进行排序
        p0 = pts_enu[0]  #最下边的节点p0
        ps = pts_enu[1:]  #其余的节点
        pts_vec = [(pt[0], pt[1] - p0[1]) for pt in ps]  #其余的节点到p0的向量
        #求各向量与X轴的夹角
        pts_cos = [(pt[0], pt[1][0] / np.sqrt(pt[1][0]**2 + pt[1][1]**2))
                   for pt in pts_vec]
        pts_cos.sort(key=lambda x: x[1])  #对余弦夹角排序
        orders = [
            p0[0],
        ] + [pt[0] for pt in pts_cos]  #获得排序标签列表
        sec_sorted_wpts = [sec_wpts[i] for i in orders]  #获得排序后的截面上的交点总体坐标系坐标
        return sec_sorted_wpts

    def getElementClipPoints(self, element):
        '''
        自定义获得截面与单元的交点
        '''
        eclip_wpts = {}
        nclip_locs = {}
        for node in element.nodes:
            nloc = self.nodeset[node].nloc  #节点全局坐标系下的坐标值
            nclip_loc = getNodeTFXYZ(nloc,self.matrix)
            nclip_locs[node] = nclip_loc  #添加项 节点号:转换值
        nodes_z = [x[2] for x in nclip_locs.values()]  #转换后的Z值列表
        if max(nodes_z) * min(nodes_z) > 0:
            return eclip_wpts  #直接返空字典
        else:
            orders = lorder[element.type]  # 单元线段排序顺序
            for order in orders:  #遍历单元的每条边求交点
                n1, n2 = map(lambda i: element.nodes[i], order)  #线段的端点序号
                p1, p2 = map(lambda i: nclip_locs[i], (n1, n2))  #转换后的节点端点坐标
                temp = p1[2] * p2[2]  #Z值的乘积
                if temp > 0:  #大于0无交点,跳过
                    continue
                else:  #小于等于0有交点
                    #如果线段刚好在切面上
                    if p1[2] == p2[2]:
                        eclip_wpts[(n1, n1)] = 0.5
                        eclip_wpts[(n2, n2)] = 0.5
                    else:
                        scale = abs(p1[2] * 1.0 / (p2[2] - p1[2]))  #比例位置
                        if TOL1 < scale < TOL2:
                            flag = (n1, n2)
                        #如果不位于容差范围
                        else:
                            flag = (n1, n1) if scale < TOL1 else (n2, n2)
                            scale = 0.5
                        eclip_wpts[flag] = scale  #存入eclip_wpts字典
            return eclip_wpts

    def getModelSectionData(self):
        '''
        获得截面与模型的交点与拓扑关系
        '''
        sec_ploys = []  #截面单元的交点拓扑列表
        sec_points = {}  #截面交点
        for e in self.elements:
            eclip_wpts = self.getElementClipPoints(e)
            #当截面与单元的交点大于等2的时候才添加
            if (len(eclip_wpts.keys()) < 2):
                continue
            pts = []
            #求交点的坐标
            for flag, scale in eclip_wpts.items():
                #求交点在总体坐标系下的点的坐标
                pt = self.getIntersection(scale, *flag)
                #用坐标位置区分节点
                pt_key = a2T(pt)
                #将交点结果加入结果字典
                sec_points[pt_key] = self.getIntersectionResult(
                    scale, *flag)
                #添加交点到单元截面交点列表
                pts.append(pt)
            #对单元截面交点进行排序
            eclip_sorted_wpts = self.getElementClipSortedPoints(pts)
            #添加单元截面交点到总体截面列表
            sec_ploys.append(eclip_sorted_wpts)
        return sec_points, sec_ploys

    def getIntersection(self, scale, p1, p2, ndigits=3):
        """
        求直线的交点的坐标
        @ndigits:精确度,交点的精确度不要太高
        """
        #如果传入的是节点号
        p1,p2=getNodesNDXYZ(self,p1,p2)
        return (p1 + (p2 - p1) * scale).round(ndigits)

    def getIntersectionResult(self, scale, n1, n2):
        """
        求直线的交点的结果值
        @p1 p2:节点的序号
        @scale:比例位置
        """
        r1, r2 = self.results[n1], self.results[n2]
        r3 = {}
        for r in RESULTS.keys():
            r3[r] = r1[r] + (r2[r] - r1[r]) * scale
        return NodeResult(r3)

    def showNodeData(self, nloc, display="Seqv"):
        # n_result= self.sec_points[nloc]
        # print "*" * 20+u"<输出类型: %s >"%display+"*" * 20
        # print u"总体坐标系:", nloc
        # print u"应力结果值:",n_result.stress().round(5)
        # print u"应力结果值:",n_result.stress(key="Seqv").round(5)
        # print u"位移结果值:",n_result.displace().round(5)

        # print u"局部坐标系:", tuple(self.matrix.dot(nloc + (1, ))[:3].round(3).tolist())
        # print u"应力结果值:",n_result.stress(self.matrix[:3,:3]).round(5)
        # print u"位移结果值:",n_result.displace(self.matrix[:3,:3]).round(5)
        print "*" * 50
        # self.createNewLine((0,0,0),nloc,10)

    def updateLocalMatrix(self, center, node):
        vec_z = self.matrix[:, :3]
        matrix = getCylindricalMatrix(vecz, center, node)
        print u"笛卡尔坐标系应力结果值:", n_result.stress(matrix).round(5)
        print u"笛卡尔坐标系位移结果值:", n_result.displace(matrix).round(5)
        print "*" * 50

    def getSectionOuterLines(self):
        "获得外轮廓线"
        lines = []
        for poly in self.sec_polys:
            nnum = len(poly)
            for r in xrange(nnum):
                if r == nnum - 1:
                    #线段排序
                    line = [a2T(poly[r],3),a2T(poly[0],3)]
                else:
                    line = [a2T(poly[r],3),a2T(poly[r+1],3)]
                line.sort()
                lines.append(tuple(line))
        couter = Counter(lines)
        self.secOuterlines = []
        for line, n in couter.items():
            if n == 1:
                self.secOuterlines.append(line)

    def getPolarPoints(self, center, matrix):
        "获得射线和网格的边界交点并排序"
        polar_pts = {}
        for line in self.secOuterlines:
              #转换面内节点坐标到matrix上
            p1, p2 = [getNodeTFXYZ(pt,matrix) for pt in line]
            if p1[1] * p2[1] > 0:  #局部坐标系的Y值
                continue
            else:
                if p1[1] == p2[1]:
                    polar_pts[(line[0], line[0])] = (p1[0], 0.5)
                    polar_pts[(line[1], line[1])] = (p2[0], 0.5)
                else:
                    scale = abs(p1[1] * 1.0 / (p2[1] - p1[1]))
                    if TOL1 < scale < TOL2:
                        flag = line
                    else:
                        flag = (line[0],
                                line[0]) if scale < TOL1 else (line[1],
                                                               line[1])
                        scale = 0.5
                    pt_x = self.getIntersection(scale, p1, p2)[0]  #交点在局部坐标系X坐标
                    if pt_x < 0:  #只计算X轴正向的交点
                        continue
                    polar_pts[flag] = (pt_x, scale)
        #将起点加入结果(如果不存在)
        center_tuple = a2T(center)
        center_key = (center_tuple, center_tuple)
        polar_pts[center_key] = (0.0, 0.5)
        #按照X轴正向进行排序
        polar_pts = OrderedDict(
            sorted(polar_pts.items(), key=lambda x: x[1][0]))
        return polar_pts

    def getSectionClipPoints(self, element):
        '''
        自定义获得截面与单元的交点
        '''
        eclip_wpts = {}
        nclip_locs = {}
        for nloc in poly:
            nclip_loc = getNodeTFXYZ(nloc,self.matrix)
            nclip_locs[node] = nclip_loc  #添加项 节点号:转换值
        nodes_y = [x[2] for x in nclip_locs.values()]  #转换后的Z值列表
        if max(nodes_z) * min(nodes_z) > 0:
            return eclip_wpts  #直接返空字典
        else:
            orders = lorder[element.type]  # 单元线段排序顺序
            for order in orders:  #遍历单元的每条边求交点
                n1, n2 = map(lambda i: element.nodes[i], order)  #线段的端点序号
                p1, p2 = map(lambda i: nclip_locs[i], (n1, n2))  #转换后的节点端点坐标
                temp = p1[2] * p2[2]  #Z值的乘积
                if temp > 0:  #大于0无交点,跳过
                    continue
                else:  #小于等于0有交点
                    #如果线段刚好在切面上
                    if p1[2] == p2[2]:
                        eclip_wpts[(n1, n1)] = 0.5
                        eclip_wpts[(n2, n2)] = 0.5
                    else:
                        scale = abs(p1[2] * 1.0 / (p2[2] - p1[2]))  #比例位置
                        if TOL1 < scale < TOL2:
                            flag = (n1, n2)
                        #如果不位于容差范围
                        else:
                            flag = (n1, n1) if scale < TOL1 else (n2, n2)
                            scale = 0.5
                        eclip_wpts[flag] = scale  #存入eclip_wpts字典
            return eclip_wpts

    def createPolarLineActor(self, center, start, end, xspace=30, yspace=30):
        "创建柱坐标扫描线"
        polar_actors = []
        #获得夹角
        angle = getAngleFrom2Vector(start - center, end - center)
        self.getSectionOuterLines()  #获得截面外轮廓线
        polar_matrix = getTMatrix(center, start, end)  #获得柱坐标系的局部矩阵
        isInSec=isPointInSection(center,self.sec_polys,polar_matrix) #圆心是否在单元内
        for ang in np.append(np.arange(0, angle, yspace), angle):
            #绕Z旋转ysapce
            ang_matrix = getRotMatrixFromZAxis(ang).dot(polar_matrix)
            #获得扫描线和截面的排序交点的字典
            pts_dict = self.getPolarPoints(center, ang_matrix)            
            pts_list=[] #交点列表
            for p in pts_dict.keys():
                p0,p1=p[0],p[1]      #线的端点
                scale=pts_dict[p][1] #比例
                pt=self.getIntersection(scale,p0,p1)  #交点1-总体坐标
                pts_list.append(pt)
            pts_num=len(pts_list)                     #边界交点的个数
            index=0 if isInSec else 1
            if pts_num>1:
                for i in xrange(index,pts_num - 1):
                    pstart=pts_list[i]
                    pend=pts_list[i+1]
                    pmid=0.5*(pstart+pend)
                    if isPointInSection(pmid,self.sec_polys,polar_matrix):
                        line_actor=createLineActor(pstart,pend)
                        polar_actors.append(line_actor) # 创建线段
                #生成均匀分布点
                pts_uniform=getUniformPoints(pts_list[index],pts_list[-1],xspace)
                pts_uniform.extend(pts_list[index:])
                pts_uniform_tuple=[a2T(p,3)  for p in pts_uniform]
                pts_uniform_set=set(pts_uniform_tuple)
                for pt in pts_uniform_set:
                    if isPointInSection(pt,self.sec_polys,ang_matrix):
                        point_actor=createPointActors([pt],r=1)
                        polar_actors.extend(point_actor) # 创建线段
            else:
                print ang
        return polar_actors

