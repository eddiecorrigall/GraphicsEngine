#include "MD2.hpp"
#include "Process.hpp"
#include "Misc.hpp"

#include <GL/gl.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cassert>

using namespace std;

/* File format:
 * 
	[MD2 Header]
	[MD2 Data
		[Skin Data]                 @skinOffset
		[Texture Coordinates]       @textureCoordinateOffset
		[Triangles]                 @triangleOffset
		[GL Commands]               @graphicCommandOffset
		[Keyframes]                 @keyframeOffset
	]                               @endOfFileOffset
 */

void MD2Model::LoadModel(const string& md2_path) {
	
	fstream md2_file(md2_path.c_str(), fstream::in | fstream::binary);

	if (md2_file.good() == false) {
		cerr << "ERROR: MD2 file failed to load!" << endl;
		engine.Stop();
		return;
	}
	
	// Get file size in bytes...

	unsigned int md2_file_size;

	md2_file.seekg(0, md2_file.end);
	md2_file_size = md2_file.tellg();

	cout << "File Size " << md2_file_size << endl;

	md2_file.seekg(0, md2_file.beg); // Reset file handle
	md2_file.clear(); // Reset file flags

	// Read header...

	unsigned int md2_header_size = sizeof(md2::Header);

	if (md2_file_size < md2_header_size) {
		cerr << "ERROR: File is smaller than header size!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	md2_file.read((char*)&header, md2_header_size);
	
	// Validate header...

	cout << "Identity " << header.identity << endl;

	if (string(header.identity) == MD2_IDENTITY) {
		cerr << "ERROR: Invalid header identity!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	cout << "Version " << header.version << endl;
	
	if (header.version != MD2_VERSION) {
		cerr << "ERROR: Invalid header version!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	cout << "Number of Skins " << header.numberOfSkins << endl;
	
	if (MD2_MAX_SKINS < header.numberOfSkins) {
		cerr << "ERROR: Number of skins exceeds maximum!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	cout << "Number of Vertices " << header.numberOfVertices << endl;
	
	if (MD2_MAX_VERTICES < header.numberOfVertices) {
		cerr << "ERROR: Number of vertices exceeds maximum!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	cout << "Number of Texture Coordinates " << header.numberOfTextureCoordinates << endl;
	
	if (MD2_MAX_TEXTURE_COORDINATES < header.numberOfTextureCoordinates) {
		cerr << "ERROR: Number of texture coordinates exceeds maximum!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	cout << "Number of Triangles " << header.numberOfTriangles << endl;
	
	if (MD2_MAX_TRIANGLES < header.numberOfTriangles) {
		cerr << "ERROR: Number of triangles exceeds maximum!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	cout << "Number of Frames " << header.numberOfFrames << endl;
	
	if (MD2_MAX_FRAMES < header.numberOfFrames) {
		cerr << "ERROR: Number of frames exceeds maximum!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}
	
	// Read texture coordinates...

	texture_coordinates = new md2::TextureCoordinate[header.numberOfTextureCoordinates];

	if (texture_coordinates == NULL) {
		cerr << "ERROR: Failed to allocate memory for texture coordinates!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	md2_file.seekg(header.textureCoordinateOffset, md2_file.beg);
	md2_file.read((char*)texture_coordinates, header.numberOfTextureCoordinates * sizeof(md2::TextureCoordinate));

	// Read triangles...

	triangles = new md2::Triangle[header.numberOfTriangles];

	if (triangles == NULL) {
		cerr << "ERROR: Failed to allocate memory for triangles!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	md2_file.seekg(header.triangleOffset, md2_file.beg);
	md2_file.read((char*)triangles, header.numberOfTriangles * sizeof(md2::Triangle));

	// Load frame data...

	frames = new md2::Frame[header.numberOfFrames];

	if (frames == NULL) {
		cerr << "ERROR: Failed to allocate memory for frames!" << endl;
		md2_file.close();
		engine.Stop();
		return;
	}

	md2_file.seekg(header.frameOffset, md2_file.beg);

	for (int frame_index = 0; frame_index < header.numberOfFrames; frame_index++) {

		md2_file.read((char*)&(frames[frame_index].scale),      sizeof_member(md2::Frame, scale));
		md2_file.read((char*)&(frames[frame_index].translate),  sizeof_member(md2::Frame, translate));
		md2_file.read((char*)&(frames[frame_index].name),       sizeof_member(md2::Frame, name));
		
		// Add an entry for the frame in the frame index...
		
		string frame_name = string(frames[frame_index].name);
		
		frame_lookup[string(frame_name)] = frame_index;
		
		cout << "Frame Index " << frame_index << " Name " << frame_name << endl;
		
		// Load frame vertices...
		
		frames[frame_index].vertices = new md2::Vertex[header.numberOfVertices];
		
		if (frames[frame_index].vertices == NULL) {
			cerr << "ERROR: Failed to allocate memory for vertices!" << endl;
			md2_file.close();
			engine.Stop();
			return;
		}

		md2_file.read((char*)frames[frame_index].vertices, header.numberOfVertices * sizeof(md2::Vertex));
	}
	
	md2_file.close();
}

void MD2Model::UnloadModel() {
	
	if (texture_coordinates != NULL) {
		delete texture_coordinates;
		texture_coordinates = NULL;
	}

	if (triangles != NULL) {
		delete triangles;
		triangles = NULL;
	}
	
	if (frames != NULL) {

		for (unsigned int i = 0; i < header.numberOfFrames; i++) {

			if (frames[i].vertices != NULL) {
				delete frames[i].vertices;
				frames[i].vertices = NULL;
			}
		}

		delete [] frames;
		frames = NULL;
	}
}

void MD2Model::LoadTexture(const string& skin_path) {
	
	unsigned int skin_width;
	unsigned int skin_height;
	unsigned char skin_bytes_per_pixel;
	
	skin_texture = SystemInstance<Video>()->LoadTexture(
		skin_path,
		skin_width,
		skin_height,
		skin_bytes_per_pixel,
		TextureFilter_Toon
	);
	
	if (skin_texture < 0) {
		cerr << "ERROR: Failed to load skin texture!" << endl;
		engine.Stop();
		return;
	}

	if (header.skinWidth != skin_width) {
		cerr << "ERROR: Image width does not match header! " << skin_width << endl;
		engine.Stop();
		return;
	}

	if (header.skinHeight != skin_height) {
		cerr << "ERROR: Image height does not match header! " << skin_height << endl;
		engine.Stop();
		return;
	}
}

void MD2Model::UnloadTexture() {
	
	SystemInstance<Video>()->UnloadTexture(skin_texture);
}

MD2Model::MD2Model(const AnimationInfo& animation_info) : AnimationModel(animation_info),
		texture_coordinates(NULL),
		triangles(NULL),
		frames(NULL),
		skin_texture(0) {
	
	cout << "Loading MD2 Model..." << endl;
	
	cout << "MD2 path " << GetModelPath() << endl;
	LoadModel(GetModelPath());
	
	cout << "Skin path " << GetTexturePath() << endl;
	LoadTexture(GetTexturePath());
}

MD2Model::MD2Model(const string& act_path) : AnimationModel(act_path) {
	MD2Model(GetAnimationInfo());
}

MD2Model::~MD2Model() {
	
	UnloadModel();
	UnloadTexture();
}

int MD2Model::FindFrameIndex(string const& frame_name) {

	/* // ##### Slow Alternative Method ##### //
	for (unsigned int i = 0; i < m_header.numberOfFrames; i++) {
		if (std::string(m_frames[i].name) == frame_name) {
			return i;
		}
	}
	return -1;
	*/
	
	map<string, int>::iterator results = frame_lookup.find(frame_name);
	
	int frame_index = -1; // Assume entry cannot be found
	
	if(results != frame_lookup.end()) { // Entry found!
		frame_index = results->second;
	}
	
	return frame_index;
}

void MD2Model::Render(Animation* object) {
	
	Video* gfx = SystemInstance<Video>();
	
	const glm::vec4 light_position = object->GetLightPosition();
	const glm::vec4 view_positon = object->GetViewPosition();
	
	const unsigned int current_frame_index = object->GetCurrentFrameIndex();
	const unsigned int next_frame_index = object->GetNextFrameIndex();
	
	const float frame_interp = object->GetFrameInterp();
	
	/* Note: frame_interpolation...
	 * - Range of 0.0f and 1.0f
	 * - Used to calculate an interpolated frame between the current frame and the next
	 * - Increased over time
	 */
	
	//cout << "Frame info " << current_frame_index << " " << next_frame_index << endl;
	
	assert(current_frame_index < header.numberOfFrames);
	assert(next_frame_index < header.numberOfFrames);
	
	// Rending buffers - Filled and then passed as arrays to OpenGL
	
	vector<float> texture_coordinate_buffer;
	vector<float> colour_buffer;
	vector<float> normal_buffer;
	vector<float> vertex_buffer;
	
	// Loop variables
	
	short k; // Temporary index
	
	glm::vec3 N, M; // Temporary normals
	glm::vec3 u, v; // Temporary vectors
	
	glm::vec2 triangle_texture_coordinates[3];
	
	glm::vec3 triangle_normals[3];
	glm::vec3 triangle_vertices[3];
	glm::vec3 triangle_colours[3];
	
	md2::Frame current_frame    = frames[current_frame_index];
	md2::Frame next_frame       = frames[next_frame_index];
	
	// ...
	
	if (gfx->IsDebuggingVectors()) {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
	}
	
	for (int i = 0; i < header.numberOfTriangles; i++) { // For each triangle...
		for (unsigned char j = 0; j < 3; j++) { // For each triangle vertex...

			// Select vertex...

			k = triangles[i].vertexIndices[j];

			if (header.numberOfVertices <= k || k < 0) {
				cerr << "ERROR: Invalid vertex index! " << k << endl;
				continue;
			}

			md2::Vertex current_vertex  = current_frame.vertices[k];
			md2::Vertex next_vertex     = next_frame.vertices[k];
			
			// ##### TEXTURE COORDINATES ##### //
			
			/* Note:
			 * 1. Scale to between 0.0 and 1.0 by dividing both the s and t components by skinWidth and skinHeight respectively
			 * 2. (FALSE) The t coordinate is flipped so it must be converted via 1.0 - t
			 */
			
			k = triangles[i].textureCoordinateIndices[j];

			if (header.numberOfTextureCoordinates <= k || k < 0) {
				cerr << "ERROR: Invalid texture coordinate index! " << k << endl;
				continue;
			}

			md2::TextureCoordinate texture_coordinate = texture_coordinates[k];

			float s = ((float)texture_coordinate.s / (float)header.skinWidth);
			float t = ((float)texture_coordinate.t / (float)header.skinHeight);
			
			triangle_texture_coordinates[j] = glm::vec2(s, t);
			
			// ##### INTERPOLATE NORMALS ##### //
			
			k = current_vertex.normalIndex;
			
			if (MD2_MAX_NORMALS <= k || k < 0) {
				cerr << "ERROR: Invalid normal index! " << k << endl;
				continue;
			}

			N[X] = md2::NORMALS[k][X];
			N[Y] = md2::NORMALS[k][Y];
			N[Z] = md2::NORMALS[k][Z];

			if (gfx->IsInterpolationEnabled()) {
				
				// Linear interpolation...
				
				/* Note:
				 * Calculate a new vertex position using:
				 * - current_frame
				 * - next_frame
				 * - frame_interp
				 * Possible for higher orders of interpolation!
				 * See Animation::GetFrameIndex
				 */
				
				k = next_vertex.normalIndex;

				if (MD2_MAX_NORMALS <= k || k < 0) {
					cerr << "ERROR: Invalid normal index! " << k << endl;
					continue;
				}

				// Lookup the next vertex normal...
				M[X] = md2::NORMALS[k][X];
				M[Y] = md2::NORMALS[k][Y];
				M[Z] = md2::NORMALS[k][Z];

				// Linearly interpolate over n and m...
				N += frame_interp * (M - N);
			}

			triangle_normals[j] = N;
			
			// ##### VERTICES ##### //
			
			/* Note:
			 * - Vertices in Quake 2 have the z and y axes are swapped
			 * - MD2 models need components swapped for correct orientation
			 */
			
			// Decompress current vertex...
			u[X] = (current_frame.scale[X] * current_vertex.components[X] + current_frame.translate[X]);
			u[Y] = (current_frame.scale[Z] * current_vertex.components[Z] + current_frame.translate[Z]);
			u[Z] = (current_frame.scale[Y] * current_vertex.components[Y] + current_frame.translate[Y]);
			
			if (gfx->IsInterpolationEnabled()) {
				
				// Linear Interpolation...

				// Decompress next vertex...
				v[X] = (next_frame.scale[X] * next_vertex.components[X] + next_frame.translate[X]);
				v[Y] = (next_frame.scale[Z] * next_vertex.components[Z] + next_frame.translate[Z]);
				v[Z] = (next_frame.scale[Y] * next_vertex.components[Y] + next_frame.translate[Y]);
				
				// Linearly interpolate over u and v...
				u += frame_interp * (v - u);
			}
			
			triangle_vertices[j] = u;
			
			// ##### COLOURS ##### //
			
			glm::vec4 L = glm::normalize(light_position - glm::vec4(u, 1.0f)); // Vector
			glm::vec4 V = glm::normalize(view_positon - glm::vec4(u, 1.0f)); // Vector
			
			triangle_colours[j] = glm::vec3(1.0f, 1.0f, 1.0f);
			
			if (gfx->IsCelshadingEnabled()) { // Phong lighting?
			
				// Note: Cel shading is calculated using Quake 2 normals!
				float intensity = glm::dot(L, glm::vec4(N, 0.0f));
				
				//glm::vec3 shadow = glm::log2(1.0f + intensity) * glm::vec3(1.0f, 1.0f, 1.0f);
				
				glm::vec3 shadow = intensity * glm::vec3(1.0f, 1.0f, 1.0f);
				
				triangle_colours[j] = shadow;
			}
			
			// ##### DEBUGGING ##### //
			
			#define ARROW_LENGTH (3.0f)
			
			if (gfx->IsDebuggingLighting()) {
				
				// Debug light direction...
				// Draw a line gradient from the surface (black) to the light position (white)...
				
				const glm::vec4 source = glm::vec4(u, 1.0f);
				const glm::vec4 destination = source + ARROW_LENGTH*L;
				
				glDisable(GL_LIGHTING);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(3.0f);

				glBegin(GL_LINES);

				glColor3f(0.0f, 0.0f, 0.0f); // Black at source
				glVertex3fv(glm::value_ptr(source));

				glColor3f(1.0f, 1.0f, 1.0f); // White at destination
				glVertex3fv(glm::value_ptr(destination));

				glEnd();
			}
			
			if (gfx->IsDebuggingView()) {
				
				// Debug view direction...
				// Draw a line gradient from the surface (black)
				// perpendicular (so it is visible) to the camera position (violet)...
				
				const glm::mat4 R = glm::rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
				
				const glm::vec4 source = glm::vec4(u, 1.0f);
				const glm::vec4 destination = source + ARROW_LENGTH*R*V;
				
				glDisable(GL_LIGHTING);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(3.0f); 

				glBegin(GL_LINES);

				glColor3f(0.0f, 0.0f, 0.0f); // Black at source
				glVertex3fv(glm::value_ptr(source));

				glColor3f(1.0f, 0.0f, 1.0f); // Violet at destination
				glVertex3fv(glm::value_ptr(destination));

				glEnd();
			}
			
			if (gfx->IsDebuggingNormals()) {
				
				// Debug normals...
				// Draw a line gradient from the surface (black) along the surface normal (green)...
				
				const glm::vec3 source = u;
				const glm::vec3 destination = source + ARROW_LENGTH*N;
				
				glDisable(GL_LIGHTING);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(3.0f); 

				glBegin(GL_LINES);

				glColor3f(0.0f, 0.0f, 0.0f); // Black at source
				glVertex3fv(glm::value_ptr(source));

				glColor3f(0.0f, 1.0f, 0.0f); // Green at destination
				glVertex3fv(glm::value_ptr(destination));

				glEnd();
			}
		}
		
		// ##### Subdivision ##### //
		
		int subdivide_depth = gfx->IsSubdivisionEnabled() ? 2 : 0;
		
		Subdivide2D( // Subdivide texture coordinates
			subdivide_depth,
			texture_coordinate_buffer,
			triangle_texture_coordinates[0],
			triangle_texture_coordinates[1],
			triangle_texture_coordinates[2]
		);
		
		Subdivide3D( // Subdivide colour coordinates
			subdivide_depth,
			colour_buffer,
			triangle_colours[0],
			triangle_colours[1],
			triangle_colours[2]
		);
		
		Subdivide3D( // Subdivide normals
			subdivide_depth,
			normal_buffer,
			triangle_normals[0],
			triangle_normals[1],
			triangle_normals[2]
		);
		
		Subdivide3D( // Subdivide vertices
			subdivide_depth,
			vertex_buffer,
			triangle_vertices[0],
			triangle_vertices[1],
			triangle_vertices[2]
		);
	}
	
	if (gfx->IsDebuggingVectors()) {
		glPopAttrib();
	}
	
	// ##### RENDERING ##### //
	// Faster drawing using buffered arrays
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skin_texture);
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glTexCoordPointer(2, GL_FLOAT, 0, &texture_coordinate_buffer[0]);
	glColorPointer(3, GL_FLOAT, 0, &colour_buffer[0]);
	glNormalPointer(GL_FLOAT, 0, &normal_buffer[0]); // Use Quake 2 normals
	//glNormalPointer(GL_FLOAT, 0, &vertex_buffer[0]);
	glVertexPointer(3, GL_FLOAT, 0, &vertex_buffer[0]);
	
	glDrawArrays(GL_TRIANGLES, 0, vertex_buffer.size() / 3);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_TEXTURE_2D);
	
	glPopAttrib();
}

const float md2::NORMALS[MD2_MAX_NORMALS][3] = {
	{ -0.525731f,  0.000000f,  0.850651f },
	{ -0.442863f,  0.238856f,  0.864188f },
	{ -0.295242f,  0.000000f,  0.955423f },
	{ -0.309017f,  0.500000f,  0.809017f },
	{ -0.162460f,  0.262866f,  0.951056f },
	{  0.000000f,  0.000000f,  1.000000f },
	{  0.000000f,  0.850651f,  0.525731f },
	{ -0.147621f,  0.716567f,  0.681718f },
	{  0.147621f,  0.716567f,  0.681718f },
	{  0.000000f,  0.525731f,  0.850651f },
	{  0.309017f,  0.500000f,  0.809017f },
	{  0.525731f,  0.000000f,  0.850651f },
	{  0.295242f,  0.000000f,  0.955423f },
	{  0.442863f,  0.238856f,  0.864188f },
	{  0.162460f,  0.262866f,  0.951056f },
	{ -0.681718f,  0.147621f,  0.716567f },
	{ -0.809017f,  0.309017f,  0.500000f },
	{ -0.587785f,  0.425325f,  0.688191f },
	{ -0.850651f,  0.525731f,  0.000000f },
	{ -0.864188f,  0.442863f,  0.238856f },
	{ -0.716567f,  0.681718f,  0.147621f },
	{ -0.688191f,  0.587785f,  0.425325f },
	{ -0.500000f,  0.809017f,  0.309017f },
	{ -0.238856f,  0.864188f,  0.442863f },
	{ -0.425325f,  0.688191f,  0.587785f },
	{ -0.716567f,  0.681718f, -0.147621f },
	{ -0.500000f,  0.809017f, -0.309017f },
	{ -0.525731f,  0.850651f,  0.000000f },
	{  0.000000f,  0.850651f, -0.525731f },
	{ -0.238856f,  0.864188f, -0.442863f },
	{  0.000000f,  0.955423f, -0.295242f },
	{ -0.262866f,  0.951056f, -0.162460f },
	{  0.000000f,  1.000000f,  0.000000f },
	{  0.000000f,  0.955423f,  0.295242f },
	{ -0.262866f,  0.951056f,  0.162460f },
	{  0.238856f,  0.864188f,  0.442863f },
	{  0.262866f,  0.951056f,  0.162460f },
	{  0.500000f,  0.809017f,  0.309017f },
	{  0.238856f,  0.864188f, -0.442863f },
	{  0.262866f,  0.951056f, -0.162460f },
	{  0.500000f,  0.809017f, -0.309017f },
	{  0.850651f,  0.525731f,  0.000000f },
	{  0.716567f,  0.681718f,  0.147621f },
	{  0.716567f,  0.681718f, -0.147621f },
	{  0.525731f,  0.850651f,  0.000000f },
	{  0.425325f,  0.688191f,  0.587785f },
	{  0.864188f,  0.442863f,  0.238856f },
	{  0.688191f,  0.587785f,  0.425325f },
	{  0.809017f,  0.309017f,  0.500000f },
	{  0.681718f,  0.147621f,  0.716567f },
	{  0.587785f,  0.425325f,  0.688191f },
	{  0.955423f,  0.295242f,  0.000000f },
	{  1.000000f,  0.000000f,  0.000000f },
	{  0.951056f,  0.162460f,  0.262866f },
	{  0.850651f, -0.525731f,  0.000000f },
	{  0.955423f, -0.295242f,  0.000000f },
	{  0.864188f, -0.442863f,  0.238856f },
	{  0.951056f, -0.162460f,  0.262866f },
	{  0.809017f, -0.309017f,  0.500000f },
	{  0.681718f, -0.147621f,  0.716567f },
	{  0.850651f,  0.000000f,  0.525731f },
	{  0.864188f,  0.442863f, -0.238856f },
	{  0.809017f,  0.309017f, -0.500000f },
	{  0.951056f,  0.162460f, -0.262866f },
	{  0.525731f,  0.000000f, -0.850651f },
	{  0.681718f,  0.147621f, -0.716567f },
	{  0.681718f, -0.147621f, -0.716567f },
	{  0.850651f,  0.000000f, -0.525731f },
	{  0.809017f, -0.309017f, -0.500000f },
	{  0.864188f, -0.442863f, -0.238856f },
	{  0.951056f, -0.162460f, -0.262866f },
	{  0.147621f,  0.716567f, -0.681718f },
	{  0.309017f,  0.500000f, -0.809017f },
	{  0.425325f,  0.688191f, -0.587785f },
	{  0.442863f,  0.238856f, -0.864188f },
	{  0.587785f,  0.425325f, -0.688191f },
	{  0.688191f,  0.587785f, -0.425325f },
	{ -0.147621f,  0.716567f, -0.681718f },
	{ -0.309017f,  0.500000f, -0.809017f },
	{  0.000000f,  0.525731f, -0.850651f },
	{ -0.525731f,  0.000000f, -0.850651f },
	{ -0.442863f,  0.238856f, -0.864188f },
	{ -0.295242f,  0.000000f, -0.955423f },
	{ -0.162460f,  0.262866f, -0.951056f },
	{  0.000000f,  0.000000f, -1.000000f },
	{  0.295242f,  0.000000f, -0.955423f },
	{  0.162460f,  0.262866f, -0.951056f },
	{ -0.442863f, -0.238856f, -0.864188f },
	{ -0.309017f, -0.500000f, -0.809017f },
	{ -0.162460f, -0.262866f, -0.951056f },
	{  0.000000f, -0.850651f, -0.525731f },
	{ -0.147621f, -0.716567f, -0.681718f },
	{  0.147621f, -0.716567f, -0.681718f },
	{  0.000000f, -0.525731f, -0.850651f },
	{  0.309017f, -0.500000f, -0.809017f },
	{  0.442863f, -0.238856f, -0.864188f },
	{  0.162460f, -0.262866f, -0.951056f },
	{  0.238856f, -0.864188f, -0.442863f },
	{  0.500000f, -0.809017f, -0.309017f },
	{  0.425325f, -0.688191f, -0.587785f },
	{  0.716567f, -0.681718f, -0.147621f },
	{  0.688191f, -0.587785f, -0.425325f },
	{  0.587785f, -0.425325f, -0.688191f },
	{  0.000000f, -0.955423f, -0.295242f },
	{  0.000000f, -1.000000f,  0.000000f },
	{  0.262866f, -0.951056f, -0.162460f },
	{  0.000000f, -0.850651f,  0.525731f },
	{  0.000000f, -0.955423f,  0.295242f },
	{  0.238856f, -0.864188f,  0.442863f },
	{  0.262866f, -0.951056f,  0.162460f },
	{  0.500000f, -0.809017f,  0.309017f },
	{  0.716567f, -0.681718f,  0.147621f },
	{  0.525731f, -0.850651f,  0.000000f },
	{ -0.238856f, -0.864188f, -0.442863f },
	{ -0.500000f, -0.809017f, -0.309017f },
	{ -0.262866f, -0.951056f, -0.162460f },
	{ -0.850651f, -0.525731f,  0.000000f },
	{ -0.716567f, -0.681718f, -0.147621f },
	{ -0.716567f, -0.681718f,  0.147621f },
	{ -0.525731f, -0.850651f,  0.000000f },
	{ -0.500000f, -0.809017f,  0.309017f },
	{ -0.238856f, -0.864188f,  0.442863f },
	{ -0.262866f, -0.951056f,  0.162460f },
	{ -0.864188f, -0.442863f,  0.238856f },
	{ -0.809017f, -0.309017f,  0.500000f },
	{ -0.688191f, -0.587785f,  0.425325f },
	{ -0.681718f, -0.147621f,  0.716567f },
	{ -0.442863f, -0.238856f,  0.864188f },
	{ -0.587785f, -0.425325f,  0.688191f },
	{ -0.309017f, -0.500000f,  0.809017f },
	{ -0.147621f, -0.716567f,  0.681718f },
	{ -0.425325f, -0.688191f,  0.587785f },
	{ -0.162460f, -0.262866f,  0.951056f },
	{  0.442863f, -0.238856f,  0.864188f },
	{  0.162460f, -0.262866f,  0.951056f },
	{  0.309017f, -0.500000f,  0.809017f },
	{  0.147621f, -0.716567f,  0.681718f },
	{  0.000000f, -0.525731f,  0.850651f },
	{  0.425325f, -0.688191f,  0.587785f },
	{  0.587785f, -0.425325f,  0.688191f },
	{  0.688191f, -0.587785f,  0.425325f },
	{ -0.955423f,  0.295242f,  0.000000f },
	{ -0.951056f,  0.162460f,  0.262866f },
	{ -1.000000f,  0.000000f,  0.000000f },
	{ -0.850651f,  0.000000f,  0.525731f },
	{ -0.955423f, -0.295242f,  0.000000f },
	{ -0.951056f, -0.162460f,  0.262866f },
	{ -0.864188f,  0.442863f, -0.238856f },
	{ -0.951056f,  0.162460f, -0.262866f },
	{ -0.809017f,  0.309017f, -0.500000f },
	{ -0.864188f, -0.442863f, -0.238856f },
	{ -0.951056f, -0.162460f, -0.262866f },
	{ -0.809017f, -0.309017f, -0.500000f },
	{ -0.681718f,  0.147621f, -0.716567f },
	{ -0.681718f, -0.147621f, -0.716567f },
	{ -0.850651f,  0.000000f, -0.525731f },
	{ -0.688191f,  0.587785f, -0.425325f },
	{ -0.587785f,  0.425325f, -0.688191f },
	{ -0.425325f,  0.688191f, -0.587785f },
	{ -0.425325f, -0.688191f, -0.587785f },
	{ -0.587785f, -0.425325f, -0.688191f },
	{ -0.688191f, -0.587785f, -0.425325f }
};
