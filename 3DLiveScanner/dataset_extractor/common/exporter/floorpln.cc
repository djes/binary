#include <data/file3d.h>
#include <exporter/floorpln.h>

namespace oc {


bool operator<(const FloorPlanPoint& lhs, const FloorPlanPoint& rhs)
{
    return lhs.x < rhs.x || (lhs.x == rhs.x && (lhs.z < rhs.z));
}

void ExporterFloorplan::Process(Dataset* dataset, std::string output)
{
    //init AABB
    int bigNumber = 9999999;
    min.x = bigNumber;
    min.z = bigNumber;
    max.x =-bigNumber;
    max.z =-bigNumber;

    //init dataset
    path = dataset->GetPath();
    int poseCount = GetPoseCount(dataset);

    //convert each frame
    for (int i = 0; i < poseCount; i++)
    {
        ConvertFrame(dataset, i);
    }
    min.x -= 10;
    min.z -= 10;
    max.x += 10;
    max.z += 10;

    FillHoles();
    Export(output);
}

void ExporterFloorplan::Process(oc::Mesh& pcl, std::vector<glm::mat4>& pose, int poseIndex)
{
    glm::mat4 matrix = pose[COLOR_CAMERA];
    for (glm::vec3& v : pcl.vertices)
    {
        if (v.y < matrix[3][1])
        {
            FloorPlanPoint p;
            p.x = v.x * 100;
            p.z = v.z * 100;

            if (heightmap.find(p) == heightmap.end())
            {
                heightmap[p] = v.y;

                if (min.x > p.x) min.x = p.x;
                if (min.z > p.z) min.z = p.z;
                if (max.x < p.x) max.x = p.x;
                if (max.z < p.z) max.z = p.z;
            }
            else if (heightmap[p] < v.y)
            {
                heightmap[p] = v.y;
            }
        }
    }
}

void ExporterFloorplan::Export(std::string output)
{
    int w = max.x - min.x + 1;
    int h = max.z - min.z + 1;
    LOGI("Exporting floorplan has dimensions %dcm x %dcm, coverage is %f%%", w, h, 100.0f * heightmap.size() / (float)(w * h));

    Image img(w, h);
    img.Clear();

    FloorPlanPoint p;
    for (p.x = min.x; p.x < max.x; p.x++)
    {
        for (p.z = min.z; p.z < max.z; p.z++)
        {
            if (heightmap.find(p) != heightmap.end())
            {
                int count = 0;
                float diff = 0;
                FloorPlanPoint q;
                float value = heightmap[p];
                for (q.x = p.x - 1; q.x <= p.x + 1; q.x++)
                {
                    for (q.z = p.z - 1; q.z <= p.z + 1; q.z++)
                    {
                        if (heightmap.find(q) != heightmap.end())
                        {
                            diff += fabs(value - heightmap[q]);
                            count++;
                        }
                    }
                }
                diff /= (float)count;
                int x = p.x - min.x;
                int y = p.z - min.z;
                float f = 1.0 - glm::clamp(diff * 2.0f, 0.0f, 1.0f);
                int r = 255 * f;
                int g = glm::clamp((int)(-value * 200.0f), 0, 255) * f;
                int b = glm::clamp((int)(-value * 100.0f), 0, 255) * f;
                img.GetData()[(y * w + x) * 4 + 0] = (r + g + b) / 3;
                img.GetData()[(y * w + x) * 4 + 1] = img.GetData()[(y * w + x) * 4 + 0];
                img.GetData()[(y * w + x) * 4 + 2] = img.GetData()[(y * w + x) * 4 + 0];
                img.GetData()[(y * w + x) * 4 + 3] = 255;
            }
        }
    }
    img.SetName("floorplan.png");
    img.Write(output + img.GetName());
}

void ExporterFloorplan::FillHoles()
{
    FloorPlanPoint p;
    std::map<FloorPlanPoint, bool> mask;
    for (p.x = min.x; p.x <= max.x; p.x++)
    {
        for (p.z = min.z; p.z <= max.z; p.z++)
        {
            if (heightmap.find(p) == heightmap.end())
            {
                for (int s = 0; s < 5; s++)
                {
                    int count = 0;
                    FloorPlanPoint q;
                    float value = 0;
                    for (q.x = p.x - s; q.x <= p.x + s; q.x++)
                    {
                        q.z = p.z - s;
                        if (mask.find(q) == mask.end())
                        {
                            if (heightmap.find(q) != heightmap.end())
                            {
                                value += heightmap[q];
                                count++;
                            }
                        }
                        q.z = p.z + s;
                        if (mask.find(q) == mask.end())
                        {
                            if (heightmap.find(q) != heightmap.end())
                            {
                                value += heightmap[q];
                                count++;
                            }
                        }
                    }
                    for (q.z = p.z - s + 1; q.z <= p.z + s - 1; q.z++)
                    {
                        q.x = p.x - s;
                        if (mask.find(q) == mask.end())
                        {
                            if (heightmap.find(q) != heightmap.end())
                            {
                                value += heightmap[q];
                                count++;
                            }
                        }
                        q.x = p.x + s;
                        if (mask.find(q) == mask.end())
                        {
                            if (heightmap.find(q) != heightmap.end())
                            {
                                value += heightmap[q];
                                count++;
                            }
                        }
                    }
                    if (count > 0)
                    {
                        heightmap[p] = value / (float)count;
                        mask[p] = true;
                        break;
                    }
                }
            }
        }
    }
}
}
