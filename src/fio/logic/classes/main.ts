import { readFile } from "../functions/read";
import { writeFile } from "../functions/write";



export class FIO {

	static async readFile(filepath: string){
		return await readFile(filepath);
	}
	
	static async writeFile(filepath: string, content: string){
		return await writeFile(filepath, content);
	}

}