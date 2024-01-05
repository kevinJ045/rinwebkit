import { webkit } from "@/src/common/data/constant/webkit";

export function print_raw(string: string){
	// @ts-ignore
	webkit.messageHandlers.OS.postMessage('CONSOLE:'+ string);
}

export function print_raw_multi(strings: string[]){
	strings.forEach(string => print_raw(string));
}