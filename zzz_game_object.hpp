// zzz

#pragma once

#include "zzz_model.hpp"

#include <memory>

namespace zzz
{
    class ZzzGameObject
    {
        struct Transform2dComponent
        {
            glm::vec2 translation {};
            glm::vec2 scale { 1.0f, 1.0f };
            float rotation;

            glm::mat2 mat2()
            {
                const float s = glm::sin(rotation);
                const float c = glm::cos(rotation);
                glm::mat2 rotMat {{ c, s }, { -s, c }};
                glm::mat2 scaleMat {{ scale.x, 0.0f }, { 0.0f, scale.y }};
                return rotMat * scaleMat;
            }
        };

        public:
            using id_t = unsigned int;

            static ZzzGameObject createGameObject()
            {
                static id_t currentId = 0;
                return ZzzGameObject { currentId++ };
            }

            ZzzGameObject(const ZzzGameObject &) = delete;
            ZzzGameObject &operator=(const ZzzGameObject &) = delete;
            ZzzGameObject(ZzzGameObject &&) = default;
            ZzzGameObject &operator=(ZzzGameObject &&) = default;

            id_t getId() { return id; }

            std::shared_ptr<ZzzModel> model {};
            glm::vec3 color {};
            Transform2dComponent transform2d {};

        private:
            ZzzGameObject(id_t objId): id{objId} {}

            id_t id;
    };
}