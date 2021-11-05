// dsk

#pragma once

#include "model.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include <memory>

namespace dsk
{
    class DskGameObject
    {
        struct Transform3dComponent
        {
            glm::vec3 translation {};
            glm::vec3 scale {1.0f, 1.0f, 1.0f};
            glm::vec3 rotation {};

            glm::mat4 mat4();
            glm::mat3 normalMatrix();
        };

        public:
            using id_t = unsigned int;

            static DskGameObject createGameObject()
            {
                static id_t currentId = 0;

                return DskGameObject {currentId++};
            }

            DskGameObject(const DskGameObject &) = delete;
            DskGameObject &operator=(const DskGameObject &) = delete;
            DskGameObject(DskGameObject &&) = default;
            DskGameObject &operator=(DskGameObject &&) = default;

            id_t getId() { return id; }

            std::shared_ptr<DskModel> model {};
            glm::vec3 color {};
            Transform3dComponent transform3d {};

            VkDescriptorSet textureDescriptorSet;

        private:
            DskGameObject(id_t objId): id{objId} {}

            id_t id;
    };
}
