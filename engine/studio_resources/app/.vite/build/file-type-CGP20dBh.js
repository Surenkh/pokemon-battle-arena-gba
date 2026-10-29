import { i as e, o as t, t as n } from "./rolldown-runtime-CJfroGDQ.js";
import { t as r } from "./main.js";
import { open as i } from "node:fs/promises";
//#region node_modules/strtok3/lib/stream/Errors.js
var a = "End-Of-Stream", o = class extends Error {
	constructor() {
		super(a), this.name = "EndOfStreamError";
	}
}, s = class extends Error {
	constructor(e = "The operation was aborted") {
		super(e), this.name = "AbortError";
	}
}, c = class {
	constructor() {
		this.endOfStream = !1, this.interrupted = !1, this.peekQueue = [];
	}
	async peek(e, t = !1) {
		let n = await this.read(e, t);
		return this.peekQueue.push(e.subarray(0, n)), n;
	}
	async read(e, t = !1) {
		if (e.length === 0) return 0;
		let n = this.readFromPeekBuffer(e);
		if (this.endOfStream || (n += await this.readRemainderFromStream(e.subarray(n), t)), n === 0 && !t) throw new o();
		return n;
	}
	readFromPeekBuffer(e) {
		let t = e.length, n = 0;
		for (; this.peekQueue.length > 0 && t > 0;) {
			let r = this.peekQueue.pop();
			if (!r) throw Error("peekData should be defined");
			let i = Math.min(r.length, t);
			e.set(r.subarray(0, i), n), n += i, t -= i, i < r.length && this.peekQueue.push(r.subarray(i));
		}
		return n;
	}
	async readRemainderFromStream(e, t) {
		let n = 0;
		for (; n < e.length && !this.endOfStream;) {
			if (this.interrupted) throw new s();
			let r = await this.readFromStream(e.subarray(n), t);
			if (r === 0) break;
			n += r;
		}
		if (!t && n < e.length) throw new o();
		return n;
	}
}, l = class extends c {
	constructor(e) {
		super(), this.reader = e;
	}
	async abort() {
		return this.close();
	}
	async close() {
		this.reader.releaseLock();
	}
}, u = class extends l {
	async readFromStream(e, t) {
		if (e.length === 0) return 0;
		let n = await this.reader.read(new Uint8Array(e.length), { min: t ? void 0 : e.length });
		return n.done && (this.endOfStream = n.done), n.value ? (e.set(n.value), n.value.length) : 0;
	}
}, d = class extends c {
	constructor(e) {
		super(), this.reader = e, this.buffer = null;
	}
	writeChunk(e, t) {
		let n = Math.min(t.length, e.length);
		return e.set(t.subarray(0, n)), this.buffer = n < t.length ? t.subarray(n) : null, n;
	}
	async readFromStream(e, t) {
		if (e.length === 0) return 0;
		let n = 0;
		for (this.buffer && (n += this.writeChunk(e, this.buffer)); n < e.length && !this.endOfStream;) {
			let t = await this.reader.read();
			if (t.done) {
				this.endOfStream = !0;
				break;
			}
			t.value && (n += this.writeChunk(e.subarray(n), t.value));
		}
		if (!t && n === 0 && this.endOfStream) throw new o();
		return n;
	}
	abort() {
		return this.interrupted = !0, this.reader.cancel();
	}
	async close() {
		await this.abort(), this.reader.releaseLock();
	}
};
//#endregion
//#region node_modules/strtok3/lib/stream/WebStreamReaderFactory.js
function f(e) {
	try {
		let t = e.getReader({ mode: "byob" });
		return t instanceof ReadableStreamDefaultReader ? new d(t) : new u(t);
	} catch (t) {
		if (t instanceof TypeError) return new d(e.getReader());
		throw t;
	}
}
//#endregion
//#region node_modules/strtok3/lib/AbstractTokenizer.js
var p = class {
	constructor(e) {
		this.numBuffer = /* @__PURE__ */ new Uint8Array(8), this.position = 0, this.onClose = e?.onClose, e?.abortSignal && e.abortSignal.addEventListener("abort", () => {
			this.abort();
		});
	}
	async readToken(e, t = this.position) {
		let n = new Uint8Array(e.len);
		if (await this.readBuffer(n, { position: t }) < e.len) throw new o();
		return e.get(n, 0);
	}
	async peekToken(e, t = this.position) {
		let n = new Uint8Array(e.len);
		if (await this.peekBuffer(n, { position: t }) < e.len) throw new o();
		return e.get(n, 0);
	}
	async readNumber(e) {
		if (await this.readBuffer(this.numBuffer, { length: e.len }) < e.len) throw new o();
		return e.get(this.numBuffer, 0);
	}
	async peekNumber(e) {
		if (await this.peekBuffer(this.numBuffer, { length: e.len }) < e.len) throw new o();
		return e.get(this.numBuffer, 0);
	}
	async ignore(e) {
		if (e < 0) throw RangeError("ignore length must be ≥ 0 bytes");
		if (this.fileInfo.size !== void 0) {
			let t = this.fileInfo.size - this.position;
			if (e > t) return this.position += t, t;
		}
		return this.position += e, e;
	}
	async close() {
		await this.abort(), await this.onClose?.();
	}
	normalizeOptions(e, t) {
		if (!this.supportsRandomAccess() && t && t.position !== void 0 && t.position < this.position) throw Error("`options.position` must be equal or greater than `tokenizer.position`");
		return {
			mayBeLess: !1,
			offset: 0,
			length: e.length,
			position: this.position,
			...t
		};
	}
	abort() {
		return Promise.resolve();
	}
}, ee = 256e3, te = class extends p {
	constructor(e, t) {
		super(t), this.streamReader = e, this.fileInfo = t?.fileInfo ?? {};
	}
	async readBuffer(e, t) {
		let n = this.normalizeOptions(e, t), r = n.position - this.position;
		if (r > 0) return await this.ignore(r), this.readBuffer(e, t);
		if (r < 0) throw Error("`options.position` must be equal or greater than `tokenizer.position`");
		if (n.length === 0) return 0;
		let i = await this.streamReader.read(e.subarray(0, n.length), n.mayBeLess);
		if (this.position += i, (!t || !t.mayBeLess) && i < n.length) throw new o();
		return i;
	}
	async peekBuffer(e, t) {
		let n = this.normalizeOptions(e, t), r = 0;
		if (n.position) {
			let t = n.position - this.position;
			if (t > 0) {
				let i = new Uint8Array(n.length + t);
				return r = await this.peekBuffer(i, { mayBeLess: n.mayBeLess }), e.set(i.subarray(t)), r - t;
			}
			if (t < 0) throw Error("Cannot peek from a negative offset in a stream");
		}
		if (n.length > 0) {
			try {
				r = await this.streamReader.peek(e.subarray(0, n.length), n.mayBeLess);
			} catch (e) {
				if (t?.mayBeLess && e instanceof o) return 0;
				throw e;
			}
			if (!n.mayBeLess && r < n.length) throw new o();
		}
		return r;
	}
	async ignore(e) {
		if (e < 0) throw RangeError("ignore length must be ≥ 0 bytes");
		let t = Math.min(ee, e), n = new Uint8Array(t), r = 0;
		for (; r < e;) {
			let i = e - r, a = await this.readBuffer(n, { length: Math.min(t, i) });
			if (a < 0) return a;
			r += a;
		}
		return r;
	}
	abort() {
		return this.streamReader.abort();
	}
	async close() {
		return this.streamReader.close();
	}
	supportsRandomAccess() {
		return !1;
	}
}, ne = class extends p {
	constructor(e, t) {
		super(t), this.uint8Array = e, this.fileInfo = {
			...t?.fileInfo ?? {},
			size: e.length
		};
	}
	async readBuffer(e, t) {
		t?.position && (this.position = t.position);
		let n = await this.peekBuffer(e, t);
		return this.position += n, n;
	}
	async peekBuffer(e, t) {
		let n = this.normalizeOptions(e, t), r = Math.min(this.uint8Array.length - n.position, n.length);
		if (!n.mayBeLess && r < n.length) throw new o();
		return e.set(this.uint8Array.subarray(n.position, n.position + r)), r;
	}
	close() {
		return super.close();
	}
	supportsRandomAccess() {
		return !0;
	}
	setPosition(e) {
		this.position = e;
	}
}, re = class extends p {
	constructor(e, t) {
		super(t), this.blob = e, this.fileInfo = {
			...t?.fileInfo ?? {},
			size: e.size,
			mimeType: e.type
		};
	}
	async readBuffer(e, t) {
		t?.position && (this.position = t.position);
		let n = await this.peekBuffer(e, t);
		return this.position += n, n;
	}
	async peekBuffer(e, t) {
		let n = this.normalizeOptions(e, t), r = Math.min(this.blob.size - n.position, n.length);
		if (!n.mayBeLess && r < n.length) throw new o();
		let i = await this.blob.slice(n.position, n.position + r).arrayBuffer();
		return e.set(new Uint8Array(i)), r;
	}
	close() {
		return super.close();
	}
	supportsRandomAccess() {
		return !0;
	}
	setPosition(e) {
		this.position = e;
	}
};
//#endregion
//#region node_modules/strtok3/lib/core.js
function ie(e, t) {
	let n = f(e), r = t ?? {}, i = r.onClose;
	return r.onClose = async () => {
		if (await n.close(), i) return i();
	}, new te(n, r);
}
function ae(e, t) {
	return new ne(e, t);
}
function oe(e, t) {
	return new re(e, t);
}
(class e extends p {
	static async fromFile(t) {
		let n = await i(t, "r"), r = await n.stat();
		return new e(n, { fileInfo: {
			path: t,
			size: r.size
		} });
	}
	constructor(e, t) {
		super(t), this.fileHandle = e, this.fileInfo = t.fileInfo;
	}
	async readBuffer(e, t) {
		let n = this.normalizeOptions(e, t);
		if (this.position = n.position, n.length === 0) return 0;
		let r = await this.fileHandle.read(e, 0, n.length, n.position);
		if (this.position += r.bytesRead, r.bytesRead < n.length && (!t || !t.mayBeLess)) throw new o();
		return r.bytesRead;
	}
	async peekBuffer(e, t) {
		let n = this.normalizeOptions(e, t), r = await this.fileHandle.read(e, 0, n.length, n.position);
		if (!n.mayBeLess && r.bytesRead < n.length) throw new o();
		return r.bytesRead;
	}
	async close() {
		return await this.fileHandle.close(), super.close();
	}
	setPosition(e) {
		this.position = e;
	}
	supportsRandomAccess() {
		return !0;
	}
}).fromFile;
//#endregion
//#region node_modules/@borewit/text-codec/lib/index.js
var se = {
	128: "€",
	130: "‚",
	131: "ƒ",
	132: "„",
	133: "…",
	134: "†",
	135: "‡",
	136: "ˆ",
	137: "‰",
	138: "Š",
	139: "‹",
	140: "Œ",
	142: "Ž",
	145: "‘",
	146: "’",
	147: "“",
	148: "”",
	149: "•",
	150: "–",
	151: "—",
	152: "˜",
	153: "™",
	154: "š",
	155: "›",
	156: "œ",
	158: "ž",
	159: "Ÿ"
}, ce = {};
for (let [e, t] of Object.entries(se)) ce[t] = Number.parseInt(e, 10);
var le;
function ue() {
	if (globalThis.TextDecoder !== void 0) return le ??= new globalThis.TextDecoder("utf-8");
}
var m = 32768, h = 65533;
function de(e, t = "utf-8") {
	switch (t.toLowerCase()) {
		case "utf-8":
		case "utf8": {
			let t = ue();
			return t ? t.decode(e) : pe(e);
		}
		case "utf-16le": return me(e);
		case "us-ascii":
		case "ascii": return he(e);
		case "latin1":
		case "iso-8859-1": return ge(e);
		case "windows-1252": return _e(e);
		default: throw RangeError(`Encoding '${t}' not supported`);
	}
}
function g(e, t) {
	t.length !== 0 && (e.push(String.fromCharCode.apply(null, t)), t.length = 0);
}
function _(e, t, n) {
	t.push(n), t.length >= m && g(e, t);
}
function fe(e, t, n) {
	n <= 65535 ? _(e, t, n) : (n -= 65536, _(e, t, 55296 + (n >> 10)), _(e, t, 56320 + (n & 1023)));
}
function pe(e) {
	let t = [], n = [], r = 0;
	for (e.length >= 3 && e[0] === 239 && e[1] === 187 && e[2] === 191 && (r = 3); r < e.length;) {
		let i = e[r];
		if (i <= 127) {
			_(t, n, i), r++;
			continue;
		}
		if (i < 194 || i > 244) {
			_(t, n, h), r++;
			continue;
		}
		if (i <= 223) {
			if (r + 1 >= e.length) {
				_(t, n, h), r++;
				continue;
			}
			let a = e[r + 1];
			if ((a & 192) != 128) {
				_(t, n, h), r++;
				continue;
			}
			_(t, n, (i & 31) << 6 | a & 63), r += 2;
			continue;
		}
		if (i <= 239) {
			if (r + 2 >= e.length) {
				_(t, n, h), r++;
				continue;
			}
			let a = e[r + 1], o = e[r + 2];
			if ((a & 192) != 128 || (o & 192) != 128 || i === 224 && a < 160 || i === 237 && a >= 160) {
				_(t, n, h), r++;
				continue;
			}
			_(t, n, (i & 15) << 12 | (a & 63) << 6 | o & 63), r += 3;
			continue;
		}
		if (r + 3 >= e.length) {
			_(t, n, h), r++;
			continue;
		}
		let a = e[r + 1], o = e[r + 2], s = e[r + 3];
		(a & 192) != 128 || (o & 192) != 128 || (s & 192) != 128 || i === 240 && a < 144 || i === 244 && a > 143 ? (_(t, n, h), r++) : (fe(t, n, (i & 7) << 18 | (a & 63) << 12 | (o & 63) << 6 | s & 63), r += 4);
	}
	return g(t, n), t.join("");
}
function me(e) {
	let t = [], n = [], r = e.length, i = 0;
	for (; i + 1 < r;) {
		let a = e[i] | e[i + 1] << 8;
		if (i += 2, a >= 55296 && a <= 56319) {
			if (i + 1 < r) {
				let r = e[i] | e[i + 1] << 8;
				r >= 56320 && r <= 57343 ? (_(t, n, a), _(t, n, r), i += 2) : _(t, n, h);
			} else _(t, n, h);
		} else a >= 56320 && a <= 57343 ? _(t, n, h) : _(t, n, a);
	}
	return i < r && _(t, n, h), g(t, n), t.join("");
}
function he(e) {
	let t = [];
	for (let n = 0; n < e.length; n += m) {
		let r = Math.min(e.length, n + m), i = Array(r - n);
		for (let t = n, a = 0; t < r; t++, a++) i[a] = e[t] & 127;
		t.push(String.fromCharCode.apply(null, i));
	}
	return t.join("");
}
function ge(e) {
	let t = [];
	for (let n = 0; n < e.length; n += m) {
		let r = Math.min(e.length, n + m), i = Array(r - n);
		for (let t = n, a = 0; t < r; t++, a++) i[a] = e[t];
		t.push(String.fromCharCode.apply(null, i));
	}
	return t.join("");
}
function _e(e) {
	let t = [], n = "";
	for (let r = 0; r < e.length; r++) {
		let i = e[r], a = i >= 128 && i <= 159 ? se[i] : void 0;
		n += a ?? String.fromCharCode(i), n.length >= m && (t.push(n), n = "");
	}
	return n && t.push(n), t.join("");
}
//#endregion
//#region node_modules/token-types/lib/index.js
function v(e) {
	return new DataView(e.buffer, e.byteOffset);
}
var ve = {
	len: 1,
	get(e, t) {
		return v(e).getUint8(t);
	},
	put(e, t, n) {
		return v(e).setUint8(t, n), t + 1;
	}
}, y = {
	len: 2,
	get(e, t) {
		return v(e).getUint16(t, !0);
	},
	put(e, t, n) {
		return v(e).setUint16(t, n, !0), t + 2;
	}
}, b = {
	len: 2,
	get(e, t) {
		return v(e).getUint16(t);
	},
	put(e, t, n) {
		return v(e).setUint16(t, n), t + 2;
	}
}, x = {
	len: 4,
	get(e, t) {
		return v(e).getUint32(t, !0);
	},
	put(e, t, n) {
		return v(e).setUint32(t, n, !0), t + 4;
	}
}, S = {
	len: 4,
	get(e, t) {
		return v(e).getUint32(t);
	},
	put(e, t, n) {
		return v(e).setUint32(t, n), t + 4;
	}
}, ye = {
	len: 4,
	get(e, t) {
		return v(e).getInt32(t);
	},
	put(e, t, n) {
		return v(e).setInt32(t, n), t + 4;
	}
}, be = {
	len: 8,
	get(e, t) {
		return v(e).getBigUint64(t, !0);
	},
	put(e, t, n) {
		return v(e).setBigUint64(t, n, !0), t + 8;
	}
}, C = class {
	constructor(e, t) {
		this.len = e, this.encoding = t;
	}
	get(e, t = 0) {
		return de(e.subarray(t, t + this.len), this.encoding);
	}
}, w = /* @__PURE__ */ n(((e, t) => {
	function n(e) {
		n.debug = n, n.default = n, n.coerce = l, n.disable = s, n.enable = a, n.enabled = c, n.humanize = r(), n.destroy = u, Object.keys(e).forEach((t) => {
			n[t] = e[t];
		}), n.names = [], n.skips = [], n.formatters = {};
		function t(e) {
			let t = 0;
			for (let n = 0; n < e.length; n++) t = (t << 5) - t + e.charCodeAt(n), t |= 0;
			return n.colors[Math.abs(t) % n.colors.length];
		}
		n.selectColor = t;
		function n(e) {
			let t, r = null, a, o;
			function s(...e) {
				if (!s.enabled) return;
				let r = s, i = Number(/* @__PURE__ */ new Date());
				r.diff = i - (t || i), r.prev = t, r.curr = i, t = i, e[0] = n.coerce(e[0]), typeof e[0] != "string" && e.unshift("%O");
				let a = 0;
				e[0] = e[0].replace(/%([a-zA-Z%])/g, (t, i) => {
					if (t === "%%") return "%";
					a++;
					let o = n.formatters[i];
					if (typeof o == "function") {
						let n = e[a];
						t = o.call(r, n), e.splice(a, 1), a--;
					}
					return t;
				}), n.formatArgs.call(r, e), (r.log || n.log).apply(r, e);
			}
			return s.namespace = e, s.useColors = n.useColors(), s.color = n.selectColor(e), s.extend = i, s.destroy = n.destroy, Object.defineProperty(s, "enabled", {
				enumerable: !0,
				configurable: !1,
				get: () => r === null ? (a !== n.namespaces && (a = n.namespaces, o = n.enabled(e)), o) : r,
				set: (e) => {
					r = e;
				}
			}), typeof n.init == "function" && n.init(s), s;
		}
		function i(e, t) {
			let r = n(this.namespace + (t === void 0 ? ":" : t) + e);
			return r.log = this.log, r;
		}
		function a(e) {
			n.save(e), n.namespaces = e, n.names = [], n.skips = [];
			let t = (typeof e == "string" ? e : "").trim().replace(/\s+/g, ",").split(",").filter(Boolean);
			for (let e of t) e[0] === "-" ? n.skips.push(e.slice(1)) : n.names.push(e);
		}
		function o(e, t) {
			let n = 0, r = 0, i = -1, a = 0;
			for (; n < e.length;) if (r < t.length && (t[r] === e[n] || t[r] === "*")) t[r] === "*" ? (i = r, a = n, r++) : (n++, r++);
			else if (i !== -1) r = i + 1, a++, n = a;
			else return !1;
			for (; r < t.length && t[r] === "*";) r++;
			return r === t.length;
		}
		function s() {
			let e = [...n.names, ...n.skips.map((e) => "-" + e)].join(",");
			return n.enable(""), e;
		}
		function c(e) {
			for (let t of n.skips) if (o(e, t)) return !1;
			for (let t of n.names) if (o(e, t)) return !0;
			return !1;
		}
		function l(e) {
			return e instanceof Error ? e.stack || e.message : e;
		}
		function u() {
			console.warn("Instance method `debug.destroy()` is deprecated and no longer does anything. It will be removed in the next major version of `debug`.");
		}
		return n.enable(n.load()), n;
	}
	t.exports = n;
})), xe = /* @__PURE__ */ n(((e, t) => {
	e.formatArgs = r, e.save = i, e.load = a, e.useColors = n, e.storage = o(), e.destroy = (() => {
		let e = !1;
		return () => {
			e || (e = !0, console.warn("Instance method `debug.destroy()` is deprecated and no longer does anything. It will be removed in the next major version of `debug`."));
		};
	})(), e.colors = /* @__PURE__ */ "#0000CC.#0000FF.#0033CC.#0033FF.#0066CC.#0066FF.#0099CC.#0099FF.#00CC00.#00CC33.#00CC66.#00CC99.#00CCCC.#00CCFF.#3300CC.#3300FF.#3333CC.#3333FF.#3366CC.#3366FF.#3399CC.#3399FF.#33CC00.#33CC33.#33CC66.#33CC99.#33CCCC.#33CCFF.#6600CC.#6600FF.#6633CC.#6633FF.#66CC00.#66CC33.#9900CC.#9900FF.#9933CC.#9933FF.#99CC00.#99CC33.#CC0000.#CC0033.#CC0066.#CC0099.#CC00CC.#CC00FF.#CC3300.#CC3333.#CC3366.#CC3399.#CC33CC.#CC33FF.#CC6600.#CC6633.#CC9900.#CC9933.#CCCC00.#CCCC33.#FF0000.#FF0033.#FF0066.#FF0099.#FF00CC.#FF00FF.#FF3300.#FF3333.#FF3366.#FF3399.#FF33CC.#FF33FF.#FF6600.#FF6633.#FF9900.#FF9933.#FFCC00.#FFCC33".split(".");
	function n() {
		if (typeof window < "u" && window.process && (window.process.type === "renderer" || window.process.__nwjs)) return !0;
		if (typeof navigator < "u" && navigator.userAgent && navigator.userAgent.toLowerCase().match(/(edge|trident)\/(\d+)/)) return !1;
		let e;
		return typeof document < "u" && document.documentElement && document.documentElement.style && document.documentElement.style.WebkitAppearance || typeof window < "u" && window.console && (window.console.firebug || window.console.exception && window.console.table) || typeof navigator < "u" && navigator.userAgent && (e = navigator.userAgent.toLowerCase().match(/firefox\/(\d+)/)) && parseInt(e[1], 10) >= 31 || typeof navigator < "u" && navigator.userAgent && navigator.userAgent.toLowerCase().match(/applewebkit\/(\d+)/);
	}
	function r(e) {
		if (e[0] = (this.useColors ? "%c" : "") + this.namespace + (this.useColors ? " %c" : " ") + e[0] + (this.useColors ? "%c " : " ") + "+" + t.exports.humanize(this.diff), !this.useColors) return;
		let n = "color: " + this.color;
		e.splice(1, 0, n, "color: inherit");
		let r = 0, i = 0;
		e[0].replace(/%[a-zA-Z%]/g, (e) => {
			e !== "%%" && (r++, e === "%c" && (i = r));
		}), e.splice(i, 0, n);
	}
	e.log = console.debug || console.log || (() => {});
	function i(t) {
		try {
			t ? e.storage.setItem("debug", t) : e.storage.removeItem("debug");
		} catch {}
	}
	function a() {
		let t;
		try {
			t = e.storage.getItem("debug") || e.storage.getItem("DEBUG");
		} catch {}
		return !t && typeof process < "u" && "env" in process && (t = process.env.DEBUG), t;
	}
	function o() {
		try {
			return localStorage;
		} catch {}
	}
	t.exports = w()(e);
	var { formatters: s } = t.exports;
	s.j = function(e) {
		try {
			return JSON.stringify(e);
		} catch (e) {
			return "[UnexpectedJSONParseError]: " + e.message;
		}
	};
})), Se = /* @__PURE__ */ n(((t, n) => {
	var r = e("tty"), i = e("util");
	t.init = d, t.log = c, t.formatArgs = o, t.save = l, t.load = u, t.useColors = a, t.destroy = i.deprecate(() => {}, "Instance method `debug.destroy()` is deprecated and no longer does anything. It will be removed in the next major version of `debug`."), t.colors = [
		6,
		2,
		3,
		4,
		5,
		1
	];
	try {
		let n = e("supports-color");
		n && (n.stderr || n).level >= 2 && (t.colors = [
			20,
			21,
			26,
			27,
			32,
			33,
			38,
			39,
			40,
			41,
			42,
			43,
			44,
			45,
			56,
			57,
			62,
			63,
			68,
			69,
			74,
			75,
			76,
			77,
			78,
			79,
			80,
			81,
			92,
			93,
			98,
			99,
			112,
			113,
			128,
			129,
			134,
			135,
			148,
			149,
			160,
			161,
			162,
			163,
			164,
			165,
			166,
			167,
			168,
			169,
			170,
			171,
			172,
			173,
			178,
			179,
			184,
			185,
			196,
			197,
			198,
			199,
			200,
			201,
			202,
			203,
			204,
			205,
			206,
			207,
			208,
			209,
			214,
			215,
			220,
			221
		]);
	} catch {}
	t.inspectOpts = Object.keys(process.env).filter((e) => /^debug_/i.test(e)).reduce((e, t) => {
		let n = t.substring(6).toLowerCase().replace(/_([a-z])/g, (e, t) => t.toUpperCase()), r = process.env[t];
		return r = /^(yes|on|true|enabled)$/i.test(r) ? !0 : /^(no|off|false|disabled)$/i.test(r) ? !1 : r === "null" ? null : Number(r), e[n] = r, e;
	}, {});
	function a() {
		return "colors" in t.inspectOpts ? !!t.inspectOpts.colors : r.isatty(process.stderr.fd);
	}
	function o(e) {
		let { namespace: t, useColors: r } = this;
		if (r) {
			let r = this.color, i = "\x1B[3" + (r < 8 ? r : "8;5;" + r), a = `  ${i};1m${t} \u001B[0m`;
			e[0] = a + e[0].split("\n").join("\n" + a), e.push(i + "m+" + n.exports.humanize(this.diff) + "\x1B[0m");
		} else e[0] = s() + t + " " + e[0];
	}
	function s() {
		return t.inspectOpts.hideDate ? "" : (/* @__PURE__ */ new Date()).toISOString() + " ";
	}
	function c(...e) {
		return process.stderr.write(i.formatWithOptions(t.inspectOpts, ...e) + "\n");
	}
	function l(e) {
		e ? process.env.DEBUG = e : delete process.env.DEBUG;
	}
	function u() {
		return process.env.DEBUG;
	}
	function d(e) {
		e.inspectOpts = {};
		let n = Object.keys(t.inspectOpts);
		for (let r = 0; r < n.length; r++) e.inspectOpts[n[r]] = t.inspectOpts[n[r]];
	}
	n.exports = w()(t);
	var { formatters: f } = n.exports;
	f.o = function(e) {
		return this.inspectOpts.colors = this.useColors, i.inspect(e, this.inspectOpts).split("\n").map((e) => e.trim()).join(" ");
	}, f.O = function(e) {
		return this.inspectOpts.colors = this.useColors, i.inspect(e, this.inspectOpts);
	};
})), Ce = /* @__PURE__ */ t((/* @__PURE__ */ n(((e, t) => {
	t.exports = typeof process > "u" || process.type === "renderer" || process.browser === !0 || process.__nwjs ? xe() : Se();
})))(), 1), T = {
	LocalFileHeader: 67324752,
	DataDescriptor: 134695760,
	CentralFileHeader: 33639248,
	EndOfCentralDirectory: 101010256
}, E = {
	get(e) {
		return {
			signature: x.get(e, 0),
			compressedSize: x.get(e, 8),
			uncompressedSize: x.get(e, 12)
		};
	},
	len: 16
}, we = {
	get(e) {
		let t = y.get(e, 6);
		return {
			signature: x.get(e, 0),
			minVersion: y.get(e, 4),
			dataDescriptor: !!(t & 8),
			compressedMethod: y.get(e, 8),
			compressedSize: x.get(e, 18),
			uncompressedSize: x.get(e, 22),
			filenameLength: y.get(e, 26),
			extraFieldLength: y.get(e, 28),
			filename: null
		};
	},
	len: 30
}, Te = {
	get(e) {
		return {
			signature: x.get(e, 0),
			nrOfThisDisk: y.get(e, 4),
			nrOfThisDiskWithTheStart: y.get(e, 6),
			nrOfEntriesOnThisDisk: y.get(e, 8),
			nrOfEntriesOfSize: y.get(e, 10),
			sizeOfCd: x.get(e, 12),
			offsetOfStartOfCd: x.get(e, 16),
			zipFileCommentLength: y.get(e, 20)
		};
	},
	len: 22
}, Ee = {
	get(e) {
		let t = y.get(e, 8);
		return {
			signature: x.get(e, 0),
			minVersion: y.get(e, 6),
			dataDescriptor: !!(t & 8),
			compressedMethod: y.get(e, 10),
			compressedSize: x.get(e, 20),
			uncompressedSize: x.get(e, 24),
			filenameLength: y.get(e, 28),
			extraFieldLength: y.get(e, 30),
			fileCommentLength: y.get(e, 32),
			relativeOffsetOfLocalHeader: x.get(e, 42),
			filename: null
		};
	},
	len: 46
};
//#endregion
//#region node_modules/@tokenizer/inflate/lib/ZipHandler.js
function D(e) {
	let t = new Uint8Array(x.len);
	return x.put(t, 0, e), t;
}
var O = (0, Ce.default)("tokenizer:inflate"), k = 262144, De = D(T.DataDescriptor), A = D(T.EndOfCentralDirectory), j = class e {
	constructor(e) {
		this.tokenizer = e, this.syncBuffer = new Uint8Array(k);
	}
	async isZip() {
		return await this.peekSignature() === T.LocalFileHeader;
	}
	peekSignature() {
		return this.tokenizer.peekToken(x);
	}
	async findEndOfCentralDirectoryLocator() {
		let e = this.tokenizer, t = Math.min(16384, e.fileInfo.size), n = this.syncBuffer.subarray(0, t);
		await this.tokenizer.readBuffer(n, { position: e.fileInfo.size - t });
		for (let r = n.length - 4; r >= 0; r--) if (n[r] === A[0] && n[r + 1] === A[1] && n[r + 2] === A[2] && n[r + 3] === A[3]) return e.fileInfo.size - t + r;
		return -1;
	}
	async readCentralDirectory() {
		if (!this.tokenizer.supportsRandomAccess()) {
			O("Cannot reading central-directory without random-read support");
			return;
		}
		O("Reading central-directory...");
		let e = this.tokenizer.position, t = await this.findEndOfCentralDirectoryLocator();
		if (t > 0) {
			O("Central-directory 32-bit signature found");
			let n = await this.tokenizer.readToken(Te, t), r = [];
			this.tokenizer.setPosition(n.offsetOfStartOfCd);
			for (let e = 0; e < n.nrOfEntriesOfSize; ++e) {
				let t = await this.tokenizer.readToken(Ee);
				if (t.signature !== T.CentralFileHeader) throw Error("Expected Central-File-Header signature");
				t.filename = await this.tokenizer.readToken(new C(t.filenameLength, "utf-8")), await this.tokenizer.ignore(t.extraFieldLength), await this.tokenizer.ignore(t.fileCommentLength), r.push(t), O(`Add central-directory file-entry: n=${e + 1}/${r.length}: filename=${r[e].filename}`);
			}
			return this.tokenizer.setPosition(e), r;
		}
		this.tokenizer.setPosition(e);
	}
	async unzip(e) {
		let t = await this.readCentralDirectory();
		if (t) return this.iterateOverCentralDirectory(t, e);
		let n = !1;
		do {
			let t = await this.readLocalFileHeader();
			if (!t) break;
			let r = e(t);
			n = !!r.stop;
			let i;
			if (await this.tokenizer.ignore(t.extraFieldLength), t.dataDescriptor && t.compressedSize === 0) {
				let e = [], n = k;
				O("Compressed-file-size unknown, scanning for next data-descriptor-signature....");
				let i = -1;
				for (; i < 0 && n === k;) {
					n = await this.tokenizer.peekBuffer(this.syncBuffer, { mayBeLess: !0 }), i = Oe(this.syncBuffer.subarray(0, n), De);
					let t = i >= 0 ? i : n;
					if (r.handler) {
						let n = new Uint8Array(t);
						await this.tokenizer.readBuffer(n), e.push(n);
					} else await this.tokenizer.ignore(t);
				}
				O(`Found data-descriptor-signature at pos=${this.tokenizer.position}`), r.handler && await this.inflate(t, ke(e), r.handler);
			} else r.handler ? (O(`Reading compressed-file-data: ${t.compressedSize} bytes`), i = new Uint8Array(t.compressedSize), await this.tokenizer.readBuffer(i), await this.inflate(t, i, r.handler)) : (O(`Ignoring compressed-file-data: ${t.compressedSize} bytes`), await this.tokenizer.ignore(t.compressedSize));
			if (O(`Reading data-descriptor at pos=${this.tokenizer.position}`), t.dataDescriptor && (await this.tokenizer.readToken(E)).signature !== 134695760) throw Error(`Expected data-descriptor-signature at position ${this.tokenizer.position - E.len}`);
		} while (!n);
	}
	async iterateOverCentralDirectory(e, t) {
		for (let n of e) {
			let e = t(n);
			if (e.handler) {
				this.tokenizer.setPosition(n.relativeOffsetOfLocalHeader);
				let t = await this.readLocalFileHeader();
				if (t) {
					await this.tokenizer.ignore(t.extraFieldLength);
					let r = new Uint8Array(n.compressedSize);
					await this.tokenizer.readBuffer(r), await this.inflate(t, r, e.handler);
				}
			}
			if (e.stop) break;
		}
	}
	async inflate(t, n, r) {
		if (t.compressedMethod === 0) return r(n);
		if (t.compressedMethod !== 8) throw Error(`Unsupported ZIP compression method: ${t.compressedMethod}`);
		return O(`Decompress filename=${t.filename}, compressed-size=${n.length}`), r(await e.decompressDeflateRaw(n));
	}
	static async decompressDeflateRaw(e) {
		let t = new ReadableStream({ start(t) {
			t.enqueue(e), t.close();
		} }), n = new DecompressionStream("deflate-raw"), r = t.pipeThrough(n);
		try {
			let e = await new Response(r).arrayBuffer();
			return new Uint8Array(e);
		} catch (e) {
			let t = e instanceof Error ? `Failed to deflate ZIP entry: ${e.message}` : "Unknown decompression error in ZIP entry";
			throw TypeError(t);
		}
	}
	async readLocalFileHeader() {
		let e = await this.tokenizer.peekToken(x);
		if (e === T.LocalFileHeader) {
			let e = await this.tokenizer.readToken(we);
			return e.filename = await this.tokenizer.readToken(new C(e.filenameLength, "utf-8")), e;
		}
		if (e === T.CentralFileHeader) return !1;
		throw Error(e === 3759263696 ? "Encrypted ZIP" : "Unexpected signature");
	}
};
function Oe(e, t) {
	let n = e.length, r = t.length;
	if (r > n) return -1;
	for (let i = 0; i <= n - r; i++) {
		let n = !0;
		for (let a = 0; a < r; a++) if (e[i + a] !== t[a]) {
			n = !1;
			break;
		}
		if (n) return i;
	}
	return -1;
}
function ke(e) {
	let t = e.reduce((e, t) => e + t.length, 0), n = new Uint8Array(t), r = 0;
	for (let t of e) n.set(t, r), r += t.length;
	return n;
}
//#endregion
//#region node_modules/@tokenizer/inflate/lib/GzipHandler.js
var Ae = class {
	constructor(e) {
		this.tokenizer = e;
	}
	inflate() {
		let e = this.tokenizer;
		return new ReadableStream({ async pull(t) {
			let n = /* @__PURE__ */ new Uint8Array(1024), r = await e.readBuffer(n, { mayBeLess: !0 });
			r === 0 ? t.close() : t.enqueue(n.subarray(0, r));
		} }).pipeThrough(new DecompressionStream("gzip"));
	}
};
new globalThis.TextDecoder("utf8"), new globalThis.TextEncoder(), Array.from({ length: 256 }, (e, t) => t.toString(16).padStart(2, "0"));
function M(e) {
	let { byteLength: t } = e;
	if (t === 6) return e.getUint16(0) * 2 ** 32 + e.getUint32(2);
	if (t === 5) return e.getUint8(0) * 2 ** 32 + e.getUint32(1);
	if (t === 4) return e.getUint32(0);
	if (t === 3) return e.getUint8(0) * 2 ** 16 + e.getUint16(1);
	if (t === 2) return e.getUint16(0);
	if (t === 1) return e.getUint8(0);
}
//#endregion
//#region node_modules/file-type/util.js
function je(e, t) {
	if (t === "utf-16le") {
		let t = [];
		for (let n = 0; n < e.length; n++) {
			let r = e.charCodeAt(n);
			t.push(r & 255, r >> 8 & 255);
		}
		return t;
	}
	if (t === "utf-16be") {
		let t = [];
		for (let n = 0; n < e.length; n++) {
			let r = e.charCodeAt(n);
			t.push(r >> 8 & 255, r & 255);
		}
		return t;
	}
	return [...e].map((e) => e.charCodeAt(0));
}
function Me(e, t = 0) {
	let n = Number.parseInt(new C(6).get(e, 148).replace(/\0.*$/, "").trim(), 8);
	if (Number.isNaN(n)) return !1;
	let r = 256;
	for (let n = t; n < t + 148; n++) r += e[n];
	for (let n = t + 156; n < t + 512; n++) r += e[n];
	return n === r;
}
var Ne = {
	get: (e, t) => e[t + 3] & 127 | e[t + 2] << 7 | e[t + 1] << 14 | e[t] << 21,
	len: 4
}, Pe = /* @__PURE__ */ "jpg,png,apng,gif,webp,flif,xcf,cr2,cr3,orf,arw,dng,nef,rw2,raf,tif,bmp,icns,jxr,psd,indd,zip,tar,rar,gz,bz2,7z,dmg,mp4,mid,mkv,webm,mov,avi,mpg,mp2,mp3,m4a,oga,ogg,ogv,opus,flac,wav,spx,amr,pdf,epub,elf,macho,exe,swf,rtf,wasm,woff,woff2,eot,ttf,otf,ttc,ico,flv,ps,xz,sqlite,nes,crx,xpi,cab,deb,ar,rpm,Z,lz,cfb,mxf,mts,blend,bpg,docx,pptx,xlsx,3gp,3g2,j2c,jp2,jpm,jpx,mj2,aif,qcp,odt,ods,odp,xml,mobi,heic,cur,ktx,ape,wv,dcm,ics,glb,pcap,dsf,lnk,alias,voc,ac3,m4v,m4p,m4b,f4v,f4p,f4b,f4a,mie,asf,ogm,ogx,mpc,arrow,shp,aac,mp1,it,s3m,xm,skp,avif,eps,lzh,pgp,asar,stl,chm,3mf,zst,jxl,vcf,jls,pst,dwg,parquet,class,arj,cpio,ace,avro,icc,fbx,vsdx,vtt,apk,drc,lz4,potx,xltx,dotx,xltm,ott,ots,otp,odg,otg,xlsm,docm,dotm,potm,pptm,jar,jmp,rm,sav,ppsm,ppsx,tar.gz,reg,dat".split(","), Fe = /* @__PURE__ */ "image/jpeg,image/png,image/gif,image/webp,image/flif,image/x-xcf,image/x-canon-cr2,image/x-canon-cr3,image/tiff,image/bmp,image/vnd.ms-photo,image/vnd.adobe.photoshop,application/x-indesign,application/epub+zip,application/x-xpinstall,application/vnd.ms-powerpoint.slideshow.macroenabled.12,application/vnd.oasis.opendocument.text,application/vnd.oasis.opendocument.spreadsheet,application/vnd.oasis.opendocument.presentation,application/vnd.openxmlformats-officedocument.wordprocessingml.document,application/vnd.openxmlformats-officedocument.presentationml.presentation,application/vnd.openxmlformats-officedocument.spreadsheetml.sheet,application/vnd.openxmlformats-officedocument.presentationml.slideshow,application/zip,application/x-tar,application/x-rar-compressed,application/gzip,application/x-bzip2,application/x-7z-compressed,application/x-apple-diskimage,application/vnd.apache.arrow.file,video/mp4,audio/midi,video/matroska,video/webm,video/quicktime,video/vnd.avi,audio/wav,audio/qcelp,audio/x-ms-asf,video/x-ms-asf,application/vnd.ms-asf,video/mpeg,video/3gpp,audio/mpeg,audio/mp4,video/ogg,audio/ogg,audio/ogg; codecs=opus,application/ogg,audio/flac,audio/ape,audio/wavpack,audio/amr,application/pdf,application/x-elf,application/x-mach-binary,application/x-msdownload,application/x-shockwave-flash,application/rtf,application/wasm,font/woff,font/woff2,application/vnd.ms-fontobject,font/ttf,font/otf,font/collection,image/x-icon,video/x-flv,application/postscript,application/eps,application/x-xz,application/x-sqlite3,application/x-nintendo-nes-rom,application/x-google-chrome-extension,application/vnd.ms-cab-compressed,application/x-deb,application/x-unix-archive,application/x-rpm,application/x-compress,application/x-lzip,application/x-cfb,application/x-mie,application/mxf,video/mp2t,application/x-blender,image/bpg,image/j2c,image/jp2,image/jpx,image/jpm,image/mj2,audio/aiff,application/xml,application/x-mobipocket-ebook,image/heif,image/heif-sequence,image/heic,image/heic-sequence,image/icns,image/ktx,application/dicom,audio/x-musepack,text/calendar,text/vcard,text/vtt,model/gltf-binary,application/vnd.tcpdump.pcap,audio/x-dsf,application/x.ms.shortcut,application/x.apple.alias,audio/x-voc,audio/vnd.dolby.dd-raw,audio/x-m4a,image/apng,image/x-olympus-orf,image/x-sony-arw,image/x-adobe-dng,image/x-nikon-nef,image/x-panasonic-rw2,image/x-fujifilm-raf,video/x-m4v,video/3gpp2,application/x-esri-shape,audio/aac,audio/x-it,audio/x-s3m,audio/x-xm,video/MP1S,video/MP2P,application/vnd.sketchup.skp,image/avif,application/x-lzh-compressed,application/pgp-encrypted,application/x-asar,model/stl,application/vnd.ms-htmlhelp,model/3mf,image/jxl,application/zstd,image/jls,application/vnd.ms-outlook,image/vnd.dwg,application/vnd.apache.parquet,application/java-vm,application/x-arj,application/x-cpio,application/x-ace-compressed,application/avro,application/vnd.iccprofile,application/x.autodesk.fbx,application/vnd.visio,application/vnd.android.package-archive,application/vnd.google.draco,application/x-lz4,application/vnd.openxmlformats-officedocument.presentationml.template,application/vnd.openxmlformats-officedocument.spreadsheetml.template,application/vnd.openxmlformats-officedocument.wordprocessingml.template,application/vnd.ms-excel.template.macroenabled.12,application/vnd.oasis.opendocument.text-template,application/vnd.oasis.opendocument.spreadsheet-template,application/vnd.oasis.opendocument.presentation-template,application/vnd.oasis.opendocument.graphics,application/vnd.oasis.opendocument.graphics-template,application/vnd.ms-excel.sheet.macroenabled.12,application/vnd.ms-word.document.macroenabled.12,application/vnd.ms-word.template.macroenabled.12,application/vnd.ms-powerpoint.template.macroenabled.12,application/vnd.ms-powerpoint.presentation.macroenabled.12,application/java-archive,application/vnd.rn-realmedia,application/x-spss-sav,application/x-ms-regedit,application/x-ft-windows-registry-hive,application/x-jmp-data".split(","), N = 4100, Ie = 4098, P = 1048576, F = 1024, Le = 2 ** 31 - 1, I = 16777216, L = P, R = P, Re = I, ze = 1, z = 100, B = I, V = 64, H = L, Be = 256, Ve = 512, He = I, Ue = 512, We = 512, U = 256, Ge = L, W = L, Ke = L, qe = I, Je = /* @__PURE__ */ new Set([
	"Unexpected signature",
	"Encrypted ZIP",
	"Expected Central-File-Header signature"
]), Ye = [
	"ZIP entry count exceeds ",
	"Unsupported ZIP compression method:",
	"ZIP entry compressed data exceeds ",
	"ZIP entry decompressed data exceeds ",
	"Expected data-descriptor-signature at position "
], Xe = /* @__PURE__ */ new Set([
	"Z_BUF_ERROR",
	"Z_DATA_ERROR",
	"ERR_INVALID_STATE"
]), G = class extends Error {};
function Ze(e) {
	let t = e?.streamReader;
	if (t?.constructor?.name !== "WebStreamByobReader") return e;
	let { reader: n } = t, r = async () => {
		await n.cancel(), n.releaseLock();
	};
	return t.close = r, t.abort = async () => {
		t.interrupted = !0, await r();
	}, e;
}
function K(e, t, n) {
	if (!Number.isFinite(e) || e < 0 || e > t) throw new G(`${n} has invalid size ${e} (maximum ${t} bytes)`);
	return e;
}
async function q(e, t, { maximumLength: n = I, reason: r = "skip" } = {}) {
	let i = K(t, n, r);
	await e.ignore(i);
}
async function J(e, t, n, { maximumLength: r = t.length, reason: i = "read" } = {}) {
	let a = K(n?.length ?? t.length, r, i);
	return e.readBuffer(t, {
		...n,
		length: a
	});
}
async function Qe(e, { maximumLength: t = P } = {}) {
	let n = new ReadableStream({ start(t) {
		t.enqueue(e), t.close();
	} }).pipeThrough(new DecompressionStream("deflate-raw")).getReader(), r = [], i = 0;
	try {
		for (;;) {
			let { done: e, value: a } = await n.read();
			if (e) break;
			if (i += a.length, i > t) throw await n.cancel(), Error(`ZIP entry decompressed data exceeds ${t} bytes`);
			r.push(a);
		}
	} finally {
		n.releaseLock();
	}
	let a = new Uint8Array(i), o = 0;
	for (let e of r) a.set(e, o), o += e.length;
	return a;
}
var $e = 134695760, Y = 16, et = 15;
function tt(e, t) {
	if (e.length < Y) return -1;
	let n = e.length - Y;
	for (let r = 0; r <= n; r++) if (x.get(e, r) === $e && x.get(e, r + 8) === t + r) return r;
	return -1;
}
function nt(e) {
	return !!(e.codePointAt(0) & 32);
}
function rt(e, t) {
	let n = new Uint8Array(t), r = 0;
	for (let t of e) n.set(t, r), r += t.length;
	return n;
}
async function it(e, { shouldBuffer: t, maximumLength: n = P } = {}) {
	let { syncBuffer: r } = e, { length: i } = r, a = [], o = 0;
	for (;;) {
		let s = await e.tokenizer.peekBuffer(r, { mayBeLess: !0 }), c = tt(r.subarray(0, s), o), l = c >= 0 ? 0 : s === i ? Math.min(et, s - 1) : 0, u = c >= 0 ? c : s - l;
		if (u === 0) break;
		if (o += u, o > n) throw Error(`ZIP entry compressed data exceeds ${n} bytes`);
		if (t) {
			let t = new Uint8Array(u);
			await e.tokenizer.readBuffer(t), a.push(t);
		} else await e.tokenizer.ignore(u);
		if (c >= 0) break;
	}
	if (Z(e.tokenizer) || (e.knownSizeDescriptorScannedBytes += o), t) return rt(a, o);
}
function at(e, t) {
	return Z(e.tokenizer) ? Math.max(0, I - (e.tokenizer.position - t)) : Math.max(0, P - e.knownSizeDescriptorScannedBytes);
}
async function ot(e, t, { shouldBuffer: n, maximumDescriptorLength: r = P } = {}) {
	if (t.dataDescriptor && t.compressedSize === 0) return it(e, {
		shouldBuffer: n,
		maximumLength: r
	});
	if (!n) {
		await q(e.tokenizer, t.compressedSize, {
			maximumLength: Z(e.tokenizer) ? P : e.tokenizer.fileInfo.size,
			reason: "ZIP entry compressed data"
		});
		return;
	}
	let i = mt(e.tokenizer);
	if (!Number.isFinite(t.compressedSize) || t.compressedSize < 0 || t.compressedSize > i) throw Error(`ZIP entry compressed data exceeds ${i} bytes`);
	let a = new Uint8Array(t.compressedSize);
	return await e.tokenizer.readBuffer(a), a;
}
j.prototype.inflate = async function(e, t, n) {
	if (e.compressedMethod === 0) return n(t);
	if (e.compressedMethod !== 8) throw Error(`Unsupported ZIP compression method: ${e.compressedMethod}`);
	return n(await Qe(t, { maximumLength: P }));
}, j.prototype.unzip = async function(e) {
	let t = !1, n = 0, r = this.tokenizer.position;
	this.knownSizeDescriptorScannedBytes = 0;
	do {
		if (Q(this.tokenizer, r, I)) throw new G(`ZIP stream probing exceeds ${I} bytes`);
		let i = await this.readLocalFileHeader();
		if (!i) break;
		if (n++, n > F) throw Error(`ZIP entry count exceeds ${F}`);
		let a = e(i);
		t = !!a.stop, await this.tokenizer.ignore(i.extraFieldLength);
		let o = await ot(this, i, {
			shouldBuffer: !!a.handler,
			maximumDescriptorLength: Math.min(P, at(this, r))
		});
		if (a.handler && await this.inflate(i, o, a.handler), i.dataDescriptor) {
			let e = new Uint8Array(Y);
			if (await this.tokenizer.readBuffer(e), x.get(e, 0) !== $e) throw Error(`Expected data-descriptor-signature at position ${this.tokenizer.position - e.length}`);
		}
		if (Q(this.tokenizer, r, I)) throw new G(`ZIP stream probing exceeds ${I} bytes`);
	} while (!t);
};
function st(e, t) {
	let n = e.getReader(), r = 0, i = !1, a = !1, o = async (e) => {
		i || a || (a = !0, await n.cancel(e));
	};
	return new ReadableStream({
		async pull(e) {
			if (r >= t) {
				e.close(), await o();
				return;
			}
			let { done: a, value: s } = await n.read();
			if (a || !s) {
				i = !0, e.close();
				return;
			}
			let c = t - r;
			s.length > c ? (e.enqueue(s.subarray(0, c)), r += c, e.close(), await o()) : (e.enqueue(s), r += s.length);
		},
		async cancel(e) {
			await o(e);
		}
	});
}
async function ct(e, t) {
	return new bt(t).fromBuffer(e);
}
function lt(e) {
	switch (e = e.toLowerCase(), e) {
		case "application/epub+zip": return {
			ext: "epub",
			mime: e
		};
		case "application/vnd.oasis.opendocument.text": return {
			ext: "odt",
			mime: e
		};
		case "application/vnd.oasis.opendocument.text-template": return {
			ext: "ott",
			mime: e
		};
		case "application/vnd.oasis.opendocument.spreadsheet": return {
			ext: "ods",
			mime: e
		};
		case "application/vnd.oasis.opendocument.spreadsheet-template": return {
			ext: "ots",
			mime: e
		};
		case "application/vnd.oasis.opendocument.presentation": return {
			ext: "odp",
			mime: e
		};
		case "application/vnd.oasis.opendocument.presentation-template": return {
			ext: "otp",
			mime: e
		};
		case "application/vnd.oasis.opendocument.graphics": return {
			ext: "odg",
			mime: e
		};
		case "application/vnd.oasis.opendocument.graphics-template": return {
			ext: "otg",
			mime: e
		};
		case "application/vnd.openxmlformats-officedocument.presentationml.slideshow": return {
			ext: "ppsx",
			mime: e
		};
		case "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet": return {
			ext: "xlsx",
			mime: e
		};
		case "application/vnd.ms-excel.sheet.macroenabled": return {
			ext: "xlsm",
			mime: "application/vnd.ms-excel.sheet.macroenabled.12"
		};
		case "application/vnd.openxmlformats-officedocument.spreadsheetml.template": return {
			ext: "xltx",
			mime: e
		};
		case "application/vnd.ms-excel.template.macroenabled": return {
			ext: "xltm",
			mime: "application/vnd.ms-excel.template.macroenabled.12"
		};
		case "application/vnd.ms-powerpoint.slideshow.macroenabled": return {
			ext: "ppsm",
			mime: "application/vnd.ms-powerpoint.slideshow.macroenabled.12"
		};
		case "application/vnd.openxmlformats-officedocument.wordprocessingml.document": return {
			ext: "docx",
			mime: e
		};
		case "application/vnd.ms-word.document.macroenabled": return {
			ext: "docm",
			mime: "application/vnd.ms-word.document.macroenabled.12"
		};
		case "application/vnd.openxmlformats-officedocument.wordprocessingml.template": return {
			ext: "dotx",
			mime: e
		};
		case "application/vnd.ms-word.template.macroenabledtemplate": return {
			ext: "dotm",
			mime: "application/vnd.ms-word.template.macroenabled.12"
		};
		case "application/vnd.openxmlformats-officedocument.presentationml.template": return {
			ext: "potx",
			mime: e
		};
		case "application/vnd.ms-powerpoint.template.macroenabled": return {
			ext: "potm",
			mime: "application/vnd.ms-powerpoint.template.macroenabled.12"
		};
		case "application/vnd.openxmlformats-officedocument.presentationml.presentation": return {
			ext: "pptx",
			mime: e
		};
		case "application/vnd.ms-powerpoint.presentation.macroenabled": return {
			ext: "pptm",
			mime: "application/vnd.ms-powerpoint.presentation.macroenabled.12"
		};
		case "application/vnd.ms-visio.drawing": return {
			ext: "vsdx",
			mime: "application/vnd.visio"
		};
		case "application/vnd.ms-package.3dmanufacturing-3dmodel+xml": return {
			ext: "3mf",
			mime: "model/3mf"
		};
	}
}
function X(e, t, n) {
	n = {
		offset: 0,
		...n
	};
	for (let [r, i] of t.entries()) if (n.mask) {
		if (i !== (n.mask[r] & e[r + n.offset])) return !1;
	} else if (i !== e[r + n.offset]) return !1;
	return !0;
}
function ut(e) {
	return Number.isFinite(e) ? Math.max(1, Math.trunc(e)) : N;
}
function dt(e, t, n) {
	return n === void 0 ? e.read(t) : (n.throwIfAborted(), new Promise((r, i) => {
		let a = () => {
			n.removeEventListener("abort", o);
		}, o = () => {
			let t = n.reason;
			a(), (async () => {
				try {
					await e.cancel(t);
				} catch {}
			})(), i(t);
		};
		n.addEventListener("abort", o, { once: !0 }), (async () => {
			try {
				let n = await e.read(t);
				a(), r(n);
			} catch (e) {
				a(), i(e);
			}
		})();
	}));
}
function ft(e) {
	return Number.isFinite(e) ? Math.max(0, Math.min(Ie, Math.trunc(e))) : 0;
}
function pt(e) {
	return Number.isFinite(e) ? Math.max(0, e) : 2 ** 53 - 1;
}
function Z(e) {
	let t = e.fileInfo.size;
	return !Number.isFinite(t) || t === 2 ** 53 - 1;
}
function Q(e, t, n) {
	return Z(e) && e.position - t > n;
}
function mt(e) {
	let t = e.fileInfo.size, n = Number.isFinite(t) ? Math.max(0, t - e.position) : 2 ** 53 - 1;
	return Math.min(n, Le);
}
function ht(e) {
	if (e instanceof o || e instanceof G) return !0;
	if (!(e instanceof Error)) return !1;
	if (Je.has(e.message) || Xe.has(e.code)) return !0;
	for (let t of Ye) if (e.message.startsWith(t)) return !0;
	return !1;
}
function gt(e, t = P) {
	let n = [e.compressedSize, e.uncompressedSize];
	for (let e of n) if (!Number.isFinite(e) || e < 0 || e > t) return !1;
	return !0;
}
function _t() {
	return {
		hasContentTypesEntry: !1,
		hasParsedContentTypesEntry: !1,
		isParsingContentTypes: !1,
		hasUnparseableContentTypes: !1,
		hasWordDirectory: !1,
		hasPresentationDirectory: !1,
		hasSpreadsheetDirectory: !1,
		hasThreeDimensionalModelEntry: !1
	};
}
function vt(e, t) {
	t.startsWith("word/") && (e.hasWordDirectory = !0), t.startsWith("ppt/") && (e.hasPresentationDirectory = !0), t.startsWith("xl/") && (e.hasSpreadsheetDirectory = !0), t.startsWith("3D/") && t.endsWith(".model") && (e.hasThreeDimensionalModelEntry = !0);
}
function $(e) {
	if (!(!e.hasContentTypesEntry || e.hasUnparseableContentTypes || e.isParsingContentTypes || e.hasParsedContentTypesEntry)) {
		if (e.hasWordDirectory) return {
			ext: "docx",
			mime: "application/vnd.openxmlformats-officedocument.wordprocessingml.document"
		};
		if (e.hasPresentationDirectory) return {
			ext: "pptx",
			mime: "application/vnd.openxmlformats-officedocument.presentationml.presentation"
		};
		if (e.hasSpreadsheetDirectory) return {
			ext: "xlsx",
			mime: "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"
		};
		if (e.hasThreeDimensionalModelEntry) return {
			ext: "3mf",
			mime: "model/3mf"
		};
	}
}
function yt(e) {
	let t = e.indexOf(".main+xml\"");
	if (t === -1) {
		let t = "application/vnd.ms-package.3dmanufacturing-3dmodel+xml";
		return e.includes(`ContentType="${t}"`) ? t : void 0;
	}
	let n = e.slice(0, t), r = n.lastIndexOf("\"");
	return n.slice(r + 1);
}
var bt = class e {
	constructor(e) {
		let t = ft(e?.mpegOffsetTolerance);
		this.options = {
			...e,
			mpegOffsetTolerance: t
		}, this.detectors = [
			...this.options.customDetectors ?? [],
			{
				id: "core",
				detect: this.detectConfident
			},
			{
				id: "core.imprecise",
				detect: this.detectImprecise
			}
		], this.tokenizerOptions = { abortSignal: this.options.signal }, this.gzipProbeDepth = 0;
	}
	getTokenizerOptions() {
		return { ...this.tokenizerOptions };
	}
	createTokenizerFromWebStream(e) {
		return Ze(ie(e, this.getTokenizerOptions()));
	}
	async parseTokenizer(e, t = 0) {
		this.detectionReentryCount = t;
		let n = e.position;
		for (let t of this.detectors) {
			let r;
			try {
				r = await t.detect(e);
			} catch (e) {
				if (e instanceof o || e instanceof G) return;
				throw e;
			}
			if (r) return r;
			if (n !== e.position) return;
		}
	}
	async fromTokenizer(e) {
		try {
			return await this.parseTokenizer(e);
		} finally {
			await e.close();
		}
	}
	async fromBuffer(e) {
		if (!(e instanceof Uint8Array || e instanceof ArrayBuffer)) throw TypeError(`Expected the \`input\` argument to be of type \`Uint8Array\` or \`ArrayBuffer\`, got \`${typeof e}\``);
		let t = e instanceof Uint8Array ? e : new Uint8Array(e);
		if (t?.length > 1) return this.fromTokenizer(ae(t, this.getTokenizerOptions()));
	}
	async fromBlob(e) {
		this.options.signal?.throwIfAborted();
		let t = oe(e, this.getTokenizerOptions());
		return this.fromTokenizer(t);
	}
	async fromStream(e) {
		this.options.signal?.throwIfAborted();
		let t = this.createTokenizerFromWebStream(e);
		return this.fromTokenizer(t);
	}
	async toDetectionStream(e, t) {
		let n = ut(t?.sampleSize ?? 4100), r, i, a = e.getReader({ mode: "byob" });
		try {
			let { value: e, done: t } = await dt(a, new Uint8Array(n), this.options.signal);
			if (i = e, !t && e) try {
				r = await this.fromBuffer(e.subarray(0, n));
			} catch (e) {
				if (!(e instanceof o)) throw e;
				r = void 0;
			}
			i = e;
		} finally {
			a.releaseLock();
		}
		let s = new TransformStream({
			async start(e) {
				e.enqueue(i);
			},
			transform(e, t) {
				t.enqueue(e);
			}
		}), c = e.pipeThrough(s);
		return c.fileType = r, c;
	}
	async detectGzip(t) {
		if (this.gzipProbeDepth >= ze) return {
			ext: "gz",
			mime: "application/gzip"
		};
		let n = st(new Ae(t).inflate(), Re), r = Z(t), i, a, o, s;
		if (r) {
			let t = new AbortController();
			i = setTimeout(() => {
				t.abort(new DOMException(`Operation timed out after ${z} ms`, "TimeoutError"));
			}, z), a = this.options.signal === void 0 ? t.signal : AbortSignal.any([this.options.signal, t.signal]), o = new e({
				...this.options,
				signal: a
			}), o.gzipProbeDepth = this.gzipProbeDepth + 1;
		} else this.gzipProbeDepth++;
		try {
			s = await (o ?? this).fromStream(n);
		} catch (e) {
			if (e?.name === "AbortError" && a?.reason?.name !== "TimeoutError") throw e;
		} finally {
			clearTimeout(i), r || this.gzipProbeDepth--;
		}
		return s?.ext === "tar" ? {
			ext: "tar.gz",
			mime: "application/gzip"
		} : {
			ext: "gz",
			mime: "application/gzip"
		};
	}
	check(e, t) {
		return X(this.buffer, e, t);
	}
	checkString(e, t) {
		return this.check(je(e, t?.encoding), t);
	}
	detectConfident = async (e) => {
		if (this.buffer = new Uint8Array(N), e.fileInfo.size === void 0 && (e.fileInfo.size = 2 ** 53 - 1), this.tokenizer = e, Z(e) && (await e.peekBuffer(this.buffer, {
			length: 3,
			mayBeLess: !0
		}), this.check([
			31,
			139,
			8
		]))) return this.detectGzip(e);
		if (await e.peekBuffer(this.buffer, {
			length: 32,
			mayBeLess: !0
		}), this.check([66, 77])) return {
			ext: "bmp",
			mime: "image/bmp"
		};
		if (this.check([11, 119])) return {
			ext: "ac3",
			mime: "audio/vnd.dolby.dd-raw"
		};
		if (this.check([120, 1])) return {
			ext: "dmg",
			mime: "application/x-apple-diskimage"
		};
		if (this.check([77, 90])) return {
			ext: "exe",
			mime: "application/x-msdownload"
		};
		if (this.check([37, 33])) return await e.peekBuffer(this.buffer, {
			length: 24,
			mayBeLess: !0
		}), this.checkString("PS-Adobe-", { offset: 2 }) && this.checkString(" EPSF-", { offset: 14 }) ? {
			ext: "eps",
			mime: "application/eps"
		} : {
			ext: "ps",
			mime: "application/postscript"
		};
		if (this.check([31, 160]) || this.check([31, 157])) return {
			ext: "Z",
			mime: "application/x-compress"
		};
		if (this.check([199, 113])) return {
			ext: "cpio",
			mime: "application/x-cpio"
		};
		if (this.check([96, 234])) return {
			ext: "arj",
			mime: "application/x-arj"
		};
		if (this.check([
			239,
			187,
			191
		])) return this.detectionReentryCount >= U ? void 0 : (this.detectionReentryCount++, await this.tokenizer.ignore(3), this.detectConfident(e));
		if (this.check([
			71,
			73,
			70
		])) return {
			ext: "gif",
			mime: "image/gif"
		};
		if (this.check([
			73,
			73,
			188
		])) return {
			ext: "jxr",
			mime: "image/vnd.ms-photo"
		};
		if (this.check([
			31,
			139,
			8
		])) return this.detectGzip(e);
		if (this.check([
			66,
			90,
			104
		])) return {
			ext: "bz2",
			mime: "application/x-bzip2"
		};
		if (this.checkString("ID3")) {
			await q(e, 6, {
				maximumLength: 6,
				reason: "ID3 header prefix"
			});
			let t = await e.readToken(Ne), n = Z(e);
			if (!Number.isFinite(t) || t < 0 || n && (t > B || e.position + t > B)) return;
			if (e.position + t > e.fileInfo.size) return n ? void 0 : {
				ext: "mp3",
				mime: "audio/mpeg"
			};
			try {
				await q(e, t, {
					maximumLength: n ? B : e.fileInfo.size,
					reason: "ID3 payload"
				});
			} catch (e) {
				if (e instanceof o) return;
				throw e;
			}
			return this.detectionReentryCount >= U ? void 0 : (this.detectionReentryCount++, this.parseTokenizer(e, this.detectionReentryCount));
		}
		if (this.checkString("MP+")) return {
			ext: "mpc",
			mime: "audio/x-musepack"
		};
		if ((this.buffer[0] === 67 || this.buffer[0] === 70) && this.check([87, 83], { offset: 1 })) return {
			ext: "swf",
			mime: "application/x-shockwave-flash"
		};
		if (this.check([
			255,
			216,
			255
		])) return this.check([247], { offset: 3 }) ? {
			ext: "jls",
			mime: "image/jls"
		} : {
			ext: "jpg",
			mime: "image/jpeg"
		};
		if (this.check([
			79,
			98,
			106,
			1
		])) return {
			ext: "avro",
			mime: "application/avro"
		};
		if (this.checkString("FLIF")) return {
			ext: "flif",
			mime: "image/flif"
		};
		if (this.checkString("8BPS")) return {
			ext: "psd",
			mime: "image/vnd.adobe.photoshop"
		};
		if (this.checkString("MPCK")) return {
			ext: "mpc",
			mime: "audio/x-musepack"
		};
		if (this.checkString("FORM")) return {
			ext: "aif",
			mime: "audio/aiff"
		};
		if (this.checkString("icns", { offset: 0 })) return {
			ext: "icns",
			mime: "image/icns"
		};
		if (this.check([
			80,
			75,
			3,
			4
		])) {
			let t, n = _t();
			try {
				await new j(e).unzip((e) => {
					vt(n, e.filename);
					let r = e.filename === "[Content_Types].xml", i = $(n);
					if (!r && i) return t = i, { stop: !0 };
					switch (e.filename) {
						case "META-INF/mozilla.rsa": return t = {
							ext: "xpi",
							mime: "application/x-xpinstall"
						}, { stop: !0 };
						case "META-INF/MANIFEST.MF": return t = {
							ext: "jar",
							mime: "application/java-archive"
						}, { stop: !0 };
						case "mimetype": return gt(e, R) ? {
							async handler(e) {
								t = lt(new TextDecoder("utf-8").decode(e).trim());
							},
							stop: !0
						} : {};
						case "[Content_Types].xml": return n.hasContentTypesEntry = !0, gt(e, R) ? (n.isParsingContentTypes = !0, {
							async handler(e) {
								let r = yt(new TextDecoder("utf-8").decode(e));
								r && (t = lt(r)), n.hasParsedContentTypesEntry = !0, n.isParsingContentTypes = !1;
							},
							stop: !0
						}) : (n.hasUnparseableContentTypes = !0, {});
						default: return /classes\d*\.dex/.test(e.filename) ? (t = {
							ext: "apk",
							mime: "application/vnd.android.package-archive"
						}, { stop: !0 }) : {};
					}
				});
			} catch (e) {
				if (!ht(e)) throw e;
				n.isParsingContentTypes && (n.isParsingContentTypes = !1, n.hasUnparseableContentTypes = !0);
			}
			return t ?? $(n) ?? {
				ext: "zip",
				mime: "application/zip"
			};
		}
		if (this.checkString("OggS")) {
			await e.ignore(28);
			let t = /* @__PURE__ */ new Uint8Array(8);
			return await e.readBuffer(t), X(t, [
				79,
				112,
				117,
				115,
				72,
				101,
				97,
				100
			]) ? {
				ext: "opus",
				mime: "audio/ogg; codecs=opus"
			} : X(t, [
				128,
				116,
				104,
				101,
				111,
				114,
				97
			]) ? {
				ext: "ogv",
				mime: "video/ogg"
			} : X(t, [
				1,
				118,
				105,
				100,
				101,
				111,
				0
			]) ? {
				ext: "ogm",
				mime: "video/ogg"
			} : X(t, [
				127,
				70,
				76,
				65,
				67
			]) ? {
				ext: "oga",
				mime: "audio/ogg"
			} : X(t, [
				83,
				112,
				101,
				101,
				120,
				32,
				32
			]) ? {
				ext: "spx",
				mime: "audio/ogg"
			} : X(t, [
				1,
				118,
				111,
				114,
				98,
				105,
				115
			]) ? {
				ext: "ogg",
				mime: "audio/ogg"
			} : {
				ext: "ogx",
				mime: "application/ogg"
			};
		}
		if (this.check([80, 75]) && (this.buffer[2] === 3 || this.buffer[2] === 5 || this.buffer[2] === 7) && (this.buffer[3] === 4 || this.buffer[3] === 6 || this.buffer[3] === 8)) return {
			ext: "zip",
			mime: "application/zip"
		};
		if (this.checkString("MThd")) return {
			ext: "mid",
			mime: "audio/midi"
		};
		if (this.checkString("wOFF") && (this.check([
			0,
			1,
			0,
			0
		], { offset: 4 }) || this.checkString("OTTO", { offset: 4 }))) return {
			ext: "woff",
			mime: "font/woff"
		};
		if (this.checkString("wOF2") && (this.check([
			0,
			1,
			0,
			0
		], { offset: 4 }) || this.checkString("OTTO", { offset: 4 }))) return {
			ext: "woff2",
			mime: "font/woff2"
		};
		if (this.check([
			212,
			195,
			178,
			161
		]) || this.check([
			161,
			178,
			195,
			212
		])) return {
			ext: "pcap",
			mime: "application/vnd.tcpdump.pcap"
		};
		if (this.checkString("DSD ")) return {
			ext: "dsf",
			mime: "audio/x-dsf"
		};
		if (this.checkString("LZIP")) return {
			ext: "lz",
			mime: "application/x-lzip"
		};
		if (this.checkString("fLaC")) return {
			ext: "flac",
			mime: "audio/flac"
		};
		if (this.check([
			66,
			80,
			71,
			251
		])) return {
			ext: "bpg",
			mime: "image/bpg"
		};
		if (this.checkString("wvpk")) return {
			ext: "wv",
			mime: "audio/wavpack"
		};
		if (this.checkString("%PDF")) return {
			ext: "pdf",
			mime: "application/pdf"
		};
		if (this.check([
			0,
			97,
			115,
			109
		])) return {
			ext: "wasm",
			mime: "application/wasm"
		};
		if (this.check([73, 73])) {
			let e = await this.readTiffHeader(!1);
			if (e) return e;
		}
		if (this.check([77, 77])) {
			let e = await this.readTiffHeader(!0);
			if (e) return e;
		}
		if (this.checkString("MAC ")) return {
			ext: "ape",
			mime: "audio/ape"
		};
		if (this.check([
			26,
			69,
			223,
			163
		])) {
			async function t() {
				let t = await e.peekNumber(ve), n = 128, r = 0;
				for (; (t & n) === 0 && n !== 0;) ++r, n >>= 1;
				let i = new Uint8Array(r + 1);
				return await J(e, i, void 0, {
					maximumLength: i.length,
					reason: "EBML field"
				}), i;
			}
			async function n() {
				let e = await t(), n = await t();
				n[0] ^= 128 >> n.length - 1;
				let r = Math.min(6, n.length), i = new DataView(e.buffer), a = new DataView(n.buffer, n.length - r, r);
				return {
					id: M(i),
					len: M(a)
				};
			}
			async function r(t) {
				let r = 0;
				for (; t > 0;) {
					if (r++, r > Be || Q(e, a, I)) return;
					let i = e.position, o = await n();
					if (o.id === 17026) {
						if (o.len > V) return;
						let t = K(o.len, V, "EBML DocType");
						return (await e.readToken(new C(t))).replaceAll(/\00.*$/g, "");
					}
					if (Z(e) && (!Number.isFinite(o.len) || o.len < 0 || o.len > H) || (await q(e, o.len, {
						maximumLength: Z(e) ? H : e.fileInfo.size,
						reason: "EBML payload"
					}), --t, e.position <= i)) return;
				}
			}
			let i = await n(), a = e.position;
			switch (await r(i.len)) {
				case "webm": return {
					ext: "webm",
					mime: "video/webm"
				};
				case "matroska": return {
					ext: "mkv",
					mime: "video/matroska"
				};
				default: return;
			}
		}
		if (this.checkString("SQLi")) return {
			ext: "sqlite",
			mime: "application/x-sqlite3"
		};
		if (this.check([
			78,
			69,
			83,
			26
		])) return {
			ext: "nes",
			mime: "application/x-nintendo-nes-rom"
		};
		if (this.checkString("Cr24")) return {
			ext: "crx",
			mime: "application/x-google-chrome-extension"
		};
		if (this.checkString("MSCF") || this.checkString("ISc(")) return {
			ext: "cab",
			mime: "application/vnd.ms-cab-compressed"
		};
		if (this.check([
			237,
			171,
			238,
			219
		])) return {
			ext: "rpm",
			mime: "application/x-rpm"
		};
		if (this.check([
			197,
			208,
			211,
			198
		])) return {
			ext: "eps",
			mime: "application/eps"
		};
		if (this.check([
			40,
			181,
			47,
			253
		])) return {
			ext: "zst",
			mime: "application/zstd"
		};
		if (this.check([
			127,
			69,
			76,
			70
		])) return {
			ext: "elf",
			mime: "application/x-elf"
		};
		if (this.check([
			33,
			66,
			68,
			78
		])) return {
			ext: "pst",
			mime: "application/vnd.ms-outlook"
		};
		if (this.checkString("PAR1") || this.checkString("PARE")) return {
			ext: "parquet",
			mime: "application/vnd.apache.parquet"
		};
		if (this.checkString("ttcf")) return {
			ext: "ttc",
			mime: "font/collection"
		};
		if (this.check([
			254,
			237,
			250,
			206
		]) || this.check([
			254,
			237,
			250,
			207
		]) || this.check([
			206,
			250,
			237,
			254
		]) || this.check([
			207,
			250,
			237,
			254
		])) return {
			ext: "macho",
			mime: "application/x-mach-binary"
		};
		if (this.check([
			4,
			34,
			77,
			24
		])) return {
			ext: "lz4",
			mime: "application/x-lz4"
		};
		if (this.checkString("regf")) return {
			ext: "dat",
			mime: "application/x-ft-windows-registry-hive"
		};
		if (this.checkString("$FL2") || this.checkString("$FL3")) return {
			ext: "sav",
			mime: "application/x-spss-sav"
		};
		if (this.check([
			79,
			84,
			84,
			79,
			0
		])) return {
			ext: "otf",
			mime: "font/otf"
		};
		if (this.checkString("#!AMR")) return {
			ext: "amr",
			mime: "audio/amr"
		};
		if (this.checkString("{\\rtf")) return {
			ext: "rtf",
			mime: "application/rtf"
		};
		if (this.check([
			70,
			76,
			86,
			1
		])) return {
			ext: "flv",
			mime: "video/x-flv"
		};
		if (this.checkString("IMPM")) return {
			ext: "it",
			mime: "audio/x-it"
		};
		if (this.checkString("-lh0-", { offset: 2 }) || this.checkString("-lh1-", { offset: 2 }) || this.checkString("-lh2-", { offset: 2 }) || this.checkString("-lh3-", { offset: 2 }) || this.checkString("-lh4-", { offset: 2 }) || this.checkString("-lh5-", { offset: 2 }) || this.checkString("-lh6-", { offset: 2 }) || this.checkString("-lh7-", { offset: 2 }) || this.checkString("-lzs-", { offset: 2 }) || this.checkString("-lz4-", { offset: 2 }) || this.checkString("-lz5-", { offset: 2 }) || this.checkString("-lhd-", { offset: 2 })) return {
			ext: "lzh",
			mime: "application/x-lzh-compressed"
		};
		if (this.check([
			0,
			0,
			1,
			186
		])) {
			if (this.check([33], {
				offset: 4,
				mask: [241]
			})) return {
				ext: "mpg",
				mime: "video/MP1S"
			};
			if (this.check([68], {
				offset: 4,
				mask: [196]
			})) return {
				ext: "mpg",
				mime: "video/MP2P"
			};
		}
		if (this.checkString("ITSF")) return {
			ext: "chm",
			mime: "application/vnd.ms-htmlhelp"
		};
		if (this.check([
			202,
			254,
			186,
			190
		])) {
			let e = S.get(this.buffer, 4), t = b.get(this.buffer, 6);
			if (e > 0 && e <= 30) return {
				ext: "macho",
				mime: "application/x-mach-binary"
			};
			if (t > 30) return {
				ext: "class",
				mime: "application/java-vm"
			};
		}
		if (this.checkString(".RMF")) return {
			ext: "rm",
			mime: "application/vnd.rn-realmedia"
		};
		if (this.checkString("DRACO")) return {
			ext: "drc",
			mime: "application/vnd.google.draco"
		};
		if (this.check([
			253,
			55,
			122,
			88,
			90,
			0
		])) return {
			ext: "xz",
			mime: "application/x-xz"
		};
		if (this.checkString("<?xml ")) return {
			ext: "xml",
			mime: "application/xml"
		};
		if (this.check([
			55,
			122,
			188,
			175,
			39,
			28
		])) return {
			ext: "7z",
			mime: "application/x-7z-compressed"
		};
		if (this.check([
			82,
			97,
			114,
			33,
			26,
			7
		]) && (this.buffer[6] === 0 || this.buffer[6] === 1)) return {
			ext: "rar",
			mime: "application/x-rar-compressed"
		};
		if (this.checkString("solid ")) return {
			ext: "stl",
			mime: "model/stl"
		};
		if (this.checkString("AC")) {
			let e = new C(4, "latin1").get(this.buffer, 2);
			if (e.match("^d*") && e >= 1e3 && e <= 1050) return {
				ext: "dwg",
				mime: "image/vnd.dwg"
			};
		}
		if (this.checkString("070707")) return {
			ext: "cpio",
			mime: "application/x-cpio"
		};
		if (this.checkString("BLENDER")) return {
			ext: "blend",
			mime: "application/x-blender"
		};
		if (this.checkString("!<arch>")) return await e.ignore(8), await e.readToken(new C(13, "ascii")) === "debian-binary" ? {
			ext: "deb",
			mime: "application/x-deb"
		} : {
			ext: "ar",
			mime: "application/x-unix-archive"
		};
		if (this.checkString("WEBVTT") && [
			"\n",
			"\r",
			"	",
			" ",
			"\0"
		].some((e) => this.checkString(e, { offset: 6 }))) return {
			ext: "vtt",
			mime: "text/vtt"
		};
		if (this.check([
			137,
			80,
			78,
			71,
			13,
			10,
			26,
			10
		])) {
			let t = {
				ext: "png",
				mime: "image/png"
			}, n = {
				ext: "apng",
				mime: "image/apng"
			};
			await e.ignore(8);
			async function r() {
				return {
					length: await e.readToken(ye),
					type: await e.readToken(new C(4, "latin1"))
				};
			}
			let i = Z(e), a = e.position, s = 0, c = !1;
			do {
				if (s++, s > Ve || Q(e, a, He)) break;
				let l = e.position, u = await r();
				if (u.length < 0) return;
				if (u.type === "IHDR") {
					if (u.length !== 13) return;
					c = !0;
				}
				switch (u.type) {
					case "IDAT": return t;
					case "acTL": return n;
					default:
						if (!c && u.type !== "CgBI") return;
						if (i && u.length > Ge) return c && nt(u.type) ? t : void 0;
						try {
							await q(e, u.length + 4, {
								maximumLength: i ? 1048580 : e.fileInfo.size,
								reason: "PNG chunk payload"
							});
						} catch (e) {
							if (!i && (e instanceof G || e instanceof o)) return t;
							throw e;
						}
				}
				if (e.position <= l) break;
			} while (e.position + 8 < e.fileInfo.size);
			return t;
		}
		if (this.check([
			65,
			82,
			82,
			79,
			87,
			49,
			0,
			0
		])) return {
			ext: "arrow",
			mime: "application/vnd.apache.arrow.file"
		};
		if (this.check([
			103,
			108,
			84,
			70,
			2,
			0,
			0,
			0
		])) return {
			ext: "glb",
			mime: "model/gltf-binary"
		};
		if (this.check([
			102,
			114,
			101,
			101
		], { offset: 4 }) || this.check([
			109,
			100,
			97,
			116
		], { offset: 4 }) || this.check([
			109,
			111,
			111,
			118
		], { offset: 4 }) || this.check([
			119,
			105,
			100,
			101
		], { offset: 4 })) return {
			ext: "mov",
			mime: "video/quicktime"
		};
		if (this.check([
			73,
			73,
			82,
			79,
			8,
			0,
			0,
			0,
			24
		])) return {
			ext: "orf",
			mime: "image/x-olympus-orf"
		};
		if (this.checkString("gimp xcf ")) return {
			ext: "xcf",
			mime: "image/x-xcf"
		};
		if (this.checkString("ftyp", { offset: 4 }) && this.buffer[8] & 96) {
			let e = new C(4, "latin1").get(this.buffer, 8).replace("\0", " ").trim();
			switch (e) {
				case "avif":
				case "avis": return {
					ext: "avif",
					mime: "image/avif"
				};
				case "mif1": return {
					ext: "heic",
					mime: "image/heif"
				};
				case "msf1": return {
					ext: "heic",
					mime: "image/heif-sequence"
				};
				case "heic":
				case "heix": return {
					ext: "heic",
					mime: "image/heic"
				};
				case "hevc":
				case "hevx": return {
					ext: "heic",
					mime: "image/heic-sequence"
				};
				case "qt": return {
					ext: "mov",
					mime: "video/quicktime"
				};
				case "M4V":
				case "M4VH":
				case "M4VP": return {
					ext: "m4v",
					mime: "video/x-m4v"
				};
				case "M4P": return {
					ext: "m4p",
					mime: "video/mp4"
				};
				case "M4B": return {
					ext: "m4b",
					mime: "audio/mp4"
				};
				case "M4A": return {
					ext: "m4a",
					mime: "audio/x-m4a"
				};
				case "F4V": return {
					ext: "f4v",
					mime: "video/mp4"
				};
				case "F4P": return {
					ext: "f4p",
					mime: "video/mp4"
				};
				case "F4A": return {
					ext: "f4a",
					mime: "audio/mp4"
				};
				case "F4B": return {
					ext: "f4b",
					mime: "audio/mp4"
				};
				case "crx": return {
					ext: "cr3",
					mime: "image/x-canon-cr3"
				};
				default: return e.startsWith("3g") ? e.startsWith("3g2") ? {
					ext: "3g2",
					mime: "video/3gpp2"
				} : {
					ext: "3gp",
					mime: "video/3gpp"
				} : {
					ext: "mp4",
					mime: "video/mp4"
				};
			}
		}
		if (this.checkString("REGEDIT4\r\n")) return {
			ext: "reg",
			mime: "application/x-ms-regedit"
		};
		if (this.check([
			82,
			73,
			70,
			70
		])) {
			if (this.checkString("WEBP", { offset: 8 })) return {
				ext: "webp",
				mime: "image/webp"
			};
			if (this.check([
				65,
				86,
				73
			], { offset: 8 })) return {
				ext: "avi",
				mime: "video/vnd.avi"
			};
			if (this.check([
				87,
				65,
				86,
				69
			], { offset: 8 })) return {
				ext: "wav",
				mime: "audio/wav"
			};
			if (this.check([
				81,
				76,
				67,
				77
			], { offset: 8 })) return {
				ext: "qcp",
				mime: "audio/qcelp"
			};
		}
		if (this.check([
			73,
			73,
			85,
			0,
			24,
			0,
			0,
			0,
			136,
			231,
			116,
			216
		])) return {
			ext: "rw2",
			mime: "image/x-panasonic-rw2"
		};
		if (this.check([
			48,
			38,
			178,
			117,
			142,
			102,
			207,
			17,
			166,
			217
		])) {
			let t = !1;
			try {
				async function n() {
					let t = /* @__PURE__ */ new Uint8Array(16);
					return await J(e, t, void 0, {
						maximumLength: t.length,
						reason: "ASF header GUID"
					}), {
						id: t,
						size: Number(await e.readToken(be))
					};
				}
				await q(e, 30, {
					maximumLength: 30,
					reason: "ASF header prelude"
				});
				let r = Z(e), i = e.position, a = 0;
				for (; e.position + 24 < e.fileInfo.size && (a++, !(a > Ue || Q(e, i, I)));) {
					let i = e.position, a = await n(), o = a.size - 24;
					if (!Number.isFinite(o) || o < 0) {
						t = !0;
						break;
					}
					if (X(a.id, [
						145,
						7,
						220,
						183,
						183,
						169,
						207,
						17,
						142,
						230,
						0,
						192,
						12,
						32,
						83,
						101
					])) {
						let t = /* @__PURE__ */ new Uint8Array(16);
						if (o -= await J(e, t, void 0, {
							maximumLength: t.length,
							reason: "ASF stream type GUID"
						}), X(t, [
							64,
							158,
							105,
							248,
							77,
							91,
							207,
							17,
							168,
							253,
							0,
							128,
							95,
							92,
							68,
							43
						])) return {
							ext: "asf",
							mime: "audio/x-ms-asf"
						};
						if (X(t, [
							192,
							239,
							25,
							188,
							77,
							91,
							207,
							17,
							168,
							253,
							0,
							128,
							95,
							92,
							68,
							43
						])) return {
							ext: "asf",
							mime: "video/x-ms-asf"
						};
						break;
					}
					if (r && o > W || (await q(e, o, {
						maximumLength: r ? W : e.fileInfo.size,
						reason: "ASF header payload"
					}), e.position <= i)) {
						t = !0;
						break;
					}
				}
			} catch (n) {
				if (n instanceof o || n instanceof G) Z(e) && (t = !0);
				else throw n;
			}
			return t ? void 0 : {
				ext: "asf",
				mime: "application/vnd.ms-asf"
			};
		}
		if (this.check([
			171,
			75,
			84,
			88,
			32,
			49,
			49,
			187,
			13,
			10,
			26,
			10
		])) return {
			ext: "ktx",
			mime: "image/ktx"
		};
		if ((this.check([
			126,
			16,
			4
		]) || this.check([
			126,
			24,
			4
		])) && this.check([
			48,
			77,
			73,
			69
		], { offset: 4 })) return {
			ext: "mie",
			mime: "application/x-mie"
		};
		if (this.check([
			39,
			10,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0
		], { offset: 2 })) return {
			ext: "shp",
			mime: "application/x-esri-shape"
		};
		if (this.check([
			255,
			79,
			255,
			81
		])) return {
			ext: "j2c",
			mime: "image/j2c"
		};
		if (this.check([
			0,
			0,
			0,
			12,
			106,
			80,
			32,
			32,
			13,
			10,
			135,
			10
		])) switch (await e.ignore(20), await e.readToken(new C(4, "ascii"))) {
			case "jp2 ": return {
				ext: "jp2",
				mime: "image/jp2"
			};
			case "jpx ": return {
				ext: "jpx",
				mime: "image/jpx"
			};
			case "jpm ": return {
				ext: "jpm",
				mime: "image/jpm"
			};
			case "mjp2": return {
				ext: "mj2",
				mime: "image/mj2"
			};
			default: return;
		}
		if (this.check([255, 10]) || this.check([
			0,
			0,
			0,
			12,
			74,
			88,
			76,
			32,
			13,
			10,
			135,
			10
		])) return {
			ext: "jxl",
			mime: "image/jxl"
		};
		if (this.check([254, 255])) return this.checkString("<?xml ", {
			offset: 2,
			encoding: "utf-16be"
		}) ? {
			ext: "xml",
			mime: "application/xml"
		} : void 0;
		if (this.check([
			208,
			207,
			17,
			224,
			161,
			177,
			26,
			225
		])) return {
			ext: "cfb",
			mime: "application/x-cfb"
		};
		if (await e.peekBuffer(this.buffer, {
			length: Math.min(256, e.fileInfo.size),
			mayBeLess: !0
		}), this.check([
			97,
			99,
			115,
			112
		], { offset: 36 })) return {
			ext: "icc",
			mime: "application/vnd.iccprofile"
		};
		if (this.checkString("**ACE", { offset: 7 }) && this.checkString("**", { offset: 12 })) return {
			ext: "ace",
			mime: "application/x-ace-compressed"
		};
		if (this.checkString("BEGIN:")) {
			if (this.checkString("VCARD", { offset: 6 })) return {
				ext: "vcf",
				mime: "text/vcard"
			};
			if (this.checkString("VCALENDAR", { offset: 6 })) return {
				ext: "ics",
				mime: "text/calendar"
			};
		}
		if (this.checkString("FUJIFILMCCD-RAW")) return {
			ext: "raf",
			mime: "image/x-fujifilm-raf"
		};
		if (this.checkString("Extended Module:")) return {
			ext: "xm",
			mime: "audio/x-xm"
		};
		if (this.checkString("Creative Voice File")) return {
			ext: "voc",
			mime: "audio/x-voc"
		};
		if (this.check([
			4,
			0,
			0,
			0
		]) && this.buffer.length >= 16) {
			let e = new DataView(this.buffer.buffer).getUint32(12, !0);
			if (e > 12 && this.buffer.length >= e + 16) try {
				let t = new TextDecoder().decode(this.buffer.subarray(16, e + 16));
				if (JSON.parse(t).files) return {
					ext: "asar",
					mime: "application/x-asar"
				};
			} catch {}
		}
		if (this.check([
			6,
			14,
			43,
			52,
			2,
			5,
			1,
			1,
			13,
			1,
			2,
			1,
			1,
			2
		])) return {
			ext: "mxf",
			mime: "application/mxf"
		};
		if (this.checkString("SCRM", { offset: 44 })) return {
			ext: "s3m",
			mime: "audio/x-s3m"
		};
		if (this.check([71]) && this.check([71], { offset: 188 }) || this.check([71], { offset: 4 }) && this.check([71], { offset: 196 })) return {
			ext: "mts",
			mime: "video/mp2t"
		};
		if (this.check([
			66,
			79,
			79,
			75,
			77,
			79,
			66,
			73
		], { offset: 60 })) return {
			ext: "mobi",
			mime: "application/x-mobipocket-ebook"
		};
		if (this.check([
			68,
			73,
			67,
			77
		], { offset: 128 })) return {
			ext: "dcm",
			mime: "application/dicom"
		};
		if (this.check([
			76,
			0,
			0,
			0,
			1,
			20,
			2,
			0,
			0,
			0,
			0,
			0,
			192,
			0,
			0,
			0,
			0,
			0,
			0,
			70
		])) return {
			ext: "lnk",
			mime: "application/x.ms.shortcut"
		};
		if (this.check([
			98,
			111,
			111,
			107,
			0,
			0,
			0,
			0,
			109,
			97,
			114,
			107,
			0,
			0,
			0,
			0
		])) return {
			ext: "alias",
			mime: "application/x.apple.alias"
		};
		if (this.checkString("Kaydara FBX Binary  \0")) return {
			ext: "fbx",
			mime: "application/x.autodesk.fbx"
		};
		if (this.check([76, 80], { offset: 34 }) && (this.check([
			0,
			0,
			1
		], { offset: 8 }) || this.check([
			1,
			0,
			2
		], { offset: 8 }) || this.check([
			2,
			0,
			2
		], { offset: 8 }))) return {
			ext: "eot",
			mime: "application/vnd.ms-fontobject"
		};
		if (this.check([
			6,
			6,
			237,
			245,
			216,
			29,
			70,
			229,
			189,
			49,
			239,
			231,
			254,
			116,
			183,
			29
		])) return {
			ext: "indd",
			mime: "application/x-indesign"
		};
		if (this.check([
			255,
			255,
			0,
			0,
			7,
			0,
			0,
			0,
			4,
			0,
			0,
			0,
			1,
			0,
			1,
			0
		]) || this.check([
			0,
			0,
			255,
			255,
			0,
			0,
			0,
			7,
			0,
			0,
			0,
			4,
			0,
			1,
			0,
			1
		])) return {
			ext: "jmp",
			mime: "application/x-jmp-data"
		};
		if (await e.peekBuffer(this.buffer, {
			length: Math.min(512, e.fileInfo.size),
			mayBeLess: !0
		}), this.checkString("ustar", { offset: 257 }) && (this.checkString("\0", { offset: 262 }) || this.checkString(" ", { offset: 262 })) || this.check([
			0,
			0,
			0,
			0,
			0,
			0
		], { offset: 257 }) && Me(this.buffer)) return {
			ext: "tar",
			mime: "application/x-tar"
		};
		if (this.check([255, 254])) {
			let e = "utf-16le";
			return this.checkString("<?xml ", {
				offset: 2,
				encoding: e
			}) ? {
				ext: "xml",
				mime: "application/xml"
			} : this.check([255, 14], { offset: 2 }) && this.checkString("SketchUp Model", {
				offset: 4,
				encoding: e
			}) ? {
				ext: "skp",
				mime: "application/vnd.sketchup.skp"
			} : this.checkString("Windows Registry Editor Version 5.00\r\n", {
				offset: 2,
				encoding: e
			}) ? {
				ext: "reg",
				mime: "application/x-ms-regedit"
			} : void 0;
		}
		if (this.checkString("-----BEGIN PGP MESSAGE-----")) return {
			ext: "pgp",
			mime: "application/pgp-encrypted"
		};
	};
	detectImprecise = async (e) => {
		this.buffer = new Uint8Array(N);
		let t = pt(e.fileInfo.size);
		if (await e.peekBuffer(this.buffer, {
			length: Math.min(8, t),
			mayBeLess: !0
		}), this.check([
			0,
			0,
			1,
			186
		]) || this.check([
			0,
			0,
			1,
			179
		])) return {
			ext: "mpg",
			mime: "video/mpeg"
		};
		if (this.check([
			0,
			1,
			0,
			0,
			0
		])) return {
			ext: "ttf",
			mime: "font/ttf"
		};
		if (this.check([
			0,
			0,
			1,
			0
		])) return {
			ext: "ico",
			mime: "image/x-icon"
		};
		if (this.check([
			0,
			0,
			2,
			0
		])) return {
			ext: "cur",
			mime: "image/x-icon"
		};
		if (await e.peekBuffer(this.buffer, {
			length: Math.min(2 + this.options.mpegOffsetTolerance, t),
			mayBeLess: !0
		}), this.buffer.length >= 2 + this.options.mpegOffsetTolerance) for (let e = 0; e <= this.options.mpegOffsetTolerance; ++e) {
			let t = this.scanMpeg(e);
			if (t) return t;
		}
	};
	async readTiffTag(e) {
		let t = await this.tokenizer.readToken(e ? b : y);
		switch (await this.tokenizer.ignore(10), t) {
			case 50341: return {
				ext: "arw",
				mime: "image/x-sony-arw"
			};
			case 50706: return {
				ext: "dng",
				mime: "image/x-adobe-dng"
			};
		}
	}
	async readTiffIFD(e) {
		let t = await this.tokenizer.readToken(e ? b : y);
		if (!(t > We) && !(Z(this.tokenizer) && 2 + t * 12 > qe)) for (let n = 0; n < t; ++n) {
			let t = await this.readTiffTag(e);
			if (t) return t;
		}
	}
	async readTiffHeader(e) {
		let t = {
			ext: "tif",
			mime: "image/tiff"
		}, n = (e ? b : y).get(this.buffer, 2), r = (e ? S : x).get(this.buffer, 4);
		if (n === 42) {
			if (r >= 6) {
				if (this.checkString("CR", { offset: 8 })) return {
					ext: "cr2",
					mime: "image/x-canon-cr2"
				};
				if (r >= 8) {
					let t = (e ? b : y).get(this.buffer, 8), n = (e ? b : y).get(this.buffer, 10);
					if (t === 28 && n === 254 || t === 31 && n === 11) return {
						ext: "nef",
						mime: "image/x-nikon-nef"
					};
				}
			}
			if (Z(this.tokenizer) && r > Ke) return t;
			let n = Z(this.tokenizer) ? qe : this.tokenizer.fileInfo.size;
			try {
				await q(this.tokenizer, r, {
					maximumLength: n,
					reason: "TIFF IFD offset"
				});
			} catch (e) {
				if (e instanceof o) return;
				throw e;
			}
			let i;
			try {
				i = await this.readTiffIFD(e);
			} catch (e) {
				if (e instanceof o) return;
				throw e;
			}
			return i ?? t;
		}
		if (n === 43) return t;
	}
	scanMpeg(e) {
		if (this.check([255, 224], {
			offset: e,
			mask: [255, 224]
		})) {
			if (this.check([16], {
				offset: e + 1,
				mask: [22]
			})) return this.check([8], {
				offset: e + 1,
				mask: [8]
			}), {
				ext: "aac",
				mime: "audio/aac"
			};
			if (this.check([2], {
				offset: e + 1,
				mask: [6]
			})) return {
				ext: "mp3",
				mime: "audio/mpeg"
			};
			if (this.check([4], {
				offset: e + 1,
				mask: [6]
			})) return {
				ext: "mp2",
				mime: "audio/mpeg"
			};
			if (this.check([6], {
				offset: e + 1,
				mask: [6]
			})) return {
				ext: "mp1",
				mime: "audio/mpeg"
			};
		}
	}
};
new Set(Pe), new Set(Fe);
//#endregion
export { ct as fileTypeFromBuffer };
