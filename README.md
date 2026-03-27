# 运行说明

Windows系统下使用CMake和MinGW-w64完成配置和编译

## 步骤
0. 确定安装有cmake和MingGW-w64，并将相应bin目录加入环境变量

<!-- 1. 阅读CMakeLists.txt修改头文件目录和库文件目录，将它们指向上传的压缩包内的openGL4.6下的对应路径 -->

2. 在项目目录下，依次执行下列命令完成配置和编译
```
mkdir build
cd build
cmake.exe -G "MinGW Makefiles" ..
mingw32-make.exe
```

3. 拷贝lib目录中的glfw.dll到build目录下（windows下使用动态链接库似乎必须要在运行目录下有对应动态库）

4. 运行.exe文件，注意命令行参数的选取。所有的.obj文件均在resource目录下，第一个参数为输入的.obj文件名，但输入时不要带".obj"后缀；第二个参数为网格简化的级数（subdivision level）。resource目录下所有可用的.obj文件，即对应输入参数，如下列出：
- cow.obj      => cow
- cube.obj    => cube
- eight.uniform.obj     => eight.uniform
- tetrahedron.obj => tetrahedron
- teddy.obj => teddy
- dinosaur.obj => dinosaur
    
    类似FPS游戏，在程序中“W”“A”“S”“D”分别控制摄像机前、左、后、右平移。鼠标（光标）会被捕捉，移动鼠标会变换视角。鼠标滚轮可以缩放视域。

    在程序中“←”“→”方向键控制切换级数（level），长按“→”做进一步的细分，长按“←”返回上一级细分的网络。

    注意：对于较大的模型，执行加密算法的时间会比较长，因此建议采用三角形面片数较少的小模型做输入。

```
A4.exe ../resource/cow 3 # 以cow.obj为输入，最多做3次细分
A4.exe ../resource/cube 4 # 以cube.obj为输入，最多做4次细分
```

具体的实现，请参见本人的CSDN博客：https://blog.csdn.net/weixin_43614211/article/details/121590305

For detailed implemenation, please refer to: https://blog.csdn.net/weixin_43614211/article/details/121590305

# 以下为《A-4. 使用半边结构实现网格加密算法-Loop scheme》博客的内容
（由于上述的CSDN需要付费才能看到完整博客内容，违背本人技术分享的初衷，此处附载博客全文）

作业要求：
1. 具体方法参见下面的链接 [http://multires.caltech.edu/pubs/sig99notes.pdf](http://multires.caltech.edu/pubs/sig99notes.pdf), 第 46 页，3.1 节（但实际上好像在4.2节？）
2. 可以选择网格加密选项 

注：不得不说，对于要自己造轮子的半边网格结构，A-4加密细分的作业远比B-7的QSlim简化要简单，因为加密只需要在原有网格上添加元素，不涉及删除，因而不需要顾忌删除过程中的`is_collapse_OK`（如面flip）而致崩溃的问题。所以建议以后将B-7的简化算法实现也加入到A类作业中。

## 算法介绍
现有的细分方案可以根据下面的三个简单的原则进行分类：

- 细化规则的类型(顶点插入或切角)
- 生成的网格类型(三角形或四边形)
- 方案是近似的还是插值的

不同代表的方案如下表所示。

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/64f1eba7bd41ecd464652f1deb3f51de.png)


最简单的细分方案之一是Loop方案，由Charles Loop发明。Loop方案属于顶点插入类型，是为三角形网格而设计的，如下图所示。新的顶点显示为黑点。原（粗）网格的每条边被分割成两条，新的顶点重新连接形成4个新的三角形，替换原（粗）网格中的每个三角形。

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/677e31fe3e6ff6dd78d2874502cebb47.png)

像大多数（但不是所有）其他细分方案一样，这种方案是基于样条基函数，称为三向四次盒样条。不同于更传统的样条（如双三次样条），三向盒样条被定义在规则三角形网格上。此样条的生成多项式为

$$S(z_1,z_2)=\frac{1}{16}(1+z_1)^2(1+z_2)^2(1+z_1z_2)^2$$

这个样条基函数是 $C^2-$ 连续的。它的细分规则和细分系数如下图所示。

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/4eff71d5f71d921a69109cd21cf74f73.png)

在一维情况中，一旦选择了样条基函数，生成曲线所需的细分规则的所有系数就完全确定了。对于表面来说，情况完全不同，且更加复杂。曲线的控制多边形的结构总是非常简单：顶点排列成一条链，链上的任意两个相同长度的部分总是具有相同的结构。

对于二维网格，网格的局部结构可能是不同的：连接到一个顶点的边的数量可能是不同的。所以，从样条基函数导出的规则**只能应用于局部规则网格的部分**；也就是说，只对那些价为6的顶点（在三角网格的情况下）。在其他情况下，我们必须为具有不同价的顶点设计新的规则。这样的顶点称为**非标准顶点**。

目前，我们只考虑了没有边界的网格。注意，用于计算插入在边缘上的控制点的四次方框样条规则（左上图）可以应用于任何地方。唯一需要修改的规则是如何计算从上一级继承的控制点的新位置。

**Loop建议使用如下图所示的系数**。结果表明，这种系数的选择保证了方案的极限表面（无限细分的表面）是“光滑的”。注意，这些新规则只影响非规则顶点附近表面的局部行为。

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/890360529f408dcb9be9fcaa8a89fb5e.png)

整个**Loop细分算法可以描述如下**：
1. 对每一个三角形面片，在三条边上各加入一个顶点（称为“**边点**”）
	边点位置由周围的原（粗）网格的顶点位置加权确定，权系数遵循以下规则：
	1. 如果边不在网格边界上：按上图（a）的interior规则计算边点的位置，即该边上两个端点权值各为3/8，该边所对的两个对面顶点的权值各为1/8
	2. 如果边在网格边界：按上图（a）的crease and boundary规则计算边点的位置，即该边上两个端点权值各为1/2
2. 对于原（粗）网格的旧有顶点（非**边点**），要调整它们的位置
	旧顶点位置由原（粗）网格的顶点位置加权确定（注意：新加入的**边点**不对旧有顶点位置产生影响），权系数遵循以下规则：
	1. 如果该旧顶点位于边界上（有2个也位于边界的旧顶点邻居）：按上图（b）的crease and boundary规则调整边顶点的位置，即自己原位置权重3/4，另两个边界旧顶点邻居权重1/8
	2. 如果该旧顶点位于网格内部：按上图（b）的interior规则调整，即自己原位置权重 $1-k\beta$ ，其它旧有顶点邻居权重 $\beta$ ，其中 $k$ 为旧顶点邻居的数目， $\beta$ 按照上图的一种方式取。

## 算法实现
### 加入新的边点
遍历三角形网格的每一个面片 $A$ ，给面的三条边对应加上一个顶点，线性时间复杂度。注意，此时形成4个新的三角形面片 $A_1, A_2, A_3, A_4$ ，而新网格中不再包含原有的 $A$ 。
```cpp
zyMesh LoopSubdivide_native(zyMesh & mesh) {
    // 返回的加密网格先保留粗网格的顶点信息（仅有位置，无拓扑）
    zyMesh newMesh;
    size_t nVerts = mesh.vertexList.size();
    for (size_t i = 0; i < nVerts; i++)
        newMesh.add_vertex(mesh.vertexData[i].Position);

    size_t nFaces = mesh.faceList.size();
    size_t newIndexOfVertices = nVerts;

    std::unordered_map<Index_Pair2, Index_Pair3, Index_Pair2_hash> edgeVertice;

    size_t * newFaces = new size_t[nFaces*3*4];//面数将会是原来的四倍
    size_t newFaces_ptr = 0;

    // 创建一个新的边点矩阵即edgeVertice以及新的三角面newFaces.
    for (size_t f_id = 0; f_id < mesh.faceList.size(); f_id++) {
        int vids[3], i = 0;
        size_t start_halfedge_id = mesh.faceList[f_id].start_halfegde_id;
        size_t halfedge_id = start_halfedge_id;
        do {
            vids[i++] = mesh.halfedgeList[halfedge_id].from_vertex_id;
            halfedge_id = mesh.halfedgeList[halfedge_id].next_halfedge_id;
        } while (halfedge_id != start_halfedge_id);

        size_t vpIndex = addEdgeVertice(edgeVertice, vids[0], vids[1], vids[2], newIndexOfVertices);
        if (vpIndex >= newMesh.vertexList.size())// vpIndex之前没有加进去过
            newMesh.add_vertex(vec3(0.0f));

        size_t vqIndex = addEdgeVertice(edgeVertice, vids[1], vids[2], vids[0], newIndexOfVertices);
        if (vqIndex >= newMesh.vertexList.size())
            newMesh.add_vertex(vec3(0.0f));

        size_t vrIndex = addEdgeVertice(edgeVertice, vids[2], vids[0], vids[1], newIndexOfVertices);
        if (vrIndex >= newMesh.vertexList.size())
            newMesh.add_vertex(vec3(0.0f));
        
        newFaces[newFaces_ptr++] = vids[0]; newFaces[newFaces_ptr++] = vpIndex; newFaces[newFaces_ptr++] = vrIndex;
        newFaces[newFaces_ptr++] = vpIndex; newFaces[newFaces_ptr++] = vids[1]; newFaces[newFaces_ptr++] = vqIndex;
        newFaces[newFaces_ptr++] = vrIndex; newFaces[newFaces_ptr++] = vqIndex; newFaces[newFaces_ptr++] = vids[2];
        newFaces[newFaces_ptr++] = vrIndex; newFaces[newFaces_ptr++] = vpIndex; newFaces[newFaces_ptr++] = vqIndex;
    }
    ... ...
}
```
注意每个面与3条半边联系在一起，而如果每条半边都新添加一个边点，则每个边点都会被添加两次而造成重复！因此需要在加入时，判断之前是否加入过这个位置（这条边）上的顶点了！这个新加入边点的**唯一性**通过边两端的顶点来唯一标记，并通过一个从`Index_Pair2`到`Index_Pair3`的哈希表来维护以便后续可以快速查询：
```cpp
class Index_Pair2 {
public:
    size_t v1, v2;
    Index_Pair2(): v1(IndexNotValid), v2(IndexNotValid) {}
    Index_Pair2(size_t v1, size_t v2): v1(v1), v2(v2) {}
    bool operator == (Index_Pair2 const & b) const {
        return v1 == b.v1 && v2 == b.v2;
    }
};
class Index_Pair3{
public:
    size_t vN, voppo1, voppo2;
    Index_Pair3():vN(IndexNotValid), voppo1(IndexNotValid), voppo2(IndexNotValid) {}
    Index_Pair3(size_t vN, size_t voppo1, size_t voppo2=IndexNotValid):vN(vN), voppo1(voppo1), voppo2(voppo2) {}
};
struct Index_Pair2_hash	{    
    size_t operator() (Index_Pair2 const & x) const {        
        return hash_val(x.v1, x.v2);
    }	
};
```
函数`addEdgeVertice()`接受一条边上的两个顶点`v1Index`，`v2Index`，查询这个点是否已经在`edgeVertice`的哈希表中，如果不存在，则加入这个新边点的序号为`newIndex`，并记录这条边所对着的第一个旧有顶点序号为`v3Index`；如果已经存在，则更新这条边所对着的第二个旧有顶点序号为`v3Index`。因此可以通过`addEdgeVertice()`的返回值判断这次是否有插入新的边点，从而是否需要调用`zyMesh::add_vertex()`添加新的顶点实例。

```cpp
int addEdgeVertice(std::unordered_map<Index_Pair2,Index_Pair3, Index_Pair2_hash> & edgeVertice, \
    size_t v1Index, size_t v2Index, size_t v3Index, size_t & newIndex) {
    if (v1Index > v2Index) {// setting v1 <= v2
        int vtmp = v1Index;
        v1Index = v2Index;
        v2Index = vtmp;
    }

    Index_Pair2 verts_pair(v1Index, v2Index);
    if (edgeVertice.find(verts_pair) == edgeVertice.end())//new vertex
        edgeVertice[verts_pair] = Index_Pair3(newIndex++, v3Index);//新加入顶点的编号 和边(v1Index,v2Index)对着的第一个顶点v3Index
    else 
        edgeVertice[verts_pair].voppo2 = v3Index;
    return edgeVertice[verts_pair].vN;
}
```

### 设置新加入边点的坐标
新加入边点的坐标，直接由旧有顶点的坐标加权平均。由于哈希表`edgeVertice`中存储的每一项就是一个新加入的边点，因此直接遍历`edgeVertice`去修改顶点`vN`坐标即可，线性时间复杂度。
```cpp
zyMesh LoopSubdivide_native(zyMesh & mesh) {
	... ...
	//  更新插入顶点（新顶点位置直接确定）
    for (std::unordered_map<Index_Pair2, Index_Pair3, Index_Pair2_hash>::iterator it = edgeVertice.begin(); it != edgeVertice.end(); it++) {// 性能高效版
        size_t v1 = it->first.v1, v2 = it->first.v2;
        size_t vN = it->second.vN;
        size_t vOppo1 = it->second.voppo1, vOppo2 = it->second.voppo2;
        assert(vN != IndexNotValid);
        assert(v1 != IndexNotValid && v2 != IndexNotValid);
        assert(vOppo1 != IndexNotValid);
        vec3 v1_pos = newMesh.vertexData[v1].Position, v2_pos = newMesh.vertexData[v2].Position;
        if (vOppo2 == IndexNotValid) {
            newMesh.vertexData[vN].Position = vec3(0.5f)*(v1_pos+v2_pos);
        } else {
            vec3 vNOppo1_pos = newMesh.vertexData[vOppo1].Position;
            vec3 vNOppo2_pos = newMesh.vertexData[vOppo2].Position;
            vec3 new_pos = vec3(0.375f)*(v1_pos+v2_pos) + vec3(0.125f)*(vNOppo1_pos+vNOppo2_pos);
            newMesh.vertexData[vN].Position = new_pos;
        }
    }
    ... ...
}
```
### 调整原有旧顶点的坐标
遍历原（粗）网格的旧有顶点，逐个更新位置。关于它是否位于边界，可以用`edgeVertice`中对应项的`voppo2`变量判断。
```cpp
zyMesh LoopSubdivide_native(zyMesh & mesh) {
	... ...
	// 顶点位置的调整(调整的是原顶点，依据的也是原顶点的拓扑关系，新加顶点不用动也不产生影响)
    for (size_t v = 0; v < nVerts; v++) {
        vec3 new_pos = vec3(0.0f), adj_boundary_pos = vec3(0.0f);
        unsigned adj_cnt = 0, adj_boundary_cnt = 0;
        std::vector<size_t> const & out_he_lists = mesh.vertexList[v].outgoing_halfedge_ids;
        for (unsigned i_ngb = 0; i_ngb < out_he_lists.size(); i_ngb++) {
            size_t vv = mesh.halfedgeList[out_he_lists[i_ngb]].to_vertex_id;
            int loc = std::min(vv,v)*3*nVerts + std::max(vv,v)*3;
            if (edgeVertice[Index_Pair2(std::min(v,vv), std::max(v,vv))].voppo2 == IndexNotValid) {//这个边点对着的第二个顶点为空，说明是边界边的边点
                adj_boundary_cnt++;
                adj_boundary_pos += mesh.vertexData[vv].Position;
            }
            new_pos += mesh.vertexData[vv].Position;
            adj_cnt++;
        }
        
        if (adj_boundary_cnt == 2) {
            new_pos = vec3(0.75f) * mesh.vertexData[v].Position + vec3(0.125f)*adj_boundary_pos;
        } else {
            double val = 0.375 + 0.25*cos(2*PI/(double)adj_cnt);
            double beta = (0.625 - val*val)/(double)adj_cnt;
            new_pos = vec3((1.0 - beta*(double)adj_cnt))*mesh.vertexData[v].Position + vec3(beta)*new_pos;
        }

        newMesh.vertexData[v].Position = new_pos;//设置到新的细网格位置
    }
    ... ...
}
```
### 加入新的网格拓扑（点边面关系）
这一步很简单，只是根据已经得到的新（细）网格上的所有顶点坐标，和组成面的顶点序号信息，通过调用`zyMesh::add_face()`函数构建一张新的网格。
```cpp
zyMesh LoopSubdivide_native(zyMesh & mesh) {
	... ...
	// 在新的加密网格上构建面的拓扑
    int new_nFaces = newFaces_ptr / 3;
    for (int f = 0; f < new_nFaces; f++) {
        int loc = f*3;
        std::vector<size_t> face_vh = {newFaces[loc], newFaces[loc+1], newFaces[loc+2]};
        newMesh.add_face(face_vh);
    }

    delete[] newFaces;
    return newMesh;
}
```
## 交互控制加密程度
类似B-7作业QSlim中交互控制简化程度。程序中设置了最大细分等级`max_level`（用户运行程序时输入的参数，具体运行方式见README.md文件），在渲染循环中，每帧都调用`processInput()`函数，来根据用户对键盘左右方向键的长按操作来实现细分等级的切换。程序动态根据用户希望的细分等级来**实时**生成加密网格，并加入到网格列表`meshList`中。
```cpp
void renderMain(GLFWwindow * window, std::string filename_nopfx, Shader & ourShader, std::vector<zyMesh> & meshList) {
    while (!glfwWindowShouldClose(window)) {
		... ...
        // input
        processInput(window);
        if (meshList.size()-1 < max_level && meshList.size()-1 <= subdvs_level_f) {// 如果当前meshList内已经有足够的（max_level）网格，则不再细分
            doSubdivision(meshList[subdivision_level], filename_nopfx, meshList);
        }
        ... ...
   }// while
}

void processInput(GLFWwindow *window) {
	... ...
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        subdvs_level_f = std::max(0.0f, subdvs_level_f-0.001f);
        subdivision_level = (int)subdvs_level_f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        subdvs_level_f = std::min((float)max_level, subdvs_level_f+0.001f);
        subdivision_level = (int)subdvs_level_f;
    }
}
```
## 效果
### 斯坦福牛
加密等级为0，1，2，3时的网格，其中等级0的为原网格。
<table>
    <tr>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/083cb5d34298d1539d9085b46a6cea40.png" >0</center></td>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/972695b2fd6b36d45f9e768160f35035.png"  >1</center></td>
    </tr>
    <tr>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/4d293499ca1681deb9cfab9ad837eb60.png" >2</center></td>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/c1d97eaab452ab850ada06e190bffc89.png" >3</center></td>
    </tr>
    <tr>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/c4dd2751df7494e7df3f45887d29ea91.png" >2-局部</center></td>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/3c65aaeb906b62f358144ecda0ba2508.png" >3-局部</center></td>
    </tr>
</table>
等级为2，3的网格过于密集，局部放大图如上所示。

### Dinosaur
加密等级为0，1，2，3时的网格，其中等级0的为原网格。
<table>
    <tr>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/33373e278849c4681efd2a10f8bdece7.png" >0</center></td>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/8061be8b44ca09d9339d98a6c5f109ab.png"  >1</center></td>
    </tr>
    <tr>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/639132d50cc966f513344387c48d754b.png" >2</center></td>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/b19acefdd09ce006dbd21a2996b79883.png" >3</center></td>
    </tr>
    <tr>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/de75dee0e2db97e35767c9a9c6f6559a.png" >2-局部</center></td>
        <td ><center><img src="https://i-blog.csdnimg.cn/blog_migrate/9146b8dd67a12eb4f13c3e1dad0811d2.png" >3-局部</center></td>
    </tr>
</table>
等级为2，3的网格过于密集，局部放大图如上所示。

