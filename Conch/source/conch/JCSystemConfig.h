
#ifndef _LAYA_CONFIG_H__
#define _LAYA_CONFIG_H__

#include <string>

namespace laya
{
    enum FRAME_TYPE
    {
        FT_SLOW = 0,
        FT_FAST,
        FT_MOUSE,
    };
	class JCSystemConfig 
	{
	public:
		JCSystemConfig();
		void reset();
	public:
		int m_nMaxTextureMemSize;
		int m_nAtlasWidth;
		int m_nAtlasHeight;
		int m_nAtlasGridSize;
		int m_nAtlasNum;
		std::string m_strStartURL;
		bool	m_bPerfStat;			//����Ч��ͳ��
        FRAME_TYPE  m_nFrameType;
        double m_nFrameThreshold;
        int     m_nSleepTime;
		int		m_nPerf_UpdateNum;
		std::string	m_strPerfOut;
		std::string m_jsonparamExt;//�������
        int m_nMemoryCanvasWidth;   //�ڴ�canvas�Ŀ�
        int m_nMemoryCanvasHeight;  //�ڴ�canvas�ĸ�
		static bool s_bIsPlug;      //��Ϊ��ʼ��̫���ˣ�����static
        static bool s_bLocalizable;//�����Ƿ��Ǳ��ذ�
        bool m_bShowInternalPerBar = false;//�Ƿ���ʾjs ondraw���ֺ�gl����״����ͼ��
        bool m_bUseChoreographer = false; //��android������choreographer
        bool m_bUseAndroidSystemFont = false;
        int m_nTextShapingMode;
        int m_nTextCacheNum;
	};
	extern JCSystemConfig g_kSystemConfig;
}

#endif