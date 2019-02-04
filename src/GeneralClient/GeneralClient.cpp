/**
 * @file   GeneralClient.cpp
 * @author Winter Guerra
 * @brief  Pulls images from Unity and saves them as PNGs
 *space.
 **/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "GeneralClient.hpp"


#define SHOW_DEBUG_IMAGE_FEED true


///////////////////////
// Constructors
///////////////////////

GeneralClient::GeneralClient(){
  startTime = flightGoggles.getTimestamp();
}


////////////////////////////////////
// Example consumers and publishers
////////////////////////////////////

void imageConsumer(GeneralClient *self){
    while (true){
      // Wait for render result (blocking).
      unity_incoming::RenderOutput_t renderOutput = self->flightGoggles.handleImageResponse();

      // Display result
      if (SHOW_DEBUG_IMAGE_FEED){
        cv::imshow("Debug RGB", renderOutput.images[0]);
        cv::imshow("Debug D", renderOutput.images[1]);
        cv::waitKey(1);
      }
    }
}

void posePublisher(GeneralClient *self){
  // Sends render requests to FlightGoggles indefinitely
  while (true){
    // Update camera position
    self->updateCameraTrajectory();
    // Update timestamp of state message (needed to force FlightGoggles to rerender scene)
    self->flightGoggles.state.utime = self->flightGoggles.getTimestamp();
    // request render
    self->flightGoggles.requestRender();
    // Throttle requests to framerate.
    usleep(1e6/self->flightGoggles.state.maxFramerate);
    }
}

void GeneralClient::addCameras(){
  // Prepopulate metadata of cameras (RGBD)
  unity_outgoing::Camera_t cam_RGB;
  cam_RGB.ID = "Camera_RGB";
  cam_RGB.channels = 3;
  cam_RGB.isDepth = false;
  cam_RGB.outputIndex = 0;

  unity_outgoing::Camera_t cam_D;
  cam_D.ID = "Camera_D";
  cam_D.channels = 1;
  cam_D.isDepth = true;
  cam_D.outputIndex = 1;

  // Add cameras to persistent state
  flightGoggles.state.cameras.push_back(cam_RGB);
  flightGoggles.state.cameras.push_back(cam_D);
}

double px, py, pz = 1.5, roll, pitch, yaw = 0;

// void playerUpdate(){
//   if (kbhit() != 0) {
//     cout << getch() << endl;
//   }
// }

// Do a simple circular trajectory
void GeneralClient::updateCameraTrajectory(){
  double period = 15.0f;
  double r = 2.0f;
  double t = (flightGoggles.getTimestamp()-startTime)/1000000.0f;
  double theta = -((t/period)*2.0f*M_PI);
  
  Transform3 camera_pose;
  camera_pose.translation() = Vector3(px, py, pz);
  // Set rotation matrix using pitch, roll, yaw
  camera_pose.linear() = Eigen::AngleAxisd(0, Eigen::Vector3d(roll,pitch,yaw)).toRotationMatrix();

  // Populate status message with new pose
  flightGoggles.setCameraPoseUsingROSCoordinates(camera_pose, 0);
  flightGoggles.setCameraPoseUsingROSCoordinates(camera_pose, 1);
}



///////////////////////
// Example Client Node
///////////////////////

int main() {
  // Create client
  GeneralClient generalClient;

  // Instantiate RGBD cameras
  generalClient.addCameras();

  // Set scene parameters.
  generalClient.flightGoggles.state.maxFramerate = 60; 
  /*
  Available scenes: [
    "Hazelwood_Loft_Full_Night"
    "Hazelwood_Loft_Full_Day",
    "Butterfly_World",
    "FPS_Warehouse_Day",
    "FPS_Warehouse_Night",
  ]
   */
  generalClient.flightGoggles.state.sceneFilename = "Hazelwood_Loft_Full_Night";
  
  // Fork sample render request thread
  // will request a simple circular trajectory
  std::thread posePublisherThread(posePublisher, &generalClient);

  // Fork a sample image consumer thread
  std::thread imageConsumerThread(imageConsumer, &generalClient);

  // Spin
  while (true) {sleep(1);}

  return 0;
}
