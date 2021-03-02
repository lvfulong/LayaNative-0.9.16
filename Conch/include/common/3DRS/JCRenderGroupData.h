/**
@file			JCRenderGroupData.h
@brief			
@author			guo
@version		1.0
@date			2016_5_31
*/

#ifndef __JCRenderGroupData_H__
#define __JCRenderGroupData_H__

#include "JCMesh.h"
#include "../geom/Rectangle.h"
#include <functional>

namespace laya 
{
    class JCMaterialBase;
    class JCRenderGroupData{
    public:
        JCMesh*             m_pMesh=nullptr;                    ///<Mesh
        void*               m_pShaderVertexInfo = nullptr;      //shaderInfo�ⲿ���ã�ֻ���ṩλ�ñ��棬������֪��ʲô����
        void*               m_pShaderUniformInfo = nullptr;
        JCMaterialBase*     m_pMaterial=nullptr;                //material
        int                 m_nVBBegin=0;                       //vb��ʼ��λ�ã�������������� glVertexAttribPointer ���������ܱ����޸�ib
        int                 m_nIBBegin = 0;
        int                 m_nEleNum = 0;
        int                 m_nGeoMode= GL_TRIANGLES;           //���� GL_TRIANGLES
        int                 m_nVertexDesc=0;                    //ʹ�� m_pMesh�е��ĸ�vertexdesc
        bool                m_bHasIndex = false;
        bool                m_bHasObjMat=false;
        float               m_matObject[16];
        int                 m_nVertNum = 0;                     //group�еĶ�������������޸�index��Ҫ��
        int                 m_nNamedDataCount = 0;              //Ϊ��3D�õģ��洢setShaderValue�ĸ�����
        std::vector<std::function<void(void)>> m_SetFunc;       //����ÿ�����е����ú���
        //bool                m_bScissor = false;
        //float               m_ClipRect[4] = { 0 };
    };
}
//------------------------------------------------------------------------------


#endif //__JCRenderGroupData_H__

//-----------------------------END FILE--------------------------------