
import { FileIOEventTarget } from "../classes/events";

export const file_output_promises: Record<string, { type: string, event: FileIOEventTarget }> = {};

// @ts-ignore
window.handleFileEvent = function(type: 'read' | 'write', timestamp: string, output: string) {
	if (file_output_promises[timestamp]) {
		file_output_promises[timestamp].event.change(output);
		delete file_output_promises[timestamp];
	}
};