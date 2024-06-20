#pragma once
#include "Object3D.h"
#include "Animation.h"
/**
 * @brief Bezier Translation an object at a continuous rate over an interval.
 */
class BezierTranslationAnimation : public Animation {
private:
	/**
	 * @brief How much to increment the position by each second.
	 */
	//glm::vec3 m_perSecond;

	/*Control points*/
	glm::vec3 controlPointA;
	glm::vec3 controlPointB;
	glm::vec3 controlPointC; // end point ? 

	/**
	 * @brief Advance the animation by the given time interval.
	 */
	void applyAnimation(float_t dt) override {
		// calculate t 
		float_t t = currentTime() / duration();

		// bezier curve:  
		// P_0 -> original point
		// P_1 -> control point
		// P_2 -> end point ?  

		glm::vec3 p0 = controlPointA;
		glm::vec3 p1 = controlPointB;
		glm::vec3 p2 = controlPointC;

		// B(t) = (1-t)[(1-t)P_0 + t*P_1] + t[(1-t]*P_1 + t*P_2], 
		// where 0 <= t <= 1 
		glm::vec3 bezPosition = (1 - t) * ((1 - t) * p0 + t * p1) + t * ((1 - t) * p1 + t * p2);

		object().setPosition(bezPosition);
		//object().move(m_perSecond * dt);
	}

public:
	/**
	 * @brief Constructs a animation of a constant translation
	 */
	BezierTranslationAnimation(Object3D& object, float_t duration, const glm::vec3& controlPointA, const glm::vec3& controlPointB, const glm::vec3& controlPointC) :
		Animation(object, duration), controlPointA(controlPointA), controlPointB(controlPointB), controlPointC(controlPointC) {}
};
