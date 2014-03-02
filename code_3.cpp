#include<highgui.h>
#include<cv.h>
#include <string.h>
#include <sstream>
#include <stdlib.h>
using namespace cv;
using namespace std;


struct Element
{
	char type; // r = resistor , i = inductor , c = capacitor , s = short
	Point node_coord[2]; // 0 = node1x , 1 = node1y , 2 = node2x , 3 = node2y
	int node_nos[2]; // 0 - node1 wrt matrix , 1 = node2 wrt matrix
};

vector<Element> el;


int hor_nodes[126][5];
int vert_nodes[8][25];
int no_of_nodes=830;
vector<Point2f> cent;
int breaks[20];
int brk_counter=0;
Scalar wire_colour;
Mat drawing,circuit,dashboard;
int sel=0;
int res_enable=0;

void draw_default()
{
	rectangle(dashboard,Rect(Point(0,0),Point(100,100)),Scalar(255,0,0),-1);
	rectangle(dashboard,Rect(Point(100,0),Point(200,100)),Scalar(0,255,0),-1);
	rectangle(dashboard,Rect(Point(200,0),Point(300,100)),Scalar(0,0,255),-1);
	rectangle(dashboard,Rect(Point(300,0),Point(400,100)),Scalar(0,0,0),-1);
	rectangle(dashboard,Rect(Point(0,100),Point(400/3,200)),Scalar(255,0,255),-1);
	rectangle(dashboard,Rect(Point(400/3,100),Point(800/3,200)),Scalar(255,255,0),-1);
	rectangle(dashboard,Rect(Point(800/3,100),Point(400,200)),Scalar(0,255,255),-1);
	putText(dashboard, "Short wires", cvPoint(150,20), 1,1.5,Scalar(0,0,0));
	//Resistors
	
	putText(dashboard, "Resistors", cvPoint(0,110), 1,1,Scalar(0,0,0));
	line(dashboard,Point(0,125),Point(400/3,125),Scalar(0,0,0),1);
	putText(dashboard, "0.1 k ohm", cvPoint(35,120), 1,1,Scalar(0,0,0));
	line(dashboard,Point(0,150),Point(400/3,150),Scalar(0,0,0),1);
	putText(dashboard, "1 k ohm", cvPoint(35,140), 1,1,Scalar(0,0,0));
	line(dashboard,Point(0,175),Point(400/3,175),Scalar(0,0,0),1);
	putText(dashboard, "10 k ohm", cvPoint(35,165), 1,1,Scalar(0,0,0));
	putText(dashboard, "100 k ohm", cvPoint(35,190), 1,1,Scalar(0,0,0));
	
	//Calacitors
	
	putText(dashboard, "Capacitors", cvPoint(400/3+0,110), 1,1,Scalar(0,0,0));
	line(dashboard,Point(400/3,125),Point(800/3,125),Scalar(0,0,0),1);
	putText(dashboard, "1 pF", cvPoint(400/3+35,120), 1,1,Scalar(0,0,0));
	line(dashboard,Point(400/3,150),Point(800/3,150),Scalar(0,0,0),1);
	putText(dashboard, "100 nF", cvPoint(400/3+35,140), 1,1,Scalar(0,0,0));
	line(dashboard,Point(400/3,175),Point(800/3,175),Scalar(0,0,0),1);
	putText(dashboard, "1  nF", cvPoint(400/3+35,165), 1,1,Scalar(0,0,0));
	putText(dashboard, "100 uF", cvPoint(400/3+35,190), 1,1,Scalar(0,0,0));
	
	
	//Inductors
	
	putText(dashboard, "Inductors", cvPoint(800/3,110), 1,1,Scalar(0,0,0));
	line(dashboard,Point(800/3,125),Point(400,125),Scalar(0,0,0),1);
	putText(dashboard, "1 uH", cvPoint(800/3+35,120), 1,1,Scalar(0,0,0));
	line(dashboard,Point(800/3,150),Point(400,150),Scalar(0,0,0),1);
	putText(dashboard, "10 uH", cvPoint(800/3+35,140), 1,1,Scalar(0,0,0));
	line(dashboard,Point(800/3,175),Point(400,175),Scalar(0,0,0),1);
	putText(dashboard, "100 uH", cvPoint(800/3+35,165), 1,1,Scalar(0,0,0));
	putText(dashboard, "1 mH", cvPoint(800/3+35,190), 1,1,Scalar(0,0,0));
}

int dist(Point a,Point b)
{
	return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}
void get_node_name(int x, int y, char str[])
{
	if ( y == 14 )
	{
		if( x <= 252 )
		{
			strcpy(str, "nv1");
		}
		else strcpy(str, "nv2");
	}
	else if ( y == 22 )
	{
		if( x <= 252 )
		{
			strcpy(str, "nv3");
		}
		else strcpy(str, "nv4");
	}
	else if ( y == 156 )
	{
		if( x <= 252 )
		{
			strcpy(str, "nv5");
		}
		else strcpy(str, "nv6");
	} 
	else if ( y == 164 )
	{
		if( x <= 252 )
		{
			strcpy(str, "nv7");
		}
		else strcpy(str, "nv8");
	}
	else if ( y <=78 )
	{
		int p = (x -12) / 8;
		strcpy(str, "nh");	
		stringstream ss;
		ss << p;
		string temp = ss.str();
		strcat(str, temp.c_str());
	}
	else 
	{
		int p = (x -12) / 8 + 63;
		strcpy(str, "nh");	
		stringstream ss;
		ss << p;
		string temp = ss.str();
		strcat(str, temp.c_str());
	}
}

void details()
{
	cout<<"\nElements details\n\n";
	char str[10];
	for(int i=0;i<el.size();i++)
	{
		cout<<el[i].type;
		get_node_name(el[i].node_coord[0].x, el[i].node_coord[0].y, str);
		//cout<<"\n"<<el[i].node_coord[0].x<<"   " <<el[i].node_coord[0].y<<"\n";
		cout<<"\n"<<str<<"\n";
		//cout<<"\n"<<el[i].node_coord[1].x<<"   " <<el[i].node_coord[1].y<<"\n";
		get_node_name(el[i].node_coord[1].x, el[i].node_coord[1].y, str);
		cout<<"\n"<<str<<"\n";
	}
	
}	
	
int draw_resistor(Point a,Point b,int ppp)
{
	line(circuit,a,b,Scalar(137,137,139),1);
	line(drawing,a,b,Scalar(137,137,139),1);
	
	
	Point center;
	center.x=(a.x+b.x)/2;
	center.y=(a.y+b.y)/2;
	float xoff=(b.x-a.x)/3;
	float yoff=(b.y-a.y)/3;
	Point p1,p2;
	p1.x=center.x-xoff;
	p2.x=center.x+xoff;
	p1.y=center.y-yoff;
	p2.y=center.y+yoff;
	
	if(ppp==1)
	line(circuit,p1,p2,Scalar(255,0,255),6);
	
	else if(ppp==2)
	line(circuit,p1,p2,Scalar(255,255,0),6);
	
	else if(ppp==3)
	line(circuit,p1,p2,Scalar(0,255,255),6);
	
	/*
	if (a.y-b.y<3)
	{
		rectangle(circuit,Rect(Point(a.x+abs(b.x-a.x)/4,a.y-2),Point(b.x-abs(b.x-a.x)/4,a.y+2)),Scalar(173,220,255),-1);
	}
	else
	{
		rectangle(circuit,Rect(Point(a.x-2,a.y+abs(b.y-a.y)/4),Point(a.x+2,b.y-abs(b.y-a.y)/4)),Scalar(173,220,255),-1);
	}
	*/
	res_enable=0;
}




Point selection;
double imp_val;		//res = resistance value
char type;		// r- resistor		l-capacitor		i-inductor	s-short
void CallBackFunc1(int event, int x, int y, int flags, void* userdata)
{

	
	
	if  ( event == EVENT_LBUTTONDOWN )
	{
		
		draw_default();
		imshow("Dashboard",dashboard);
		if (y<=100)
		{
			
			Vec3b temp=dashboard.at<Vec3b>(y,x);
			wire_colour=Scalar(temp[0],temp[1],temp[2]);
		}
		else if(y>100)
		{
		
			if(x<400/3)
			{
			res_enable=1;
			type='r';
			if(y<125)
			{
				rectangle(dashboard,Rect(Point(0,100),Point(400/3,125)),Scalar(255,255,255),-1);
				imp_val=100;
			}
			else if(y<150)
			{
				rectangle(dashboard,Rect(Point(0,125),Point(400/3,150)),Scalar(255,255,255),-1);
				imp_val=1000;
			}
			else if(y<175)
			{
				imp_val=10000;
				rectangle(dashboard,Rect(Point(0,150),Point(400/3,175)),Scalar(255,255,255),-1);
			}
			else 
			{
				imp_val=100000;
				rectangle(dashboard,Rect(Point(0,175),Point(400/3,200)),Scalar(255,255,255),-1);
			}
			
			}
			
			else if(x<800/3)
			{
			res_enable=2;
			type='c';
			if(y<125)
			{
				imp_val=pow(10,-12);
				rectangle(dashboard,Rect(Point(400/3,100),Point(800/3,125)),Scalar(255,255,255),-1);
			}
			else if(y<150)
			{
				imp_val=pow(10,-11);
				rectangle(dashboard,Rect(Point(400/3,125),Point(800/3,150)),Scalar(255,255,255),-1);
			}
			else if(y<175)
			{
				imp_val=pow(10,-9);
				rectangle(dashboard,Rect(Point(400/3,150),Point(800/3,175)),Scalar(255,255,255),-1);
			}
			else 
			{
				imp_val=pow(10,-8);
				rectangle(dashboard,Rect(Point(400/3,175),Point(800/3,200)),Scalar(255,255,255),-1);
			}
			
			}
			
			else if(x<400)
			{
			res_enable=3;
			type='l';
			if(y<125)
			{
				imp_val=pow(10,-6);
				rectangle(dashboard,Rect(Point(800/3,100),Point(400,125)),Scalar(255,255,255),-1);
			}
			else if(y<150)
			{
				imp_val=pow(10,-5);
				rectangle(dashboard,Rect(Point(800/3,125),Point(400,150)),Scalar(255,255,255),-1);
			}
			else if(y<175)
			{
				imp_val=pow(10,-4);
				rectangle(dashboard,Rect(Point(800/3,150),Point(400,175)),Scalar(255,255,255),-1);
			}
			else
			{
				rectangle(dashboard,Rect(Point(800/3,175),Point(400,200)),Scalar(255,255,255),-1); 
				imp_val=pow(10,-3);
			}
			
			}
			
		}
		
		imshow("Dashboard",dashboard);
		
	}
}	


void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if  ( event == EVENT_LBUTTONDOWN )
	{
		Element e;
		int min_dist=32000,min_index[2];
		cout<<y<<" "<<cent[vert_nodes[4][0]].y<<" "<<cent[vert_nodes[3][0]].y<<endl;
		if (y>=cent[vert_nodes[4][0]].y || y<=cent[vert_nodes[3][0]].y)
		{
			for (int i=0;i<8;i++)
			{
				for (int j=0;j<25;j++)
				{
					if (dist(Point(x,y),cent[vert_nodes[i][j]])<min_dist)
					{
						min_dist=dist(Point(x,y),cent[vert_nodes[i][j]]);
						min_index[0]=i;
						min_index[1]=j;
					}
				}
			}
			if (min_dist<1000)
			{
				if (sel==0)
				{
					selection=cent[vert_nodes[min_index[0]][min_index[1]]];
					circle(drawing,cent[vert_nodes[min_index[0]][min_index[1]]],2,Scalar(0,255,0),-1);
					sel=1;
				}
				else
				{
					if (res_enable==0)
					{
						line(drawing,selection,cent[vert_nodes[min_index[0]][min_index[1]]],wire_colour,1);
						line(circuit,selection,cent[vert_nodes[min_index[0]][min_index[1]]],wire_colour,3);
						e.type='s';
						e.node_coord[0]=selection;
						e.node_coord[1]=cent[vert_nodes[min_index[0]][min_index[1]]];
						el.push_back(e);
					}
					else 
					{
						if(res_enable==1)
							e.type='r';
						else if(res_enable==2)
							e.type='c';
						else if(res_enable==3)
							e.type='l';
						draw_resistor(selection,cent[vert_nodes[min_index[0]][min_index[1]]],res_enable);
						cout<<"man"<<res_enable<<"\n";
						
						e.node_coord[0]=selection;
						e.node_coord[1]=cent[vert_nodes[min_index[0]][min_index[1]]];
						el.push_back(e);
					}
					imshow("circuit",circuit);	
					sel=0;
				}
				imshow("drawing",drawing);
			}	
		}	
		else
		{
			for (int i=0;i<126;i++)
			{
				for (int j=0;j<5;j++)
				{
					if (dist(Point(x,y),cent[hor_nodes[i][j]])<min_dist)
					{
						min_dist=dist(Point(x,y),cent[hor_nodes[i][j]]);
						min_index[0]=i;
						min_index[1]=j;
					}
				}
			}
			if (min_dist<1000)
			{
				if (sel==0)
				{
					selection=cent[hor_nodes[min_index[0]][min_index[1]]];
					circle(drawing,cent[hor_nodes[min_index[0]][min_index[1]]],2,Scalar(0,255,0),-1);
					sel=1;
				}
				else
				{
					if (res_enable==0)
					{	
						line(drawing,selection,cent[hor_nodes[min_index[0]][min_index[1]]],wire_colour,1);
						line(circuit,selection,cent[hor_nodes[min_index[0]][min_index[1]]],wire_colour,3);
						e.type='s';
						e.node_coord[0]=selection;
						e.node_coord[1]=cent[hor_nodes[min_index[0]][min_index[1]]];
						el.push_back(e);
					}
					else 
					{
						
						if(res_enable==1)
							e.type='r';
						else if(res_enable==2)
							e.type='c';
						else if(res_enable==3)
							e.type='l';
						draw_resistor(selection,cent[hor_nodes[min_index[0]][min_index[1]]],res_enable);
						
						
						e.node_coord[0]=selection;
						e.node_coord[1]=cent[hor_nodes[min_index[0]][min_index[1]]];
						el.push_back(e);
					}
					imshow("circuit",circuit);
					sel=0;
				}
				imshow("drawing",drawing);
			}
		}
		details();
	}
	else if  ( event == EVENT_RBUTTONDOWN )
	{
	
	}
	else if  ( event == EVENT_MBUTTONDOWN )
	{
		
		//middle click
	}
     	else if ( event == EVENT_MOUSEMOVE )
     	{
        	//mouse motion
        	
     	}
     	
     	
	
}

int classify(int size)
{
	for (int j=0;j<4;j++)
	{
		for (int i=0;i<25;i++)
		{
			vert_nodes[j][i]=25*j+i;
		}
	}
	for (int j=4;j<8;j++)
	{
		for (int i=0;i<25;i++)
		{
			vert_nodes[j][i]=730+25*(j-4)+i;
		}
	}
	int start=100;
	for (int i=0;i<63;i++)
	{
		for (int j=0;j<5;j++)
		{
			hor_nodes[i][j]=(start+i)+63*j;
		}
	}
	start=414;
	for (int i=0;i<63;i++)
	{
		for (int j=0;j<5;j++)
		{
			hor_nodes[63+i][j]=(start+i)+63*j;
		}
	}
}


int find_breaks(int size)
{
	int eps=5;
	for (int i=0;i<size-1;i++)
	{
		//cout<<cent[i].y<<endl;
		if (abs(cent[i].y-cent[i+1].y)>=eps)
		{
			cout<<"here"<<endl;
			breaks[brk_counter]=i+1;
			brk_counter++;
		}
	}
}


int sort_vec_y(int size)
{
	Point temp;
	for (int i=0;i<size;i++)
	{
		for (int j=i;j<size;j++)
		{
			if (cent[i].y>cent[j].y)
			{
				temp=cent[i];
				cent[i]=cent[j];
				cent[j]=temp;
			}
		}
	}
}


int sort_vec_x(int size)
{
	int end;
	Point temp;
	for (int i=0;i<brk_counter;i++)
	{
		if (i==brk_counter-1)
			end=size;
		else
			end=breaks[i+1];
		for (int j=breaks[i];j<end;j++)
		{
			for (int k=j;k<end;k++)
			{
				if (cent[j].x>cent[k].x)
				{	
					temp=cent[j];
					cent[j]=cent[k];
					cent[k]=temp;
				}
			}
		}
	}
}



int main(int argc,char **argv)
{
	Mat a=imread(argv[1]);
	resize(a,a,Size(),0.5,0.5);
	circuit=a.clone();
	cvtColor(a,a,CV_BGR2GRAY);
	threshold (a,a,70,255,CV_THRESH_BINARY);
	Mat ker=cvCreateMat(3,3,CV_32FC1);
	erode(a,a,ker);
	Mat con_inp=a.clone();
	dashboard=cvCreateMat(200,400,CV_8UC3);
	dashboard=Scalar(255,255,255);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours( con_inp, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	drawing = Mat::zeros( a.size(), CV_8UC3 );
	vector<Moments> mu(contours.size() );
  	for( int i = 1; i < contours.size(); i++ )
	{
		mu[i] = moments( contours[i], false );
	 }
	///  Get the mass centers:
  	vector<Point2f> mc( contours.size() );
  	for( int i = 1; i < contours.size(); i++ )
     	{
     		mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); 
     	}
     	cent=mc;
	sort_vec_y(no_of_nodes);
	find_breaks(no_of_nodes);
	//cout<<"breaks "<<brk_counter<<endl;
	//for (int i=0;i<brk_counter;i++)
	//{
	//circle(drawing,cent[breaks[i]],2,Scalar(0,255,255),-1);
	//cout<<breaks[i]<<endl;
	//}
	sort_vec_x(no_of_nodes);
	classify(no_of_nodes);
	for (int i=0;i<8;i++)
	{
		for (int j=0;j<25;j++)
		{
			circle(drawing,cent[vert_nodes[i][j]],1,Scalar(0,0,255),-1);
			imshow("drawing",drawing);
			//cvWaitKey(1);
			//drawContours( drawing, contours, i, Scalar(0,255,0), 1, 8, vector<Vec4i>(), 0, Point() );
		}
	}
	for (int i=0;i<126;i++)
	{
		for (int j=0;j<5;j++)
		{
			circle(drawing,cent[hor_nodes[i][j]],1,Scalar(0,0,255),-1);
			imshow("drawing",drawing);
			//cvWaitKey(1);
			//drawContours( drawing, contours, i, Scalar(0,255,0), 1, 8, vector<Vec4i>(), 0, Point() );
		}
	}
	
	draw_default();
	
		
	//rectangle(dashboard,Rect(Point(0,100),Point(400,200)),Scalar(255,0,255),-1);
	imshow("threshed",a);
	imshow("drawing",drawing);
	imshow("circuit",circuit);
	imshow("Dashboard",dashboard);
	moveWindow("drawing",600,0);
	moveWindow("Dashboard",0,400);
	setMouseCallback("circuit", CallBackFunc, NULL);
	setMouseCallback("Dashboard", CallBackFunc1, NULL);
	details();
	
	cvWaitKey(0);
}
