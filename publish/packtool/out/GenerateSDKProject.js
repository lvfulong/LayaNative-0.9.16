"use strict";
const fs = require('fs');
const path = require('path');
const fs_extra = require('fs-extra');
const gen_dcc = require('layadcc');
class GenerateSDKProject {
    constructor(type) {
        if (type == "iOS") {
            this.template_path = path.join(__dirname, "../template/iOS");
        }
        else if (type == "Android-eclipse") {
            this.template_path = path.join(__dirname, "../template/Android_eclipse");
        }
        else if (type == 'Android-Studio') {
            this.template_path = path.join(__dirname, "../template/Android_studio");
        }
        else {
            var desc = 'invalid parameter.';
            throw desc;
        }
    }
    excute(proj_name, proj_path, url, package_name, res_path) {
        console.log('start generate project...');
        console.log('proj_name: ' + proj_name);
        console.log('proj_path: ' + proj_path);
        console.log('url：' + url ? url : "http://stand.alone.version/index.html");
        console.log('package_name: ' + package_name);
        console.log('res_path' + res_path);
        if (!proj_name || proj_name == "") {
            var desc = 'proj_name  [' + proj_name + '] invalid.';
            throw desc;
        }
        if (!proj_path || proj_path == "") {
            var desc = 'proj_path  [' + proj_path + '] invalid.';
            throw desc;
        }
        var config = fs_extra.readJSONSync(path.join(this.template_path, "config.json"));
        var me = this;
        if (fs.existsSync(path.join(proj_path, proj_name))) {
            alert("同名文件 " + path.join(proj_path, proj_name) + " 已经存在,请选择其他输出目录");
            return;
        }
        config["template"]["source"].forEach(function (source) {
            var srcPath = path.join(me.template_path, source);
            var destPath = path.join(proj_path, source);
            if (fs.existsSync(destPath)) {
                alert("发现同名文件，请选择其他输出目录");
                return;
            }
            fs_extra.copySync(srcPath, destPath);
        });
        if (!url) {
            if (config.localize && config.localize.replace) {
                config.localize.replace.forEach((v, i, arr) => {
                    var p = path.join(proj_path, v);
                    var s = me.read(p);
                    s = s.replace(new RegExp(config.localize.src, 'g'), config.localize.des);
                    fs.writeFileSync(p, s);
                });
            }
            url = "http://stand.alone.version/index.html";
        }
        {
            config["url"]["replace"].forEach(function (file) {
                var srcPath = path.join(proj_path, file);
                var str = me.read(srcPath);
                str = str.replace(new RegExp(config["url"]["src"]), config["url"]["des"].replace("${url}", url));
                fs_extra.outputFileSync(srcPath, str);
            });
        }
        if (package_name && package_name != "") {
            config["package"]["replace"].forEach(function (file) {
                var destPath = path.join(proj_path, file);
                var str = me.read(destPath);
                str = str.replace(new RegExp(config["package"]["name"], "g"), package_name);
                fs_extra.outputFileSync(destPath, str);
            });
        }
        if (fs.existsSync(res_path) && res_path && res_path != "") {
            var outpath = url;
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
            outpath = path.join(config["res"]["path"], outpath);
            outpath = path.join(proj_path, outpath);
            if (!fs.existsSync(outpath)) {
                fs_extra.mkdirsSync(outpath);
            }
            gen_dcc.gendcc(res_path, outpath, true, false);
        }
        {
            config["template"]["replace"].forEach(function (file) {
                var srcPath = path.join(proj_path, file);
                var str = me.read(srcPath);
                str = str.replace(new RegExp(config["template"]["name"], "g"), proj_name);
                fs_extra.outputFileSync(srcPath, str);
            });
            config["template"]["rename"].forEach(function (file) {
                var oldPath = path.join(proj_path, file);
                var newPath = path.join(proj_path, file);
                var dir_name = path.dirname(newPath);
                var base_name = path.basename(newPath);
                var new_base_name = base_name.replace(config["template"]["name"], proj_name);
                newPath = path.join(dir_name, new_base_name);
                fs.renameSync(oldPath, newPath);
            });
        }
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
}
exports.GenerateSDKProject = GenerateSDKProject;
