#ifndef _COORD_H
#define _COORD_H

class coord{
public:
	int MinX;
	int MaxX;
	int MinY;
	int MaxY;
	double cX;
	double cY;
	int lX;
	int lY;
	bool flag;

	coord(){
		MinX=0;
		MaxX=0;
		MinY=0;
		MaxY=0;
		cX=0;
		cY=0;
		lX=0;
		lY=0;
		flag=false;
	}

	coord(int x,int y){
		MinX=0;
		MaxX=0;
		MinY=0;
		MaxY=0;
		cX=x;
		cY=y;
		lX=0;
		lY=0;
		flag=true;
	}

	coord(int maxx,int minx, int maxy, int miny){
		MinX=minx;
		MaxX=maxy;
		MinY=miny;
		MaxY=maxy;
		cX=(MinX+MaxX)/2;
		cY=(MinY+MaxY)/2;
		lX=MaxX-MinX;
		lY=MaxY-MinY;
		flag=true;
	}

	void set(int x,int y){
		MinX = x - lX/2;
		MaxX = x + lX/2;
		MinY = y - lY/2;
		MaxY = y + lY/2;
		cX=x;
		cY=y;
		flag=true;

	}

	void set(int maxx,int minx, int maxy, int miny){
		MinX=minx;
		MaxX=maxx;
		MinY=miny;
		MaxY=maxy;
		cX=(MinX+MaxX)/2;
		cY=(MinY+MaxY)/2;
		lX=MaxX-MinX;
		lY=MaxY-MinY;
		flag=true;
	}

};

#endif
