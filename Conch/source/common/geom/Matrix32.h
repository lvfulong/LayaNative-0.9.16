﻿#ifndef __MATH_Matrix32_H__
#define __MATH_Matrix32_H__

#include <vector>

namespace laya{

class Matrix32{
public:
	/*
		0 1
		2 3
		4 5
	*/
	float	m[6];
	int	    bOnlyTrans;
	int	    bHasRotate;

public:
    Matrix32();
	static void lerp( Matrix32* mo,Matrix32* m1,Matrix32* m2,float f );
	static void lerp1(Matrix32* mo,std::vector<float> m1,std::vector<float> m2,float f );
	static void fromSRT( Matrix32* mo,float sx,float sy,float r,float tx,float ty );
	static void fromRST(Matrix32* mo, float sx,float sy,float r,float tx,float ty );
	static void mul( Matrix32* mo, Matrix32* m1, Matrix32* m2 );
    Matrix32* clone();
	void identity ();
    Matrix32* copy( Matrix32* p_pOther );
	bool IsEqual( Matrix32* p_pOther );
	void translate (float x, float y );	 
	void scale_rotate( float arc,float x,float y );
	void rotate ( float angle );
	void scale (float sx,float sy );
	void setTransform( float n11,float n12,float n21,float n22,float n31,float n32 );
	void transform( float n11,float n12,float n21,float n22,float n31,float n32 );
	void invert ();
	void calcTSR();
	void concat ( Matrix32* mtx );
	bool isIdentity();
    void transVert(float x, float y, float& ox, float& oy);
    void skew(float x, float y);
    float getScaleX();
    float getScaleY();
};
}
#endif
