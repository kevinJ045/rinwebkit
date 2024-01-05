

export class FileIOEventTarget {

	onchange: (e: string) => void = (e: string) => {};

	change(e: string){
		this.onchange(e);
	}

}