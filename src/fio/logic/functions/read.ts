import { file_output_promises } from "./handler";
import { webkit } from "../../../common/data/constant/webkit";
import { FileIOEventTarget } from "../classes/events";
import { base64Decode } from "./base64";
import { basename } from "./basename";
import { File } from "../../data/models/file";


export function readFile(filepath: string) : Promise<File> {
	const timestamp = Date.now().toString()+filepath.replace(/[\W]/g, '_').substring(0, 12);

	const event = new FileIOEventTarget();

	const action = {
		type: "read",
		event
	}

	file_output_promises[timestamp] = action;

	// @ts-ignore
	webkit.messageHandlers.OS.postMessage(`FOPEN:${timestamp},${filepath}`);
	
	return new Promise((resolve) => {
		event.onchange = (e: string) => {
			resolve(new File({
				name: basename(filepath),
				content: base64Decode(e),
				base64: e,
			}));
		};
	});
}