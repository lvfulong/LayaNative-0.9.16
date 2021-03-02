var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var ConchByte = (function () {
    function ConchByte(data) {
        this.getUint8 = function () {
            return this._d_.getUint8(this._pos_++);
        };
        this._xd_ = true;
        this._pos_ = 0;
        this._u8d_ = new Uint8Array(data);
        this._d_ = new DataView(this._u8d_.buffer);
        this._length = this._d_.byteLength;
    }
    Object.defineProperty(ConchByte.prototype, "pos", {
        get: function () {
            return this._pos_;
        },
        set: function (p) {
            this._pos_ = p;
        },
        enumerable: false,
        configurable: true
    });
    ConchByte.prototype.getFloat32 = function () {
        var v = this._d_.getFloat32(this._pos_, this._xd_);
        this._pos_ += 4;
        return v;
    };
    ConchByte.prototype.getUint16 = function () {
        var us = this._d_.getUint16(this._pos_, this._xd_);
        this._pos_ += 2;
        return us;
    };
    ConchByte.prototype.getInt16 = function () {
        var us = this._d_.getInt16(this._pos_, this._xd_);
        this._pos_ += 2;
        return us;
    };
    ConchByte.prototype.getInt32 = function () {
        var float = this._d_.getInt32(this._pos_, this._xd_);
        this._pos_ += 4;
        return float;
    };
    return ConchByte;
}());
var ConchNodeNew = (function (_super) {
    __extends(ConchNodeNew, _super);
    function ConchNodeNew() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    ConchNodeNew.prototype.addChild = function (child) {
        this._childs = this._childs || [];
        this.addChildAt(child, this._childs.length);
        this._childs.push(child);
        child.parentNode = this;
        return child;
    };
    ConchNodeNew.checkexits = function (fname) {
        if (!fs_exists(ConchNodeNew.Path + fname)) {
            var ab = conch.readFileFromAsset(fname, 'raw');
            fs_writeFileSync(ConchNodeNew.Path + fname, ab);
        }
    };
    ConchNodeNew.prototype.loadImage = function (url) {
        this._graphics = this._graphics || new ConchGraphics();
        var img = document.createElement("image");
        var that = this;
        img.onload = function (e) {
            var w = img.width, h = img.height;
            that._graphics.drawImageM(img, 0, 0, w, h, 0, 0, w, h, null);
        };
        ConchNodeNew.checkexits(url);
        img.src = ConchNodeNew.Protocol + ConchNodeNew.Path + url;
        this.graphics(this._graphics);
    };
    ConchNodeNew.prototype.removeChildNew = function (node) {
        if (!this._childs)
            return null;
        var i = this._childs.indexOf(node);
        if (i != -1) {
            this._childs.splice(i, 1);
            this.removeChild(node);
            node.parentNode = null;
        }
        return node;
    };
    ConchNodeNew.prototype.removeSelf = function () {
        this.parentNode && this.parentNode.removeChildNew(this);
        return this;
    };
    ConchNodeNew.prototype.updateZOrder = function () {
        if ((!this._childs) || this._childs.length < 2)
            return;
        var c = this._childs[0];
        var i = 1, sz = this._childs.length;
        var z = c.zOrder || 0, low, high, mid, zz;
        for (i = 1; i < sz; i++) {
            c = this._childs[i];
            if (!c)
                continue;
            if ((z = c.zOrder || 0) < 0)
                z = c.zOrder || 0;
            if (z < this._childs[i - 1].zOrder || 0) {
                mid = low = 0;
                high = i - 1;
                while (low <= high) {
                    mid = (low + high) >>> 1;
                    if (!this._childs[mid])
                        break;
                    zz = this._childs[mid].zOrder || 0;
                    if (zz < 0)
                        zz = this._childs[mid].zOrder || 0;
                    if (zz < z)
                        low = mid + 1;
                    else if (zz > z)
                        high = mid - 1;
                    else
                        break;
                }
                if (z > (this._childs[mid].zOrder || 0))
                    mid++;
                var f = c.parentNode;
                this._childs.splice(i, 1);
                this._childs.splice(mid, 0, c);
                if (f) {
                    f.removeChild(c);
                    f.addChildAt(c, mid);
                }
            }
        }
    };
    ConchNodeNew.prototype.removeChildren = function () {
        if (this._childs && this._childs.length > 0) {
            var len = this._childs.length;
            var childs = this._childs.splice(0, len);
            this._childs.length = 0;
            for (var i = 0; i < len; i++) {
                this.removeChild(childs[i]);
            }
        }
    };
    return ConchNodeNew;
}(ConchNode));
var ConchLoadingView = (function (_super) {
    __extends(ConchLoadingView, _super);
    function ConchLoadingView() {
        var _this = _super.call(this) || this;
        _this._tips = ["新世界的大门即将打开", "敌军还有30秒抵达战场", "妈妈说，心急吃不了热豆腐"];
        _this._len = [];
        _this.prei = 0;
        _this.loadingAutoClose = true;
        _this.showTextInfo = true;
        fs_mkdir(conchConfig.getStoragePath() + "logo");
        fs_mkdir(conchConfig.getStoragePath() + "logo/image");
        ConchNodeNew.Protocol = "file:///";
        ConchNodeNew.Path = conchConfig.getStoragePath();
        _this.p = 0;
        _this.need = 2000;
        _this.barlen = 260;
        _this.barx = _this.barlen / 100;
        _this.fontstr = "normal 100 16px Arial";
        _this.fc = "#000000";
        _this.initlen();
        _this.initSwf();
        _this.setLoadingView();
        return _this;
    }
    ConchLoadingView.prototype.initlen = function () {
        ConchTextCanvas.setFontInfo(this.fontstr);
        for (var i = 0, n = this._tips.length; i < n; i++) {
            this._len[i] = ConchTextCanvas.measureText(this._tips[i]).width;
        }
    };
    ConchLoadingView.prototype.setFontColor = function (c) {
        this.fc = c;
    };
    ConchLoadingView.prototype.setTips = function (a) {
        this._tips = a;
        this.initlen();
    };
    ConchLoadingView.prototype.createNode = function (img, x, y, w, h) {
        var n = new ConchNodeNew();
        var g = n._graphics = new ConchGraphics();
        n.graphics(g);
        g.drawImageM(img, x, y, w, h, 0, 0, w, h, null);
        return n;
    };
    ConchLoadingView.prototype.initSwf = function () {
        var _t = this;
        var logo = document.createElement("img");
        var mc = this.mc = new ConchMovieClip(null);
        logo.onload = function (e) {
            ConchLoadingView.IsMV = false;
            _t.need = 0;
            _t._init(logo);
        };
        logo.onerror = function (e) {
            _t.mc.load("logo.swf", _t.onloaded.bind(_t), null);
            _t._init(null);
        };
        ConchNodeNew.checkexits("logo/logo.png");
        logo.src = ConchNodeNew.Protocol + ConchNodeNew.Path + "logo/logo.png";
        this.bgColor("#ffffff");
        this.tips = new ConchNodeNew();
        var tg = this.tips._graphics = new ConchGraphics();
        this.tips.graphics(tg);
    };
    ConchLoadingView.prototype._init = function (logo) {
        var _t = this;
        var n = this.n = new ConchNodeNew();
        var lbar = new ConchNodeNew();
        if (!logo) {
            var img = document.createElement("img");
            img.onload = function () {
                var g = _t.createNode(img, 25, 1, 15, 20);
                var f = _t.createNode(img, 6, 1, 15, 20);
                var _g = _t.createNode(img, 25, 1, 15, 20);
                _g.scale(-1, 1);
                var _f = _t._f = _t.createNode(img, 6, 1, 15, 20);
                _f.scale(-1, 1);
                var base = _t.createMBar(img, _t.barlen, 44, 1, 1, 20);
                lbar.addChild(g);
                lbar.addChild(f);
                lbar.addChild(_g);
                _g.pos(_t.barlen + 30, 0);
                lbar.addChild(base);
                base.pos(15, 0);
                _f.pos(30 + _t.p * _t.barx, 0);
                _t.bar = _t.createMBar(img, 0, 1, 1, 1, 20);
                _t.bar.pos(15, 0);
                lbar.addChild(_t.bar);
                lbar.addChild(_f);
            };
            ConchNodeNew.checkexits("logo/loading.png");
            img.src = ConchNodeNew.Protocol + ConchNodeNew.Path + "logo/loading.png";
        }
        if (!logo) {
            n.addChild(this.mc);
        }
        else {
            var i = 0;
            this._int = setInterval(function () {
                _t.renderText(i);
                i++;
            }, 1000 / 41);
            this.logoNode = this.createNode(logo, 0, 0, logo.width, logo.height);
            this.logoNode.pivot(1 / 2 * logo.width, 1 / 2 * logo.height);
            n.addChild(this.logoNode);
        }
        n.addChild(lbar);
        n.addChild(this.tips);
        this.addChild(n);
        this.onresize();
        document.addEventListener("resize", this.onresize.bind(this));
    };
    ConchLoadingView.prototype.resume = function () {
        HTMLCanvasElement.RS = false;
        this.mainCanvas.onCSS_Transform = this.ot;
        this.mainCanvas.width = this.mainCanvas._w;
        this.mainCanvas.height = this.mainCanvas._h;
        this.mainCanvas._tranform && this.mainCanvas.onCSS_Transform(this.mainCanvas._tranform);
    };
    ConchLoadingView.prototype.onresize = function () {
        if (!window["loadingView"])
            return;
        if (!this.mainCanvas) {
            this.mainCanvas = document.createElement("canvas");
            this.mainCanvas.getContext("conch");
            this.ot = this.mainCanvas.onCSS_Transform;
            this.mainCanvas.onCSS_Transform = function (mat) {
                this._tranform = mat;
                this._hasTransform = true;
            };
            HTMLCanvasElement.RS = true;
        }
        this.w = window.innerWidth;
        this.h = window.innerHeight;
        this.mainCanvas.setSize(this.w, this.h);
        this.size(this.w, this.h);
        var a, b;
        if (ConchLoadingView.IsMV) {
            a = 480;
            b = 640;
        }
        else {
            a = 1024;
            b = 768;
            this.logoNode.pos(1 / 2 * a, 1 / 2 * b);
        }
        var sx = this.w / a;
        var sy = this.h / b;
        var scale = Math.min(sx, sy);
        var x = (this.w - scale * a) / 2;
        var y = (this.h - scale * b) / 2;
        if (this.n) {
            this.n.scale(scale, scale);
            this.n.pos(x, y);
            var lbar = this.n._childs[1];
            if (lbar) {
                lbar.pos(100, b / 1.5);
            }
            this.tips.pos(a / 2, b / 1.3);
        }
    };
    ConchLoadingView.prototype.renderText = function (i) {
        if (i % 41 == 0) {
            this.prei = i / 41 % this._tips.length;
            this._dt();
        }
        if (i == 72 || this.need == 0) {
            this.need = 0;
            if (this._del)
                this.destory();
        }
    };
    ConchLoadingView.prototype._dt = function () {
        if (this.showTextInfo == true) {
            var g = this.tips._graphics;
            g.clear();
            g.fillText(this._tips[this.prei], -26, 0, this.fontstr, this.fc, "center");
            g.fillText("(" + this.p + "%)", 1 / 2 * this._len[this.prei] - 26, 0, this.fontstr, this.fc, "left");
        }
    };
    ConchLoadingView.prototype.onloaded = function () {
        this.mc.renderText = this.renderText.bind(this);
    };
    ConchLoadingView.prototype.destory = function () {
        if (window["loadingView"]) {
            this.setLoadingView();
            this.resume();
            this._int && clearInterval(this._int);
            this.mc.destory();
            window["loadingView"] = null;
        }
    };
    ConchLoadingView.prototype.createMBar = function (img, l, x, y, w, h) {
        var t = new ConchNodeNew();
        var g = t._graphics = new ConchGraphics();
        t.graphics(t._graphics);
        var _t = this;
        var p = new CanvasPattern(img.imgId, 1, x, y, w, h);
        g["pat"] = p;
        g._fillImage(p, 0, 0, 0, 0, l || _t.p * this.barx, h);
        return t;
    };
    ConchLoadingView.prototype.loading = function (pre) {
        if (pre > 100)
            pre = 100;
        if (pre < 0)
            pre = 0;
        if (this.p != pre) {
            this.p = pre;
            this._dt();
            if (this.bar && this.bar._graphics) {
                var g = this.bar._graphics;
                g.clear();
                var p = g["pat"];
                p && g._fillImage(p, 0, 0, 0, 0, pre * this.barx, p.h);
                this._f.pos(30 + pre * this.barx, 0);
            }
            if (pre == 100) {
                this._del = true;
                if (this.need == 0 || !ConchLoadingView.IsMV) {
                    this.destory();
                }
                return;
            }
        }
    };
    ConchLoadingView.IsMV = true;
    return ConchLoadingView;
}(ConchNodeNew));
var ConchMovieClip = (function (_super) {
    __extends(ConchMovieClip, _super);
    function ConchMovieClip(parentMovieClip) {
        var _this = _super.call(this) || this;
        _this._ids = {};
        _this._idOfSprite = [];
        _this._reset(true);
        _this._start = _this._Pos = _this._c = 0;
        _this._ended == true;
        _this._parentMovieClip = parentMovieClip;
        if (!parentMovieClip) {
            _this._movieClipList = [_this];
        }
        else {
            _this._movieClipList = parentMovieClip._movieClipList;
            _this._movieClipList.push(_this);
        }
        return _this;
    }
    ConchMovieClip.prototype.start = function () {
        this._inv = setInterval(this.updates.bind(this), this.interval);
    };
    ConchMovieClip.prototype._clear = function () {
        this.stop();
        this._idOfSprite.length = 0;
        if (!this._parentMovieClip) {
            var i, len;
            len = this._movieClipList.length;
            for (i = 0; i < len; i++) {
                if (this._movieClipList[i] != this)
                    this._movieClipList[i]._clear();
            }
            this._movieClipList.length = 0;
        }
        this.removeChildren();
        this._graphics = null;
        this._parentMovieClip = null;
    };
    ConchMovieClip.prototype.load = function (src, cb, oe) {
        var a = new XMLHttpRequest();
        this.basePath = src.split(".swf")[0] + "/image/";
        this._clear();
        a.responseType = "arraybuffer";
        this._movieClipList = [this];
        var that = this;
        a.onload = function (e) {
            that._initData(a.response);
            that._graphics = new ConchGraphics();
            that.graphics(that._graphics);
            that.pos(45, 150);
            that.start();
            cb && cb();
        };
        a.onerror = function (e) {
            oe && oe();
        };
        ConchNodeNew.checkexits(src);
        a.open("GET", ConchNodeNew.Protocol + ConchNodeNew.Path + src, false);
        a.send(null);
    };
    ConchMovieClip.prototype.stop = function () {
        this._playing = false;
    };
    ConchMovieClip.prototype.destory = function () {
        clearInterval(this._inv);
    };
    ConchMovieClip.prototype._initData = function (data) {
        this._data = new ConchByte(data);
        var i, len = this._data.getUint16();
        for (i = 0; i < len; i++)
            this._ids[this._data.getInt16()] = this._data.getInt32();
        this.interval = 1000 / this._data.getUint16();
        this._setData(this._data, this._ids[32767]);
        this._initState();
        this.play(0, true);
    };
    ConchMovieClip.prototype._setData = function (data, start) {
        this._data = data;
        this._start = start + 3;
    };
    ConchMovieClip.prototype._initState = function () {
        this._reset(true);
        this._ended = false;
        var preState = this._playing;
        this._playing = false;
        this._curIndex = 0;
        while (!this._ended) {
            this._parse(++this._curIndex);
        }
        this._playing = preState;
    };
    ConchMovieClip.prototype._reset = function (rm) {
        if (rm && this._curIndex != 1)
            this.removeChildren();
        this._preIndex = this._curIndex = -1;
        this._Pos = this._start;
    };
    ConchMovieClip.prototype.play = function (index, loop) {
        this.loop = loop;
        this._playing = true;
        if (this._data)
            this._displayFrame(index);
    };
    ConchMovieClip.prototype._displayFrame = function (frameIndex) {
        if (frameIndex != -1) {
            if (this._curIndex > frameIndex)
                this._reset(true);
            this._parse(frameIndex);
        }
    };
    ConchMovieClip.prototype._update = function () {
        if (!this._data)
            return;
        if (!this._playing)
            return;
        this._playIndex++;
        if (this._playIndex >= this._count) {
            if (!this.loop) {
                this._playIndex--;
                this.stop();
                return;
            }
            this._playIndex = 0;
        }
        this._parse(this._playIndex);
    };
    ConchMovieClip.prototype.updates = function () {
        if (this._parentMovieClip)
            return;
        var i, len;
        len = this._movieClipList.length;
        for (i = 0; i < len; i++) {
            this._movieClipList[i] && this._movieClipList[i]._update();
        }
        this.renderText && this.renderText(this._c++);
    };
    ConchMovieClip.prototype._parse = function (frameIndex) {
        var curChild = this;
        var mc, sp, key, type, tPos, ttype, ifAdd = false;
        var _idOfSprite = this._idOfSprite, _data = this._data, eStr;
        if (this._ended)
            this._reset(true);
        _data.pos = this._Pos;
        this._ended = false;
        this._playIndex = frameIndex;
        if (this._curIndex > frameIndex && frameIndex < this._preIndex) {
            this._reset(true);
            _data.pos = this._Pos;
        }
        while ((this._curIndex <= frameIndex) && (!this._ended)) {
            type = _data.getUint16();
            switch (type) {
                case 12:
                    key = _data.getUint16();
                    tPos = this._ids[_data.getUint16()];
                    this._Pos = _data.pos;
                    _data.pos = tPos;
                    if ((ttype = _data.getUint8()) == 0) {
                        var pid = _data.getUint16();
                        sp = this._idOfSprite[key];
                        if (!sp) {
                            sp = _idOfSprite[key] = new ConchNodeNew();
                            var spp = new ConchNodeNew();
                            spp.loadImage(this.basePath + pid + ".png");
                            sp.addChild(spp);
                            spp.size(_data.getFloat32(), _data.getFloat32());
                            spp.transform(_data.getFloat32(), _data.getFloat32(), _data.getFloat32(), _data.getFloat32(), _data.getFloat32(), _data.getFloat32());
                        }
                        sp.alpha(1);
                    }
                    else if (ttype == 1) {
                        mc = _idOfSprite[key];
                        if (!mc) {
                            _idOfSprite[key] = mc = new ConchMovieClip(this);
                            mc.interval = this.interval;
                            mc._ids = this._ids;
                            mc.basePath = this.basePath;
                            mc._setData(_data, tPos);
                            mc._initState();
                            mc.play(0, true);
                        }
                        mc.alpha(1);
                    }
                    _data.pos = this._Pos;
                    break;
                case 3:
                    (this.addChild(_idOfSprite[_data.getUint16()])).zOrder = _data.getUint16();
                    ifAdd = true;
                    break;
                case 4:
                    _idOfSprite[_data.getUint16()].removeSelf();
                    break;
                case 7:
                    sp = _idOfSprite[_data.getUint16()];
                    sp.transform(_data.getFloat32(), _data.getFloat32(), _data.getFloat32(), _data.getFloat32(), _data.getFloat32(), _data.getFloat32());
                    break;
                case 10:
                    _idOfSprite[_data.getUint16()].alpha(_data.getFloat32());
                    break;
                case 99:
                    this._curIndex = _data.getUint16();
                    ifAdd && this.updateZOrder();
                    break;
                case 100:
                    this._count = this._curIndex + 1;
                    this._ended = true;
                    this._reset(false);
                    break;
                default:
                    alert(type);
                    break;
            }
        }
        this._Pos = _data.pos;
    };
    ConchMovieClip._ValueList = ["x", "y", "width", "height", "scaleX", "scaleY", "rotation", "alpha"];
    return ConchMovieClip;
}(ConchNodeNew));
window["startLoading"] = function () {
    window["loadingView"] = new ConchLoadingView();
};
