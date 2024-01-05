import { file_output_promises } from "./handler";
import { webkit } from "../../../common/data/constant/webkit";
import { FileIOEventTarget } from "../classes/events";
import { File } from "../../data/models/file";
import { readFile } from "./read";
import { base64Encode } from "./base64";


export function writeFile(filepath: string, content: string) : Promise<File> {
	const timestamp = Date.now().toString()+filepath.replace(/[\W]/g, '_').substring(0, 12);

	const event = new FileIOEventTarget();

	const action = {
		type: "write",
		event
	}

	file_output_promises[timestamp] = action;

	// @ts-ignore
	webkit.messageHandlers.OS.postMessage(`FWRITE:${timestamp},${filepath}\n${content}`);
	
	return new Promise((resolve) => {
		event.onchange = async (e: string) => {
			let file = await readFile(filepath);
			file.content = content;
			file.base64 = base64Encode(content);
			resolve(file);
		};
	});
}