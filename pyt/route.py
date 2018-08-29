import json;

# same as getHashValue function in c in strhsh
"""
def getHashValue(s):
	ans = 0;i = 1;last = 0;curr = 0;diff = 0;lastoccur = [0] * 128;iter_i = 0
	while iter_i < len(s) :
		curr = ( ord(s[iter_i]) & 0x7f )
		if( i == 1 ):
			diff = 1
		else:
			diff = abs( last - curr ) + 1
		delta = curr * i * diff * (i - lastoccur[curr])
		ans = ans + delta;
		iter_i+=1
		last = curr
		lastoccur[curr] = i
		i+=1
		if i%5 == 0 :
			ans = ans + ((ans%13)*(diff+2)*(diff%3)*(int(diff/7))) + ((ans%29)*last) + ((ans%37)*curr) + (ans%11)
	return ans
"""

def getHashValue(s):
	return len(s)


routing_config_file = open("../con/routing.con","r")

routes = json.loads(routing_config_file.read())

mydict = {}
for route in routes :
	for method in route['methods']:
		if not (method in mydict):
			mydict[method] = {}
		for path in route['paths']:
			hashval = getHashValue(path)
			if not (hashval in mydict[method]):
				mydict[method][hashval] = {}
			mydict[method][hashval][path] = route['controller']



case_string          = "\tswitch(METHOD)"
case_string         += "\n\t{"
for method in mydict:
	case_string     += "\n\t\tcase " + method + " :"
	case_string     += "\n\t\t{"
	case_string     += "\n\t\t\tswitch(PATH)"
	case_string     += "\n\t\t\t{"
	for hashval in mydict[method]:
		case_string += "\n\t\t\t\t// case for path = "
		case_string += "\n\t\t\t\tcase " + str(hashval) + " :"
		case_string += "\n\t\t\t\t{"
		for path in mydict[method][hashval]:
			case_string += "\n\t\t\t\t\tif( strcmp( path_str , \"" + path + "\" ) )"
			case_string += "\n\t\t\t\t\t{"
			case_string += "\n\t\t\t\t\t\tint error = " + mydict[method][hashval][path] + "(hrq,hrp);"
			case_string += "\n\t\t\t\t\t\tint routing_resolved = 1;"
			case_string += "\n\t\t\t\t\t}"
		case_string += "\n\t\t\t\t}"
	case_string     += "\n\t\t\t}"
	case_string     += "\n\t\t}"
case_string += "\n\t}\n"
		
print (case_string)