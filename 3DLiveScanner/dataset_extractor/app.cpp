#include <data/dataset.h>
#include <exporter/floorpln.h>
#include <exporter/ply.h>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        LOGI("Usage: ./dataset_extractor <path to the dataset>");
        return 0;
    }
    oc::Dataset* dataset = new oc::Dataset(std::string(argv[1]));
    oc::ExporterFloorplan().Process(dataset, dataset->GetPath() + "/");
    oc::ExporterPLY().Process(dataset, dataset->GetPath() + "/output.ply");
    return 0;
}
