#pragma once
#include "Object3D.h"
#include "Animation.h"
/**
 * @brief Pause an object at a continuous rate over an interval.
 */
class PauseAnimation : public Animation {
private:

	/**
	 * @brief How much to increment the position by each second.
	 */
	glm::vec3 m_perSecond;

	/**
	 * @brief Advance the animation by the given time interval. -> do nothing
	 */
	void applyAnimation(float_t dt) override {
		object();
	}

public:
	/**
	 * @brief Constructs a animation of a constant translation
	 */
	PauseAnimation(Object3D& object, float_t duration) :
		Animation(object, duration) {}
};
