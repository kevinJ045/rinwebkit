import { print_raw_multi } from "../functions/print_raw";
import { stringify_object } from "../functions/stringify_object";


export const io = new (class IO {
	print_raw(...args: string[]){
		print_raw_multi(args);
	}
	printw(item: any){
		io.print_raw(stringify_object(item));
	}
	print(...args: any[]){
		args.forEach(arg => io.printw(arg));
	}
});