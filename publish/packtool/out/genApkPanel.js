'use strict';
function createTag(tag, prop, parent) {
    const prebuildTag = ['div', 'img', 'input', 'span', 'a', 'select', 'button'];
    var div = null;
    if (prebuildTag.indexOf(tag) >= 0) {
        div = document.createElement(tag);
    }
    else {
        if (tag.indexOf('\'') >= 0) {
            div = document.createElement('span');
            var r = /\'([^\']*)\'/.exec(tag);
            div.innerHTML = r[1];
        }
        else {
            div = document.createElement('div');
            div.setAttribute('class', tag);
        }
    }
    const propHandle = {
        'cls': function (v) { this.setAttribute('class', v); },
        'id': function (v) { this.id = v; },
        'text': function (v) {
            if (tag == 'select') {
                for (let vi = 0; vi < v.length; vi++) {
                    div.appendChild(new Option(v[vi]));
                }
            }
            else {
                this.innerHTML = v.replace(/\s/g, '&nbsp;');
            }
        },
        'type': function (v) {
            this.type = v;
            if (v == 'input') {
                this.onfocuse = null;
                this.onblur = null;
            }
        }
    };
    for (var p in prop) {
        var h = propHandle[p];
        if (h) {
            h.call(div, prop[p]);
        }
        else {
            div.setAttribute(p, prop[p]);
        }
    }
    if (parent)
        parent.appendChild(div);
    return div;
}
class HtmlNodeBranch {
    constructor(desc) {
        this.root = null;
    }
    createFromDesc(desc, createFunc) {
        if (!createFunc)
            createFunc = createTag;
        var lines = desc.split('\n');
        var nodeLayer = new Array();
        for (let i in lines) {
            var l = lines[i];
            if (l.length <= 0)
                continue;
            var ldesc = this.getLayer(l);
            var lprops = eval('new Object({' + ldesc.prop + '})');
            var ele = createFunc(ldesc.tag, lprops);
            nodeLayer[ldesc.layer] = ele;
            if (ldesc.layer > 0) {
                nodeLayer[ldesc.layer - 1].appendChild(ele);
            }
        }
        this.root = nodeLayer[0];
    }
    getLayer(desc) {
        var layer = 0;
        var i = 0;
        for (; i < desc.length; i++) {
            if (desc[i] == '|')
                layer++;
            else
                break;
        }
        if (desc.substr(i, 2) != '+-') {
            throw 'errror! bad format :' + desc;
        }
        var left = desc.substr(i + 2);
        var typepos = left.indexOf(' ');
        var tag = left;
        if (typepos > 0) {
            tag = left.substr(0, typepos);
            return { layer: layer, tag: tag, prop: left.substr(typepos + 1) };
        }
        else {
            return { layer: layer, tag: tag, prop: '' };
        }
    }
}
exports.HtmlNodeBranch = HtmlNodeBranch;
