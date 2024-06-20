#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>
#include "Object3D.h"
#include <iostream>

void Object3D::rebuildModelMatrix() {
	auto m = glm::translate(glm::mat4(1), m_position);
	m = glm::translate(m, m_center * m_scale);
	m = glm::rotate(m, m_orientation[2], glm::vec3(0, 0, 1));
	m = glm::rotate(m, m_orientation[0], glm::vec3(1, 0, 0));
	m = glm::rotate(m, m_orientation[1], glm::vec3(0, 1, 0));
	m = glm::scale(m, m_scale);
	m = glm::translate(m, -m_center);
	m = m * m_baseTransform;
	m_modelMatrix = m;
}

///// Simulation
 
void Object3D::tick(float_t dt) {
	// add up all values in forces list and divide that sum by obj mass
	glm::vec3 sum_force(0, 0, 0);
	for (auto i = 0; i < m_forces.size(); i++) {
		sum_force += m_forces[i];
	}

	// acceleration modify velocity 
	auto acceleration = sum_force / m_mass; // F = ma -> a = F/m 

	//m_velocity += m_acceleration * dt;
	m_velocity += acceleration * dt; // new one 

	// velocity modify position 
	m_position += m_velocity * dt;

	// rotational velocity added to objects orientation w/ acceleration after each tick ?
	m_rotationalVelocity += m_rotationalAcceleration * dt;
	m_orientation += m_rotationalVelocity * dt;


	rebuildModelMatrix();

	// clear forces at the end of tick
	m_forces.clear();
}

void Object3D::addForceToList(const glm::vec3& force) {
	m_forces.push_back(force);
}

///// Texture!
void Object3D::addTexture(Texture texture) {
	for (auto& mesh : m_meshes) {
		mesh.addTexture(texture); 
	}
}  

Object3D::Object3D(std::vector<Mesh3D>&& meshes)
	: Object3D(std::move(meshes), glm::mat4(1)) {
	rebuildModelMatrix();
}

Object3D::Object3D(std::vector<Mesh3D>&& meshes, const glm::mat4& baseTransform)
	: m_meshes(meshes), m_position(), m_orientation(), m_scale(1.0),
	m_center(), m_baseTransform(baseTransform)
{
	rebuildModelMatrix();
}

const glm::vec3& Object3D::getPosition() const {
	return m_position;
}

const glm::vec3& Object3D::getOrientation() const {
	return m_orientation;
}

const glm::vec3& Object3D::getScale() const {
	return m_scale;
}

/**
 * @brief Gets the center of the object's rotation.
 */
const glm::vec3& Object3D::getCenter() const {
	return m_center;
}

const std::string& Object3D::getName() const {
	return m_name;
}

const glm::vec3& Object3D::getVelocity() const {
	return m_velocity;
}
 

const glm::vec3& Object3D::getRotationalAcceleration() const {
	return m_rotationalAcceleration;
}

const glm::vec3& Object3D::getRotationalVelocity() const {
	return m_rotationalVelocity;
}

const float Object3D::getMass() const {
	return m_mass;
}


size_t Object3D::numberOfChildren() const {
	return m_children.size();
}

const Object3D& Object3D::getChild(size_t index) const {
	return m_children[index];
}

Object3D& Object3D::getChild(size_t index) {
	return m_children[index];
}

void Object3D::setPosition(const glm::vec3& position) {
	m_position = position;
	rebuildModelMatrix();
}

void Object3D::setOrientation(const glm::vec3& orientation) {
	m_orientation = orientation;
	rebuildModelMatrix();
}

void Object3D::setScale(const glm::vec3& scale) {
	m_scale = scale;
	rebuildModelMatrix();
}

void Object3D::setVelocity(const glm::vec3& velocity) {
	m_velocity = velocity;
	rebuildModelMatrix();
}
 

void Object3D::setRotationalAcceleration(const glm::vec3& rotacceleration) {
	m_rotationalAcceleration = rotacceleration;
	rebuildModelMatrix();
}

void Object3D::setRotationalVelocity(const glm::vec3& rotvelocity) {
	m_rotationalVelocity = rotvelocity;
	rebuildModelMatrix();
}

void Object3D::setMass(const float mass) {
	m_mass = mass;
}


/**
 * @brief Sets the center point of the object's rotation, which is otherwise a rotation around 
   the origin in local space..
 */
void Object3D::setCenter(const glm::vec3& center)
{
	m_center = center;
}

void Object3D::setName(const std::string& name) {
	m_name = name;
}

void Object3D::move(const glm::vec3& offset) {
	m_position = m_position + offset;
	rebuildModelMatrix();
}

void Object3D::rotate(const glm::vec3& rotation) {
	m_orientation = m_orientation + rotation;
	rebuildModelMatrix();
}

void Object3D::grow(const glm::vec3& growth) {
	m_scale = m_scale * growth;
	rebuildModelMatrix();
}

void Object3D::addChild(Object3D&& child)
{
	m_children.emplace_back(child);
}

void Object3D::render(sf::RenderWindow& window, ShaderProgram& shaderProgram) const {
	renderRecursive(window, shaderProgram, glm::mat4(1));
}

/**
 * @brief Renders the object and its children, recursively.
 * @param parentMatrix the model matrix of this object's parent in the model hierarchy.
 */
void Object3D::renderRecursive(sf::RenderWindow& window, ShaderProgram& shaderProgram, const glm::mat4& parentMatrix) const {
	// This object's true model matrix is the combination of its parent's matrix and the object's matrix.
	glm::mat4 trueModel = parentMatrix * m_modelMatrix;
	shaderProgram.setUniform("model", trueModel);
	// Render each mesh in the object.
	for (auto& mesh : m_meshes) {
		mesh.render(window, shaderProgram);
	}
	// Render the children of the object.
	for (auto& child : m_children) {
		child.renderRecursive(window, shaderProgram, trueModel);
	}
}
