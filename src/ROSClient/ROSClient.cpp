/**
 * @file   SyntheticImagesPulisher.cpp
 * @author Muyuan Lin, adapted from @Winter Guerra
 * @brief  Pulls images from Unity and publish them as ROS images.
 *
 **/

#include "ROSClient.hpp"

#define SHOW_DEBUG_IMAGE_FEED false
typedef const boost::function< void(const nav_msgs::Odometry::ConstPtr &)> callback;

// Constructor
ROSClient::ROSClient(){
}

void ROSClient::populateRenderSettings(){
  // Scene/Render settings
  flightGoggles.state.maxFramerate = 60; 
  /*
  Available scenes:
  sceneFilename = "Butterfly_World";
  sceneFilename = "FPS_Warehouse_Day";
  sceneFilename = "FPS_Warehouse_Night";
  sceneFilename = "Hazelwood_Loft_Full_Day";
  sceneFilename = "Hazelwood_Loft_Full_Night";
   */
  flightGoggles.state.sceneFilename = "Hazelwood_Loft_Full_Night";
  
  // Prepopulate metadata of cameras
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


void ROSClient::poseSubscriber(const nav_msgs::Odometry::ConstPtr& msg){
    // ROS_INFO("Seq: [%d]", msg->header.seq);
    // Sends render requests to FlightGoggles indefinitely
    geometry_msgs::Pose cam_pose_tf = msg->pose.pose;


    // Old hack to convert ROS TF to ENU poses.
    // float x = cam_pose_tf.position.x;
    // cam_pose_tf.position.x = -cam_pose_tf.position.y;
    // cam_pose_tf.position.y = x;

    Transform3 cam_pose_eigen;
    poseMsgToEigen(cam_pose_tf, cam_pose_eigen);

    // Populate status message with new pose
    flightGoggles.setCameraPoseUsingROSCoordinates(cam_pose_eigen, 0);
    flightGoggles.setCameraPoseUsingROSCoordinates(cam_pose_eigen, 1);

    // Update timestamp of state message (needed to force FlightGoggles to rerender scene)
    flightGoggles.state.utime = flightGoggles.getTimestamp();
    // request render
    flightGoggles.requestRender();
    
}

///////////////////////
// Example Client Node
///////////////////////

void ROSClient::poseStampedSubscriber(const geometry_msgs::PoseStamped::ConstPtr& msg){
    // ROS_INFO("%lf\n", msg->pose.position.x);

    geometry_msgs::Pose cam_pose_tf = msg->pose;
    ROS_INFO("x = %lf\ty = %lf\tz = %lf\n", cam_pose_tf.position.x, cam_pose_tf.position.y, cam_pose_tf.position.z);
    ROS_INFO("w = %lf\tx = %lf\ty = %lf\tz = %lf\n", cam_pose_tf.orientation.w, cam_pose_tf.orientation.x, cam_pose_tf.orientation.y, cam_pose_tf.orientation.z);


    // Old hack to convert ROS TF to ENU poses.
    // float x = cam_pose_tf.position.x;
    // cam_pose_tf.position.x = -cam_pose_tf.position.y;
    // cam_pose_tf.position.y = x;

    Transform3 cam_pose_eigen;
    poseMsgToEigen(cam_pose_tf, cam_pose_eigen);

    // Populate status message with new pose
    flightGoggles.setCameraPoseUsingROSCoordinates(cam_pose_eigen, 0);
    flightGoggles.setCameraPoseUsingROSCoordinates(cam_pose_eigen, 1);

    // Update timestamp of state message (needed to force FlightGoggles to rerender scene)
    flightGoggles.state.utime = flightGoggles.getTimestamp();
    // request render
    flightGoggles.requestRender();

}

int main(int argc, char **argv) {
    ros::init(argc, argv, "pose_listener");

    ros::NodeHandle node;
    
    // Create client
    ROSClient client;

    // Load params
    client.populateRenderSettings();

    ROS_INFO("THIS WORKS\n");

    // callback poseCallback = boost::bind(&ROSClient::poseSubscriber, &client, _1);
    ros::Subscriber sub = node.subscribe("/quad_rotor/pose", 10, &ROSClient::poseStampedSubscriber, &client);

    // Spin
    ros::spin();

    return 0;
}
