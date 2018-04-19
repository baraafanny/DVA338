char* readfile(char* textfile)
{
	size_t size = 0;

	FILE *fp = fopen(textfile, "r");

	// Get the buffer size
	fseek(fp, 0, SEEK_END); // Go to end of file 
	size = ftell(fp); // Get bytes passed 

	// Set position of stream to the beginning 
	rewind(fp);
	
	/* Allocate the buffer (no need to initialize it with calloc) */
	char *buffer = (char*)malloc((size + 1) * sizeof(*buffer)); /* size + 1 byte for the \0 */

												   /* Read the file into the buffer */
	fread(buffer, size, 1, fp); /* Read 1 chunk of size bytes from fp into buffer */

								/* NULL-terminate the buffer */
	buffer[size] = '\0';

	/* Print it ! */
	printf("%s\n", buffer);

	return buffer;
}
static const char * vs_n2c_src2[] = { readfile("vs.txt") };

static const char * fs_ci_src2[] = { readfile("fs.txt") };
// Simple vertex shader treating vertex normals as RGB colors


static const char * vs_n2c_src[] = {
	"#version 420 core                                                 \n"
	"                                                                  \n"
	" in vec3 vPos;                                                    \n"
	" in vec3 vNorm;                                                   \n"
	" out vec4 color;                                                  \n"
	" uniform mat4 PV;                                                 \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"    color = abs(vec4(vNorm, 1.0));                                \n"
	"    gl_Position = PV * vec4(vPos, 1.0f);                          \n"
	"}                                                                 \n"
};

// Simple fragment shader for color interpolation
static const char * fs_ci_src[] = {
	"#version 420 core                                                 \n"
	"                                                                  \n"
	"in vec4 color;                                                    \n"
	"out vec4 fColor;                                                  \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"    fColor = color;                                               \n"
	"}                                                                 \n"
};


// The source code for additional shaders can be added here
// ...
