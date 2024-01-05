

export class CommandExecutionEventTarget {

	onchange: (e: string) => void = (e: string) => {};

	change(e: string){
		this.onchange(e);
	}

}