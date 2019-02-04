#ifndef SYNTHETICIMAGEPUBLISHER_H
#define SYNTHETICIMAGEPUBLISHER_H
/**
 * @file   GeneralClient.hpp
 * @author Winter Guerra
 * @brief  Basic client interface for FlightGoggles.
 */

#include <FlightGogglesClient.hpp>
// #include <jsonMessageSpec.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/PoseStamped.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

class ROSClient {
public:
    // FlightGoggles interface object
	FlightGogglesClient flightGoggles;

	// constructor
	ROSClient();

	// Populate starting settings into state
	void populateRenderSettings();

	void poseSubscriber(const nav_msgs::Odometry::ConstPtr& msg);
  void poseStampedSubscriber(const geometry_msgs::PoseStamped::ConstPtr& msg);
private:
    void poseMsgToEigen(const geometry_msgs::Pose &m, Eigen::Affine3d &e)
    {
     	e = Eigen::Translation3d(m.position.x,
                              m.position.y,
                              m.position.z) *
        Eigen::Quaterniond(m.orientation.w,
                          m.orientation.x,
                          m.orientation.y,
                          m.orientation.z);
   }

};

#endif
