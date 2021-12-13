#ifndef DATA_DATASET_H
#define DATA_DATASET_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

typedef float Tango3DR_Vector4[4];

typedef struct Tango3DR_PointCloud {
  double timestamp;
  uint32_t num_points;
  Tango3DR_Vector4* points;
} Tango3DR_PointCloud;

namespace oc {

    enum Pose { COLOR_CAMERA, OPENGL_CAMERA, SCREEN_CAMERA, MAX_CAMERA };

    class Dataset {
    public:

        Dataset(std::string path);
        std::string GetFileName(int index, std::string extension);
        std::string GetPath() { return dataset; }

        Tango3DR_PointCloud ReadPointCloud(int index);
        std::vector<glm::mat4> ReadPose(int index);
        void ReadState(int& count, int& width, int& height, double& cx, double& cy, double& fx, double& fy);
        float ReadYaw();

    private:
        std::string dataset;
    };
}
#endif
