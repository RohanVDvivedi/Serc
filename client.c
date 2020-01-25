#include<stdio.h>
#include<stdlib.h>

#include<http_client.h>

int main()
{
	transaction_client* http_client = get_http_client("localhost", "6900", 1);

	if(http_client == NULL)
	{
		printf("Could not find server or setup connection\n");
		return 0;
	}

	HttpRequest* hrq = getNewHttpRequest();
	//hrq->method = GET;
	hrq->method = POST;
	//append_to_dstring(hrq->path, "/api");
	//append_to_dstring(hrq->path, "/api/project");
	append_to_dstring(hrq->path, "/index/get_post");
	addParameter("projectName", "project-name", hrq->parameters);
	addParameter("projectId", "project-id", hrq->parameters);

	append_to_dstring(hrq->body, "{\"Name\":\"project-name\",\"ProjectField\":\"the field : embedded, fpga, electronics, backend, frontend, system programming etc\",\"ProjectDescriptionShort\":\"project description short\",\"ProjectDescriptionLong\":\"project description at length, this will be shown on the inner when the user opens the project\",\"ProjectImage\":\"/img/pcb.jpeg\",\"GithubLink\":\"https://github.com/RohanVDvivedi/rohandvivedi.com\",\"YoutubeLink\":\"https://www.youtube.com/watch?v=fjIN9XPqp_A\",\"Explanation\":[{\"ExplanationText\":\"this must be a valid html consisting only of inline contents\",\"ExplanationImage\":\"link to the image that we show next to the given paragraph\"},{\"ExplanationText\":\"this must be a valid html consisting only of inline contents\",\"ExplanationImage\":\"link to the image that we show next to the given paragraph\"}],\"SetupInstructions\":[{\"InstructionType\":\"to be followed or terminal instruction\",\"InstructionStatement\":\"The statement of the instruction or the command to write in terminal\",\"InstructionReason\":\"this is the reason, or the ouput of performing the command\",\"InstructionImage\":\"there can be an instruction inage showning how to assemble the product\"},{\"InstructionType\":\"to be followed or terminal instruction\",\"InstructionStatement\":\"The statement of the instruction or the command to write in terminal\",\"InstructionReason\":\"this is the reason, or the ouput of performing the command\",\"InstructionImage\":\"there can be an instruction inage showning how to assemble the product\"}],\"RunInstruction\":[{\"InstructionType\":\"to be followed or terminal instruction\",\"InstructionStatement\":\"The statement of the instruction or the command to write in terminal\",\"InstructionReason\":\"this is the reason, or the ouput of performing the command\",\"InstructionImage\":\"there can be an instruction inage showning how to assemble the product\"},{\"InstructionType\":\"to be followed or terminal instruction\",\"InstructionStatement\":\"The statement of the instruction or the command to write in terminal\",\"InstructionReason\":\"this is the reason, or the ouput of performing the command\",\"InstructionImage\":\"there can be an instruction inage showning how to assemble the product\"}],\"UsageInstruction\":[{\"InstructionType\":\"to be followed or terminal instruction\",\"InstructionStatement\":\"The statement of the instruction or the command to write in terminal\",\"InstructionReason\":\"this is the reason, or the ouput of performing the command\",\"InstructionImage\":\"there can be an instruction inage showning how to assemble the product\"},{\"InstructionType\":\"to be followed or terminal instruction\",\"InstructionStatement\":\"The statement of the instruction or the command to write in terminal\",\"InstructionReason\":\"this is the reason, or the ouput of performing the command\",\"InstructionImage\":\"there can be an instruction inage showning how to assemble the product\"}]}");

	job* promise = send_request_async(http_client, hrq, "rohandvivedi.com");

	HttpResponse* hrp = wait_or_get_response(promise, NULL);

	if(hrp == NULL)
	{
		printf("Response : NULL\n\n");
	}
	else
	{
		printResponse(hrp);
		deleteHttpResponse(hrp);
	}

	deleteHttpRequest(hrq);

	shutdown_and_delete_http_client(http_client);
	return 0;
}