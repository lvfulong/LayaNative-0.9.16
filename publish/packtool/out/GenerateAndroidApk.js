"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator.throw(value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : new P(function (resolve) { resolve(result.value); }).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments)).next());
    });
};
const fs = require('fs');
const path = require('path');
const fs_extra = require('fs-extra');
const child_process = require('child_process');
const gen_dcc = require('layadcc');
const xmldom = require('xmldom');
const nativeImage = require('electron').nativeImage;
function ResizeImg(img, nw, nh, file) {
    var canv = document.createElement('canvas');
    canv.width = nw;
    canv.height = nh;
    var ctx = canv.getContext('2d');
    ctx.drawImage(img, 0, 0, img.width, img.height, 0, 0, nw, nh);
    var buf = canv.toDataURL('image/png');
    var nimg = nativeImage.createFromDataURL(buf);
    var sz = nimg.getSize();
    console.log('new img:' + sz.width + ',' + sz.height);
    fs.writeFileSync(file, nimg.toPng());
}
function loadImgSync(url) {
    return __awaiter(this, void 0, Promise, function* () {
        return new Promise(function (res, rej) {
            var img = new Image();
            img.src = url;
            img.onload = function () {
                res(img);
            };
            img.onerror = function () {
                res(null);
            };
        });
    });
}
function sleepMs(ms) {
    return __awaiter(this, void 0, Promise, function* () {
        return new Promise(function (res, rej) {
            setTimeout(res, ms);
        });
    });
}
function copyFile(srcf, dstf) {
    return __awaiter(this, void 0, Promise, function* () {
        return new Promise(function (res, rej) {
            fs_extra.copy(srcf, dstf, function (e) {
                if (e) {
                    rej(false);
                    return;
                }
                res(true);
            });
        });
    });
}
function runCmd(cmd, param, onout) {
    return __awaiter(this, void 0, Promise, function* () {
        console.log('run:' + cmd + '\n' + param.join(' '));
        return new Promise(function (res, rej) {
            var cproc = child_process.spawn(cmd, param);
            var hasErr = false;
            cproc.stdout.on('data', function (data) {
                console.log('cmd out:' + data.toString());
                onout('stdout:' + data.toString());
            });
            cproc.stderr.on('data', function (data) {
                if (data.toString().toLowerCase().indexOf('error') != -1) {
                    hasErr = true;
                }
                console.log('stderr:' + data.toString());
                onout('stderr:' + data.toString());
            });
            cproc.on('exit', function (code) {
                if (code == 0 && !hasErr) {
                    res(true);
                }
                else {
                    console.log("runcmd error:" + code);
                    res(false);
                }
            });
        });
    });
}
class GenerateAndroidApk {
    constructor() {
        this.jdkbinpath = null;
        this.apk_path = path.join(__dirname, "../apk/layabox.apk");
        this.confini_path = path.join(__dirname, '../apk/config.ini');
        this.tool_path = path.join(__dirname, "../tool");
        if (process.platform == "win32") {
            this.exec_opt = {
                stdio: [
                    0,
                    'pipe',
                    fs.openSync('err.out', 'w')
                ]
            };
        }
    }
    excute(app_name, output_path, url, loading_view_show, gen_res_pkg, res_path, package_name, icon_path, keystore_path, keystore_pwd, keystore_alias, keystore_alias_pwd, confjs, logoimg, onProcess) {
        console.log('start generate apk...');
        console.log('app_name: ' + app_name);
        console.log('output_path: ' + output_path);
        console.log('url：' + url);
        console.log('loading_view_show：' + loading_view_show);
        console.log('gen_res_pkg： ' + gen_res_pkg);
        console.log('res_path： ' + res_path);
        console.log('package_name： ' + package_name);
        console.log('keystore_path：  ' + keystore_path);
        console.log('icon_path： ' + icon_path);
        console.log('keystore_pwd： ' + keystore_pwd);
        console.log('keystore_alias：  ' + keystore_alias);
        console.log('keystore_alias_pwd：  ' + keystore_alias_pwd);
        if (process.platform == "darwin") {
            var cmd = "chmod -R 755 ";
            cmd += this.tool_path;
            child_process.execSync(cmd);
        }
        if (!app_name || app_name == "") {
            var desc = 'app_name  [' + app_name + '] invalid.';
            throw desc;
        }
        if (!output_path || output_path == "") {
            var desc = 'output_path  [' + output_path + '] invalid.';
            throw desc;
        }
        if (!url)
            url = '';
        var date = new Date;
        this.temp_path = path.join(output_path, String(date.getTime()));
        var me = this;
        function exec() {
            return __awaiter(this, void 0, void 0, function* () {
                function processMsg(msg, err, percent) {
                    if (msg) {
                        console.log('==> ' + percent + '% ' + msg);
                        if (onProcess)
                            onProcess(msg, percent);
                    }
                    if (err)
                        throw err;
                }
                var error = false;
                var arrive_clean = false;
                yield sleepMs(1);
                try {
                    processMsg('copy src apk', null, 10);
                    var tmpApkPath = path.join(me.temp_path, "temp.apk");
                    try {
                        fs_extra.copySync(me.apk_path, tmpApkPath);
                    }
                    catch (e) {
                        processMsg(null, '拷贝文件失败，请检查目录是否正确。', 0);
                    }
                    processMsg('decompile apk.', null, 15);
                    var decomplilePath = path.join(me.temp_path, "decompile");
                    var ret = yield runCmd(me.getToolPath("java"), [
                        '-jar', '-Xms512m', '-Xmx512m',
                        path.join(me.tool_path, "apktool2.jar"),
                        '-q', 'd', '-b',
                        '-f', tmpApkPath,
                        '-o', decomplilePath], function (str) { console.log(str); });
                    if (!ret) {
                        if (confirm('apktool运行错误，可能是java版本太低，系统需要java 1.7以上，点确定下载。')) {
                            window.open('http://www.oracle.com/technetwork/java/javase/downloads/jdk7-downloads-1880260.html');
                        }
                    }
                    processMsg('decompile apk', ret ? null : 'apktool运行错误\n', 20);
                    var hasconfigini_startfromwebview = false;
                    try {
                        var cfgPath = path.join(decomplilePath, "/assets/config.ini");
                        var logoimgPath = path.join(decomplilePath, '/assets/logo/logo.png');
                        if (fs.existsSync(logoimg)) {
                            fs_extra.copySync(logoimg, logoimgPath);
                        }
                        if (fs.existsSync(me.confini_path)) {
                            fs_extra.copySync(me.confini_path, cfgPath);
                            hasconfigini_startfromwebview = true;
                        }
                        var str = me.read(cfgPath);
                        if (loading_view_show) {
                            str = str.replace("loadingViewShow=1", "loadingViewShow=0");
                        }
                        else {
                            str = str.replace("loadingViewShow=0", "loadingViewShow=1");
                        }
                        if (!_inline) {
                            str = str.replace(/^CheckNetwork=.*/m, 'CheckNetwork=0');
                        }
                        str = str.replace(/^IsHandleUpdateAPK=.*/m, 'IsHandleUpdateAPK=0');
                        fs_extra.outputFileSync(cfgPath, str);
                    }
                    catch (e) {
                        error = true;
                    }
                    processMsg('config loading view', null, 25);
                    var urlPath = path.join(decomplilePath, "/assets/scripts/index.js");
                    try {
                        var str = me.read(urlPath);
                        var _inline = url && url.length > 5;
                        if (!_inline) {
                            str = str.replace(/^\s*var\s*_inline\s*=\s*([^;^\n]*)/m, 'var _inline = false;');
                        }
                        if (hasconfigini_startfromwebview) {
                            str = str.replace(/^\s*loadUrl\s*\(.*$/m, `//loadUrl(conch.presetUrl||'${url}');`);
                        }
                        else
                            str = str.replace(/^\s*loadUrl\s*\(.*$/m, `loadUrl(conch.presetUrl||'${url}');`);
                        fs_extra.outputFileSync(urlPath, str);
                    }
                    catch (e) {
                        error = true;
                    }
                    processMsg('replace url', error ? '替换url出错，可能是解包不对。' : null, 30);
                    try {
                        if (fs.existsSync(confjs)) {
                            var confjs_dst = path.join(decomplilePath, "/assets/scripts/config.js");
                            fs_extra.copySync(confjs, confjs_dst);
                        }
                    }
                    catch (e) {
                        processMsg(null, '拷贝config js文件失败，请检查目录是否正确。', 0);
                    }
                    var genCache = gen_res_pkg && res_path && res_path != "";
                    if (genCache) {
                        if (!fs.existsSync(res_path)) {
                            if (url.length > 0) {
                                if (confirm(`
    指定的资源目录不存在！如果继续，包中将不包含资源
        ${res_path}
    还要继续打包么？
                        `)) {
                                    genCache = false;
                                }
                                else {
                                    if (onProcess)
                                        onProcess('error', 0);
                                    return;
                                }
                            }
                            else {
                                me.clean();
                                alert("即没有指定url，又没有指定有效的资源目录，无法继续打包。");
                                if (onProcess)
                                    onProcess('error', 0);
                                return;
                            }
                        }
                    }
                    if (genCache) {
                        var outpath = url.length > 0 ? url : 'http://stand.alone.version/index.html';
                        var index = outpath.indexOf('?');
                        if (index > 0) {
                            outpath = outpath.substring(0, index);
                        }
                        index = outpath.lastIndexOf('/');
                        if (index > 0) {
                            outpath = outpath.substring(0, index);
                        }
                        outpath = outpath.replace("http://", "");
                        outpath = outpath.replace(/:/g, '.');
                        outpath = outpath.replace(/\\/g, '_');
                        outpath = outpath.replace(/\//g, '_');
                        outpath = path.join(path.join(decomplilePath, "/assets/cache/"), outpath);
                        if (!fs.existsSync(outpath)) {
                            fs_extra.mkdirsSync(outpath);
                        }
                        gen_dcc.gendcc(res_path, outpath, true, false);
                        processMsg('make cache', null, 40);
                    }
                    if (package_name && package_name != "") {
                        me.renameApkPackage(path.join(decomplilePath, "/AndroidManifest.xml"), package_name);
                    }
                    processMsg('modify package', null, 50);
                    if (icon_path && icon_path != "") {
                        var img = yield loadImgSync(icon_path);
                        if (img) {
                            var ps = [
                                { f: 'drawable-xxxhdpi-v4', sz: 192 },
                                { f: 'drawable-xxhdpi-v4', sz: 144 },
                                { f: 'drawable-xhdpi-v4', sz: 96 },
                                { f: 'drawable-hdpi-v4', sz: 72 },
                                { f: 'drawable-mdpi-v4', sz: 48 },
                                { f: 'drawable-ldpi-v4', sz: 36 },
                                { f: 'mipmap-xxxhdpi-v4', sz: 192 },
                                { f: 'mipmap-xxhdpi-v4', sz: 144 },
                                { f: 'mipmap-xhdpi-v4', sz: 96 },
                                { f: 'mipmap-hdpi-v4', sz: 72 },
                                { f: 'mipmap-mdpi-v4', sz: 48 },
                                { f: 'mipmap-ldpi-v4', sz: 36 }
                            ];
                            ps.forEach((v, i, arr) => {
                                let f = path.join(decomplilePath, "res/" + v.f + "/ic_launcher.png");
                                if (fs.existsSync(f)) {
                                    ResizeImg(img, v.sz, v.sz, f);
                                }
                            });
                        }
                        processMsg('make icon', img ? null : ('加载图标文件错误：' + icon_path), 55);
                    }
                    me.modifyAppName(path.join(decomplilePath, "res/values/strings.xml"), app_name);
                    me.modifyAppName(path.join(decomplilePath, "res/values-zh-rCN/strings.xml"), app_name);
                    processMsg('modify app name', null, 60);
                    yield me.produceNewRFile(app_name, package_name, decomplilePath, function (msg, err) {
                        if (err) {
                            throw err;
                        }
                    });
                    processMsg('gen R file', null, 70);
                    var tempApkPath = path.join(me.temp_path, "temp2.apk");
                    ret = yield runCmd(me.getToolPath("java"), [
                        '-jar', '-Xms512m', '-Xmx512m',
                        path.join(me.tool_path, "apktool2.jar"),
                        '-q', 'b',
                        '-f', decomplilePath,
                        '-o', tempApkPath
                    ], function (str) { console.log(str); });
                    processMsg('repacking', ret ? null : '重新打包失败！', 80);
                    console.log('...');
                    if (fs.existsSync(path.join(decomplilePath, "unknown"))) {
                        var unknowPath = path.join(decomplilePath, "AddForRoot");
                        fs.renameSync(path.join(decomplilePath, "unknown"), unknowPath);
                        me.addForRootDir(tempApkPath, unknowPath);
                    }
                    var default_keystore_path = path.join(__dirname, "../config/keystore/default.keystore");
                    var realSign = (keystore_path && keystore_path.length > 0) &&
                        (keystore_pwd && keystore_pwd.length > 0) &&
                        (keystore_alias && keystore_alias.length > 0) &&
                        (keystore_alias_pwd && keystore_alias_pwd.length > 0);
                    if (realSign) {
                        if (fs.existsSync(keystore_path)) {
                            yield me.signApk(tempApkPath, keystore_path, keystore_pwd, keystore_alias, keystore_alias_pwd);
                        }
                        else {
                            if (confirm(`
    指定的签名文件不存在：
    ${keystore_path}
    如果继续打包，将使用缺省签名，是否继续？    
                        `)) {
                                realSign = false;
                            }
                            else {
                                me.clean();
                                if (onProcess)
                                    onProcess('error', 0);
                                return;
                            }
                        }
                    }
                    if (!realSign) {
                        yield me.signApk(tempApkPath, default_keystore_path, '123456', '123456', '123456');
                    }
                    processMsg('singing', null, 90);
                    console.log('align apk.');
                    var output_apk_path = path.join(output_path, "game.apk");
                    yield me.alignAPK(tempApkPath, output_apk_path);
                    processMsg('clean', null, 99);
                    arrive_clean = true;
                    me.clean();
                    processMsg('complete', null, 100);
                }
                catch (err) {
                    try {
                        me.clean();
                    }
                    catch (e) { }
                    if (arrive_clean) {
                        processMsg('complete', null, 100);
                    }
                    else {
                        alert(err);
                        if (onProcess)
                            onProcess('error', 0);
                    }
                }
            });
        }
        exec();
    }
    produceNewRFile(app_name, package_name, decomplile_path, eventh) {
        return __awaiter(this, void 0, void 0, function* () {
            var temp_path = path.join(decomplile_path, "tempRFile");
            var target_res_path = path.join(temp_path, "res");
            fs_extra.copySync(path.join(decomplile_path, "res"), target_res_path);
            var gen_path = path.join(temp_path, "gen");
            if (!fs.existsSync(gen_path)) {
                fs.mkdirSync(gen_path);
            }
            var me = this;
            var ret = yield runCmd(me.getToolPath("aapt"), ['p', '-f', '-m',
                '-J', gen_path,
                '-S', target_res_path,
                '-I', me.getToolPath("android"),
                '-M', path.join(decomplile_path, "AndroidManifest.xml")
            ], function (str) { });
            if (!ret && eventh) {
                eventh(null, '修改包名错误，请检查包名是否规范。');
                return;
            }
            var package_array = package_name.split(".");
            var r_path = gen_path;
            for (var i = 0; i < package_array.length; i++) {
                r_path = path.join(r_path, package_array[i]);
            }
            r_path = path.join(r_path, 'R.java');
            ret = yield runCmd(me.getToolPath("javac"), ['-source', '1.7',
                '-target', '1.7',
                '-encoding', 'UTF-8',
                r_path], function (str) { });
            if (!ret && eventh) {
                eventh(null, '重新编译java文件错误。');
                return;
            }
            var dex_path = path.join(temp_path, 'class.dex');
            if (process.platform == "win32") {
                ret = yield runCmd(me.getToolPath("dx"), ['--dex', '--output=' + dex_path, gen_path], function (str) { });
            }
            else {
                ret = yield runCmd(me.getToolPath("java"), ['-jar', '-Xms512m', '-Xmx512m',
                    me.getToolPath("dx"),
                    '--dex', '--output=' + dex_path,
                    gen_path
                ], function (str) { });
            }
            var smali_path = path.join(decomplile_path, "smali");
        });
    }
    addForRootDir(tempApkPath, src) {
    }
    signApk(src_apk, keyStore, storepassword, keyalias, aliaspassword) {
        return __awaiter(this, void 0, void 0, function* () {
            var files = '';
            var me = this;
            var ret = yield runCmd(me.getToolPath("aapt"), [
                'list', src_apk
            ], function (str) { files += str; });
            if (!ret) {
                throw 'appt list error！';
            }
            var array = files.toString().split("\n");
            for (var i = 0; i < array.length; i++) {
                var file_name = array[i];
                if (file_name != "" && file_name.includes("META-INF")) {
                    console.log('remove ' + array[i]);
                    var ret = yield runCmd(me.getToolPath("aapt"), ['remove', src_apk, array[i]], function (str) { });
                    if (!ret) {
                        throw 'aapt remove error!';
                    }
                }
            }
            ret = yield runCmd(me.getToolPath("jarsigner"), [
                '-keystore', keyStore,
                '-storepass', storepassword,
                '-keypass', aliaspassword,
                src_apk, keyalias,
                '-sigalg', 'SHA1withRSA', '-digestalg', 'SHA1'
            ], function (str) {
                console.log(str);
            });
            if (!ret) {
                throw '重新签名错误，可能是密码不对。';
            }
        });
    }
    alignAPK(src_apk, des_apk) {
        return __awaiter(this, void 0, void 0, function* () {
            var me = this;
            var ret = yield runCmd(me.getToolPath("zipalign"), ['-f', '4', src_apk, des_apk], function (str) { });
        });
    }
    renameApkPackage(manifest_path, package_name) {
        var str = this.read(manifest_path);
        var doc = new xmldom.DOMParser().parseFromString(str);
        var allres = doc.documentElement.setAttribute('package', package_name);
        fs_extra.outputFileSync(manifest_path, doc.toString());
    }
    modifyAppName(file_path, app_name) {
        var str = this.read(file_path);
        if (!str || str.length <= 0)
            return;
        var doc = new xmldom.DOMParser().parseFromString(str);
        var allres = doc.documentElement.getElementsByTagName('string');
        for (var i = 0; i < allres.length; i++) {
            if (allres[i].getAttribute('name') == 'app_name') {
                allres[i].childNodes[0]['data'] = app_name;
            }
        }
        fs_extra.outputFileSync(file_path, doc.toString());
    }
    read(path) {
        try {
            var text = fs.readFileSync(path, "utf-8");
            text = text.replace(/^\uFEFF/, '');
        }
        catch (err0) {
            return "";
        }
        return text;
    }
    getToolPath(tool) {
        if (process.platform == "win32") {
            if (tool == "zipalign") {
                return path.join(this.tool_path, "win/zipalign.exe");
            }
            else if (tool == "aapt") {
                return path.join(this.tool_path, "win/aapt.exe");
            }
            else if (tool == "java") {
                return path.join(this.tool_path, "win/jre/bin/java");
            }
            else if (tool == "jarsigner") {
                return path.join(this.tool_path, "win/jre/bin/jarsigner");
            }
            else if (tool == "javac") {
                return path.join(this.tool_path, "win/jre/bin/javac");
            }
            else if (tool == "android") {
                return path.join(this.tool_path, "android.jar");
            }
            else if (tool == "dx") {
                return path.join(this.tool_path, "win/dx.bat");
            }
            else if (tool == "baksmali") {
                return path.join(this.tool_path, "baksmali.jar");
            }
        }
        else if (process.platform == "darwin") {
            if (tool == "zipalign") {
                return path.join(this.tool_path, "mac/zipalign");
            }
            else if (tool == "aapt") {
                return path.join(this.tool_path, "mac/aapt");
            }
            else if (tool == "java") {
                return 'java';
            }
            else if (tool == "jarsigner") {
                return tool;
            }
            else if (tool == "javac") {
                return 'javac';
            }
            else if (tool == "android") {
                return path.join(this.tool_path, "android.jar");
            }
            else if (tool == "dx") {
                return path.join(this.tool_path, "dx.jar");
            }
            else if (tool == "baksmali") {
                return path.join(this.tool_path, "baksmali.jar");
            }
        }
        else {
            var desc = process.platform + 'paltform not support';
            throw desc;
        }
    }
    clean() {
        if (this.temp_path && fs.existsSync(this.temp_path)) {
            fs_extra.removeSync(this.temp_path);
        }
    }
    checkJavaEnv() {
        var javaok = true;
        var cmd = this.getToolPath('javac');
        var jret = child_process.spawnSync(cmd, ['-version'], { stdio: [0, 'pipe', 'pipe'] });
        console.log('javac -version');
        console.log('  out:' + jret.stdout);
        console.log('  err:' + jret.stderr);
        if (jret.status != 0) {
            console.log('执行javac出错:\n' + jret.stderr);
            javaok = false;
        }
        else {
            var rret = /javac\s*(.*)/.exec(jret.stderr.toString());
            if (!rret)
                javaok = false;
            else {
                var vers = rret[1].split('.');
                if (vers.length < 2) {
                    javaok = false;
                }
                else {
                    if (vers[0] >= '1' && parseInt(vers[1]) >= 7) { }
                    else
                        javaok = false;
                }
            }
        }
        return javaok;
    }
}
exports.GenerateAndroidApk = GenerateAndroidApk;

