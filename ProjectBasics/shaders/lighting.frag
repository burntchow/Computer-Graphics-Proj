#version 330
// A fragment shader for rendering fragments in the Phong reflection model.
layout (location=0) out vec4 FragColor;

// Inputs: the texture coordinates, world-space normal, and world-space position
// of this fragment, interpolated between its vertices.
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragWorldPos;

// Uniforms: MUST BE PROVIDED BY THE APPLICATION.

// The mesh's base (diffuse) texture.
uniform sampler2D baseTexture;

// Material parameters for the whole mesh: k_a, k_d, k_s, shininess.
uniform vec4 material;

// Ambient light color.
uniform vec3 ambientColor;

// Direction and color of a single directional light.
uniform vec3 directionalLight; // this is the "I" vector, not the "L" vector.
uniform vec3 directionalColor;

// Location of the camera.
uniform vec3 viewPos;


// Spot light things
uniform vec3 spotLightPos;
uniform vec3 spotLightDir;
uniform vec3 spotLightColor; 
uniform float spotLightCutOff;
uniform float spotLightOuterCutOff;

uniform float spotLightAmbient;
uniform float spotLightDiffuse;
uniform float spotLightSpecular;


void main() {
    // TODO: using the lecture notes, compute ambientIntensity, diffuseIntensity, 
    // and specularIntensity.

    //vec3 ambientIntensity = vec3(0);
    vec3 ambientIntensity = material.x * ambientColor; 

    vec3 diffuseIntensity = vec3(0);
    vec3 norm = normalize(Normal);
    vec3 lightDir = -directionalLight;
    float lambertFactor = dot(norm, normalize(lightDir));

    vec3 specularIntensity = vec3(0);

    if(lambertFactor > 0){
        diffuseIntensity = material.y * directionalColor * lambertFactor;
        
        vec3 eyeDir = normalize(viewPos - FragWorldPos);
        vec3 reflectDir = normalize(reflect(-lightDir, norm));
        float spec = dot(reflectDir, eyeDir);

        if(spec > 0){
            specularIntensity = material.z * directionalColor * pow(spec,material.w);
        }
    }

    // ref https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/5.4.light_casters_spot_soft/5.4.light_casters.fs
    // spotlight
    //vec3 ambientIntensitySpot = material.x * spotLightColor;
    //vec3 diffuseIntensitySpot = diffuseIntensity * spotLightColor;
    //vec3 specularIntensitySpot = specularIntensity * spotLightColor;

    // spot ambient
    
    vec3 ambientIntensitySpot = spotLightAmbient * spotLightColor;

    // spot diffuse 
    vec3 normSpot = normalize(Normal); 
    vec3 lightDirSpot = normalize(spotLightPos - FragWorldPos);
    float diff = max(dot(normSpot, spotLightDir), 0.0);
    vec3 diffuseIntensitySpot = spotLightDiffuse * diff * spotLightColor;

    // spot specular
    vec3 viewSpotDir = normalize(viewPos - FragWorldPos);
    vec3 reflectSpotDir = reflect(-lightDirSpot, normSpot);
    float spec = pow(max(dot(viewSpotDir, reflectSpotDir), 0.0), material.w); // material.w -> shininess?
    vec3 specularIntensitySpot = spotLightSpecular * spec * spotLightColor;

    float theta = dot(lightDir, normalize(-spotLightDir));
    float epsilon = (spotLightCutOff - spotLightOuterCutOff);
    float intensity = clamp((theta - spotLightOuterCutOff) / epsilon, 0.0, 1.0);
    diffuseIntensitySpot *= intensity; //
    specularIntensitySpot *= intensity; //
     

    // attenuation
    float distance = length(spotLightPos - FragWorldPos);
    // light constant 1 ? light linear 0.09 ? light quadratic 0.032
    float attentuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    ambientIntensitySpot *= attentuation; //
    diffuseIntensitySpot *= attentuation; // 
    specularIntensitySpot *= attentuation;// 

    // put together -> https://learnopengl.com/Lighting/Multiple-lights 
    ambientIntensity += ambientIntensitySpot;
    diffuseIntensity += diffuseIntensitySpot;
    specularIntensity += specularIntensitySpot; 

    vec3 lightIntensity = ambientIntensity + diffuseIntensity + specularIntensity;
    FragColor = vec4(lightIntensity, 1) * texture(baseTexture, TexCoord);

   
}