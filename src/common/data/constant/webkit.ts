import { webkitPolyfill } from "../../../common/data/constant/webkit.polyfill";

// @ts-ignore
if(typeof webkit == "undefined"){
	// @ts-ignore
	window.webkit = webkitPolyfill;
}

// @ts-ignore
const w = window.webkit;

export { w as webkit };