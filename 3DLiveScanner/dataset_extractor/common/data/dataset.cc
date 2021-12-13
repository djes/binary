#include "data/dataset.h"
#include <sstream>

namespace oc {
    Dataset::Dataset(std::string path) {
        dataset = path;
    }

    std::string Dataset::GetFileName(int index, std::string extension) {
        std::ostringstream ss;
        ss << index;
        std::string number = ss.str();
        while(number.size() < 8)
            number = "0" + number;
        return dataset + "/" + number + extension;
    }

    std::vector<glm::mat4> Dataset::ReadPose(int index) {
        glm::mat4 mat;
        std::vector<glm::mat4> output;
        FILE* file = fopen(GetFileName(index, ".mat").c_str(), "r");
        for (int i = 0; i < MAX_CAMERA; i++) {
            for (int j = 0; j < 4; j++)
                fscanf(file, "%f %f %f %f\n", &mat[j][0], &mat[j][1], &mat[j][2], &mat[j][3]);
            output.push_back(mat);
        }
        fclose(file);
        return output;
    }

    void Dataset::ReadState(int &count, int &width, int &height, double& cx, double& cy, double& fx, double& fy) {
        FILE* file = fopen((dataset + "/state.txt").c_str(), "r");
        if (file) {
            fscanf(file, "%d %d %d %lf %lf %lf %lf\n", &count, &width, &height, &cx, &cy, &fx, &fy);
            fclose(file);
        }
    }

    float Dataset::ReadYaw() {
        float yaw = -90;
        FILE* file = fopen((dataset + "/rotation.txt").c_str(), "r");
        if (file) {
            fscanf(file, "%f\n", &yaw);
            fclose(file);
        }
        return yaw;
    }

    Tango3DR_PointCloud Dataset::ReadPointCloud(int index) {

        Tango3DR_PointCloud t3dr_depth;
        FILE* file = fopen(GetFileName(index, ".pcl").c_str(), "rb");
        fread(&t3dr_depth.num_points, sizeof(int), 1, file);

        t3dr_depth.points = new Tango3DR_Vector4[t3dr_depth.num_points];
        fread(t3dr_depth.points, sizeof(Tango3DR_Vector4), t3dr_depth.num_points, file);
        fclose(file);
        return t3dr_depth;
    }
}
