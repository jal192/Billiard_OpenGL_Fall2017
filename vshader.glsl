#version 130

in vec4 vPosition;
in vec4 vNormal;
out vec4 color;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform mat4 ctm;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct, LightPosition;
uniform float shininess, attenuation_constant, attenuation_linear, attenuation_quadratic;
vec4 ambient, diffuse, specular;

uniform int isShadow;

void main()
{	
	vec4 vP = ctm * vPosition;
	if(isShadow == 1) {
		vec4 shadow_color = vec4(0.0, 0.06, 0.0, 1.0);
		color = shadow_color;
		float x = LightPosition.x - LightPosition.y * ((LightPosition.x - vP.x) / (LightPosition.y - vP.y));
		float y = 0.001;
		float z = LightPosition.z - LightPosition.y * ((LightPosition.z - vP.z) / (LightPosition.y - vP.y));
		gl_Position = projection_matrix * model_view_matrix * vec4(x, y, z, 1.0);
	}
	else {
		ambient = AmbientProduct;
		vec4 N = normalize(model_view_matrix * vNormal);
		vec4 L_temp = model_view_matrix * (LightPosition - vP);
		vec4 L = normalize(L_temp);
		diffuse = max(dot(L,N), 0.0) * DiffuseProduct;
		vec4 EyePoint = vec4(0.0, 0.0, 0.0, 1.0);
		vec4 V = normalize(EyePoint - (model_view_matrix * vP));
		vec4 H = normalize(L + V);
		specular = pow(max(dot(N,H), 0.0), shininess) * SpecularProduct;
		float distance = length(L_temp);
		float attenuation = 1/(attenuation_constant + (attenuation_linear * distance) + (attenuation_quadratic * distance * distance));

		color = ambient + attenuation * (diffuse + specular);
		gl_Position = projection_matrix * model_view_matrix * vP;
	}
	
}