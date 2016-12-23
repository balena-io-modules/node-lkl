#!/usr/bin/node
const Handlebars = require('handlebars');
const fs = require('fs');

function parseSyscall(s) {
	let regexp = /^(async )?\s*(\w+)\(([^()]+)\)\s*$/;
	let p = s.match(regexp);

	if (!p) {
		return false;
	}

	let knownTypes = [ 'int', 'int64', 'uint32', 'string', '[]byte' ]

	let inp = p[3].split(/\s*,\s*/).map(function (param) {
		let f = param.split(/\s+/);
		if (knownTypes.indexOf(f[1]) < 0) {
			console.log(param)
			throw new Error('Unknown type: ' + f[1])
		}
		return { name: f[0], type: f[1] }
	})

	return {
		async: [1] ? true : false,
		name: p[2],
		inp: inp
	};
}

Handlebars.registerHelper("switch", function(value, options) {
    this._switch_value_ = value;
    var html = options.fn(this); // Process the body of the switch block
    delete this._switch_value_;
    return html;
});

Handlebars.registerHelper("case", function(value, options) {
    if (value == this._switch_value_) {
        return options.fn(this);
    }
});

Handlebars.registerHelper("indent", function(value, options) {
    var body = options.fn(this);
	body = body.replace(/^\s+/, '');
	body = body.replace(/\n\s+/g, '\n' + value);
	return value + body;
});

let syscallSource = fs.readFileSync('syscall.in', 'utf8');
let syscalls = syscallSource.split(/\r?\n/).map(parseSyscall).filter(Boolean);

let context = {syscalls: syscalls}

let source = fs.readFileSync('syscall.cc.tmpl', 'utf8');
let template = Handlebars.compile(source)
fs.writeFileSync('zsyscall.cc', template(context));

source = fs.readFileSync('syscall.h.tmpl', 'utf8');
template = Handlebars.compile(source)
fs.writeFileSync('zsyscall.h', template(context));

source = fs.readFileSync('bindings.cc.tmpl', 'utf8');
template = Handlebars.compile(source)
fs.writeFileSync('zbindings.cc', template(context));
