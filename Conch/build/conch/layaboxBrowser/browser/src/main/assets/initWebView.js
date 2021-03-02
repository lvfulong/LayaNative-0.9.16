var __extends = (this && this.__extends) || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
};
window.platCallBack = function (ret) {
    var objid, m, rs, c, rJSON;
    if (ret == null)
        return;
    objid = ret.objId;
    m = ret.mName;
    c = ret.cName;
    rs = ret.v;
    var platC;
    if (objid != -1) {
        platC = PlatformObj.objMap[objid];
    }
    else {
        platC = PlatformClass.clsMap[c];
    }
    if (platC) {
        var callback = platC.callBackList[m];
        callback && callback(rs);
    }
};
var PlatformBase = (function () {
    function PlatformBase() {
    }
    PlatformBase.prototype.call = function (methodName, p1, p2, p3, p4, p5, p6, p7, p8, p9) {
        var a = Array.prototype.slice.call(arguments);
        a.shift();
        try {
            var rs = JSON.parse(runtime.callMethod(this.objid, true, this.className, methodName, JSON.stringify(a)) || "{}");
            return rs.v;
        }
        catch (e) {
            return null;
        }
    };
    PlatformBase.prototype.callWithBack = function (callback, methodName, p1, p2, p3, p4, p5, p6, p7, p8, p9) {
        this.callBackList[methodName] = callback;
        var a = Array.prototype.slice.call(arguments);
        a.splice(0, 2);
        try {
            var rs = JSON.parse(runtime.callMethod(this.objid, false, this.className, methodName, JSON.stringify(a)) || "{}");
            return rs.v;
        }
        catch (e) {
            return null;
        }
    };
    return PlatformBase;
}());
var PlatformClass = (function (_super) {
    __extends(PlatformClass, _super);
    function PlatformClass(clsName) {
        _super.call(this);
        this.objid = -1;
        this.className = clsName;
        this.callBackList = {};
        PlatformClass.clsMap[clsName] = this;
    }
    PlatformClass.createClass = function (clsName) {
        return PlatformClass.clsMap[clsName] || new PlatformClass(clsName);
    };
    PlatformClass.prototype.newObject = function (p1, p2, p3, p4, p5, p6, p7, p8, p9) {
        var a = Array.prototype.slice.call(arguments);
        a.splice(0, 0, "<init>");
        var rs = new PlatformObj(this);
        rs.init.apply(rs, a);
        return rs;
    };
    PlatformClass.clsMap = {};
    return PlatformClass;
}(PlatformBase));
var PlatformObj = (function (_super) {
    __extends(PlatformObj, _super);
    function PlatformObj(cls) {
        _super.call(this);
        this.cls = cls;
        this.className = cls.className;
        this.callBackList = {};
    }
    PlatformObj.prototype.init = function () {
        this.objid = PlatformObj.objNum++;
        this.call.apply(this, arguments) || -1;
        PlatformObj.objMap[this.objid] = this;
    };
    PlatformObj.objMap = {};
    PlatformObj.objNum = 0;
    return PlatformObj;
}(PlatformBase));
window.PlatformClass = PlatformClass;
window.PlatformObj = PlatformObj;
if( !window.runtime )
{
    window.runtime=
    {
        ScriptFrameParent:null,
        callObjectC:function(js)
        {
            if(!this.ScriptFrameParent)
            {
                this.ScriptFrameParent= document.createElement("div");
                this.ScriptFrameParent.cssText="position:absolute;left:0px;top:0;width:1px;height:1px;display:none";
                document.documentElement.appendChild(this.ScriptFrameParent);
            }
            var iframe = document.createElement("IFRAME");
            iframe.setAttribute("src", "runtime:"+js );
            this.ScriptFrameParent.appendChild(iframe);
            this.ScriptFrameParent.removeChild(iframe);
            iframe = null;
        },
        callConchJSFunction:function(name,json,callback)
        {
            this.callObjectC( "callConchJSFunction"+"\5" + name+"\5"+json+"\5"+callback );
        },
        JSLog:function( message )
        {
            this.callObjectC( "JSLog" + "\5" + message );
        }
    }
}
window.postRuntimeMessage=function(d,s){
    window.runtime.postMessage(encodeURIComponent( JSON.stringify(d)),s);
};
window.__getMessemage=function(d,s){
    d=decodeURIComponent(d);
    var e=document.createEvent("MessageEvent");
    e.initMessageEvent("message",false,false,JSON.parse(d),"","",window);
    e.target=window;
    window.dispatchEvent(e);
};