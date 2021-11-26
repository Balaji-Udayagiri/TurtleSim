#include "geometry_msgs/Twist.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "turtlesim/TeleportAbsolute.h"
#include "turtlesim/Pose.h"
#include "std_srvs/Empty.h"

/*the x,y,theta position of the turtlebot updated from the subscriber callback function*/
float x_m;
float y_m;
float theta_m;

/*Center of the rectangle*/
float centerx = 5.5;
float centery = 5.5;

/*Dimension of the rectangle*/
float step = 0.2;
float length = 10;
float width  = 3;

/*Angle*/
float angle = (50.0/180.0)*(3.1415);


/*Call back function of the Subscriber. Updates the x_m and y_m global variables*/
void Callback(turtlesim::Pose posemsg)
{
	x_m = posemsg.x;
	y_m = posemsg.y;
	theta_m = posemsg.theta;
	return;
}

int main(int argc, char **argv) 
{
  ros::init(argc, argv, "move");
  ros::NodeHandle n;

  /*Publishes the required velocity cmd to the turtle1/cmd_vel topic which the
   turle will subscribe form and move accordingly*/
	ros::Publisher chatter_pub = n.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);

	/*Subcribes from turtle1/pose topic the current coordinates of the turtle*/
	ros::Subscriber sub = n.subscribe("turtle1/pose", 1000, Callback);

	/*Clients for initializing the turtle*/
  ros::ServiceClient client = n.serviceClient<turtlesim::TeleportAbsolute>("turtle1/teleport_absolute");
  ros::ServiceClient client2 = n.serviceClient<std_srvs::Empty>("reset");
  std_srvs::Empty srv_clear;
  client2.call(srv_clear);
  turtlesim::TeleportAbsolute srv;
  srv.request.x = centerx + length*step;
  srv.request.y = centery + width*step;
  srv.request.theta = angle;	
	client.call(srv);

  ros::Rate loop_rate(10);
  int count = 0;
	int flag = 3;
  geometry_msgs::Twist msg;

  while(ros::ok())
  {

  	//STEP 1
		ROS_INFO("This is the position %f,%f,%f",x_m,y_m,theta_m);
		if(flag == 3 && (y_m-centery)>=(width*step - 0.01))
		{			
			ROS_INFO("I AM INSIDE");
			msg.linear.x = -step*cos(angle);
			msg.linear.y = step*sin(angle);
			msg.angular.x = 0;
			msg.angular.y = 0;
			msg.angular.z = 0;
			flag = 0;
		}
		if((x_m-centerx)>-length*step && flag == 0)
		{
			chatter_pub.publish(msg);
		}


		//STEP 2
		if(flag == 0 && (x_m-centerx)<=(-length*step +0.01 ))
		{
			flag = 1;
			msg.linear.x = -step*sin(angle);
			msg.linear.y = -step*cos(angle);
		}
			
		if( (y_m - centery)>-width*step && flag == 1) 
		{
			chatter_pub.publish(msg);
		}

		//STEP 3
		if(flag == 1 && (y_m-centery)<=(-width*step + 0.01))
		{
			flag = 2;
			msg.linear.x = step*cos(angle);
			msg.linear.y = -step*sin(angle);
		}
		if((x_m-centerx)<length*step && flag == 2)
		{
			chatter_pub.publish(msg);
		}

		//STEP 4
		if(flag == 2 && (x_m - centerx)>=(length*step - 0.01))
		{
			flag = 3;
			msg.linear.x = step*sin(angle);
			msg.linear.y = step*cos(angle);
		}

		if((y_m-centery)<width*step && flag == 3)
		{
			chatter_pub.publish(msg);
		}

  	ros::spinOnce();
    loop_rate.sleep();
	  ++count;
		
  }
  return 0;
}