#version 430 core

layout( local_size_x = 64, local_size_y =1, local_size_z = 1  ) in;

uniform float squaredSoftening;
uniform int numParticles;

layout(std430, binding=0) buffer positionsBuffer
{
    vec4 positions[];
};

layout(std430, binding=3) buffer massesBuffer
{
    vec4 masses[];
};

layout(std430, binding=4) buffer forcesBuffer
{
    vec4 forces[];
};

layout(std430, binding=5) buffer weightsBuffer
{
    float weights[];
};

void main() {
    uint index = gl_GlobalInvocationID.x;

    if (index < numParticles) {


        float G = 1.0;
        vec3 totalForce = vec3(0);
        float particleMass = masses[index].x;

        vec3 particlePosition = positions[index].xyz;


        for (uint j = 0;  j < numParticles; j++){
            float inputArray[6] = float[6](particlePosition.x, particlePosition.y, particlePosition.z, positions[j].x, positions[j].y, positions[j].z);

            // Perform matrix multiplication by summing the products
            float res[6 * 3];
            for (int i = 0; i < 6; ++i) {
                for (int j = 0; j < 3; ++j) {
                    int index = i * 3 + j;

                    res[index] = inputArray[i] * weights[j];
                }
            }

                // Initialize the 3D vector
            vec3 resultVec;

            // Calculate the size of each part
            int partSize = 6 * 3 / 3;

            // Calculate the mean of each part and assign it to the components of the vector
            for (int i = 0; i < 3; ++i) {
                float sum = 0.0;
                for (int j = i * partSize; j < (i + 1) * partSize; ++j) {
                    sum += res[j];
                }
                totalForce[i] += (sum / partSize * 0.001);
            }


            //const vec3 vector_i_j = positions[j].xyz - particlePosition;
            //const float distance_i_j = pow(dot(vector_i_j, vector_i_j) + squaredSoftening, 2);
            //totalForce += (((G * particleMass * masses[j].x) / distance_i_j) * vector_i_j) * (1 + 0.005 * distance_i_j);
        }


        // Write to global memory the result
        forces[index] = vec4(totalForce, 0);

    }

}
