#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
	ROS_INFO_STREAM("Moving Robot");
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    if (!client.call(srv))
        ROS_ERROR("Failed to call");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
	int location_index = 0;
	bool white_pixel_present = false;
	int step = img.step;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
 	for (int i = 0; i < img.height * step; i++) {
        if (img.data[i] == white_pixel) {
            white_pixel_present = true;
			location_index = i;
            break;
        }
    }
	
	if(white_pixel_present){   //check if white ball is present
		int x = location_index % step; //location of the ball in image
		if(x<step*0.3) drive_robot(0, 0.5);
		else if(x>step*0.7) drive_robot(0, -0.5);
		else drive_robot(0.5, 0.0);
	} else drive_robot(0.0, 0.0); //stops the robot from moving

    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
