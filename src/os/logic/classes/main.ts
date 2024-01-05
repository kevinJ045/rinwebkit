import io from "@/src/logger/main";
import { os_exec } from "../functions/os_exec";


export const OS = new (class OS {
	async exec(command: string) : Promise<string> {
		return os_exec(command).then(r => r.trim());
	}
})();