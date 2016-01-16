#version 330

layout (location = 0) in vec4 pos;
uniform float loopDur;
uniform float time;

void main() {
	float timeScale = 3.1415 * 2.0f / loopDur;
	float curTime = mod(time, loopDur);

	vec4 offset = vec4( 
		cos(curTime*timeScale)*0.5f,
		sin(curTime*timeScale)*0.5f,
		0.0f,
		0.0f
	);
	gl_Position = pos + offset;
}