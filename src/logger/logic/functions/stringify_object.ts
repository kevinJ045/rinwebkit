

export function stringify_object(item: any, indentation: string = "") : string {
	if(typeof item == "object"){
		if(Array.isArray(item)){
			return item.length == 0 ? indentation+'Array[0]' : indentation+'[\n'+item.map((item: any, index: number) => indentation+index+': '+stringify_object(item, indentation+" ")).join('\n')+'\n'+indentation+']';
		} else {
			let key_value_string = indentation+"{\n";
			for(let key in item){
				const value = item[key];
				key_value_string += `${indentation}${key}: ${stringify_object(value, indentation+" ")}\n`;
			}
			key_value_string += indentation+"}";
			return key_value_string;
		}
	} else {
		return (indentation)+item.toString() as string;
	}
}