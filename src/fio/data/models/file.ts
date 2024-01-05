

export class File {
	name: string = "";
	fullpath: string = "";
	content: string = "";

	base64: string = "";

	constructor(config : { name?: string, content?: string, base64?: string} = {}){
		this.set(config);
	}

	set({ name, content, base64 } : { name?: string, content?: string, base64?: string} = {}){
		if(name) this.name = name;
		if(content) this.content = content;
		if(base64) this.base64 = base64;
	}
	
}