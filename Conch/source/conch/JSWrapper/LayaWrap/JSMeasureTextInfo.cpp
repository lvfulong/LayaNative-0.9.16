/**
@file			JSMeasureTextInfo.cpp
@brief			
@author			wyw
@version		1.0
@date			2013_11_12
*/

#include "JSMeasureTextInfo.h"
#include "../JSInterface/JSInterface.h"
#include "util/Log.h"
#include <util/JCMemorySurvey.h>
//#include <FontRender/JCMeasureTextManager.h>
#include "../../JCScrpitRuntime.h"
#include "JSGraphics.h"
#include "../../../conch/JCSystemConfig.h"
#ifdef ANDROID
#include "../../../conch/CToJavaBridge.h"
#endif
namespace laya
{
	ADDJSCLSINFO(JSMeasureTextInfo, JSObjNode);
    //------------------------------------------------------------------------------
    JSMeasureTextInfo::JSMeasureTextInfo()
    {
	    m_nWidth = 0;
	    m_nHeight = 0;
	    AdjustAmountOfExternalAllocatedMemory( 8 );
	    JCMemorySurvey::GetInstance()->newClass( "measureText",12,this );
    }
    //------------------------------------------------------------------------------
    JSMeasureTextInfo::~JSMeasureTextInfo()
    {
	    JCMemorySurvey::GetInstance()->releaseClass( "measureText",this );
    }
    //------------------------------------------------------------------------------
    int JSMeasureTextInfo::GetWidth()
    {
	    return m_nWidth;
    }
    int JSMeasureTextInfo::GetHeight()
    {
	    return m_nHeight;
    }
    //------------------------------------------------------------------------------
    void JSMeasureTextInfo::measureText(JCFontInfo* pFontInfo, const char* p_sBuffer)
    {
        if (g_kSystemConfig.m_nTextShapingMode == TextShapingMode::Simple)
        {
            JCScriptRuntime::s_JSRT->m_pMeasureTextManager->measureText(pFontInfo, (char*)p_sBuffer, m_nWidth, m_nHeight);
            if (JSGraphics::s_nTextBaseLineType == TBL_TYPE_MIDDLE)
            {
                m_nHeight = pFontInfo->m_nFontSize;
            }
        }
        else
        {
            const JCShapedGlyphSequence& pShapedGlyphSequence = JCScriptRuntime::s_JSRT->m_pShapedTextCache->getShapedTextSequence(p_sBuffer, pFontInfo, Direction::EAuto);//??Auto
            pShapedGlyphSequence.getMeasuredSize(m_nWidth, m_nHeight);
        }
    }
    void JSMeasureTextInfo::exportJS() 
    {
        JSP_CLASS("measureText", JSMeasureTextInfo);
        JSP_ADD_PROPERTY_RO(width, JSMeasureTextInfo, GetWidth);
        JSP_ADD_PROPERTY_RO(height, JSMeasureTextInfo, GetHeight);
	    JSP_INSTALL_CLASS("measureText", JSMeasureTextInfo);
    }
}
//------------------------------------------------------------------------------


//-----------------------------END FILE--------------------------------
