// ashi

#pragma once

#include "model.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include <memory>

namespace ashi
{
    class AshiGameObject
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

            static AshiGameObject createGameObject()
            {
                static id_t currentId = 0;

                return AshiGameObject { currentId++ };
            }

            AshiGameObject(const AshiGameObject &) = delete;
            AshiGameObject &operator=(const AshiGameObject &) = delete;
            AshiGameObject(AshiGameObject &&) = default;
            AshiGameObject &operator=(AshiGameObject &&) = default;

            id_t getId() { return id; }

            std::shared_ptr<AshiModel> model {};
            glm::vec3 color {};
            Transform3dComponent transform3d {};

        private:
            AshiGameObject(id_t objId): id{objId} {}

            id_t id;
    };
}
