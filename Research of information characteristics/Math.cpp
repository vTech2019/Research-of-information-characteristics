#include "Math.h"
float4 cross_vec3(float4 const first_vector, float4 const second_vector) {
	float4 vector_1_1 = float4{ first_vector.y, first_vector.z, first_vector.x, 0.0f };
	float4 vector_1_2 = float4{ first_vector.z, first_vector.x, first_vector.y, 0.0f };
	float4 vector_2_1 = float4{ second_vector.z, second_vector.x, second_vector.y, 0.0f };
	float4 vector_2_2 = float4{ second_vector.y, second_vector.z, second_vector.x, 0.0f };
	return float4{ vector_1_1 * vector_2_1 - vector_1_2 * vector_2_2 };
}
float4 normalize_vec3(float4 _vector) {
	GLfloat vector = 1.0f / sqrtf((_vector.x * _vector.x) + (_vector.y * _vector.y) + (_vector.z * _vector.z));
	return _vector * vector;
}

