#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader_class/shader.h"
#include "camera_class/camera.h"
#include "glad/gldebug.h"

#include <iostream>
#include "zyMesh.h"
#include <unordered_map>
#include <windows.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
static float PI = 3.1415926;

// camera
Camera camera(glm::vec3(2.0f, 2.0f, 1.0f));
// 鼠标状态
bool CURSOR_DISABLED = true;//是否选择cursor追随模式（FPS相机）
bool MOUSE_LEFT_ON = false;
bool MOUSE_RIGHT_ON = false;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting 全局vec3变量来表示光源在场景的世界空间坐标中的位置
glm::vec3 lightPos(3.0f, 3.0f, 0.0f);

// 模型显示状态
enum MODEL_SHOWTYPE {WIREFRAME, POINT_ONLY, FACE_ONLY, FACE_EDGE};
MODEL_SHOWTYPE showType = WIREFRAME;

class Index_Pair2 {
public:
    size_t v1, v2;
    Index_Pair2(): v1(IndexNotValid), v2(IndexNotValid) {}
    Index_Pair2(size_t v1, size_t v2): v1(v1), v2(v2) {}
    bool operator == (Index_Pair2 const & b) const {
        return v1 == b.v1 && v2 == b.v2;
    }
};

template <typename T>
inline void hash_combine(std::size_t &seed, const T &val) {
    seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
// auxiliary generic functions to create a hash value using a seed
template <typename T> inline void hash_val(std::size_t &seed, const T &val) {
    hash_combine(seed, val);
}
template <typename T, typename... Types> inline void hash_val(std::size_t &seed, const T &val, const Types &... args) {
    hash_combine(seed, val);
    hash_val(seed, args...);
}
template <typename... Types> inline std::size_t hash_val(const Types &... args) {
    std::size_t seed = 0;
    hash_val(seed, args...);
    return seed;
}

struct Index_Pair2_hash	{    
    size_t operator() (Index_Pair2 const & x) const {        
        return hash_val(x.v1, x.v2);
    }	
};

class Index_Pair3{
public:
    size_t vN, voppo1, voppo2;
    Index_Pair3():vN(IndexNotValid), voppo1(IndexNotValid), voppo2(IndexNotValid) {}
    Index_Pair3(size_t vN, size_t voppo1, size_t voppo2=IndexNotValid):vN(vN), voppo1(voppo1), voppo2(voppo2) {}
};

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
            // newMesh.add_vertex(glm::vec3(0.0f, 0.0f, 0.0f));
            newMesh.add_vertex(vec3(0.0f));

        size_t vqIndex = addEdgeVertice(edgeVertice, vids[1], vids[2], vids[0], newIndexOfVertices);
        if (vqIndex >= newMesh.vertexList.size())
            // newMesh.add_vertex(glm::vec3(0.0f, 0.0f, 0.0f));
            newMesh.add_vertex(vec3(0.0f));

        size_t vrIndex = addEdgeVertice(edgeVertice, vids[2], vids[0], vids[1], newIndexOfVertices);
        if (vrIndex >= newMesh.vertexList.size())
            // newMesh.add_vertex(glm::vec3(0.0f, 0.0f, 0.0f));
            newMesh.add_vertex(vec3(0.0f));
        
        newFaces[newFaces_ptr++] = vids[0]; newFaces[newFaces_ptr++] = vpIndex; newFaces[newFaces_ptr++] = vrIndex;
        newFaces[newFaces_ptr++] = vpIndex; newFaces[newFaces_ptr++] = vids[1]; newFaces[newFaces_ptr++] = vqIndex;
        newFaces[newFaces_ptr++] = vrIndex; newFaces[newFaces_ptr++] = vqIndex; newFaces[newFaces_ptr++] = vids[2];
        newFaces[newFaces_ptr++] = vrIndex; newFaces[newFaces_ptr++] = vpIndex; newFaces[newFaces_ptr++] = vqIndex;
    }

    //  更新插入顶点（新顶点位置直接确定）
    // for (size_t v1 = 0; v1 < nVerts; v1++){// 性能低效版
    //     // glm::vec3 v1_pos = newMesh.vertexData[v1].Position;
    //     vec3 v1_pos = newMesh.vertexData[v1].Position;
    //     for (size_t v2 = v1; v2 < nVerts; v2++){
    //         if (v1 == v2) continue;
    //         // glm::vec3 v2_pos = newMesh.vertexData[v2].Position;
    //         vec3 v2_pos = newMesh.vertexData[v2].Position;
    //         // int loc = v1*3*nVerts + v2*3;
    //         // int vNIndex = edgeVertice[loc];
    //         if (edgeVertice.find(Index_Pair2(v1,v2)) != edgeVertice.end()){
    //             Index_Pair3 newVert = edgeVertice[Index_Pair2(v1,v2)];
    //             if (newVert.vN != IndexNotValid) { 
    //                 size_t const & vN = newVert.vN;
    //                 if (newVert.voppo2 == IndexNotValid){//边界：没有第二个opposite顶点
    //                     // newMesh.vertexData[vN].Position = glm::vec3(0.5f)*(v1_pos+v2_pos);
    //                     newMesh.vertexData[vN].Position = vec3(0.5f)*(v1_pos+v2_pos);
    //                 } else {
    //                     vec3 vNOppo1_pos = newMesh.vertexData[newVert.voppo1].Position;
    //                     vec3 vNOppo2_pos = newMesh.vertexData[newVert.voppo2].Position;
    //                     vec3 new_pos = vec3(0.375f)*(v1_pos+v2_pos) + vec3(0.125f)*(vNOppo1_pos+vNOppo2_pos);
    //                     newMesh.vertexData[vN].Position = new_pos;
    //                 }
    //             }
    //         }
    //     }
    // }
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

    // 顶点位置的调整(调整的是原顶点，依据的也是原顶点的拓扑关系，新加顶点不用动也不产生影响)
    for (size_t v = 0; v < nVerts; v++) {
        // glm::vec3 new_pos(0.0f, 0.0f, 0.0f), adj_boundary_pos(0.0f, 0.0f, 0.0f);
        vec3 new_pos = vec3(0.0f), adj_boundary_pos = vec3(0.0f);
        unsigned adj_cnt = 0, adj_boundary_cnt = 0;
        std::vector<size_t> const & out_he_lists = mesh.vertexList[v].outgoing_halfedge_ids;
        for (unsigned i_ngb = 0; i_ngb < out_he_lists.size(); i_ngb++) {
            size_t vv = mesh.halfedgeList[out_he_lists[i_ngb]].to_vertex_id;
            int loc = std::min(vv,v)*3*nVerts + std::max(vv,v)*3;
            // if (edgeVertice[loc+2] == NotAVertex) {//这个边点对着的第二个顶点为空，说明是边界边的边点
            if (edgeVertice[Index_Pair2(std::min(v,vv), std::max(v,vv))].voppo2 == IndexNotValid) {//这个边点对着的第二个顶点为空，说明是边界边的边点
                adj_boundary_cnt++;
                adj_boundary_pos += mesh.vertexData[vv].Position;
            }
            new_pos += mesh.vertexData[vv].Position;
            adj_cnt++;
        }
        
        if (adj_boundary_cnt == 2) {
            new_pos = vec3(0.75f) * mesh.vertexData[v].Position + vec3(0.125f)*adj_boundary_pos;
            // new_pos = 0.75f * mesh.vertexData[v].Position + 0.125f * adj_boundary_pos;
        } else {
            double val = 0.375 + 0.25*cos(2*PI/(double)adj_cnt);
            double beta = (0.625 - val*val)/(double)adj_cnt;
            new_pos = vec3((1.0 - beta*(double)adj_cnt))*mesh.vertexData[v].Position + vec3(beta)*new_pos;
            // new_pos = (1.0 - beta*(double)adj_cnt) * mesh.vertexData[v].Position + beta * new_pos;
        }

        newMesh.vertexData[v].Position = new_pos;//设置到新的细网格位置
    }
    
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

int subdivision_level = 0, max_level = 2;
float subdvs_level_f = (float)subdivision_level;

GLFWwindow * initGLFW(unsigned int const major_version, unsigned int const minor_version) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);//如果需要请求调试输出

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "A-4", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (CURSOR_DISABLED) {
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// tell GLFW to capture our mouse
    }
    glfwSetScrollCallback(window, scroll_callback);
    return window;
}

void doSubdivision(zyMesh & mesh, std::string filename_nopfx, std::vector<zyMesh> & meshList) {
    unsigned long start_time, end_time;
    start_time = GetTickCount();
    zyMesh newMesh = LoopSubdivide_native(mesh);
    end_time = GetTickCount();
    std::cout << "level " << meshList.size() << " subdivision cost: " << end_time-start_time << " ms" << std::endl;
    std::string filename = filename_nopfx + "-dense-" + std::to_string(meshList.size()) + ".obj";
    newMesh.write_obj(filename.c_str());
    newMesh.setupMesh();//注意在LoopSubdivide_native里是没有setup的
    meshList.push_back(newMesh);
}

void renderMain(GLFWwindow * window, std::string filename_nopfx, Shader & ourShader, std::vector<zyMesh> & meshList) {
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        if (meshList.size()-1 < max_level && meshList.size()-1 <= subdvs_level_f) {// 如果当前meshList内已经有足够的（max_level）网格，则不再细分
            doSubdivision(meshList[subdivision_level], filename_nopfx, meshList);
        }

        // render
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);

        ourShader.setBool("sgColor", true);
        ourShader.setVec3("backColor", glm::vec3(160.0f, 32.0f, 240.0f)/255.0f);
        if (showType == FACE_ONLY || showType == FACE_EDGE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//面
        } else if (showType == WIREFRAME) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//边（网格线 wireframe）
        } else if (showType == POINT_ONLY) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);//点
        }
        
        meshList[subdivision_level].Draw(ourShader);

        if (showType == FACE_EDGE) {
            // 面+边（点）模式，则先画面，再画边（点）
            ourShader.setBool("sgColor", true);
            ourShader.setVec3("backColor", glm::vec3(0.0f, 0.0f, 0.0f));
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);//在面的基础上叠加上边
            glEnable(GL_POLYGON_OFFSET_LINE);
            // glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);//在面的基础上叠加上点
            // glEnable(GL_POLYGON_OFFSET_POINT);
            glPolygonOffset(-0.5, -0.5); // move closer to camera
            meshList[subdivision_level].Draw(ourShader);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main(int argc, char* argv[])
{
    GLFWwindow* window = initGLFW(4, 6);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){// glad: load all OpenGL function pointers
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);// configure global opengl state
    glLineWidth(1.0f);
    glPointSize(2.0f);

    glEnable(GL_CULL_FACE);//面剔除，如果想要背面也画出来（尤其是GL_LINE和GL_POINT模式下），则关掉这两行
    glCullFace(GL_BACK);//剔除掉背向面
    // build and compile shaders
    Shader ourShader("../resource/model_loading.vs", "../resource/model_loading.fs");

    max_level = atoi(argv[2]);
    std::string filename_nopfx = std::string(argv[1]);
    std::string filename = filename_nopfx + ".obj";// 注意输入参数是不带obj后缀的文件名
    
    std::vector<zyMesh> meshList;
    meshList.push_back(zyMesh(filename));

    // render loop
    renderMain(window, filename_nopfx, ourShader, meshList);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);


    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {//正在被按下
        if (MOUSE_LEFT_ON == false) {//之前没被按下
            MOUSE_LEFT_ON = true;
            //保存下来lastX和lastY
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastX = xpos;
            lastY = ypos;
        } else {//之前一直被按着
            //重新获得当前的位置
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            camera.ProcessMouseMovement(-xoffset, -yoffset);//更改摄像机位置
        }
    } else {//被松开
        MOUSE_LEFT_ON = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {//正在被按下
        if (MOUSE_RIGHT_ON == false) {//之前没被按下
            MOUSE_RIGHT_ON = true;
            //保存下来lastX和lastY
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastX = xpos;
            lastY = ypos;
        } else {//之前一直被按着
            //重新获得当前的位置
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            float sensitivity = 3e-3;
            float xoffset = (xpos - lastX)*sensitivity;
            float yoffset = (lastY - ypos)*sensitivity;
            lastX = xpos;
            lastY = ypos;

            camera.ProcessKeyboard(RIGHT, -xoffset);
            camera.ProcessKeyboard(UP   , -yoffset);
        }
    } else {//被松开
        MOUSE_RIGHT_ON = false;
    }

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        showType = WIREFRAME;
    } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        showType = POINT_ONLY;
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        showType = FACE_ONLY;
    } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        showType = FACE_EDGE;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        subdvs_level_f = std::max(0.0f, subdvs_level_f-0.01f);
        subdivision_level = (int)subdvs_level_f;
    }
        
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        subdvs_level_f = std::min((float)max_level, subdvs_level_f+0.01f);
        subdivision_level = (int)subdvs_level_f;
    }
        
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}