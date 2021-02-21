#include "obs-detector.h"
using namespace std;

ObsDetector::ObsDetector(DataSource source, OperationMode mode, ViewerType viewer) : source(source), mode(mode), viewer(viewer), record(false)
{
    setupParamaters("");

    if(source == DataSource::ZED) {
        zed.open(init_params); 
        auto camera_config = zed.getCameraInformation(cloud_res).camera_configuration;
        defParams = camera_config.calibration_parameters.left_cam;
    } else if(source == DataSource::FILESYSTEM) {
        fileReader.open(readDir);
    }

    if(mode != OperationMode::SILENT && viewer == ViewerType::PCL) {
        //readData(); 
        //setPointCloud(0); 
        //pclViewer = createRGBVisualizer(pc_pcl);

    } else if(mode != OperationMode::SILENT && viewer == ViewerType::GL) {
        int argc = 1;
        char *argv[1] = {(char*)"Window"};
        glViewer.init(argc, argv, defParams);
        //graphicsThread = std::thread( [this] { this->spinViewer(); } );
        //graphicsThread.detach();
    }
};

//TODO: Make it read params from a file
void ObsDetector::setupParamaters(std::string parameterFile) {
    //Operating resolution
    cloud_res = sl::Resolution(320/2, 180/2);
    readDir = "../data/";

    //Zed params
    init_params.coordinate_units = sl::UNIT::MILLIMETER;
    init_params.camera_resolution = sl::RESOLUTION::VGA; 
    init_params.camera_fps = 100;
    
    //Set the viewer paramas
    defParams.fx = 79.8502;
    defParams.fy = 80.275;
    defParams.cx = 78.8623;
    defParams.cy = 43.6901;
    defParams.image_size.width = 160;
    defParams.image_size.height = 90;

    //Obs Detecting Algorithm Params
    passZ = new PassThrough('z', 200.0, 7000.0); //7000
    ransacPlane = new RansacPlane(sl::float3(0, 1, 0), 7, 400, 150, cloud_res.area());
    ece = new EuclideanClusterExtractor(100, 50, 0, cloud_res.area(), 9); 
}


/*
void ObsDetector::update() {
  //  if(source == sl::Mat )
} */


void ObsDetector::update() {
    //sl::Mat frame(cloud_res, sl::MAT_TYPE::F32_C4, sl::MEM::CPU); //ZED Renderer frame
    sl::Mat frame (cloud_res, sl::MAT_TYPE::F32_C4, sl::MEM::GPU);
    GPU_Cloud_F4 pc; //GPU processing cloud

    // Get the next frame from ZED
    if(source == DataSource::ZED) {

        zed.grab();
        zed.retrieveMeasure(frame, sl::MEASURE::XYZRGBA, sl::MEM::GPU, cloud_res); 
        pc = getRawCloud(frame);
    }
    // Get the next frame from a file
    else if(source == DataSource::FILESYSTEM) {
        fileReader.load(frameNum, frame);
    }

    // Processing 
    passZ->run(pc);
    ransacPlane->computeModel(pc, true);
    obstacles = ece->extractClusters(pc);
    


    //Rendering
    if(mode != OperationMode::SILENT) {
        clearStale(pc, cloud_res.area());
        if(viewer == ViewerType::GL) {
            glViewer.updatePointCloud(frame);
        }
    }

    frameNum++;
}

void ObsDetector::spinViewer() {
    glViewer.isAvailable();
    updateObjectBoxes(obstacles.size, obstacles.minX, obstacles.maxX, obstacles.minY, obstacles.maxY, obstacles.minZ, obstacles.maxZ );
}

 ObsDetector::~ObsDetector() {
     delete passZ;
     delete ransacPlane;
     delete ece;
 }

int main() {
    ObsDetector obs(DataSource::ZED, OperationMode::DEBUG, ViewerType::GL);
    //std::thread viewer(obs.spinViewer);
    Timer obsTimer("Obs");
    while(true) {
        //cout << "hi" << endl;
        obsTimer.reset();
        obs.spinViewer();
        obs.update();
    }
    return 0;
}