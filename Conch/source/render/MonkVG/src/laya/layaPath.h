/**
@file			layaPath.h
@brief
@author			guo
@version		1.0
@date			2016_6_6
*/

#ifndef _LAYA_PATH_H__
#define _LAYA_PATH_H__

#include "mkPath.h"
#include "glPlatform.h"
#include <list>
#include <vector>
#include "glPaint.h"
#include <algorithm>
#include <geom/RectGeometry.h>

using std::min;
using std::max;

namespace MonkVG {
    class layaPaint;
    class IContext;
	class layaPath : public IPath {
	public:
	
        layaPath( VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities )
			:	IPath( pathFormat, datatype, scale, bias, segmentCapacityHint, coordCapacityHint, capabilities )
			,	_fillTesseleator( 0 )
			,	_fillPaintForPath( 0 )
			,	_strokePaintForPath( 0 )
		{

		}
		virtual ~layaPath();
		
		
		virtual bool draw( VGbitfield paintModes );
		virtual void clear( VGbitfield caps );
		virtual void buildFillIfDirty();

        void* getStrokeData(int& len) {
            len = _strokeVertices.size()*sizeof(laya::RectGeometry::VertexInfo);
            if (len <= 0)
                return nullptr;
            return &_strokeVertices[0];
        }
        void* getFillData(int& len) {
            len = _fillVertices.size()*sizeof(laya::RectGeometry::VertexInfo);
            if (len <= 0)
                return nullptr;
            return &_fillVertices[0];
        }
	private:
        struct v2_t {
            GLfloat x, y;
        };

        struct v3_t {
            GLdouble x, y, z;
            v3_t() {}
            v3_t(GLdouble * v) : x(v[0]), y(v[1]), z(v[2]) {}
            v3_t(GLdouble ix, GLdouble iy, GLdouble iz) : x(ix), y(iy), z(iz) {}
            void print() const {
                printf("(%f, %f)\n", x, y);
            }
        };

	private:
		
		GLUtesselator*		_fillTesseleator;
		vector<GLfloat>		_vertices;          //��ʽ�� 0,1=x,y, 2,3=x,y, ...
                                                //fill���м䶥�㣬ֻ��λ�ã������tess��Ľ����ÿ���������ʾһ�������Ρ�
		vector<laya::RectGeometry::VertexInfo>	_strokeVertices;    //stroke�Ľ��
        vector<laya::RectGeometry::VertexInfo>	_fillVertices;      //fill�Ľ��
		list<v3_t>			_tessVertices;      //����tess�Ķ��㣬����Ѿ�����ά���ˣ�ֻ��ʵ��z����0
                                                //���������������Ҫtess��������ʵ�ʾ���tess��Ķ������
                                                //_vertices����
		GLenum				_primType;
		//GLuint				_fillVBO;
		//GLuint				_strokeVBO;
		int					_numberFillVertices;
		int					_numberStrokeVertices;
		layaPaint*		    _fillPaintForPath;
		layaPaint*  		_strokePaintForPath;
		
		
	private:		// tesseleator callbacks
		static void tessBegin( GLenum type, GLvoid* user );
		static void tessEnd( GLvoid* user );
		static void tessVertex( GLvoid *vertex, GLvoid* user );
		static void tessCombine( GLdouble coords[3], void *data[4],
								GLfloat weight[4], void **outData,
								void *polygonData );
		static void tessError( GLenum errorCode );
		void endOfTesselation( VGbitfield paintModes );
		
	private:	// utility methods
		
		GLenum primType() {
			return _primType;
		}
		void setPrimType( GLenum t ) {
			_primType = t;
		}
		
		GLdouble* tessVerticesBackPtr() {
			return &(_tessVertices.back().x);
		} 
		
		void updateBounds(float x, float y) {
			_minX = min(_minX, x);
			_width = max(_width, x);
			_minY = min(_minY, y);
			_height = max(_height, y);
		}

		void addVertex( GLdouble* v ) {
			VGfloat x = (VGfloat)v[0];
			VGfloat y = (VGfloat)v[1];
			updateBounds(x, y);
			_vertices.push_back(x);
			_vertices.push_back(y);
		}

		GLdouble * addTessVertex( const v3_t & v ) {
			//updateBounds(v.x, v.y);
			_tessVertices.push_back( v );
			return tessVerticesBackPtr();
		}
		
		void buildFill();
		void buildStroke();
        /** 
         *  @brief ����һ���߶Ρ�
         *  @param[out] vertices ��������˵�ǰ�Ľ��������������������push������Ϊ�������push 4�����㡣
         *  @param[out] 
         *  @return 
         *  @TODO �����ʽ����ֻ��xy�ġ�
         */
		void buildFatLineSegment( vector<laya::RectGeometry::VertexInfo>& vertices, const v2_t& p0, const v2_t& p1, const float stroke_width );
    public:
        IContext* m_pCtx=nullptr;
	};
}

#endif