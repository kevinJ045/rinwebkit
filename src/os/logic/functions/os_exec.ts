import io from "../../../logger/main";
import { CommandExecutionEventTarget } from "../classes/events";
import { webkit } from "@/src/common/data/constant/webkit";

const output_promises: Record<string, CommandExecutionEventTarget> = {};

// @ts-ignore
window.handleCommandExecutions = function(timestamp: string, output: string) {
	if (output_promises[timestamp]) {
		output_promises[timestamp].change!(output);
		delete output_promises[timestamp];
	}
};

let commands = 0;
export function os_exec(string: string): Promise<string> {
	const timestamp = Date.now().toString()+commands.toString();

	output_promises[timestamp] = new CommandExecutionEventTarget();

	const promise = new Promise((resolve, reject) => {
		output_promises[timestamp].onchange = (e: string) => {
			resolve(e);
		}
	});

	// @ts-ignore
	webkit.messageHandlers.OS.postMessage(`EXEC:${timestamp},${string}`);
	commands++;
	return promise as Promise<string>;
}
