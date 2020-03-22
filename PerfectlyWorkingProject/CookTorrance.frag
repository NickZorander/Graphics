#version 330 core


in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;


out vec3 color;


uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;


float CookTorrance(vec3 _normal, vec3 _light, vec3 _view, float roughness_val)
{
	if (roughness_val <= 0.0) return 0.0; //���� ����������� �������� �������, �� ������� ������

	// �������� ������� ������ ����� ���������� ��������� ����� � �������� �������
	vec3  half_vec = normalize( _view + _light );
	// ������ ������������� ��������� ������������ :)
	float NdotL    = max( dot( _normal, _light ), 0.0 );
	float NdotV    = max( dot( _normal, _view ), 0.0 );
	float NdotH    = max( dot( _normal, half_vec ), 1.0e-7 );
	float VdotH    = max( dot( _view,   half_vec ), 0.0 );
	// NdotH �� ����� ���� ������ ����, ��� ��� � ����������� �� ���� ���� ����� ������

	

	float geometric = min (1, 2*NdotH*NdotV/VdotH);
	geometric= min(geometric, 2*NdotH*NdotL/VdotH);

	// �������� ��������� ������������� �����������
	float r_sq          = roughness_val * roughness_val;
	float NdotH_sq      = NdotH * NdotH;
	float e				= exp( ( NdotH_sq - 1) / (r_sq * NdotH_sq));
	float roughness		= e/(r_sq*NdotH_sq*NdotH_sq);
	

	// �������� ����������� �������, �� ����� �������������� ��������
	float fresnel       = 1.0 / (1.0 + NdotV);

	// ��������� ���������, �������� � ����������� ����� ��������
	// ����� �� ���� ������� �� ����
	return min(1.0, (fresnel * geometric * roughness) / (3.141592 * NdotV * NdotL + 1.0e-7));
}


void main(){

	//�������� ����� 
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;
	
	// �������� ��������� 
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.2,0.2,0.2) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// ���������� �� �����
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// ������� 
	vec3 n = normalize( Normal_cameraspace );
	// ����������� �� ���� 
	vec3 l = normalize( LightDirection_cameraspace );
	// �������
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// ������ �� ������ 
	vec3 E = normalize(EyeDirection_cameraspace);	
	// ����������� ��������� �����
	vec3 R = reflect(-l,n);
	// ������� 
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	color = 
		// ��� 
		MaterialAmbientColor +
		// ��������� 
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// �������� 
		MaterialSpecularColor * LightColor * LightPower * CookTorrance(n, l, E, 0.5) / (distance*distance);
}