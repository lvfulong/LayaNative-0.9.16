#include "JSWindowEditBox.h"
#include "../JSInterface/JSInterface.h"
#include "util/Log.h"
#include "util/JCColor.h"
#include "windows.h"
#include "util/JCMemorySurvey.h"
#include "JSMeasureTextInfo.h"
#include "../../JCScrpitRuntime.h"
#include "../../JCConch.h"
#include "JSGraphics.h"

namespace laya
{
    const float CARET_NODE_WIDTH = 2.0f;
#define MEM_EDITBOX_RENDER_COMMAND_SIZE 65536
    JSWindowEditBox* JSWindowEditBox::ms_pEditBox = NULL;
    ADDJSCLSINFO(JSWindowEditBox, JSObjNode);

    //------------------------------------------------------------------------------
    JSWindowEditBox::JSWindowEditBox()
    {
        m_nLeft = 0;
        m_nTop = 0;
        m_nWidth = 0;
        m_nHeight = 0;
        m_fOpacity = 1;
        m_nMaxLength = -1;
        m_sType = "text";
        m_sRegular = "";
        m_sStyle = "";
        m_sValue = "";
        m_sBgColor = "#ffffff";
        m_sBoarderColor = "#00ff00";
        m_nCaretColor = 0xff000000;
        m_bVisible = false;
        m_bFocus = false;
        m_sFont = "normal 100 16px Arial";
        m_nFontSize = 16;
        m_sFontColor = "#000000";
        m_nLastTextWidth = 0;
        m_bCaretOn = false;
        m_bMouseDrag = false;
        m_nCaret = 0;
        m_nCaretLast = 0;
        m_nSelStart = 0;
        m_nCaretPos = 0;
        m_nCaretStart = 0;
        m_nCaretEnd = 0;
        m_bForbidEdit = false;
        m_nGraphicsID = -1;
        m_nNodeID = -1;
        //AdjustAmountOfExternalAllocatedMemory( 540 + 65536 );
        JCMemorySurvey::GetInstance()->newClass("WindowEditBox", 540 + 65536, this);
        if (ms_pEditBox != NULL)
        {
            delete ms_pEditBox;
            ms_pEditBox = NULL;
        }
        ms_pEditBox = this;
        m_CallbackRef.reset(new int(1));
        if (JCConch::s_pConchRender->m_pRootNode)
        {
            //var node = new Node;
            JCCmdDispathManager* pCmdDispathManager = JCConch::s_pConchRender->m_pCmdDispathManager;
            m_nNodeID = JCScriptRuntime::s_JSRT->getDispathGlobalID();
            JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
            JCMemClass* pRenderCmd = JCScriptRuntime::s_JSRT->m_pRenderCmd;
            pRenderCmd->append(PUBLIC_CMD_DISPATH);
            pRenderCmd->append(PFT_createNode);
            pRenderCmd->append(m_nNodeID);

            _setPosition(m_nNodeID, (float)m_nLeft, (float)m_nTop);
            _setSize(m_nNodeID, (float)m_nWidth, (float)m_nHeight);

            //var selectNode = new Node; 
            m_nSelectNode = JCScriptRuntime::s_JSRT->getDispathGlobalID();
            pRenderCmd->append(PUBLIC_CMD_DISPATH);
            pRenderCmd->append(PFT_createNode);
            pRenderCmd->append(m_nSelectNode);

            _setPosition(m_nSelectNode, 0.0f, 0.0f);
            _setSize(m_nSelectNode, 0.0f, 0.0f);
            _setBgColor(m_nSelectNode, "#555555");
            _setBgColor(m_nSelectNode, "#ffffff");
            _setAlpha(m_nSelectNode, 0.5f);

            //node.addChildAt(selectNode,0);

            JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
            pRenderCmd->append(m_nNodeID);
            pRenderCmd->append(0);
            pRenderCmd->append(m_nSelectNode);
            pRenderCmd->append(0);


            //var caretNode = new Node; 
            m_nCaretNode = JCScriptRuntime::s_JSRT->getDispathGlobalID();
            pRenderCmd->append(PUBLIC_CMD_DISPATH);
            pRenderCmd->append(PFT_createNode);
            pRenderCmd->append(m_nCaretNode);

            _setPosition(m_nCaretNode, (float)m_nLeft, (float)m_nTop);
            _setSize(m_nCaretNode, CARET_NODE_WIDTH, (float)m_nHeight);
            _setBgColor(m_nCaretNode, "#222222");
            //_setBgColor(m_nCaretNode, "#ffffff");

            int nRootNodeID = JCConch::s_pConchRender->m_pRootNode->m_nID;

            //rootNode.addChildAt(node,0);

            JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
            pRenderCmd->append(nRootNodeID);
            pRenderCmd->append(0);
            pRenderCmd->append(m_nNodeID);
            pRenderCmd->append(0);

            //node.addChildAt(caretNode,0); 
            JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
            pRenderCmd->append(nRootNodeID);
            pRenderCmd->append(0);
            pRenderCmd->append(m_nCaretNode);
            pRenderCmd->append(0);

            //var graphics = new Graphics;
            m_nGraphicsID = JCScriptRuntime::s_JSRT->getDispathGlobalID();
            JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
            pRenderCmd->append(PUBLIC_CMD_DISPATH);
            pRenderCmd->append(PFT_createGraphics);
            pRenderCmd->append(m_nGraphicsID);
            pRenderCmd->append(JSGraphics::s_nTextBaseLineType);

            //node.graphic
            JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
            pRenderCmd->append(m_nNodeID);
            pRenderCmd->append(19);
            pRenderCmd->append(m_nGraphicsID);
        }
    }
    void JSWindowEditBox::_clearText()
    {
        //graphics.clear();
        JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
        JCMemClass* pRenderCmd = JCScriptRuntime::s_JSRT->m_pRenderCmd;
        pRenderCmd->append(m_nGraphicsID);
        pRenderCmd->append(0);
    }
    void JSWindowEditBox::_fillText(char* sText, const char* sFont, int nColor, float x, float y, int nTextAlign)
    {
        //graphics.fillText(text, x, y, font, color, textAlign, decorationLine) 
        JCMemClass* pRenderCmd = JCScriptRuntime::s_JSRT->m_pRenderCmd;
        JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
        pRenderCmd->append(m_nGraphicsID);
        pRenderCmd->append(11);
        pRenderCmd->append(x);
        pRenderCmd->append(y);
        pRenderCmd->append(nColor);
        pRenderCmd->append(nTextAlign);

        int nFontLength = strlen(sFont) + 1;
        pRenderCmd->append(nFontLength);
        pRenderCmd->append(sFont, nFontLength);

        int nTextLength = strlen(sText) + 1;
        pRenderCmd->append(nTextLength);
        pRenderCmd->append(sText, nTextLength);
    }

    void JSWindowEditBox::_setPosition(int nodeID, float x, float y)
    {
        //node.pos(x, y);
        JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
        JCMemClass* pRenderCmd = JCScriptRuntime::s_JSRT->m_pRenderCmd;
        pRenderCmd->append(nodeID);
        pRenderCmd->append(3);
        pRenderCmd->append(x);
        pRenderCmd->append(y);
    }
    void JSWindowEditBox::_setSize(int nodeID, float w, float h)
    {
        //node.size(x, y);
        JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
        JCMemClass* pRenderCmd = JCScriptRuntime::s_JSRT->m_pRenderCmd;
        pRenderCmd->append(nodeID);
        pRenderCmd->append(2);
        pRenderCmd->append(w);
        pRenderCmd->append(h);
    }
    void JSWindowEditBox::_setBgColor(int nodeID, const char* p_sColor)
    {    
        //node.bgcolor(x, y);
        JCColorInt nColor;
        nColor.setColorFromString(p_sColor);
        JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
        JCMemClass* pRenderCmd = JCScriptRuntime::s_JSRT->m_pRenderCmd;
        pRenderCmd->append(nodeID);
        pRenderCmd->append(8);
        pRenderCmd->append(nColor.color);
    }
    void JSWindowEditBox::_setAlpha(int nodeID, float alpha)
    {
        //node.alpha(alpha);
        JCScriptRuntime::s_JSRT->flushSharedCmdBuffer();
        JCMemClass* pRenderCmd = JCScriptRuntime::s_JSRT->m_pRenderCmd;
        pRenderCmd->append(nodeID);
        pRenderCmd->append(13);
        pRenderCmd->append(alpha);
    }
    //------------------------------------------------------------------------------
    JSWindowEditBox::~JSWindowEditBox()
    {
        JCMemClass* pRenderCmd = JCScriptRuntime::s_JSRT->m_pGCCmd;

        if (m_nGraphicsID != -1)
        {
            pRenderCmd->append(PUBLIC_CMD_DISPATH);
            pRenderCmd->append(PFT_deleteGraphics);
            pRenderCmd->append(m_nGraphicsID);
        }
        if (m_nNodeID != -1)
        {
            pRenderCmd->append(PUBLIC_CMD_DISPATH);
            pRenderCmd->append(PFT_deleteNode);
            pRenderCmd->append(m_nNodeID);
        }

        if (ms_pEditBox != NULL && ms_pEditBox == this) {
            ms_pEditBox = NULL;
        }
        JCMemorySurvey::GetInstance()->releaseClass("WindowEditBox", this);
    }

    void JSWindowEditBox::addEventListener(const char* p_sName, JSValueAsParam p_pFunction)
    {

    }
    //------------------------------------------------------------------------------
    JSWindowEditBox* JSWindowEditBox::GetInstance()
    {
        /*if (ms_pEditBox == NULL)
        {
            ms_pEditBox = new JSWindowEditBox();
        }*/
        return ms_pEditBox;
    }
    /*void JSWindowEditBox::DelInstance()
    {
        if (ms_pEditBox) {
            delete ms_pEditBox;
            ms_pEditBox = NULL;
        }
    }*/
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setFontSize(int p_nFontSize)
    {
        m_nFontSize = p_nFontSize;
        char sBuffer[1024] = { 0 };
        sprintf(sBuffer, "normal 100 %dpx Arial", m_nFontSize);
        m_sFont = sBuffer;
        //std::string temp = m_sFont;
        //m_pNode->font(temp);
        //m_pCaretNode->size(2, (float)m_nFontSize);
        _setSize(m_nCaretNode, CARET_NODE_WIDTH, (float)m_nFontSize);
        //focus();
    }
    //------------------------------------------------------------------------------
    int JSWindowEditBox::set_Left(int p_nLeft)
    {
        m_nLeft = p_nLeft;
        _setPosition(m_nNodeID, (float)m_nLeft, (float)m_nTop);
        _setPosition(m_nCaretNode, (float)m_nLeft, (float)m_nTop);
        return m_nLeft;
    }
    //------------------------------------------------------------------------------
    int JSWindowEditBox::get_Left()
    {
        return m_nLeft;
    }
    //------------------------------------------------------------------------------
    int JSWindowEditBox::set_Top(int p_nTop)
    {
        m_nTop = p_nTop;
        _setPosition(m_nNodeID, (float)m_nLeft, (float)m_nTop);
        _setPosition(m_nCaretNode, (float)m_nLeft, (float)m_nTop);
        return m_nTop;
    }
    //------------------------------------------------------------------------------
    int JSWindowEditBox::get_Top()
    {
        return m_nTop;
    }
    //------------------------------------------------------------------------------
    int JSWindowEditBox::set_Width(int p_nWidth)
    {
        m_nWidth = p_nWidth;
        _setSize(m_nNodeID, (float)m_nWidth, (float)m_nHeight);
        _setSize(m_nCaretNode, (float)CARET_NODE_WIDTH, (float)m_nHeight);
        return m_nWidth;
    }
    //------------------------------------------------------------------------------
    int JSWindowEditBox::get_Width()
    {
        return m_nWidth;
    }
    //------------------------------------------------------------------------------
    int JSWindowEditBox::set_Height(int p_nHeight)
    {
        m_nHeight = p_nHeight;
        _setSize(m_nNodeID, (float)m_nWidth, (float)m_nHeight);
        _setSize(m_nCaretNode, (float)CARET_NODE_WIDTH, (float)m_nHeight);
        return m_nHeight;
    }
    //------------------------------------------------------------------------------
    int JSWindowEditBox::get_Height()
    {
        return m_nHeight;
    }
    //------------------------------------------------------------------------------
    float JSWindowEditBox::set_Opacity(float p_Opacity)
    {
        m_fOpacity = p_Opacity;
        return m_fOpacity;
    }
    //------------------------------------------------------------------------------
    float JSWindowEditBox::get_Opacity()
    {
        return m_fOpacity;
    }
    //------------------------------------------------------------------------------
    const char* JSWindowEditBox::set_Value(const char* p_sValue)
    {
        m_sValue = (p_sValue != NULL) ? p_sValue : "";
        m_vValue.clear();
        m_nCaret = m_nSelStart = 0;
        if (p_sValue != NULL)
        {
            int pLength = strlen(p_sValue);
            m_nCaret = m_nSelStart = pLength;
            const char* pUTF8 = p_sValue;
            for (int i = 0; i < pLength; i++)
            {
                int ucode;
                int l = UTF8ToUnicode((unsigned char*)pUTF8, &ucode);
                pUTF8 += l;
                if (ucode == 0)
                {
                    break;
                }
                m_vValue.push_back(ucode);
            }
        }
        return m_sValue.c_str();
    }
    //------------------------------------------------------------------------------
    const char* JSWindowEditBox::get_Value()
    {
        m_sValue = "";
        for (int i = 0; i < (int)m_vValue.size(); i++)
        {
            char pTemp[10] = {};
            char* pCharEnd = UnicodeToUTF8(m_vValue[i], pTemp);
            m_sValue.append(pTemp);
        }
        return m_sValue.c_str();
    }
    //------------------------------------------------------------------------------
    const char* JSWindowEditBox::set_Style(const char* p_sStyle)
    {
        if (p_sStyle == NULL)
            m_sStyle = "";
        else
            m_sStyle = p_sStyle;
        return m_sStyle.c_str();
    }
    //------------------------------------------------------------------------------
    const char* JSWindowEditBox::get_Style()
    {
        return m_sStyle.c_str();
    }
    //------------------------------------------------------------------------------
    bool JSWindowEditBox::set_Visible(bool p_bVisible)
    {
        m_bVisible = p_bVisible;
        return m_bVisible;
    }
    //------------------------------------------------------------------------------
    bool JSWindowEditBox::get_Visible()
    {
        return m_bVisible;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setLeft(int p_nLeft)
    {
        set_Left(p_nLeft);
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setTop(int p_nTop)
    {
        set_Top(p_nTop);
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setWidth(int p_nWidth)
    {
        set_Width(p_nWidth);
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setHeight(int p_nHeight)
    {
        set_Height(p_nHeight);
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setOpacity(float p_Opacity)
    {
        set_Opacity(p_Opacity);
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setValue(const char* p_sValue)
    {
        if (p_sValue == NULL)
            set_Value("");
        else
            set_Value(p_sValue);
    }
    //------------------------------------------------------------------------------
    const char* JSWindowEditBox::getValue()
    {
        return get_Value();
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setStyle(const char* p_sStyle)
    {
        set_Style(p_sStyle);
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setFont(const char* p_sFont)
    {
        m_sFont = p_sFont; 
    }
    //------------------------------------------------------------------------------
    //并不是每帧都调，只有内容发生改变了，才会调用。包括光标位置改变
    void JSWindowEditBox::drawEditboxAll()
    {
        if (m_bVisible == false)
        {
            return;
        }
        else
        {
            //绘制底色和边框
            //m_pNode->bgcolor( m_sBgColor.c_str() );
            //m_pNode->border( m_sBoarderColor.c_str() );

            //绘制文字和选中的
            drawText();
            drawSelected();	
            ResetCaretBlink();
        }
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setVisible(bool p_bVisible)
    {
        //	if( m_bVisible == p_bVisible )return;
        m_bVisible = p_bVisible;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::DeleteSelectionText()
    {
        if (m_nCaret < m_nSelStart)
        {
            m_vValue.erase(m_vValue.begin() + m_nCaret, m_vValue.begin() + m_nSelStart);
            m_nSelStart = m_nCaret;
            drawEditboxAll();
        }
        else
        {
            m_vValue.erase(m_vValue.begin() + m_nSelStart, m_vValue.begin() + m_nCaret);
            if (m_nSelStart > m_nCaretStart)
            {
                m_nCaret = m_nSelStart;
            }
            else if (m_nSelStart <= m_nCaretStart)
            {
                m_nCaret = m_nSelStart;
                m_nCaretStart = m_nSelStart;
            }
            drawEditboxAll();
        }
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::CopyToClipboard()
    {
        /**/
        // Copy the selection text to the clipboard
        if (m_nCaret != m_nSelStart && OpenClipboard(NULL))
        {
            EmptyClipboard();
            HGLOBAL hBlock = GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR) * (m_vValue.size() + 1));
            if (hBlock)
            {
                WCHAR* pwszText = (WCHAR*)GlobalLock(hBlock);
                if (pwszText)
                {
                    int nFirst = __min(m_nCaret, m_nSelStart);
                    int nLast = __max(m_nCaret, m_nSelStart);
                    if (nLast - nFirst > 0)
                        CopyMemory(pwszText, &m_vValue[nFirst], (nLast - nFirst) * sizeof(WCHAR));
                    pwszText[nLast - nFirst] = L'\0';  // Terminate it
                    GlobalUnlock(hBlock);
                }
                SetClipboardData(CF_UNICODETEXT, hBlock);
            }
            CloseClipboard();
            // We must not free the object until CloseClipboard is called.
            if (hBlock)
                GlobalFree(hBlock);
        }
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::PasteFromClipboard()
    {
        if (m_nCaret != m_nSelStart)
        {
            DeleteSelectionText();
        }
        if (OpenClipboard(NULL))
        {
            HANDLE handle = GetClipboardData(CF_UNICODETEXT);
            if (handle)
            {
                char* pwszText = (char*)GlobalLock(handle);
                if (pwszText)
                {
                    for (int i = 0; i < lstrlen(pwszText); i++)
                    {
                        m_vValue.insert(m_vValue.begin() + m_nCaret + i, pwszText[i]);
                    }
                    PlaceCaret(m_nCaret + lstrlen(pwszText));
                    m_nSelStart = m_nCaret;
                    GlobalUnlock(handle);
                }
            }
            CloseClipboard();
        }
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::PlaceCaret(int nCP)
    {
        if (nCP >= 0 && nCP <= (int)m_vValue.size())
        {
            m_nCaret = nCP;
            //如果超出了editbox的右边界，则修改 m_nCaretStart 让光标始终可见
            if (m_nCaret > m_nCaretEnd)
            {
                int p_nLastTextWidth = 0;
                for (int i = m_nCaret - 1; i > 0; i--)
                {
                    char pTemp[10] = {};
                    UnicodeToUTF8(m_vValue[i], pTemp);
                    p_nLastTextWidth += measureText(pTemp);
                    m_nCaretStart = i;
                    if (p_nLastTextWidth + 10 >= m_nWidth)
                    {
                        break;
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------------
    //只有鼠标按下的时候才会调用。
    bool JSWindowEditBox::handleMouseEvent(int p_nMouseX, int p_nMouseY)
    {
        if (m_nWidth <= 0 || m_nHeight <= 0) return false;
        if (p_nMouseX >= m_nLeft && p_nMouseX <= (m_nLeft + m_nWidth) &&
            p_nMouseY >= m_nTop && p_nMouseY <= (m_nTop + m_nHeight))
        {
            m_bFocus = true;
            m_bCaretOn = true;
            int p_nPos = 0;
            int i = m_nCaretStart;
            while (p_nMouseX >= (m_nLeft + p_nPos + 13) && i<(int)m_vValue.size())
            {
                char pTemp[10] = {};
                UnicodeToUTF8(m_vValue[i], pTemp);
                p_nPos += measureText(pTemp);
                i++;
            }
            m_nCaret = i;
            m_nSelStart = m_nCaret;
            focus();
            return true;
        }
        else {
            blur();
        }
        return false;
    }
    //------------------------------------------------------------------------------
    //
    void JSWindowEditBox::mouseMove(int p_MouseX, int p_MouseY)
    {
        int p_nPos = 0;
        int i = m_nCaretStart;
        while (p_MouseX >= (m_nLeft + p_nPos + 13) && i<(int)m_vValue.size())
        {
            char pTemp[10] = {};
            UnicodeToUTF8(m_vValue[i], pTemp);
            p_nPos += measureText(pTemp);
            i++;
        }
        m_nCaret = i;
        if (p_MouseX<m_nLeft && m_nCaret>0)
        {
            m_nCaret -= 1;
        }
        if (m_nCaret != m_nCaretLast)
        {
            if (m_nCaret>m_nCaretEnd)
            {
                int p_nLastTextWidth = 0;
                for (int i = m_nCaret - 1; i > 0; i--)
                {
                    char pTemp[10] = {};
                    UnicodeToUTF8(m_vValue[i], pTemp);
                    p_nLastTextWidth += measureText(pTemp);
                    m_nCaretStart = i;
                    if (p_nLastTextWidth + 10 >= m_nWidth)
                    {
                        break;
                    }
                }
            }
            if (m_nCaret<m_nCaretStart)
            {
                m_nCaretStart = m_nCaret;
            }
            drawEditboxAll();
        }
        m_nCaretLast = m_nCaret;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::drawSelected()
    {
        if (m_nCaret != m_nSelStart)
        {
            int p_nCarLen = 0;
            for (int i = m_nCaretStart; i < m_nCaret; i++)
            {
                char pTemp[10] = {};
                UnicodeToUTF8(m_vValue[i], pTemp);
                p_nCarLen += measureText(pTemp);
            }
            int p_nSelLen = 0;
            for (int i = m_nCaretStart; i < m_nSelStart; i++)
            {
                char pTemp[10] = {};
                UnicodeToUTF8(m_vValue[i], pTemp);
                p_nSelLen += measureText(pTemp);
            }
            float fAlpha = 0.7f;
            int nSelectBgColor = 0xff777777;
            if (m_nSelStart > m_nCaret)
            {
                if (m_nSelStart > m_nCaretEnd)
                {
                    _setPosition(m_nSelectNode, (float)p_nCarLen, 0);
                    _setSize(m_nSelectNode, (float)(m_nWidth - p_nCarLen), (float)m_nFontSize);
                }
                else
                {
                    _setPosition(m_nSelectNode, (float)p_nCarLen, 0);
                    _setSize(m_nSelectNode, (float)(p_nSelLen - p_nCarLen), (float)m_nFontSize);
                }
            }
            else if (m_nSelStart < m_nCaret)
            {
                _setPosition(m_nSelectNode, (float)(p_nSelLen), 0);
                _setSize(m_nSelectNode, (float)(p_nCarLen - p_nSelLen), (float)m_nFontSize);
            }
        }
        else
        {
            _setSize(m_nSelectNode, 0.0f, 0.0f);
        }
    }

    void  JSWindowEditBox::onInputCallJSFunction(std::weak_ptr<int> callbackref)
    {
        if (!callbackref.lock())
            return;
        m_pJSFunctionOnInput.Call();
    }

    void JSWindowEditBox::onKeyDownCallJSFunction(int keyCode, std::weak_ptr<int> callbackref)
    {
        if (!callbackref.lock())
            return;
        m_pJSFunctionOnKeydown.Call(keyCode);
    }

    void JSWindowEditBox::setSelectionRange(int start, int end)
    {
        setCursorPosition(start);
    }
    //------------------------------------------------------------------------------
    bool JSWindowEditBox::onKeyChar(short p_sBuffer)
    {
        std::weak_ptr<int> cbrefKedown(m_CallbackRef);
        std::function<void(void)> pFunction = std::bind(&JSWindowEditBox::onKeyDownCallJSFunction, this, (int)p_sBuffer, cbrefKedown);
        if (JCScriptRuntime::s_JSRT)
        {
            JCScriptRuntime::s_JSRT->m_ScriptThread.post(pFunction);
        }
        if (m_nSelStart != m_nCaret)
        {
            DeleteSelectionText();
        }
        m_vValue.insert(m_vValue.begin() + m_nCaret, p_sBuffer);
        char pTemp[10] = {};
        UnicodeToUTF8((unsigned short)p_sBuffer, pTemp);
        int nCharLen = 0;
        nCharLen = measureText(pTemp);
        m_nCaret += 1;
        m_nSelStart = m_nCaret;


        std::weak_ptr<int> cbref(m_CallbackRef);
        pFunction = std::bind(&JSWindowEditBox::onInputCallJSFunction, this, cbref);
        if (JCScriptRuntime::s_JSRT)
        {
            JCScriptRuntime::s_JSRT->m_ScriptThread.post(pFunction);
        }

        if (nCharLen > m_nWidth - m_nCaretPos + 13)
        {
            int nLen = 0;
            while (nLen < nCharLen)
            {
                char pTemp[10] = {};
                UnicodeToUTF8(m_vValue[m_nCaretStart], pTemp);
                nLen += measureText(pTemp);
                m_nCaretStart++;
            }
        }
        //fillText(pTemp, "normal 100 16px Arial", -16777216, nCharLen, 0, -26);
        drawEditboxAll();
        return true;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::onDeleteCharB()
    {
        std::weak_ptr<int> cbrefKedown(m_CallbackRef);
        std::function<void(void)> pFunction = std::bind(&JSWindowEditBox::onKeyDownCallJSFunction, this, 8, cbrefKedown);
        if (JCScriptRuntime::s_JSRT)
        {
            JCScriptRuntime::s_JSRT->m_ScriptThread.post(pFunction);
        }
        m_nCaret -= 1;
        int nCharLen = 0;
        char pTemp[10] = {};
        UnicodeToUTF8(m_vValue[m_nCaret], pTemp);
        nCharLen = measureText(pTemp);
        m_nSelStart = m_nCaret;
        //	m_nLastTextWidth -= nCharLen;
        m_vValue.erase(m_vValue.begin() + m_nCaret);
        if (m_nCaretStart != 0)
        {
            int nLen = 0;
            while (nLen < nCharLen)
            {
                char pTemp[10] = {};
                UnicodeToUTF8(m_vValue[m_nCaretStart], pTemp);
                nLen += measureText(pTemp);
                m_nCaretStart--;
                if (m_nCaretStart == 0)
                    break;
            }
        }
        drawEditboxAll();

        std::weak_ptr<int> cbref(m_CallbackRef);
        pFunction = std::bind(&JSWindowEditBox::onInputCallJSFunction, this, cbref);
        if (JCScriptRuntime::s_JSRT)
        {
            JCScriptRuntime::s_JSRT->m_ScriptThread.post(pFunction);
        }
    }
    //------------------------------------------------------------------------------
    //delete 。这里没有更新 m_nCaretEnd 的值，因为drawText的时候会重新计算
    //调用者在调用这个后，会再调用drawText
    void JSWindowEditBox::onDeleteCharD()
    {
        //	m_nLastTextWidth -= measureText( getLpChar(m_nCaret) );
        std::weak_ptr<int> cbrefKedown(m_CallbackRef);
        std::function<void(void)> pFunction = std::bind(&JSWindowEditBox::onKeyDownCallJSFunction, this, 46, cbrefKedown);
        if (JCScriptRuntime::s_JSRT)
        {
            JCScriptRuntime::s_JSRT->m_ScriptThread.post(pFunction);
        }
        m_vValue.erase(m_vValue.begin() + m_nCaret);
        std::weak_ptr<int> cbref(m_CallbackRef);
        pFunction = std::bind(&JSWindowEditBox::onInputCallJSFunction, this, cbref);
        if (JCScriptRuntime::s_JSRT)
        {
            JCScriptRuntime::s_JSRT->m_ScriptThread.post(pFunction);
        }
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::drawText()
    {
        int p_nLastTextWidth = 0;
        int p_nX = 0;
        int p_nY = 0;
        _clearText();
        for (int i = m_nCaretStart; i < (int)m_vValue.size(); i++)
        {
            m_nCaretEnd = i + 1;
            p_nX += p_nLastTextWidth;
            char pTemp[10] = {0};
            memset(pTemp, 0, 10);
            char* pCharEnd = UnicodeToUTF8(m_vValue[i], pTemp);
            p_nLastTextWidth = measureText(pTemp);
            JCColorInt nColor;
            nColor.setColorFromString(m_sFontColor.c_str());
            _fillText(pTemp, m_sFont.c_str(), nColor.color, p_nX, 0, -26);
            if (p_nX + p_nLastTextWidth - m_nLeft + 30 >= m_nWidth)
            {
                //break;//TODO
            }
        }
    }
    //-------------------------------------------------------------------------------
    //光标右移一个字
    void JSWindowEditBox::caretMoveRight()
    {
        if (m_nCaret == m_nCaretEnd && m_nCaret<(int)m_vValue.size())
        {
            int p_nLastTextWidth = 0;
            for (int i = m_nCaret; i > 0; i--)
            {
                char pTemp[10] = {};
                UnicodeToUTF8(m_vValue[i], pTemp);
                p_nLastTextWidth += measureText(pTemp);
                m_nCaretStart = i;
                if (p_nLastTextWidth + 10 >= m_nWidth)
                {
                    PlaceCaret(m_nCaret + 1);
                    return;
                }
            }
        }
        PlaceCaret(m_nCaret + 1);
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::caretMoveToEnd()
    {
        int p_nLastTextWidth = 0;
        for (int i = m_vValue.size(); i > 0; i--)
        {
            char pTemp[10] = {};
            UnicodeToUTF8(m_vValue[i - 1], pTemp);
            p_nLastTextWidth += measureText(pTemp);
            m_nCaretStart = i - 1;
            if (p_nLastTextWidth + 10 >= m_nWidth)
            {
                return;
            }
        }
    }
    //-------------------------------------------------------------------------------
    int JSWindowEditBox::measureText(const char* p_sText)
    {
        JCFontInfo * pFontInfo = JCScriptRuntime::s_JSRT->m_pFontManager->getFontInfoFromText(m_sFont);
        int w = 0;
        int h = 0;
        //JCScriptRuntime::s_JSRT->m_pMeasureTextManager->measureText(pFontInfo, p_sText, w, h);
        return w;
    }
    //------------------------------------------------------------------------------
    char* JSWindowEditBox::getLpChar(int index)
    {
        return 0;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::focus()
    {
        m_bFocus = true;
        m_bVisible = true;
        drawEditboxAll();
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::blur()
    {
        setVisible(false);
        m_bFocus = false;
        m_bCaretOn = false;
        m_bMouseDrag = false;
    }
    //------------------------------------------------------------------------------
    //显示光标
    void JSWindowEditBox::ResetCaretBlink()
    {
        int p_nlen = 0;
        m_bCaretOn = true;
        for (int i = m_nCaretStart; i < m_nCaret; i++)
        {
            char pTemp[10] = {};
            UnicodeToUTF8(m_vValue[i], pTemp);
            p_nlen += measureText(pTemp);
        }
        _setPosition(m_nCaretNode, (float)(m_nLeft + p_nlen), (float)m_nTop);
        m_nCaretPos = p_nlen;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setColor(const char* p_sColor)
    {
        m_sFontColor = p_sColor;
        m_nCaret = 0;
        m_nSelStart = m_nCaret;
        m_bMouseDrag = false;
        //focus();//单机两次是因为先处理的单机事件然后再赋值(应该反过来)，因此应该在赋值以后应该调用focus响应,相当于单击事件的延续
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setPos(int x, int y)
    {
        m_nLeft = x;
        m_nTop = y;
        _setPosition(m_nNodeID, (float)m_nLeft, (float)m_nTop);
        _setPosition(m_nCaretNode, (float)m_nLeft, (float)m_nTop);
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setSize(int w, int h)
    {
        m_nWidth = w;
        m_nHeight = h;
        _setSize(m_nNodeID, (float)m_nWidth, (float)m_nHeight);
        _setSize(m_nCaretNode, (float)CARET_NODE_WIDTH, (float)m_nHeight);
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setCursorPosition(int pos)
    {

    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setScale(float p_nSx, float p_nSy)
    {

    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setMaxLength(int p_nMaxLength)
    {
        m_nMaxLength = p_nMaxLength;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setType(const char* p_sType)
    {
        m_sType = p_sType;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setRegular(const char* p_sRegular)
    {
        m_sRegular = p_sRegular;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::setNumberOnly(bool p_bNumberOnly)
    {

    }
    void JSWindowEditBox::setMultiAble(bool p_bMultiAble)
    {
        
    }
    void JSWindowEditBox::setForbidEdit(bool bForbidEdit)
    {
        m_bForbidEdit = bForbidEdit;
    }
    bool JSWindowEditBox::getForbidEdit()
    {
        return m_bForbidEdit;
    }
    //------------------------------------------------------------------------------
    void JSWindowEditBox::exportJS()
    {
        JSP_CLASS("ConchInput", JSWindowEditBox);
        JSP_ADD_PROPERTY(left, JSWindowEditBox, get_Left, set_Left);
        JSP_ADD_PROPERTY(top, JSWindowEditBox, get_Top, set_Top);
        JSP_ADD_PROPERTY(width, JSWindowEditBox, get_Width, set_Width);
        JSP_ADD_PROPERTY(height, JSWindowEditBox, get_Height, set_Height);
        JSP_ADD_PROPERTY(opacity, JSWindowEditBox, get_Opacity, set_Opacity);
        JSP_ADD_PROPERTY(style, JSWindowEditBox, get_Style, set_Style);
        JSP_ADD_PROPERTY(value, JSWindowEditBox, get_Value, set_Value);
        JSP_ADD_PROPERTY(visible, JSWindowEditBox, get_Visible, set_Visible);
        JSP_ADD_METHOD("addEventListener", JSWindowEditBox::addEventListener);
        JSP_ADD_METHOD("setLeft", JSWindowEditBox::setLeft);
        JSP_ADD_METHOD("setTop", JSWindowEditBox::setTop);
        JSP_ADD_METHOD("setWidth", JSWindowEditBox::setWidth);
        JSP_ADD_METHOD("setHeight", JSWindowEditBox::setHeight);
        JSP_ADD_METHOD("setOpacity", JSWindowEditBox::setOpacity);
        JSP_ADD_METHOD("setValue", JSWindowEditBox::setValue);
        JSP_ADD_METHOD("getValue", JSWindowEditBox::getValue);
        JSP_ADD_METHOD("setStyle", JSWindowEditBox::setStyle);
        JSP_ADD_METHOD("setVisible", JSWindowEditBox::setVisible);
        JSP_ADD_METHOD("focus", JSWindowEditBox::focus);
        JSP_ADD_METHOD("blur", JSWindowEditBox::blur);
        JSP_ADD_METHOD("setColor", JSWindowEditBox::setColor);
        JSP_ADD_METHOD("setFontSize", JSWindowEditBox::setFontSize);
        JSP_ADD_METHOD("setPos", JSWindowEditBox::setPos);
        JSP_ADD_METHOD("setSize", JSWindowEditBox::setSize);
        JSP_ADD_METHOD("setCursorPosition", JSWindowEditBox::setCursorPosition);
        JSP_ADD_METHOD("setScale", JSWindowEditBox::setScale);
        JSP_ADD_METHOD("setMaxLength", JSWindowEditBox::setMaxLength);
        JSP_ADD_METHOD("setType", JSWindowEditBox::setType);
        JSP_ADD_METHOD("setNumberOnly", JSWindowEditBox::setNumberOnly);
        JSP_ADD_METHOD("setRegular", JSWindowEditBox::setRegular);
        JSP_ADD_METHOD("setFont", JSWindowEditBox::setFont);
        JSP_ADD_METHOD("setMultiAble", JSWindowEditBox::setMultiAble);
        JSP_ADD_METHOD("setForbidEdit", JSWindowEditBox::setForbidEdit);
        JSP_ADD_METHOD("getForbidEdit", JSWindowEditBox::getForbidEdit);
        JSP_INSTALL_CLASS("ConchInput", JSWindowEditBox);
    }
}