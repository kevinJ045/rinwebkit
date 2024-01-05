


export function base64Decode(base64: string){
	return atob(base64);
}

export function base64Encode(base64: string){
	return btoa(base64);
}