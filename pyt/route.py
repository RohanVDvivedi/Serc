#!/usr/bin/env python3

"""

command is used as below

python3 pyt/route.py <routing_file_0> <routing_file_1> <routing_file_2> <routing_file_3> ...
routing file must have a .con extension at the end,
routing file must contain be a json array

example. : 
[
	{
		"controller"	:	"first_controller",
		"methods"		:	["GET","POST"],
		"paths"			:	["/index/get_post"]
	},
	{
		"controller"	:	"first_controller",
		"methods"		:	["DELETE","POST"],
		"paths"			:	["/index/delete_post"]
	},
	{
		"controller"	:	"wild_controller",
		"methods"		:	["*"],
		"paths"			:	["/wild/*"]
	}
]

"""

# for parsing the routing.con file
import json
# for reading and writing files from disk
# and also accessing the command line variables passed
import sys
# to get path of the current file
import os

# gets the directory path where the script is stored
# the template file distributer_source.temp, must be stored in share folder right beside the directory of this script file
dir_path = os.path.dirname(os.path.realpath(__file__))

# same as getHashValue function in c in strhsh (it must be same as the implementation you find in c source code of serc)
# this has function will also be encountered in c (src/strhsh,h), which helps in routing request by hashing them
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

# the below function takes a template filename and code file name to edit
# it checks for each line in templateFile,
# if that line is found as key in the compareCodeHash, 
# them the value corresponding to the key in compareCodeHash, is pasted in the codeFile
# else it will copy paste the same line from templateFile to the codeFile
def replaceLineWithCode(templateFileName,codeFileName,compareCodeHash):
	global dir_path
	templatefile = open(dir_path + "/../share/" + templateFileName,"r");
	codefile = open(codeFileName,"w");

	for line in templatefile:
		test = line
		if test in compareCodeHash:
			codefile.write(compareCodeHash[line])
		else:
			codefile.write(line)

	templatefile.close();
	codefile.close();

# read all commandline args
command_line_args = sys.argv.copy()[1:]

# below is the list of http methods supported by the serc framework
supported_methods = ["GET", "POST", "PUT", "DELETE",
					"HEAD", "PATCH", "OPTIONS",
					"TRACE", "CONNECT"]

# all the routing are stored in a global dictionary
# hence as each routing file is used conflicts are resolved by
# simply using the latter file as the higher priority
mydict = {}

# this is the set containting the names of all the controller functions in the system
functions_declarations = set()

# loop throught all the routing files and collect all paths
for routing_file in command_line_args:

	# a routing configuration file must end with .con
	if not routing_file.endswith(".con") :
		continue

	# read the file that contains route configurations
	routing_config_file = open(routing_file, "r")

	# since the file is in json use json.loads to use it as object
	routes = json.loads(routing_config_file.read())

	routing_config_file.close()

	# use a dictionery to store the routes in format as required for making switch case
	# format is mydict[method][hashvalue_of_path][path_in_string] = controller
	# the below given loop builds up structure mydict for distributer.c and functions_declarations for controller.h
	for route in routes :

		# if routing methods is a string, and it is "*", it means the context is all the methods
		if isinstance(route['methods'], str) :
			if route['methods'] == "*" :
				route['methods'] = supported_methods
			else :
				route['methods'] = [route['methods']]
		elif isinstance(route['methods'], list) :
			if "*" in route['methods'] :
				route['methods'] = supported_methods

		for method in route['methods']:
			if not (method in mydict):
				mydict[method] = {}
			for path in route['paths']:

				path_route_hash = None

				if "*" in path :
					if not ("wild_card_paths" in mydict[method]):
						mydict[method]["wild_card_paths"] = {}
					if not (path in mydict[method]["wild_card_paths"]):
						mydict[method]["wild_card_paths"][path] = {}
						path_route_hash = mydict[method]["wild_card_paths"][path]
				else :
					hashval = getHashValue(path)
					if not (hashval in mydict[method]):
						mydict[method][hashval] = {}
					if not (path in mydict[method][hashval]):
						mydict[method][hashval][path] = {}
						path_route_hash = mydict[method][hashval][path]

				# this is the controller function that will be called if, 
				# the METHOD and PATH satisfy the condition, and they match in the request
				if 'controller' in route :
					controller = route['controller']
					path_route_hash['controller'] = controller
					# add the controller to the functions_declarations set
					functions_declarations.add(controller)

				# controller like methods that get called before and after the controller
				if 'before' in route :
					if isinstance(route['before'], str) :
						path_route_hash['before'] = [route['before']]
						functions_declarations.add(route['before'])
					elif isinstance(route['before'], list) :
						path_route_hash['before'] = route['before']
						functions_declarations.union(set(path_route_hash['before']))

				if 'after' in route :
					if isinstance(route['after'], str) :
						path_route_hash['after'] = [route['after']]
						functions_declarations.add(route['after'])
					elif isinstance(route['after'], list) :
						path_route_hash['after'] = route['after']
						functions_declarations.union(set(path_route_hash['after']))






# below for loop builds the switch case statements for routing the request to specified controller

"""
outer most routing part is switch by HttpMethodType
after that we route by the hash value of path
but note that any 2 strings can and may have same hash value
so for this we compare all the strings having the same hash value
"""
# create whole of case statement in case_string
case_string              			 = "\tswitch(hrq->method)"
case_string             			+= "\n\t{"
for method in mydict:
	case_string         			+= "\n\t\tcase " + method + " :"
	case_string         			+= "\n\t\t{"
	case_string         			+= "\n\t\t\tswitch(PATH_HASH)"
	case_string         			+= "\n\t\t\t{"
	for hashval in mydict[method]:
		# wild card paths will be handled by the default case
		if hashval == "wild_card_paths" :
			continue
		case_string     			+= "\n\t\t\t\tcase " + str(hashval) + " :"
		case_string     			+= "\n\t\t\t\t{"
		for path in mydict[method][hashval]:
			case_string 			+= "\n\t\t\t\t\t// case for path = " + path + " and supports method = " + method
			case_string 			+= "\n\t\t\t\t\tif( 0 == compare_dstring(&(hrq->path), &get_literal_cstring(\"" + path + "\")) )"
			case_string 			+= "\n\t\t\t\t\t{"
			if ('set_response_headers' in mydict[method][hashval][path]) and mydict[method][hashval][path]['set_response_headers'] is not None :
				case_string 		+= "\n\t\t\t\t\t\t// now here we add headers to the response, that we have to send"
				for header_key, header_value in mydict[method][hashval][path]['set_response_headers'].items() :
					case_string 	+= "\n\t\t\t\t\t\tinsert_unique_in_dmap_cstr(&(hrp->headers),\"" + header_key + "\", \"" + header_value + "\");"
			if ('before' in mydict[method][hashval][path]) and mydict[method][hashval][path]['before'] is not None :
				for before in mydict[method][hashval][path]['before'] :
					case_string 	+= "\n\t\t\t\t\t\tclose_connection |= " + before + "(hrq, hrp);"
			if ('controller' in mydict[method][hashval][path]) and mydict[method][hashval][path]['controller'] is not None :
				case_string 		+= "\n\t\t\t\t\t\trouting_resolved = 1;"
				case_string 		+= "\n\t\t\t\t\t\thrp->status = 200;"
				case_string 		+= "\n\t\t\t\t\t\tclose_connection |= " + mydict[method][hashval][path]['controller'] + "(hrq, hrp);"
			if ('after' in mydict[method][hashval][path]) and mydict[method][hashval][path]['after'] is not None :
				for after in mydict[method][hashval][path]['after'] :
					case_string 	+= "\n\t\t\t\t\t\tclose_connection |= " + after + "(hrq, hrp);"
			if ('redirect_to' in mydict[method][hashval][path]) and mydict[method][hashval][path]['redirect_to'] is not None :
				case_string 		+= "\n\t\t\t\t\t\trouting_resolved = 1;"
				status = -1
				if ('with_status' in mydict[method][hashval][path]['redirect_to']) :
					status = mydict[method][hashval][path]['redirect_to']['with_status']
				case_string 		+= "\n\t\t\t\t\t\tredirectTo(" + str(status) + ", \"" + mydict[method][hashval][path]['redirect_to']['url'] + "\", hrp);"
			case_string 			+= "\n\t\t\t\t\t}"
		case_string     			+= "\n\t\t\t\t\tbreak;"
		case_string     			+= "\n\t\t\t\t}"
	if ("wild_card_paths" in mydict[method]) and len(mydict[method]["wild_card_paths"]):
		case_string					+= "\n\t\t\t\tdefault : "
		case_string					+= "\n\t\t\t\t{"
		case_string					+= "\n\t\t\t\t\tunsigned int last_wild_card_at = 0;"
		first_wild_card = True
		for path in mydict[method]["wild_card_paths"]:
			case_string 			+= "\n\t\t\t\t\t// case for path = " + path + " and supports method = " + method
			path_parts = path.split("*")
			if first_wild_card == True :
				case_string			+= "\n\t\t\t\t\t"
				first_wild_card = False
			else :
				case_string 		+= "\n\t\t\t\t\telse "
			case_string				+= "if( (" + str(len("".join(path_parts))) + " < get_char_count_dstring(&(hrq->path)))"
			previous_path_part = None
			path_part_iter = 0
			for path_part in path_parts :
				if path_part != "" :
					if path_part_iter == 0 :
						from_str = "&(hrq->path)"
					else :
						from_str = "&get_literal_dstring(get_byte_array_dstring(&(hrq->path)) + last_wild_card_at + " + str(len(previous_path_part)) + ", get_char_count_dstring(&(hrq->path)) - last_wild_card_at - " + str(len(previous_path_part)) + " )"
						case_string += " && (last_wild_card_at + " + str(len(previous_path_part)) + " < get_char_count_dstring(&(hrq->path)))"
					case_string		+= " && ((last_wild_card_at = contains_dstring_RK(" + from_str + " , &get_literal_cstring(\"" + path_part + "\"))) != INVALID_INDEX)"
					path_part_iter += 1
					previous_path_part = path_part
			case_string				+= " )"
			case_string 			+= "\n\t\t\t\t\t{"
			if ('set_response_headers' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['set_response_headers'] is not None :
				case_string 		+= "\n\t\t\t\t\t\t// now here we add headers to the response, that we have to send"
				for header_key, header_value in mydict[method]["wild_card_paths"][path]['set_response_headers'].items() :
					case_string 	+= "\n\t\t\t\t\t\tinsert_unique_in_dmap_cstr(&(hrp->headers),\"" + header_key + "\", \"" + header_value + "\");"
			if ('before' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['before'] is not None :
				for before in mydict[method]["wild_card_paths"][path]['before'] :
					case_string 	+= "\n\t\t\t\t\t\tclose_connection |= " + before + "(hrq, hrp);"
			if ('controller' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['controller'] is not None :
				case_string 		+= "\n\t\t\t\t\t\trouting_resolved = 1;"
				case_string 		+= "\n\t\t\t\t\t\thrp->status = 200;"
				case_string 		+= "\n\t\t\t\t\t\tclose_connection |= " + mydict[method]["wild_card_paths"][path]['controller'] + "(hrq, hrp);"
			if ('after' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['after'] is not None :
				for after in mydict[method]["wild_card_paths"][path]['after'] :
					case_string 	+= "\n\t\t\t\t\t\tclose_connection |= " + after + "(hrq, hrp);"
			if ('redirect_to' in mydict[method]["wild_card_paths"][path]) and mydict[method]["wild_card_paths"][path]['redirect_to'] is not None :
				case_string 		+= "\n\t\t\t\t\t\trouting_resolved = 1;"
				status = -1
				if ('with_status' in mydict[method]["wild_card_paths"][path]['redirect_to']) :
					status = mydict[method]["wild_card_paths"][path]['redirect_to']['with_status']
				case_string 		+= "\n\t\t\t\t\t\tredirectTo(" + str(status) + ", \"" + mydict[method]["wild_card_paths"][path]['redirect_to']['url'] + "\", hrp);"
			case_string 			+= "\n\t\t\t\t\t}"
		case_string					+= "\n\t\t\t\t\telse"
		case_string					+= "\n\t\t\t\t\t{"
		case_string					+= "\n\t\t\t\t\t\thrp->status = 404;"
		case_string					+= "\n\t\t\t\t\t}"
		case_string					+= "\n\t\t\t\t\tbreak;"
		case_string					+= "\n\t\t\t\t}"
	case_string         			+= "\n\t\t\t}"
	case_string         			+= "\n\t\t\tbreak;"
	case_string         			+= "\n\t\t}"
case_string							+= "\n\t\tdefault :"
case_string							+= "\n\t\t{"
#case_string							+= "\n\t\t\thrp->status = 404;"
case_string							+= "\n\t\t}"
case_string             			+= "\n\t}\n"
		

# below for loop builds forward declations for all the controllers that you will be using in your application
declarations = ""
for function_name in functions_declarations :
	declarations += "\nint " + function_name + "(HttpRequest* hrq, stream* raw_strm, const void* server_params);"





replaceLineWithCode(
						"distributer_source.temp",
						"distributer.c",
						{
							"//@controller_definitions\n":declarations,
							"//@switch_case\n":case_string
						}
					)
