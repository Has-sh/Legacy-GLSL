#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glut.h"
#include "vendor/imgui/imgui_impl_opengl2.h"

struct Vertex {
    float x, y, z, w = 1.0;
};

std::vector<Vertex> vertices;
std::vector<std::vector<int>> faces;
std::vector<Vertex> originalVertices;

//new code
static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);//end of new code

bool parseOFF(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    if (!getline(file, line) || line != "OFF") {
        std::cerr << "Invalid OFF format: missing or incorrect header" << std::endl;
        return false;
    }

    int numVertices, numFaces, numEdges;
    if (!(file >> numVertices >> numFaces >> numEdges)) {
        std::cerr << "Failed to read OFF header" << std::endl;
        return false;
    }

    vertices.resize(numVertices);
    for (int i = 0; i < numVertices; ++i) {
        if (!(file >> vertices[i].x >> vertices[i].y >> vertices[i].z)) {
            std::cerr << "Failed to read vertex data" << std::endl;
            return false;
        }
    }

    faces.resize(numFaces);
    for (int i = 0; i < numFaces; ++i) {
        int numVerticesInFace;
        if (!(file >> numVerticesInFace) || numVerticesInFace < 3) {
            std::cerr << "Invalid number of vertices in face" << std::endl;
            return false;
        }
        faces[i].resize(numVerticesInFace);
        for (int j = 0; j < numVerticesInFace; ++j) {
            if (!(file >> faces[i][j]) || faces[i][j] < 0 || faces[i][j] >= numVertices) {
                std::cerr << "Invalid vertex index in face" << std::endl;
                return false;
            }
        }
    }
    originalVertices = vertices;
    file.close();
    return true;
}

Vertex calculateCentroid(const std::vector<Vertex>& vertices) {
    Vertex centroid = { 0.0f, 0.0f, 0.0f };

    for (const auto& vertex : vertices) {
        centroid.x += vertex.x;
        centroid.y += vertex.y;
        centroid.z += vertex.z;
    }

    centroid.x /= vertices.size();
    centroid.y /= vertices.size();
    centroid.z /= vertices.size();

    return centroid;
}

void rotateHelperX(float angle) {
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    float rotationMatrix[4][4] = {
    {1, 0, 0, 0},
    {0, cosAngle, -sinAngle, 0},
    {0, sinAngle, cosAngle, 0},
    {0, 0, 0, 1}
    };

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * rotationMatrix[0][0] + y * rotationMatrix[0][1] + z * rotationMatrix[0][2] + w * rotationMatrix[0][3];
        vertex.y = x * rotationMatrix[1][0] + y * rotationMatrix[1][1] + z * rotationMatrix[1][2] + w * rotationMatrix[1][3];
        vertex.z = x * rotationMatrix[2][0] + y * rotationMatrix[2][1] + z * rotationMatrix[2][2] + w * rotationMatrix[2][3];
        vertex.w = x * rotationMatrix[3][0] + y * rotationMatrix[3][1] + z * rotationMatrix[3][2] + w * rotationMatrix[3][3];
    }
}

void rotateHelperY(float angle) {
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    float rotationMatrix[4][4] = {
        {cosAngle, 0, sinAngle, 0},
        {0, 1, 0, 0},
        {-sinAngle, 0, cosAngle, 0},
        {0, 0, 0, 1}
    };

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * rotationMatrix[0][0] + y * rotationMatrix[0][1] + z * rotationMatrix[0][2] + w * rotationMatrix[0][3];
        vertex.y = x * rotationMatrix[1][0] + y * rotationMatrix[1][1] + z * rotationMatrix[1][2] + w * rotationMatrix[1][3];
        vertex.z = x * rotationMatrix[2][0] + y * rotationMatrix[2][1] + z * rotationMatrix[2][2] + w * rotationMatrix[2][3];
        vertex.w = x * rotationMatrix[3][0] + y * rotationMatrix[3][1] + z * rotationMatrix[3][2] + w * rotationMatrix[3][3];
    }
}

void rotateHelperZ(float angle) {
    angle = angle *  M_PI / 180.0;
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    float rotationMatrix[4][4] = {
        {cosAngle, -sinAngle, 0, 0},
        {sinAngle, cosAngle, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * rotationMatrix[0][0] + y * rotationMatrix[0][1] + z * rotationMatrix[0][2] + w * rotationMatrix[0][3];
        vertex.y = x * rotationMatrix[1][0] + y * rotationMatrix[1][1] + z * rotationMatrix[1][2] + w * rotationMatrix[1][3];
        vertex.z = x * rotationMatrix[2][0] + y * rotationMatrix[2][1] + z * rotationMatrix[2][2] + w * rotationMatrix[2][3];
        vertex.w = x * rotationMatrix[3][0] + y * rotationMatrix[3][1] + z * rotationMatrix[3][2] + w * rotationMatrix[3][3];
    }
}

void translateVertices(float tx, float ty, float tz) {
    float translationMatrix[4][4] = {
        {1, 0, 0, tx},
        {0, 1, 0, ty},
        {0, 0, 1, tz},
        {0, 0, 0, 1}
    };

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;

        vertex.x = x * translationMatrix[0][0] + y * translationMatrix[0][1] + z * translationMatrix[0][2] + translationMatrix[0][3];
        vertex.y = x * translationMatrix[1][0] + y * translationMatrix[1][1] + z * translationMatrix[1][2] + translationMatrix[1][3];
        vertex.z = x * translationMatrix[2][0] + y * translationMatrix[2][1] + z * translationMatrix[2][2] + translationMatrix[2][3];
    }
}

void rotateX(float angle) {
    angle = angle * M_PI / 180.0;
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    Vertex center;
    center = calculateCentroid(vertices);

    translateVertices(-center.x, -center.y, -center.z);

    float rotationMatrix[4][4] = {
        {1, 0, 0, 0},
        {0, cosAngle, -sinAngle, 0},
        {0, sinAngle, cosAngle, 0},
        {0, 0, 0, 1}
    };

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * rotationMatrix[0][0] + y * rotationMatrix[0][1] + z * rotationMatrix[0][2] + w * rotationMatrix[0][3];
        vertex.y = x * rotationMatrix[1][0] + y * rotationMatrix[1][1] + z * rotationMatrix[1][2] + w * rotationMatrix[1][3];
        vertex.z = x * rotationMatrix[2][0] + y * rotationMatrix[2][1] + z * rotationMatrix[2][2] + w * rotationMatrix[2][3];
        vertex.w = x * rotationMatrix[3][0] + y * rotationMatrix[3][1] + z * rotationMatrix[3][2] + w * rotationMatrix[3][3];
    }

    translateVertices(center.x, center.y, center.z);
}

void rotateY(float angle) {
    angle = angle * M_PI / 180.0;
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    Vertex center;

    float rotationMatrix[4][4] = {
        {cosAngle, 0, sinAngle, 0},
        {0, 1, 0, 0},
        {-sinAngle, 0, cosAngle, 0},
        {0, 0, 0, 1}
    };

    center = calculateCentroid(vertices);

    translateVertices(-center.x, -center.y, -center.z);

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * rotationMatrix[0][0] + y * rotationMatrix[0][1] + z * rotationMatrix[0][2] + w * rotationMatrix[0][3];
        vertex.y = x * rotationMatrix[1][0] + y * rotationMatrix[1][1] + z * rotationMatrix[1][2] + w * rotationMatrix[1][3];
        vertex.z = x * rotationMatrix[2][0] + y * rotationMatrix[2][1] + z * rotationMatrix[2][2] + w * rotationMatrix[2][3];
        vertex.w = x * rotationMatrix[3][0] + y * rotationMatrix[3][1] + z * rotationMatrix[3][2] + w * rotationMatrix[3][3];
    }

    translateVertices(center.x, center.y, center.z);
}

void rotateZ(float angle) {
    angle = angle * M_PI / 180.0;
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    Vertex center;

    float rotationMatrix[4][4] = {
        {cosAngle, -sinAngle, 0, 0},
        {sinAngle, cosAngle, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    center = calculateCentroid(vertices);

    translateVertices(-center.x, -center.y, -center.z);

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * rotationMatrix[0][0] + y * rotationMatrix[0][1] + z * rotationMatrix[0][2] + w * rotationMatrix[0][3];
        vertex.y = x * rotationMatrix[1][0] + y * rotationMatrix[1][1] + z * rotationMatrix[1][2] + w * rotationMatrix[1][3];
        vertex.z = x * rotationMatrix[2][0] + y * rotationMatrix[2][1] + z * rotationMatrix[2][2] + w * rotationMatrix[2][3];
        vertex.w = x * rotationMatrix[3][0] + y * rotationMatrix[3][1] + z * rotationMatrix[3][2] + w * rotationMatrix[3][3];
    }

    translateVertices(center.x, center.y, center.z);
}

void scale(float sx, float sy, float sz) {
    
    Vertex center;

    float scalingMatrix[4][4] = {
        {sx, 0, 0, 0},
        {0, sy, 0, 0},
        {0, 0, sz, 0},
        {0, 0, 0, 1}
    };

    center = calculateCentroid(vertices);

    translateVertices(-center.x, -center.y, -center.z);

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;

        vertex.x = x * scalingMatrix[0][0] + y * scalingMatrix[0][1] + z * scalingMatrix[0][2] + scalingMatrix[0][3];
        vertex.y = x * scalingMatrix[1][0] + y * scalingMatrix[1][1] + z * scalingMatrix[1][2] + scalingMatrix[1][3];
        vertex.z = x * scalingMatrix[2][0] + y * scalingMatrix[2][1] + z * scalingMatrix[2][2] + scalingMatrix[2][3];
    }

    translateVertices(center.x, center.y, center.z);
}

void shearX(float shY, float shZ) {

    Vertex center;
    center = calculateCentroid(vertices);

    translateVertices(-center.x, -center.y, -center.z);

    float shearingMatrix[4][4] = {
        {1, shY, shZ, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * shearingMatrix[0][0] + y * shearingMatrix[0][1] + z * shearingMatrix[0][2] + w * shearingMatrix[0][3];
        vertex.y = x * shearingMatrix[1][0] + y * shearingMatrix[1][1] + z * shearingMatrix[1][2] + w * shearingMatrix[1][3];
        vertex.z = x * shearingMatrix[2][0] + y * shearingMatrix[2][1] + z * shearingMatrix[2][2] + w * shearingMatrix[2][3];
        vertex.w = x * shearingMatrix[3][0] + y * shearingMatrix[3][1] + z * shearingMatrix[3][2] + w * shearingMatrix[3][3];

    }

    translateVertices(center.x, center.y, center.z);

}

void shearY(float shX, float shZ) {

    Vertex center;
    center = calculateCentroid(vertices);

    translateVertices(-center.x, -center.y, -center.z);

    float shearingMatrix[4][4] = {
        {1, 0, 0, 0},
        {shX, 1, shZ, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * shearingMatrix[0][0] + y * shearingMatrix[0][1] + z * shearingMatrix[0][2] + w * shearingMatrix[0][3];
        vertex.y = x * shearingMatrix[1][0] + y * shearingMatrix[1][1] + z * shearingMatrix[1][2] + w * shearingMatrix[1][3];
        vertex.z = x * shearingMatrix[2][0] + y * shearingMatrix[2][1] + z * shearingMatrix[2][2] + w * shearingMatrix[2][3];
        vertex.w = x * shearingMatrix[3][0] + y * shearingMatrix[3][1] + z * shearingMatrix[3][2] + w * shearingMatrix[3][3];

    }

    translateVertices(center.x, center.y, center.z);
}

void shearZ(float shX, float shY) {
    
    Vertex center;
    center = calculateCentroid(vertices);

    translateVertices(-center.x, -center.y, -center.z);
    
    float shearingMatrix[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {shX, shY, 1, 0},
        {0, 0, 0, 1}
    };

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * shearingMatrix[0][0] + y * shearingMatrix[0][1] + z * shearingMatrix[0][2] + w * shearingMatrix[0][3];
        vertex.y = x * shearingMatrix[1][0] + y * shearingMatrix[1][1] + z * shearingMatrix[1][2] + w * shearingMatrix[1][3];
        vertex.z = x * shearingMatrix[2][0] + y * shearingMatrix[2][1] + z * shearingMatrix[2][2] + w * shearingMatrix[2][3];
        vertex.w = x * shearingMatrix[3][0] + y * shearingMatrix[3][1] + z * shearingMatrix[3][2] + w * shearingMatrix[3][3];

    }

    translateVertices(center.x, center.y, center.z);
}

void rotateOverArbitrary(Vertex P1, Vertex P2, float angle) {
    Vertex axis = { P2.x - P1.x, P2.y - P1.y, P2.z - P1.z };
    float magnitude = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    axis.x /= magnitude;
    axis.y /= magnitude;
    axis.z /= magnitude;

    translateVertices(-P1.x, -P1.y, -P1.z);

    float d = sqrt(axis.y * axis.y + axis.z * axis.z);
    float e = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    
    if (d == 0) {
        rotateHelperX(angle * M_PI / 180.0);
    }
    else
    {
        float cos_alpha = axis.z / d;
        float sin_alpha = axis.y / d;
        float cos_beta = d / e;
        float sin_beta = axis.x / e;

        rotateHelperX(acosf(cos_alpha));
        rotateHelperY(asinf(sin_beta));

        rotateHelperZ(angle);

        rotateHelperY(-asinf(sin_beta));
        rotateHelperX(-acosf(cos_alpha));
    }

    translateVertices(P1.x, P1.y, P1.z);

}

void reflect() {
    float translationMatrix[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, -1, 0},
        {0, 0, 0, 1}
    };

    for (auto& vertex : vertices) {
        float x = vertex.x;
        float y = vertex.y;
        float z = vertex.z;
        float w = vertex.w;

        vertex.x = x * translationMatrix[0][0] + y * translationMatrix[0][1] + z * translationMatrix[0][2] + w * translationMatrix[0][3];
        vertex.y = x * translationMatrix[1][0] + y * translationMatrix[1][1] + z * translationMatrix[1][2] + w * translationMatrix[1][3];
        vertex.z = x * translationMatrix[2][0] + y * translationMatrix[2][1] + z * translationMatrix[2][2] + w * translationMatrix[2][3];
        vertex.w = x * translationMatrix[3][0] + y * translationMatrix[3][1] + z * translationMatrix[3][2] + w * translationMatrix[3][3];
    }
}

void reflectOverArbitrary(Vertex planePoint, Vertex planeNormal){

    translateVertices(-planePoint.x, -planePoint.y, -planePoint.z);

    float d = sqrt(planeNormal.y * planeNormal.y + planeNormal.z * planeNormal.z);
    float e = sqrt(planeNormal.x * planeNormal.x + planeNormal.y * planeNormal.y + planeNormal.z * planeNormal.z);
    
    if (d == 0) {
        float translationMatrix[4][4] = {
        {-1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
        };

        for (auto& vertex : vertices) {
            float x = vertex.x;
            float y = vertex.y;
            float z = vertex.z;
            float w = vertex.w;

            vertex.x = x * translationMatrix[0][0] + y * translationMatrix[0][1] + z * translationMatrix[0][2] + w * translationMatrix[0][3];
            vertex.y = x * translationMatrix[1][0] + y * translationMatrix[1][1] + z * translationMatrix[1][2] + w * translationMatrix[1][3];
            vertex.z = x * translationMatrix[2][0] + y * translationMatrix[2][1] + z * translationMatrix[2][2] + w * translationMatrix[2][3];
            vertex.w = x * translationMatrix[3][0] + y * translationMatrix[3][1] + z * translationMatrix[3][2] + w * translationMatrix[3][3];
        }
    }
    else {
        float cos_alpha = planeNormal.z / d;
        float sin_alpha = planeNormal.y / d;
        float cos_beta = d / e;
        float sin_beta = planeNormal.x / e;

        rotateHelperX(acosf(cos_alpha));
        rotateHelperY(asinf(sin_beta));

        reflect();

        rotateHelperY(-asinf(sin_beta));
        rotateHelperX(-acosf(cos_alpha));
    }
    translateVertices(planePoint.x, planePoint.y, planePoint.z);

}

void renderImGui() {
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    {
        static Vertex translation = { 0.0f,0.0f,0.0f };
        static Vertex scaler = { 1.0f,1.0f,1.0f };

        static float rotation_angle_x = 0.0f;
        static float rotation_angle_y = 0.0f;
        static float rotation_angle_z = 0.0f;

        ImGui::Begin("Transformation Controls");

        static Vertex prev_translation = { 0.0f,0.0f,0.0f };
        static Vertex prev_scaler = { 1.0f,1.0f,1.0f };

        static float prev_rotation_angle_x = 0.0f;
        static float prev_rotation_angle_y = 0.0f;
        static float prev_rotation_angle_z = 0.0f;

        if (ImGui::SliderFloat3("Translation", &translation.x, -1.0f, 1.0f)) {
            translateVertices(translation.x - prev_translation.x, translation.y - prev_translation.y, translation.z - prev_translation.z);
            prev_translation = translation;
        }
        if (ImGui::SliderFloat("Rotation X with respect to pivot", &rotation_angle_x, 0.0f, 360.0f)) {
            rotateX(rotation_angle_x - prev_rotation_angle_x);
            prev_rotation_angle_x = rotation_angle_x;
        }
        if (ImGui::SliderFloat("Rotation Y with respect to pivot", &rotation_angle_y, 0.0f, 360.0f)) {
            rotateY(rotation_angle_y - prev_rotation_angle_y);
            prev_rotation_angle_y = rotation_angle_y;
        }
        if (ImGui::SliderFloat("Rotation Z with respect to pivot", &rotation_angle_z, 0.0f, 360.0f)) {
            rotateZ(rotation_angle_z - prev_rotation_angle_z);
            prev_rotation_angle_z = rotation_angle_z;
        }
        if (ImGui::SliderFloat3("Scaling w.r.t point", &scaler.x, -1.0f, 1.0f)) {
            scale(scaler.x - prev_scaler.x, scaler.y - prev_scaler.y, scaler.z - prev_scaler.z);
            prev_scaler = scaler;
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::End();
    }

    ImGui::Render();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void renderScene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0); //comment for gui
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    for (const auto& face : faces) {
        for (size_t i = 0; i < face.size(); ++i) {
            const auto& vertex = vertices[face[i]];
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    glEnd();

    //renderImGui(); //uncomment for gui

    glutSwapBuffers();
    glutPostRedisplay();//new code
}

void initScene() {

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

}

void reshape(int w, int h) {
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void processNormalKeys(unsigned char key, int x, int y) {
    
    float shX, shY, shZ;
    float sx, sy, sz;
    float angle;
    float tx, ty, tz;
    Vertex P1, P2, n;

    switch (key) {
    case 't'://transformation
        printf("Enter translation values (tx ty tz): ");
        scanf_s("%f %f %f", &tx, &ty, &tz);
        translateVertices(tx, ty, tz);
        break;
    case 'x'://x axis rotation around pivot
        printf("Enter rotation angle around x-axis: ");
        scanf_s("%f", &angle);
        rotateX(angle);
        break;
    case 'y'://y axis rotation around pivot
        printf("Enter rotation angle around y-axis: ");
        scanf_s("%f", &angle);
        rotateY(angle);
        break;
    case 'z'://z axis rotation around pivot
        printf("Enter rotation angle around z-axis: ");
        scanf_s("%f", &angle);
        rotateZ(angle);
        break;
    case 's'://scaling w.r.t
        printf("Enter scaling factors (sx sy sz): ");
        scanf_s("%f %f %f", &sx, &sy, &sz);
        scale(sx, sy, sz);
        break;
    case 'q'://shear in X Direstion
        printf("Enter shearing factors (shY shZ) for shearing in X-Direction: ");
        scanf_s("%f %f", &shY, &shZ);
        shearX(shY, shZ);
        break;
    case 'w'://shear in Y Direstion
        printf("Enter shearing factors (shX shZ) for shearing in Y-Direction: ");
        scanf_s("%f %f", &shX, &shZ);
        shearY(shX, shZ);
        break;
    case 'e'://shear in Z Direstion
        printf("Enter shearing factors (shX shY) for shearing in Z-Direction: ");
        scanf_s("%f %f", &shX, &shY);
        shearZ(shX, shY);
        break;
    case 'r'://Rotation around arbitrary axis and parallel axis
        printf("Enter coordinates of two points (x1 y1 z1 x2 y2 z2) defining the axis: ");
        scanf_s("%f %f %f %f %f %f", &P1.x, &P1.y, &P1.z, &P2.x, &P2.y, &P2.z);
        printf("Enter rotation angle: ");
        scanf_s("%f", &angle);
        rotateOverArbitrary(P1, P2, angle);
        break;
    case 'u'://Reflection around arbitrary axis
        printf("Enter coordinates of a point and a normal (x1 y1 z1 nx ny nz) defining the plane: ");
        scanf_s("%f %f %f %f %f %f", &P1.x, &P1.y, &P1.z, &n.x, &n.y, &n.z);
        reflectOverArbitrary(P1, n);
        break;
    case 'a'://reset transformations
        vertices = originalVertices;
    default:
        break;
    }

    glutPostRedisplay();
}

void setupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL2_Init();
    ImGui_ImplGLUT_InstallFuncs();//end of new code
}

void cleanupImGui() {
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

int main(int argc, char** argv) {
    
    if (!parseOFF("off/6.off")) {
        return 1;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
    glutInitWindowPosition(200,100);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Assignment 2 CPU");
    
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    initScene();

    glutKeyboardFunc(processNormalKeys);

    //setupImGui();    //uncomment for gui

    glutMainLoop();

    //cleanupImGui();//uncomment for gui

    return 0;
}