// dsk

#pragma once

#include "model.hpp"
#include "texture.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include <memory>

namespace dsk
{
    class DskGameObject
    {
        struct Transform3dComponent
        {
            glm::vec3 translation {};
            glm::vec3 scale {1.0f};
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

            id_t getId() { return id; }

            DskGameObject &setModel
            (
                DskDevice &dskDevice,
                std::string filename
            );
            DskGameObject &setTexture
            (
                DskDevice &dskDevice,
                std::string filename,
                DskTexture textures[]
            );
            DskGameObject &setTranslation(glm::vec3 translation);
            DskGameObject &setScale(glm::vec3 scale);
            DskGameObject &setRotation(glm::vec3 rotation);
            DskGameObject &build
            (
                std::vector<DskGameObject> *gameObjects
            );

            glm::vec3 getTranslation()
            { return transform3d.translation; }

            glm::vec3 getScale()
            { return transform3d.scale; }

            glm::vec3 getRotation()
            { return transform3d.rotation; }

            std::shared_ptr<DskModel> model {};
            glm::vec3 color {};
            Transform3dComponent transform3d {};

        private:
            DskGameObject(id_t objId): id{objId} {}

            id_t id;
    };
}
