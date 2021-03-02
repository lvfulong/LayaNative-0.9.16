"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator.throw(value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : new P(function (resolve) { resolve(result.value); }).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments)).next());
    });
};
window._conchInfo = { version: '2.1.3.1' };
var _inline = !conchConfig.localizable;
console.log('======================================================  ');
console.log('             LAYA CONCH            ');
console.log('     runtimeversion:' + conchConfig.getRuntimeVersion());
console.log('          jsversion:' + window._conchInfo.version);
console.log('             isplug:' + conchConfig.getIsPlug());
console.log('======================================================');
function log(m) {
    console.log(m);
}
const asyncs = require('async');
function initFreeType() {
    return __awaiter(this, void 0, void 0, function* () {
        var sOS = conchConfig.getOS();
        var bRet = false;
        if (sOS == "Conch-window") {
            bRet = conch.initFreeTypeDefaultFontFromFile("C:/Windows/Fonts/simhei.ttf");
        }
        else if (sOS == "Conch-android") {
            var fSystemVersion = navigator.sv;
            if (fSystemVersion >= 2.0 && fSystemVersion < 5.0) {
                bRet = conch.initFreeTypeDefaultFontFromFile("/system/fonts/DFHEIA5A.ttf");
                if (bRet == false) {
                    bRet = conch.initFreeTypeDefaultFontFromFile("/system/fonts/DroidSansFallback.ttf");
                }
            }
            else if (fSystemVersion >= 5.0 && fSystemVersion < 6.0) {
                var vDefaultStrings = [];
                vDefaultStrings.push("/system/fonts/NotoSansHans-Regular.otf");
                vDefaultStrings.push("/system/fonts/Roboto-Regular.ttf");
                bRet = conch.initFreeTypeDefaultFontFromFile(vDefaultStrings.join('|'));
            }
            else if (fSystemVersion >= 6.0 && fSystemVersion < 7.0) {
                var vDefaultStrings = [];
                vDefaultStrings.push("/system/fonts/NotoSansSC-Regular.otf");
                vDefaultStrings.push("/system/fonts/Roboto-Regular.ttf");
                bRet = conch.initFreeTypeDefaultFontFromFile(vDefaultStrings.join('|'));
            }
            else if (fSystemVersion > 7.0 && fSystemVersion < 8.0) {
                bRet = false;
            }
        }
        else if (sOS == "Conch-ios") {
            bRet = conch.initFreeTypeDefaultFontFromFile("");
        }
        if (bRet == false) {
            var sTempFontPath = conch.getCachePath() + "/runtimeFont/";
            if (!fs_exists(sTempFontPath)) {
                fs_mkdir(sTempFontPath);
            }
            sTempFontPath += "layabox.ttf";
            bRet = conch.initFreeTypeDefaultFontFromFile(sTempFontPath);
            if (bRet == false) {
                var data = (yield asyncs.downloadSync(location.fullpath + '/font/simhei.ttf', true, null));
                if (!data) {
                    data = (yield asyncs.downloadSync('http://runtime.layabox.com/font/simhei.ttf', true, null));
                }
                if (!data) {
                    alert('>>>download ttf error ');
                    return;
                }
                fs_writeFileSync(sTempFontPath, data);
                conch.initFreeTypeDefaultFontFromFile(sTempFontPath);
            }
        }
    });
}
function setOrientation(s) {
    var nameToVal = {
        landscape: 0, portrait: 1, user: 2, behind: 3, sensor: 4, nosensor: 5, sensor_landscape: 6,
        sensor_portrait: 7, reverse_landscape: 8, reverse_portrait: 9, full_sensor: 10,
    };
    var nOri = (function (name) {
        try {
            var n = nameToVal[name];
            return n || 0;
        }
        catch (e) {
            return 0;
        }
    })(s);
    conchConfig.setScreenOrientation(nOri);
    ;
}
function initHtml(data) {
    var tag = new RegExp("<(/|)([\\w.]+)(?: +|)?([^>]*?)(/|)>", "g");
    var res, attrs;
    var scripts = [];
    var all = [];
    var lastTag = {};
    var lastIndex = 0;
    var _comments = new RegExp("<!--[\\s\\S]*?-->", "g");
    var _blank = new RegExp("^[^\\S]*|[^\\S]*$", "g");
    var _attributes = new RegExp("(\\w+)\\s*=\\s*(\"[^\"]*\"|'[^']*')", "g");
    data = data.replace(_comments, "");
    var layaMeta = null;
    while (res = tag.exec(data)) {
        var temp = {};
        temp.tagName = res[2];
        if (res[3]) {
            temp.attributes = {};
            while ((attrs = _attributes.exec(res[3])) != null) {
                temp.attributes[attrs[1]] = attrs[2].substring(1, attrs[2].length - 1);
            }
        }
        if (temp.tagName === 'meta' && temp.attributes.name === 'laya')
            layaMeta = temp.attributes;
        if (!res[1] && !res[4])
            temp.type = -1;
        else if (!res[1] && res[4])
            temp.type = 0;
        else if (res[1] && !res[4])
            temp.type = 1;
        else {
            alert("解析标签出错</" + temp.tagName + "/>");
            return;
        }
        temp.index = res.index;
        all.push(temp);
        if (temp.tagName == "script" || temp.tagName == "meta") {
            if (temp.type == -1) {
                if (lastTag.tagName == temp.tagName && lastTag.type == -1) {
                    if (temp.tagName == "meta") {
                        scripts.push(temp);
                    }
                    else {
                        alert("解析标签" + temp.tagName + "出错,连续两个script标签可能是 脚本中包含<script>");
                        return;
                    }
                }
                scripts.push(temp);
            }
            else if (temp.type == 1) {
                if (lastTag.tagName == temp.tagName && lastTag.type == -1) {
                    var text = data.substring(lastIndex, temp.index);
                    text = text.replace(_blank, "");
                    if (text) {
                        temp = scripts.pop();
                        temp.type = 1;
                        temp.attributes = temp.attributes || {};
                        temp.attributes.text = text;
                        scripts.push(temp);
                    }
                }
            }
            else {
                if (temp.attributes)
                    scripts.push(temp);
            }
        }
        lastIndex = res.index + res[0].length;
        lastTag = temp;
    }
    var chkjs = layaMeta && layaMeta.layajsprotect && layaMeta.layajsprotect == 'true';
    for (var d in scripts) {
        var one = scripts[d];
        if (chkjs) {
            if (!one.attributes)
                continue;
            if (one.tagName === 'script' && one.attributes.loader != 'laya')
                continue;
        }
        var t = document.createElement(one.tagName);
        for (var attr in one.attributes) {
            t[attr] = one.attributes[attr];
        }
        document.head.appendChild(t);
    }
    if (layaMeta)
        setOrientation(layaMeta.screenorientation);
    var onloadScipt = document.createElement("script");
    onloadScipt.text = "window.onload&&window.onload()";
}
function loadUrl(url) {
    return __awaiter(this, void 0, void 0, function* () {
        if (url === 'http://stand.alone.version/index.html')
            _inline = false;
        if (!_inline) {
            url = 'http://stand.alone.version/index.html';
        }
        console.log("loadURL:" + url);
        if (history.length <= 0) {
            history._push(url);
        }
        if (url.length < 2)
            return;
        location.setHref(url);
        var urlpath = location.fullpath + '/';
        var cache = window.appcache = new AppCache(urlpath);
        document.loadCookie();
        yield initFreeType();
        //window["startLoading"] && window["startLoading"]();
        try {
            require("config");
        }
        catch (e) {
        }
        function updateDcc() {
            return __awaiter(this, void 0, void 0, function* () {
                cache.setResourceID('appurl', urlpath);
                var curassets = cache.getResourceID('netassetsid');
                var assetsidStr = (yield asyncs.downloadSync(urlpath + 'update/assetsid.txt?rand=' + Math.random() * Date.now(), false, null));
                if (!assetsidStr) {
                }
                else {
                    if (curassets != assetsidStr) {
                        log('need update;');
                        var txtdcc = '';
                        var bindcc = yield asyncs.downloadSync(urlpath + 'update/filetable.bin?' + assetsidStr, true, null);
                        if (!bindcc || !(bindcc instanceof ArrayBuffer)) {
                            txtdcc = (yield asyncs.downloadSync(urlpath + 'update/filetable.txt?' + assetsidStr, false, null));
                        }
                        else {
                            if (bindcc.byteLength % 8 != 0) {
                                log('下载的的filetable.bin的长度不对。是不是错了。');
                            }
                            else {
                                var v = new Uint32Array(bindcc);
                                if (v[0] != 0xffeeddcc || v[1] != 1) {
                                    log('dcc.bin file err!');
                                }
                                else {
                                    if (v[2] == 0x00ffffff) {
                                        var stp = (4 + 8) / 2;
                                        var md5int = v.slice(4, 12);
                                        var md5char = new Uint8Array(md5int.buffer);
                                        var so = String.fromCharCode.apply(null, md5char);
                                        console.log('--------------------------------------------');
                                        console.log('so=' + so);
                                        console.log('netid=' + assetsidStr);
                                        if (so == assetsidStr) {
                                            for (var ii = stp, isz = v.length / 2; ii < isz; ii++)
                                                txtdcc += v[ii * 2].toString(16) + ' ' + v[ii * 2 + 1].toString(16) + '\n';
                                        }
                                    }
                                    else {
                                        console.log('----------------old format');
                                        for (var ii = 1, isz = v.length / 2; ii < isz; ii++)
                                            txtdcc += v[ii * 2].toString(16) + ' ' + v[ii * 2 + 1].toString(16) + '\n';
                                    }
                                }
                            }
                        }
                        if (txtdcc.length > 0) {
                            cache.saveFileTable(txtdcc);
                            cache = new AppCache(urlpath);
                            cache.setResourceID('netassetsid', assetsidStr);
                        }
                        else {
                        }
                    }
                }
            });
        }
        if (_inline) {
            yield updateDcc();
        }
        var data = yield asyncs.loadText(url);
        for (var n = 0; n < 3 && !data; n++) {
            data = yield asyncs.loadText(url);
        }
        if (!data) {
            if (window["loadingView"] && window["loadingView"].setTips) {
                window["loadingView"].setFontColor("#FF0000");
                window["loadingView"].setTips(['网络异常，请检查您的网络或与开发商联系。']);
            }
            data = cache.loadCachedURL(url);
            if (!data)
                return;
        }
        console.log("");
        if (url.substr(url.length - 3) === '.js') {
            window.eval(data + `
        window.onload&&window.onload();
        //@ sourceURL=${url}
        `);
        }
        else {
            initHtml(data);
        }
        if (window["loadingView"] && window["loadingView"].loadingAutoClose) {
            conch.showLoadingView(false);
        }
    });
}
window.document.addEventListener('keydown', function (e) {
    switch (e.keyCode) {
        case 115:
            simAndroidDeviceLosted();
            break;
        case 116:
            reloadJS(true);
            break;
        case 117:
            history.back();
            break;
        case 118:
            break;
        case 119:
            break;
        case 120:
            gc();
            break;
    }
});
window.loadConchUrl = loadUrl;
//loadUrl(conch.presetUrl || "http://runtime.layabox.com/layaplayer/index.html");
loadUrl(conch.presetUrl || "http://10.10.20.200:8899/node-image.html");
